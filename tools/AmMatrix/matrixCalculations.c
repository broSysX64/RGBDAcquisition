#include "matrixCalculations.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "quaternions.h"
#include "matrixTools.h"
#include "matrix3x3Tools.h"
#include "matrix4x4Tools.h"
#include "solveLinearSystemGJ.h"
#include "solveHomography.h"

enum mat3x3MItem
{
    m0 = 0 , m1  , m2  ,
    m3     , m4  , m5  ,
    m6     , m7 ,  m8
};


enum mat4x4EItem
{
    e0 = 0 , e1  , e2  , e3 ,
    e4     , e5  , e6  , e7 ,
    e8     , e9  , e10 , e11 ,
    e12    , e13 , e14 , e15
};



//-----------------------------------------------------------------------------
// Name: CylTest_CapsFirst
// Orig: Greg James - gjames@NVIDIA.com
// Lisc: Free code - no warranty & no money back.  Use it all you want
// Desc:
//    This function tests if the 3D point 'testpt' lies within an arbitrarily
// oriented cylinder.  The cylinder is defined by an axis from 'pt1' to 'pt2',
// the axis having a length squared of 'lengthsq' (pre-compute for each cylinder
// to avoid repeated work!), and radius squared of 'radius_sq'.
//    The function tests against the end caps first, which is cheap -> only
// a single dot product to test against the parallel cylinder caps.  If the
// point is within these, more work is done to find the distance of the point
// from the cylinder axis.
//    Fancy Math (TM) makes the whole test possible with only two dot-products
// a subtract, and two multiplies.  For clarity, the 2nd mult is kept as a
// divide.  It might be faster to change this to a mult by also passing in
// 1/lengthsq and using that instead.
//    Elminiate the first 3 subtracts by specifying the cylinder as a base
// point on one end cap and a vector to the other end cap (pass in {dx,dy,dz}
// instead of 'pt2' ).
//
//    The dot product is constant along a plane perpendicular to a vector.
//    The magnitude of the cross product divided by one vector length is
// constant along a cylinder surface defined by the other vector as axis.
//
// Return:  -1.0 if point is outside the cylinder
// Return:  distance squared from cylinder axis if point is inside.
//
//-----------------------------------------------------------------------------

float pointIsInsideCylinder( float * pt1, float * pt2, float lengthsq, float radius_sq, float * testpt )
{
	float dx, dy, dz;	// vector d  from line segment point 1 to point 2
	float pdx, pdy, pdz;	// vector pd from point 1 to test point
	float dot;

	dx = pt2[0] - pt1[0];	// translate so pt1 is origin.  Make vector from
	dy = pt2[1] - pt1[1];     // pt1 to pt2.  Need for this is easily eliminated
	dz = pt2[2] - pt1[2];

	pdx = testpt[0] - pt1[0];		// vector from pt1 to test point.
	pdy = testpt[1] - pt1[1];
	pdz = testpt[2] - pt1[2];

	// Dot the d and pd vectors to see if point lies behind the
	// cylinder cap at pt1.x, pt1.y, pt1.z

	dot = (pdx * dx) + (pdy * dy) + (pdz * dz);

	// If dot is less than zero the point is behind the pt1 cap.
	// If greater than the cylinder axis line segment length squared
	// then the point is outside the other end cap at pt2.

	if( dot < 0.0f || dot > lengthsq )
	{
		return( -1.0f );
	}
	else
	{
		// Point lies within the parallel caps, so find
		// distance squared from point to line, using the fact that sin^2 + cos^2 = 1
		// the dot = cos() * |d||pd|, and cross*cross = sin^2 * |d|^2 * |pd|^2
		// Carefull: '*' means mult for scalars and dotproduct for vectors
		// In short, where dist is pt distance to cyl axis:
		// dist = sin( pd to d ) * |pd|
		// distsq = dsq = (1 - cos^2( pd to d)) * |pd|^2
		// dsq = ( 1 - (pd * d)^2 / (|pd|^2 * |d|^2) ) * |pd|^2
		// dsq = pd * pd - dot * dot / lengthsq
		//  where lengthsq is d*d or |d|^2 that is passed into this function

		// distance squared to the cylinder axis:

		float dsq;
		dsq = (pdx*pdx + pdy*pdy + pdz*pdz) - dot*dot/lengthsq;

		if( dsq > radius_sq )
		{
			return( -1.0f );
		}
		else
		{
			return( dsq );		// return distance squared to axis
		}
	}
}




int slerp2RotTransMatrices4x4(double * result4, double * a4, double * b4 , float step )
{
  if ( (result4==0) || (a4==0) || (b4==0) ) { return 0; }

  //Enforce some limits on wrong values
  if (step>1.0) { step=1.0; } else
  if (step<0.0) { step=0.0; }

  int conventionToUseInternally = 0;

  double qA[4];
  matrix4x42Quaternion(qA, conventionToUseInternally , a4);

  double qB[4];
  matrix4x42Quaternion(qB, conventionToUseInternally , b4);

  double qOut[4];
  quaternionSlerp( qOut, qA , qB, step);
  quaternion2Matrix4x4(result4 , qOut , conventionToUseInternally );

  result4[3]  = (b4[3]-a4[3]) * step;
  result4[7]  = (b4[7]-a4[7]) * step;
  result4[11] = (b4[11]-a4[11]) * step;
 return 1;
}




int slerp2RotTransMatrices4x4F(float * result4, float * a4, float * b4 , float step )
{
 double a4D[16],b4D[16],rD[16];


 copy4x4FMatrixToD(a4D,a4);
 copy4x4FMatrixToD(b4D,b4);

   slerp2RotTransMatrices4x4( rD, a4D, b4D , step );

 copy4x4DMatrixToF(result4, rD);
 return 1;
}




int pointInsideBoundingBox3D(
                             float bboxX1,float bboxY1,float bboxZ1,
                             float bboxX2,float bboxY2,float bboxZ2,
                             float X , float Y , float Z
                             )
{
   return (
                   (bboxX1<X)&& (bboxX2>X) &&
                   (bboxY1<Y)&& (bboxY2>Y) &&
                   (bboxZ1<Z)&& (bboxZ2>Z)
          );
}



/*
  TAKEN FROM http://www.lighthouse3d.com/opengl/maths/index.php?raytriint
*/
int rayIntersectsTriangle(float *p, float *d,float *v0, float *v1, float *v2)
{
	float e1[3],e2[3],h[3],s[3],q[3];
	float a,f,u,v;

	vector(e1,v1,v0);
	vector(e2,v2,v0);
	crossProduct(h,d,e2);
	a = innerProduct(e1,h);

	if (a > -0.00001 && a < 0.00001)
		return(0);

	f = 1/a;
	vector(s,p,v0);
	u = f * (innerProduct(s,h));

	if (u < 0.0 || u > 1.0)
		return(0);

	crossProduct(q,s,e1);
	v = f * innerProduct(d,q);
	if (v < 0.0 || u + v > 1.0)
		return(0);
	// at this stage we can compute t to find out where
	// the intersection point is on the line
	float t = f * innerProduct(e2,q);
	if (t > 0.00001) // ray intersection
		return(1);
	else // this means that there is a line intersection
		 // but not a ray intersection
		 return (0);
}



int pointInsidePyramid(float *pyramidTop,float * pyramidBase,float X,float Y, float Z)
{
 fprintf(stderr,"Point inside pyramid is a stub..!\n");
 return 0;

}




int rayIntersectsRectangle(float *p, float *d,float *v0, float *v1, float *v2, float *v3)
{
   if (  rayIntersectsTriangle(p,d,v0,v1,v2) )
     {
       return 1;
     }

   if (  rayIntersectsTriangle(p,d,v1,v2,v3) )
     {
       return 1;
     }

   return 0;
}

/*
//http://ilab.usc.edu/wiki/index.php/Fast_Square_Root
static inline float sqrt_fast_approximation(const float x)
{
  union
  {
    int i;
    float x;
  } u;

  u.x = x;
  u.i = (1<<29) + (u.i >> 1) - (1<<22);
  return u.x;
}
*/



double distanceBetween3DPoints(double * p1, double * p2)
{
  double x1 = p1[0] , y1 = p1[1] , z1 = p1[2];
  double x2 = p2[0] , y2 = p2[1] , z2 = p2[2];

  double dx=x1-x2;
  double dy=y1-y2;
  double dz=z1-z2;

  //I Could actually skip this
  //if (x1>=x2) { dx=x1-x2; } else { dx=x2-x1; }
  //if (y1>=y2) { dy=y1-y2; } else { dy=y2-y1; }
  //if (z1>=z2) { dz=z1-z2; } else { dz=z2-z1; }
  //==========================

  return (double) sqrt( (dx * dx) + (dy * dy) + (dz * dz) );
}


float distanceBetween3DPointsFast(float *x1,float*y1,float *z1,float *x2,float*y2,float *z2)
{
    //sqrt_fast_approximation
  float dx,dy,dz;

  if (*x1>=*x2) { dx=*x1-*x2; } else { dx=*x2-*x1; }
  if (*y1>=*y2) { dy=*y1-*y2; } else { dy=*y2-*y1; }
  if (*z1>=*z2) { dz=*z1-*z2; } else { dz=*z2-*z1; }

  return (float) sqrt_fast_approximation( (dx * dx) + (dy * dy) + (dz * dz) );
}

float squaredDistanceBetween3DPoints(float *x1,float*y1,float *z1,float *x2,float*y2,float *z2)
{
  float dx,dy,dz;

  if (*x1>=*x2) { dx=*x1-*x2; } else { dx=*x2-*x1; }
  if (*y1>=*y2) { dy=*y1-*y2; } else { dy=*y2-*y1; }
  if (*z1>=*z2) { dz=*z1-*z2; } else { dz=*z2-*z1; }

  return (float)  (dx * dx) + (dy * dy) + (dz * dz) ;
}







int projectPointsFrom3Dto2D(double * x2D, double * y2D , double * x3D, double *y3D , double * z3D , double * intrinsics , double * rotation3x3 , double * translation)
{
  double fx = intrinsics[0];
  double fy = intrinsics[4];
  double cx = intrinsics[2];
  double cy = intrinsics[5];

  double * t = translation;
  double * r = rotation3x3;

  //Result
  //fx * t0 + cx * t2 + (x3D) * ( fx * r0 + cx * r6 )  + (y3D) * ( fx * r1 + cx * r7 ) + (z3D) * (fx * r2 +cx * r8) / t3 + r7 x3D + r8 * y3D + r9 * z3D
  //fy * t1 + cy * t2 + x3D * ( fy * r3 + cy * r6 )  + y3D * ( fy * r4 + cy * r7 ) + z3D * (fy * r5 +cy * r8) / t3 + r7 x3D + r8 * y3D + r9 * z3D
  //1

  double x2DBuf =  fx * t[0] + cx * t[2] + (*x3D) * ( fx * r[0] + cx * r[6] )  + (*y3D) * ( fx * r[1] + cx * r[7] ) + (*z3D) * (fx * r[2] +cx * r[8]);
  double y2DBuf =  fy * t[1] + cy * t[2] + (*x3D) * ( fy * r[3] + cy * r[6] )  + (*y3D) * ( fy * r[4] + cy * r[7] ) + (*z3D) * (fy * r[5] +cy * r[8]);
  double scale =   t[2] + r[6] * (*x3D) + r[7] * (*y3D) + r[8] * (*z3D);

  if ( scale == 0.0 ) { fprintf(stderr,"could not projectPointsFrom3Dto2D"); return 0; }
  *x2D = x2DBuf / scale;
  *y2D = y2DBuf / scale;

 return 1;
}



int move3DPoint(double * resultPoint3D, double * transformation4x4, double * point3D  )
{
  return transform3DPointVectorUsing4x4Matrix(resultPoint3D,transformation4x4,point3D);
}





float calculateDistance(float from_x,float from_y,float from_z,float to_x,float to_y,float to_z)
{
   float vect_x = from_x - to_x;
   float vect_y = from_y - to_y;
   float vect_z = from_z - to_z;

   return  (sqrt(pow(vect_x, 2) + pow(vect_y, 2) + pow(vect_z, 2)));

}


void vectorDirection(float src_x,float src_y,float src_z,float targ_x,float targ_y,float targ_z,float *vect_x,float *vect_y,float *vect_z)
{
    *vect_x = src_x - targ_x;
    *vect_y = src_y - targ_y;
    *vect_z = src_z - targ_z;

    float len = (sqrt(pow(*vect_x, 2) + pow(*vect_y, 2) + pow(*vect_z, 2)));
    if(len == 0) len = 1.0f;

    *vect_x /= len ;
    *vect_y /= len ;
    *vect_z /= len ;
}



void findNormal(float *outX, float *outY, float *outZ,float v1x, float v1y, float v1z, float v2x, float v2y, float v2z, float v3x, float v3y, float v3z )
{
 const int x = 0;
 const int y = 1;
 const int z = 2;

 float temp_v1[4];
 float temp_v2[4];
 float temp_length;
 float CNormal[4];

 temp_v1[x] = v1x - v2x;
 temp_v1[y] = v1y - v2y;
 temp_v1[z] = v1z - v2z;

 temp_v2[x] = v2x - v3x;
 temp_v2[y] = v2y - v3y;
 temp_v2[z] = v2z - v3z;

 // calculate cross product
 CNormal[x] = temp_v1[y]*temp_v2[z] - temp_v1[z]*temp_v2[y];
 CNormal[y] = temp_v1[z]*temp_v2[x] - temp_v1[x]*temp_v2[z];
 CNormal[z] = temp_v1[x]*temp_v2[y] - temp_v1[y]*temp_v2[x];

 // normalize normal
 temp_length =(CNormal[x]*CNormal[x])+ (CNormal[y]*CNormal[y])+ (CNormal[z]*CNormal[z]);

 temp_length = sqrt(temp_length);

 // prevent n/0
 if (temp_length == 0) { temp_length = 1;}

 CNormal[x] /= temp_length;
 CNormal[y] /= temp_length;
 CNormal[z] /= temp_length;


 *outX=CNormal[x];
 *outY=CNormal[y];
 *outZ=CNormal[z];
}





int pointFromRelationWithObjectToAbsolute(double * absoluteOutPoint3DRotated, double * objectPosition , double * objectRotation3x3 ,  double * relativeInPoint3DUnrotated)
{
  //  What we want to do ( in mathematica )
  // (  { {r0,r1,r2,0} , {r3,r4,r5,0} , {r6,r7,r8,0} , {0,0,0,1} } * { { X }  , { Y }  , { Z } , { 1.0 } } ) + { {ObjX} , {ObjY} , {ObjZ} , { 0 }  }

  //We have a coordinate space in Relation to our object so we want to first rotate our point and then translate it
  //back to absolute coordinate space

  double objectRotation4x4[4*4]={0};
  //We make the 3x3 matrix onto a 4x4 by adding zeros and 1 as the diagonal element
  upscale3x3to4x4(objectRotation4x4,objectRotation3x3);

  objectRotation4x4[e3]=objectPosition[0];
  objectRotation4x4[e7]=objectPosition[1];
  objectRotation4x4[e11]=objectPosition[2];
  objectRotation4x4[e15]=1.0;

  transform3DPointVectorUsing4x4Matrix(absoluteOutPoint3DRotated,objectRotation4x4,relativeInPoint3DUnrotated);

  //Normalization is done automatically
  normalize3DPointVector(absoluteOutPoint3DRotated);

  return 1;
}


/*
    We have an object with an absolute Position X,Y,Z (objectPosition[]) and Rotation (objectRotation3x3[])
    We also have an absolute position of a 3D point , and we want to calculate the relative position
    of the 3D point in relation to the object ( unrotated relative position )
*/
int pointFromAbsoluteToInRelationWithObject(double * relativeOutPoint3DUnrotated, double * objectPosition , double * objectRotation3x3 , double * absoluteInPoint3DRotated )
{
  //printf("pointFromAbsoluteToInRelationWithObject Using Inversion Code\n");
  double objectRotation4x4[4*4]={0};
  //We make the 3x3 matrix onto a 4x4 by adding zeros and 1 as the diagonal element
  upscale3x3to4x4(objectRotation4x4,objectRotation3x3);

  objectRotation4x4[e3]=objectPosition[0];
  objectRotation4x4[e7]=objectPosition[1];
  objectRotation4x4[e11]=objectPosition[2];
  objectRotation4x4[e15]=1.0;


  double objectInvRotation4x4[4*4]={0};
  invert4x4MatrixD(objectInvRotation4x4,objectRotation4x4);

  transform3DPointVectorUsing4x4Matrix(relativeOutPoint3DUnrotated,objectInvRotation4x4,absoluteInPoint3DRotated);
  return 1;
}







/*
    We have an object with an absolute Position X,Y,Z (objectPosition[]) and Rotation (objectRotation3x3[])
    We also have an absolute position of a 3D point , and we want to calculate the relative position
    of the 3D point in relation to the object ( unrotated relative position )
*/
int pointFromAbsoluteToRelationWithObject_PosXYZRotationXYZ(double * relativeOutPoint3DUnrotated, double * objectPosition , double * objectRotation , double * absoluteInPoint3DRotated )
{
    double objectRotation3x3[9];
    create3x3EulerRotationXYZOrthonormalMatrix(objectRotation3x3,objectRotation);

     pointFromAbsoluteToInRelationWithObject(relativeOutPoint3DUnrotated,objectPosition,objectRotation3x3,absoluteInPoint3DRotated);

    //We have to try to normalize the output point , although it should already be normalized..
    normalize3DPointVector(relativeOutPoint3DUnrotated);

    return 1;
}




/*
    We have an object with an absolute Position X,Y,Z (objectPosition[]) and Rotation (objectRotation3x3[])
    We also have an absolute position of a 3D point , and we want to calculate the relative position
    of the 3D point in relation to the object ( unrotated relative position )
*/
int pointFromAbsoluteToRelationWithObject_PosXYZQuaternionXYZW(double * relativeOutPoint3DUnrotated, double * objectPosition , double * objectQuaternion , double * absoluteInPoint3DRotated )
{
    double objectRotation3x3[9];

    //printf("Object Position is %f,%f,%f  \n", objectPosition[0], objectPosition[1], objectPosition[2] );
    //printf("Quaternion %f,%f,%f,%f \n",objectQuaternion[0],objectQuaternion[1],objectQuaternion[2],objectQuaternion[3]);
    normalizeQuaternions(&objectQuaternion[0],&objectQuaternion[1],&objectQuaternion[2],&objectQuaternion[3]);
    //printf("Normalized Quaternion %f,%f,%f,%f \n",objectQuaternion[0],objectQuaternion[1],objectQuaternion[2],objectQuaternion[3]);

    quaternion2Matrix3x3(objectRotation3x3,objectQuaternion,qXqYqZqW);

    //print3x3DMatrix("Quaternion to 3x3",objectRotation3x3);
    //print3x3DMathematicaMatrix("Quat3x3",objectRotation3x3);

    pointFromAbsoluteToInRelationWithObject(relativeOutPoint3DUnrotated,objectPosition,objectRotation3x3,absoluteInPoint3DRotated);

    //We have to try to normalize the output point , although it should already be normalized..
    normalize3DPointVector(relativeOutPoint3DUnrotated);

    return 1;
}


/*
    We have an object with a relative Position X,Y,Z to an Object (objectPosition[])
*/
int pointFromRelationWithObjectToAbsolute_PosXYZRotationXYZ(double * absoluteOutPoint3DRotated , double * objectPosition , double * objectRotation ,double * relativeInPoint3DUnrotated)
{
    double objectRotation3x3[9]={0};
    create3x3EulerRotationXYZOrthonormalMatrix(objectRotation3x3,objectRotation);
    pointFromRelationWithObjectToAbsolute(absoluteOutPoint3DRotated,objectPosition,objectRotation3x3,relativeInPoint3DUnrotated);

    //We have to try to normalize the output point , although it should already be normalized..
    normalize3DPointVector(absoluteOutPoint3DRotated);

    return 1;
}



/*
    We have an object with a relative Position X,Y,Z to an Object (objectPosition[])
*/
int pointFromRelationWithObjectToAbsolute_PosXYZQuaternionXYZW(double * absoluteOutPoint3DRotated , double * objectPosition , double * objectQuaternion ,double * relativeInPoint3DUnrotated)
{
    double objectRotation3x3[9];

    //printf("Object Position is %f,%f,%f  \n", objectPosition[0], objectPosition[1], objectPosition[2] );
    //printf("Quaternion %f,%f,%f,%f \n",objectQuaternion[0],objectQuaternion[1],objectQuaternion[2],objectQuaternion[3]);
    normalizeQuaternions(&objectQuaternion[0],&objectQuaternion[1],&objectQuaternion[2],&objectQuaternion[3]);
    //printf("Normalized Quaternion %f,%f,%f,%f \n",objectQuaternion[0],objectQuaternion[1],objectQuaternion[2],objectQuaternion[3]);

    quaternion2Matrix3x3(objectRotation3x3,objectQuaternion,qXqYqZqW);

    //print3x3DMatrix("Quaternion to 3x3",objectRotation3x3);
    //print3x3DMathematicaMatrix("Quat3x3",objectRotation3x3);

    pointFromRelationWithObjectToAbsolute(absoluteOutPoint3DRotated,objectPosition,objectRotation3x3,relativeInPoint3DUnrotated);

    //We have to try to normalize the output point , although it should already be normalized..
    normalize3DPointVector(absoluteOutPoint3DRotated);

    return 1;
}


void testMatrices()
{
   //testHomographySolver();
   testGJSolver();
  return ;


  double A[16]={ 1 ,2 ,3 ,4,
                 5 ,6 ,7 ,8,
                 9 ,10,11,12,
                 13,14,15,16 };


  double B[16]={ 1 ,2 ,3 ,4,
                 4 ,3 ,2 ,1,
                 1 ,2 ,3 ,4,
                 4 ,3 ,2 ,1 };

  double Res[16]={0};

  multiplyTwo4x4Matrices(Res,A,B);
/*
  28.000000 26.000000 24.000000 22.000000
  68.000000 66.000000 64.000000 62.000000
  108.000000 106.000000 104.000000 102.000000
  148.000000 146.000000 144.000000 142.000000*/

}
