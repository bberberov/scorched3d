////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#include <GLEXT/GLFont2dStorage.h>
#include <common/DefinesAssert.h>
#include <stdio.h>
#include <stdlib.h>

unsigned int GLFont2dStorage::totalCharacterBlocks_(0);

GLFont2dStorage::CharEntry::CharEntry() :
	texture(0), displaylist(0)
{

}

GLFont2dStorage::CharEntry::~CharEntry()
{
	if (displaylist != 0) glDeleteLists(displaylist, 1);
	if (texture != 0) glDeleteTextures(1, &texture);
}

GLFont2dStorage::StorageBlock::StorageBlock()
{
	entries = new CharEntry[256];
}

GLFont2dStorage::StorageBlock::~StorageBlock()
{
	delete [] entries;
}

GLFont2dStorage::GLFont2dStorage()
{
	blocks_ = new StorageBlock*[40];
	for (int i=0; i<40; i++) blocks_[i] = 0;
}

GLFont2dStorage::~GLFont2dStorage()
{
	for (int i=0; i<40; i++) delete blocks_[i];
	delete [] blocks_;
}

GLFont2dStorage::CharEntry *GLFont2dStorage::getEntry(unsigned int character)
{
	unsigned int blockCount = character >> 8;
	unsigned int remainderCount = character & 0xFF;

	if (blockCount >= 40) return getEntry('?');

	// Get storage block
	StorageBlock *block = blocks_[blockCount];
	if (!block)
	{
		block = new StorageBlock();
		blocks_[blockCount] = block;
		totalCharacterBlocks_++;
	}

	// Get character
	CharEntry *entry = &block->entries[remainderCount];
	return entry;
}
