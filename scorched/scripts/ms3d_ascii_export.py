#!BPY

"""
Name: 'MS3D ASCII (.txt) v 1.5.2'
Blender: 242
Group: 'Export'
Tooltip: 'MilkShape3d ASCII format for Scorched3d models v1.5.2'
"""

#Copyright (C) 2004-2007 Paul Vint cbx550f@sourceforge.net
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#
#-------------------------------------------------------------
#
# initial alpha release, 17/08/2004
# update: Fixed point of view - front, side top etc are now correct 27/08/2004
# Added error checking for number of verts per face 07/09/2004
# Reversed vertex winding to fix flipped normals problem & misc speedups 12/03/06
# Updated for Blender 2.4x - thanks for the help, Berem!
# TODO: Stop using face nrmals, should be vertex normals! - done. 05/09/06
# Added proper support for emitted light 05/11/06
# TODO - Have it export quads as triangles automagically (a la 3ds export)
# Fix: Smoothing groups are now properly supported 28/10/2007

import Blender
from Blender import Draw, BGL
from Blender.BGL import *
from Blender.Draw import *
import os, time, sys
import meshtools

## Events
EVENT_NOEVENT = 1
EVENT_DRAW = 2
EVENT_EXIT = 3
EVENT_EXPORT = 4
EVENT_CHOOSE_FILENAME = 5
EVENT_PROGRESS = 6

def stripPath(path):
        return path.split('/')[-1].split('\\')[-1]

def fs_callback(filename):
	global FILENAME
        if filename.find('.txt', -4) <= 0: filename += '.txt'
	FILENAME.val = filename
        #write(filename)

def createS3DPath():
	global FILENAME
        filename = Blender.Get('filename')

        if filename.find('.') != -1:
                filename = filename.split('.')[0]
                filename += ".txt"

        #return filename
	FILENAME.val = filename
	return filename

def fileSelect():
        Blender.Window.FileSelector(fs_callback, "Select", createS3DPath())

FILENAME = Create('filename')
createS3DPath()

showProgress = Create(1)

### Basic GUI functions
def draw():     # Define the draw function (which draws your GUI).
        #Blender.BGL.glClear(Blender.BGL.GL_COLOR_BUFFER_BIT) # This clears the window
        # Add here drawing commands to draw your GUI, for example:
        #Blender.Draw.Toggle("Clear origin",1,10,20,100,20,0,"Tooltip")
        # The line abive will draw a toggle button.
        # Note the first number, '1' means this is button number 1
	global EVENT_NOEVENT,EVENT_DRAW,EVENT_EXIT,EVENT_EXPORT,EVENT_CHOOSE_FILENAME,FILENAME,EVENT_PROGRESS, showProgress

	ypos = 110
	glClear(GL_COLOR_BUFFER_BIT)
	glRasterPos2d(8, ypos)
	Text("MilkShape3d ASCII Export version 1.5")
	glRasterPos2d(8, ypos - 15)
	Text("Copyright 2007 - Paul Vint cbx550f@users.sourceforge.net")
	glRasterPos2d(8, ypos - 30)
	Text("For updates visit http://sourceforge.net/projects/scorched3d-mods")

	## Buttons etc
	glRasterPos2d(8, 83)
	Button("Export",EVENT_EXPORT, 10, 10, 80, 18)
	Button("Exit",EVENT_EXIT, 140, 10, 80, 18)
	showProgress = Toggle("Show Progress",EVENT_PROGRESS,10, 30, 80, 18, showProgress.val, "Deselect to disable the progress bar (faster exporting)")

	glRasterPos2d(8, 83)
        FILENAME = String("Filename: ", EVENT_NOEVENT, 10, 55, 210, 18,
                            FILENAME.val, 255, ".txt file to export")
        Button("Browse",EVENT_CHOOSE_FILENAME,220,55,80,18)


        
def event(evt,val):  # Define mouse and keyboard press events
        if (evt == Blender.Draw.ESCKEY): # Example if esc key pressed
                Blender.Draw.Exit()    # then exit script
                return                 # return from the function

def bevent(evt): #Manage button events
	global FILENAME,showProgress
	if (evt == EVENT_EXIT):
		Blender.Draw.Exit() 
		return
	elif (evt == EVENT_CHOOSE_FILENAME):
		fileSelect()
		Blender.Redraw()
		return
	elif (evt == EVENT_EXPORT):
		#print showProgress
		#showProgress = showProgress
		write(FILENAME.val)
		Blender.Draw.Exit()
		return


#Blender.Draw.Register(draw,event,button)


#======= Write ms3d .txt format =========

dbg = 0   #set to enable debugging

	
def write(filename):
	global showProgress
	print showProgress
	start = time.clock()
	
	convertToTris = 0;
	if Blender.sys.exists(filename) and (Blender.Draw.PupMenu("File exists, replace?%t|YES|NO") == 2):
		fileSelect()
		return

		
	file = open(filename, "wb")

	# ms3d header
	file.write("// MilkShape 3D ASCII\n\nFrames: 30\nFrame: 1\n\n")

	file.write("Meshes: " + str(len(Blender.Object.GetSelected())) + '\n')

	objects = Blender.Object.GetSelected()

	for obj in range(len(objects)):
		meshname = objects[ obj].data.name
		mesh = Blender.NMesh.GetRaw(meshname)
		thismesh = meshname

		if dbg:
			print "\nDEBUG: Mesh", mesh, "Obj: ", obj, "Name: ", objects[obj].data.name
		if mesh:
			print "Mesh",objects[obj].data.name,
			meshname = "\"" + objects[obj].data.name + "\" 0 " + str(obj) + "\n"  # Setting flags=0, mat index=object number for now, also note that I'm using object name, not meshname
			file.write(meshname)

			file.write(str(3 * len(mesh.faces)) + '\n') # should be better way to get # of verts, but this is fast
			for i in range(len(mesh.faces)):  # New in v 0.3 - getting verts from faces directly
				faceVerts = len(mesh.faces[i].v) # Reversing the winding to fix flipped normals problem
				for fv in range(faceVerts):  # Scan through each vert in face
					line = '0 ' + str(-round(mesh.faces[i].v[faceVerts-fv-1][0],4)) + ' ' + str(round(mesh.faces[i].v[faceVerts-fv-1][2],4)) + ' ' + str(round(-mesh.faces[i].v[faceVerts-fv-1][1],4)) + ' ' #+ str(round(mesh.faces[i].uv[fv][0],5)) + ' ' + str(round(mesh.faces[i].uv[fv][1],5))  + ' -1\n'
					if (len(mesh.faces[i].uv)):
						line += str(round(mesh.faces[i].uv[faceVerts-fv-1][0],5)) + ' ' + str(round(1-mesh.faces[i].uv[faceVerts-fv-1][1],5))  + ' -1\n'
					else:
						line += '0.0 0.0 -1\n'  

					if not i%50 and showProgress.val:
						Blender.Window.DrawProgressBar(float(i)/len(mesh.faces), thismesh + "Verts")
					file.write(line)

			# Normals
			#print "Norms: ",len(mesh.faces),  #FIX! Temp only using # of faces
			file.write(str(3 * len(mesh.faces)))
			for i in range(len(mesh.faces)):
				if not i%50 and showProgress.val:
					Blender.Window.DrawProgressBar(float(i)/len(mesh.faces), thismesh + "Normals")
				#file.write('\n' + str(round(mesh.faces[i].no[0],4)) + ' ' + str(round(mesh.faces[i].no[2],4)) + ' ' + str(round(mesh.faces[i].no[1],4)))
				faceVerts = len(mesh.faces[i].v)
				for fv in range(faceVerts):  # Scan through each vert in face
					# Check if the face is smoothed or not:
					if (mesh.faces[i].smooth):
						file.write('\n' + str(round(-mesh.faces[i].v[faceVerts-fv-1].no[0],4)))
						file.write(' ' + str(round(mesh.faces[i].v[faceVerts-fv-1].no[2],4)))
						file.write(' ' + str(round(-mesh.faces[i].v[faceVerts-fv-1].no[1],4)))
					else:
						file.write('\n' + str(round(-mesh.faces[i].no[0],4)) + ' ' + str(round(mesh.faces[i].no[2],4)) + ' ' + str(round(-mesh.faces[i].no[1],4)))

			# Faces
			print "Triangles: " ,len(mesh.faces)
			file.write('\n' + str(len(mesh.faces)) + '\n')
			idx = 0
			line = ""
			for i in range(len(mesh.faces)):  # one face at a time
				if not i%50 and showProgress.val:
					Blender.Window.DrawProgressBar(float(i)/len(mesh.faces), thismesh + "Triangles")
				if (len(mesh.faces[i].v) != 3):       #Ensure each face is a triangle!
					print "\n\n*********** Error!   **********\n"
					print "Object",objects[obj].data.name, "has", len(mesh.faces[i].v), "vertices in face", i, "Must have THREE verts per face - convert to triangles with CTRL-T!"
					Blender.Window.WaitCursor(0)
					exitmsg = 'MS3D Export Error:|Mesh \"' + objects[obj].data.name + '\" has ' + str(len(mesh.faces[i].v)) + ' verts in a face, needs to be converted to triangles with CTRL-T!' 
					Blender.Draw.PupMenu(exitmsg)
					return
					######## Keeping depricated (and broken) tri2quad for future reference
					#if convertToTris==0:	#only ask once
					#	print "Object",objects[obj].data.name, "has", len(mesh.faces[i].v), "vertices in face", i, "Must have THREE verts per face"
					#	convertToTris=Blender.Draw.PupMenu("Model not made entirely out of Triangles-Convert?%t|YES|NO")
					#if convertToTris==1:
					#	for f in mesh.faces:
					#		f.sel = 1
					#	Blender.Mesh.Mode(3) 
					#	mesh.quadToTriangle(0)
					#elif convertToTris==2:
					#	exitmsg = 'MS3D Export Error:|Mesh \"' + objects[obj].data.name + '\" has ' + str(len(mesh.faces[i].v)) + ' verts in a face, needs to be converted to triangles with CTRL-T!'
					#	Blender.Draw.PupMenu(exitmsg)
					#	return
						

				for v in range(len(mesh.faces[i].v)):  #Go through each vertex in face - only need triangles for MS3D, but this would allow for quads too for portability
					line += str(idx) + " "
					idx += 1
						#print cnt  #debug info
				#file.write('1 ' + line + str( i) + ' ' + str(i) + ' ' + str(i) + ' 1\n')
				file.write('1 ' + line + line + ' 1\n')

				line = ""  # fresh start for next face

		######### Materials! ##########
	numMats = 0
	for obj in range(len(objects)):         #FIXME! Stupid cludge for counting materials
		meshname = objects[ obj].data.name
		mesh = Blender.NMesh.GetRaw(meshname)
		if (len(mesh.materials) == 0):	#Ensure the mesh has a material
			print '*** Error! ', meshname, 'must have a material & texture!'
			message = 'MS3D Export Error:|Mesh \"' + objects[obj].data.name + '\"needs to have a material!'
			meshtools.print_boxed(message)
        		Blender.Window.WaitCursor(0)
        		Blender.Draw.PupMenu(message)
        		return

		if mesh.materials[0]:
			numMats+=1
	file.write("\nMaterials: " + str(numMats) + "\n")
	
	for obj in range(len(objects)):
		meshname = objects[ obj].data.name
		mesh = Blender.NMesh.GetRaw(meshname)

		for material in mesh.materials:
			file.write("\"" + material.name + "\"\n") 

			#file.write(str(round(material.ref,5))+ " " + str(round(material.ref,5))+ " " + str(round(material.ref,5)) + " " + str(round(material.alpha,5)) + "\n")
			#TODO Why are these values the same??? TODO
			# file.write(str("%5f %5f %5f %5f\n" % (material.ref,material.ref,material.ref,material.alpha)))
			file.write(str("%5f %5f %5f %5f\n" % (material.amb,material.amb,material.amb,material.alpha)))
			#file.write(str(round(material.rgbCol[0],5)) + " " + str(round(material.rgbCol[1],5)) + " " + str(round(material.rgbCol[2],5)) + " " + str(round(material.alpha,5)) + "\n")
			file.write(str("%5f %5f %5f %5f\n" % (material.rgbCol[0],material.rgbCol[1],material.rgbCol[2],material.alpha)))
			file.write(str("%5f %5f %5f %5f\n" % (material.spec,material.spec,material.spec,material.alpha)))
			
			#file.write(str(round(material.spec,5)) + " " + str(round(material.spec,5)) + " " + str(round(material.spec,5)) + " " + str(round(material.alpha,5)))
			file.write(str("%5f %5f %5f %5f\n" % (material.emit,material.emit,material.emit,material.alpha)))
			file.write(str(material.ref) + "\n")   
			file.write(str(material.alpha) + "\n")
			# get the current texture image
			imageName=""
			mytex = material.getTextures()
			if (dbg):
				print 'MyTex: ', mytex[0]
			if ( mytex[0] != None ):
				image = material.getTextures()[0].tex.getImage()
				imageName = Blender.sys.basename(image.getFilename())

				file.write("\".\\" + stripPath(imageName) + "\"")

			else:
				print 'Warning: Mesh ' + meshname + ' does not have a texture image!'
				message = 'MS3D Export Warning:|Mesh \"' + meshname + '\" does not have a texture image!'
				meshtools.print_boxed(message)
				Blender.Window.WaitCursor(0)
				Blender.Draw.PupMenu(message)

				file.write("\"\"")

			#file.write("\".\\" + imageName + "\"")
			file.write("\n\"\"\n")  


	
	
	file.write('\nBones: 0\n') # we don't need no stinking bones for Scorched! ;)
	file.write('GroupComments: 0\n')
	file.write('MaterialComments: 0\n')
	file.write('BoneComments: 0\n')
	file.write('ModelComment: 0\n')
	Blender.Window.DrawProgressBar(1.0, '') #clear progressbar
 	file.close()
	end = time.clock()
	seconds = " in %.2f %s" % (end-start, "seconds")
	message = "Successfully exported " + os.path.basename(filename) + seconds
	meshtools.print_boxed(message)
	Blender.Window.WaitCursor(0)
        Blender.Draw.PupMenu(message)
	return


Blender.Draw.Register(draw,event,bevent)



