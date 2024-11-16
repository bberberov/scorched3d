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

#include <common/SplineCurve.h>

//
// This code is taken from 
// http://astronomy.swin.edu.au/~pbourke/curves/spline/
// with some changes to make it less reliant on fixed c arrays
//

/*
   Calculate the blending value, this is done recursively.
   
   If the numerator and denominator are 0 the expression is 0.
   If the deonimator is 0 the expression is 0
*/
static fixed splineBlend(int k,int t,int *u,fixed v)
{
   fixed value;

   if (t == 1) {
      if ((v > u[k]) && (v < u[k+1]))
         value = 1;
      else
         value = 0;
   } else {
      if ((u[k+t-1] == u[k]) && (u[k+t] == u[k+1]))
         value = 0;
      else if (u[k+t-1] == u[k]) 
         value = (fixed(u[k+t]) - v) / (u[k+t] - u[k+1]) * splineBlend(k+1,t-1,u,v);
      else if (u[k+t] == u[k+1])
         value = (v - u[k]) / (u[k+t-1] - u[k]) * splineBlend(k,t-1,u,v);
     else
         value = (v - u[k]) / (u[k+t-1] - u[k]) * splineBlend(k,t-1,u,v) + 
                 (fixed(u[k+t]) - v) / (u[k+t] - u[k+1]) * splineBlend(k+1,t-1,u,v);
   }
   return(value);
}

/*
   This returns the point "output" on the spline curve.
   The parameter "v" indicates the position, it ranges from 0 to n-t+2
   
*/
static void splinePoint(int *u,int n,int t,fixed v, 
	std::vector<FixedVector> &control, FixedVector &output)
{
   output[0] = 0;
   output[1] = 0;
   output[2] = 0;

   for (int k=0;k<=n;k++) {
      fixed b = splineBlend(k,t,u,v);
      output[0] += (control[k])[0] * b;
      output[1] += (control[k])[1] * b;
      output[2] += (control[k])[2] * b;
   }
}

/*
   The positions of the subintervals of v and breakpoints, the position
   on the curve are called knots. Breakpoints can be uniformly defined
   by setting u[j] = j, a more useful series of breakpoints are defined
   by the function below. This set of breakpoints localises changes to
   the vicinity of the control point being modified.
*/
static void splineKnots(int *u,int n,int t)
{
   int j;

   for (j=0;j<=n+t;j++) {
      if (j < t)
         u[j] = 0;
      else if (j <= n)
         u[j] = j - t + 1;
      else if (j > n)
         u[j] = n - t + 2;	
   }
}

/*-------------------------------------------------------------------------
   Create all the points along a spline curve
   Control points "inp", "n" of them.
   Knots "knots", degree "t".
   Ouput curve "outp", "res" of them.
*/
static void splineCurve(std::vector<FixedVector> &inp_list,
	int n,int *knots,int t, 
	std::vector<FixedVector> &outp_list,int res)
{
   int i;
   fixed interval,increment;

   interval = fixed(true, 10);
   increment = fixed(n - t + 2) / fixed(res - 1);
   for (i=0;i<=res-1;i++) {

	   FixedVector outp;
      splinePoint(knots,n,t,interval,inp_list,outp);
	  outp_list.push_back(outp);
      interval += increment;
   }
   outp_list[res-1] = inp_list[n];
}

void SplineCurve::generate(
		std::vector<FixedVector> &inPoints, 
		std::vector<FixedVector> &outPoints,
		int resolution,
		int polynomials)
{
	int N = int(inPoints.size()) - 1;
	int T = polynomials;
	int *knots = new int[N + T + 1];
	
	splineKnots(knots,N,T);
	splineCurve(inPoints,N,knots,T,outPoints,resolution);

	delete [] knots;
}
