#include "TrajectoryCalculator.h"
#include "TrajectoryParserDataStructures.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PI 3.141592653589793238462643383279502884197


#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */


#include "../../../../tools/AmMatrix/matrixCalculations.h"

int smoothTrajectoriesOfObject(struct VirtualStream * stream,unsigned int ObjID)
{
  float avg=0.0;
  unsigned int pos=0;
  for (pos=1; pos<stream->object[ObjID].numberOfFrames; pos++)
  {
    //------------------------------------------------------------------------------------
    avg = stream->object[ObjID].frame[pos-1].x + stream->object[ObjID].frame[pos].x;
    stream->object[ObjID].frame[pos-1].x = avg / 2;

    avg = stream->object[ObjID].frame[pos-1].y + stream->object[ObjID].frame[pos].y;
    stream->object[ObjID].frame[pos-1].y = avg / 2;

    avg = stream->object[ObjID].frame[pos-1].z + stream->object[ObjID].frame[pos].z;
    stream->object[ObjID].frame[pos-1].z = avg / 2;
    //------------------------------------------------------------------------------------


    //------------------------------------------------------------------------------------
    avg = stream->object[ObjID].frame[pos-1].rot1 + stream->object[ObjID].frame[pos].rot1;
    stream->object[ObjID].frame[pos-1].rot1 = avg / 2;

    avg = stream->object[ObjID].frame[pos-1].rot2 + stream->object[ObjID].frame[pos].rot2;
    stream->object[ObjID].frame[pos-1].rot2 = avg / 2;

    avg = stream->object[ObjID].frame[pos-1].rot3 + stream->object[ObjID].frame[pos].rot3;
    stream->object[ObjID].frame[pos-1].rot3 = avg / 2;

    avg = stream->object[ObjID].frame[pos-1].rot4 + stream->object[ObjID].frame[pos].rot4;
    stream->object[ObjID].frame[pos-1].rot4 = avg / 2;
    //------------------------------------------------------------------------------------
  }
 return 1;
}

int smoothTrajectories(struct VirtualStream * stream)
{
  fprintf(stderr,"Smoothing %u objects \n",stream->numberOfObjects);
  unsigned int objID=0;
  for (objID=0; objID<stream->numberOfObjects; objID++)
  {
     smoothTrajectoriesOfObject(stream,objID);
  }
 return 1;
}



float calculateDistanceTra(float from_x,float from_y,float from_z,float to_x,float to_y,float to_z)
{
   float vect_x = from_x - to_x;
   float vect_y = from_y - to_y;
   float vect_z = from_z - to_z;

   return  (sqrt(pow(vect_x, 2) + pow(vect_y, 2) + pow(vect_z, 2)));

}

void euler2QuaternionsInternal(double * quaternions,double * euler,int quaternionConvention)
{
  //This conversion follows the rule euler X Y Z  to quaternions W X Y Z
  //Our input is degrees so we convert it to radians for the sin/cos functions
  double eX = (double) (euler[0] * PI) / 180;
  double eY = (double) (euler[1] * PI) / 180;
  double eZ = (double) (euler[2] * PI) / 180;

  //fprintf(stderr,"eX %f eY %f eZ %f\n",eX,eY,eZ);

  //http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
  //eX Roll  φ - rotation about the X-axis
  //eY Pitch θ - rotation about the Y-axis
  //eZ Yaw   ψ - rotation about the Z-axis

  double cosX2 = cos((double) eX/2); //cos(φ/2);
  double sinX2 = sin((double) eX/2); //sin(φ/2);
  double cosY2 = cos((double) eY/2); //cos(θ/2);
  double sinY2 = sin((double) eY/2); //sin(θ/2);
  double cosZ2 = cos((double) eZ/2); //cos(ψ/2);
  double sinZ2 = sin((double) eZ/2); //sin(ψ/2);

  switch (quaternionConvention )
  {
   case 1 :
   /*qX*/ quaternions[0] = (sinX2 * cosY2 * cosZ2) - (cosX2 * sinY2 * sinZ2);
   /*qY*/ quaternions[1] = (cosX2 * sinY2 * cosZ2) + (sinX2 * cosY2 * sinZ2);
   /*qZ*/ quaternions[2] = (cosX2 * cosY2 * sinZ2) - (sinX2 * sinY2 * cosZ2);
   /*qW*/ quaternions[3] = (cosX2 * cosY2 * cosZ2) + (sinX2 * sinY2 * sinZ2);
   break;

   case 0 :
   /*qW*/ quaternions[0] = (cosX2 * cosY2 * cosZ2) + (sinX2 * sinY2 * sinZ2);
   /*qX*/ quaternions[1] = (sinX2 * cosY2 * cosZ2) - (cosX2 * sinY2 * sinZ2);
   /*qY*/ quaternions[2] = (cosX2 * sinY2 * cosZ2) + (sinX2 * cosY2 * sinZ2);
   /*qZ*/ quaternions[3] = (cosX2 * cosY2 * sinZ2) - (sinX2 * sinY2 * cosZ2);
   break;

   default :
    fprintf(stderr,"Unhandled quaternion order given (%u) \n",quaternionConvention);
   break;
  };

}


int affixSatteliteToPlanetFromFrameForLength(struct VirtualStream * stream,unsigned int satteliteObj,unsigned int planetObj , unsigned int frameNumber , unsigned int duration)
{
  if ( satteliteObj >= stream->numberOfObjects ) { fprintf(stderr,RED "affixSatteliteToPlanetFromFrameForLength referencing non existent Object %u\n" NORMAL,satteliteObj); return 0; }
  if ( planetObj >= stream->numberOfObjects )    { fprintf(stderr,RED "affixSatteliteToPlanetFromFrameForLength referencing non existent Object %u\n" NORMAL,planetObj);    return 0; }

  unsigned int satteliteObjFrameNumber = stream->object[satteliteObj].numberOfFrames;
  unsigned int planetObjFrameNumber    = stream->object[planetObj].numberOfFrames;

  if ( satteliteObjFrameNumber <= frameNumber )
     {
       fprintf(stderr,RED " referencing non existent frames ( %u ) \n" NORMAL,frameNumber);
       return 0;
     }
  if ( satteliteObjFrameNumber < frameNumber+duration )
     {
       fprintf(stderr,RED " referencing non existent frames ( want %u + %u frames , but max frame is %u ) \n" NORMAL,frameNumber,duration,satteliteObjFrameNumber);
       duration = satteliteObjFrameNumber-frameNumber;
       fprintf(stderr,RED " correcting duration to %u\n" NORMAL,duration);
     }

  if ( planetObjFrameNumber <= frameNumber )
     {
       fprintf(stderr,RED " referencing non existent frames ( %u ) \n" NORMAL,frameNumber);
       return 0;
     }
  if ( planetObjFrameNumber < frameNumber+duration )
     {
       fprintf(stderr,RED " referencing non existent frames ( want %u + %u frames , but max frame is %u ) \n" NORMAL,frameNumber,duration,planetObjFrameNumber);
       duration = planetObjFrameNumber-frameNumber;
       fprintf(stderr,RED " correcting duration to %u\n" NORMAL,duration);
     }

#if USE_QUATERNIONS_FOR_ORBITING
    //There is literally no good reason to go from rotation -> quaternion -> 3x3 -> quaternion -> rotation this could be optimized
    //==================================================================================
    double satPosAbsolute[4]={0};
    satPosAbsolute[0] = (double) stream->object[satteliteObj].frame[frameNumber].x;
    satPosAbsolute[1] = (double) stream->object[satteliteObj].frame[frameNumber].y;
    satPosAbsolute[2] = (double) stream->object[satteliteObj].frame[frameNumber].z;
    satPosAbsolute[3] = 1.0;

    //==================================================================================
    double planetPosAbsolute[4]={0};
    planetPosAbsolute[0] = (double) stream->object[planetObj].frame[frameNumber].x;
    planetPosAbsolute[1] = (double) stream->object[planetObj].frame[frameNumber].y;
    planetPosAbsolute[2] = (double) stream->object[planetObj].frame[frameNumber].z;
    planetPosAbsolute[3] = 1.0;


    double planetQuatAbsolute[4]={0};
    double planetRotAbsolute[4]={0};
    double planetRotAbsoluteF[4]={0};
    planetRotAbsolute[0] = (double) stream->object[planetObj].frame[frameNumber].rot1;
    planetRotAbsolute[1] = (double) stream->object[planetObj].frame[frameNumber].rot2;
    planetRotAbsolute[2] = (double) stream->object[planetObj].frame[frameNumber].rot3;
    euler2QuaternionsInternal(planetQuatAbsolute , planetRotAbsolute,1);


    double satPosRelative[4]={0};
    pointFromAbsoluteToRelationWithObject_PosXYZQuaternionXYZW(0,satPosRelative,planetPosAbsolute,planetQuatAbsolute,satPosAbsolute);

    unsigned int pos=0;
    for (pos=frameNumber+1; pos<frameNumber+duration; pos++)
    {
       planetPosAbsolute[0] = (double) stream->object[planetObj].frame[pos].x;
       planetPosAbsolute[1] = (double) stream->object[planetObj].frame[pos].y;
       planetPosAbsolute[2] = (double) stream->object[planetObj].frame[pos].z;
       planetPosAbsolute[3] = 1.0;

       planetRotAbsoluteF[0] = stream->object[planetObj].frame[pos].rot1;
       planetRotAbsoluteF[1] = stream->object[planetObj].frame[pos].rot2;
       planetRotAbsoluteF[2] = stream->object[planetObj].frame[pos].rot3;

       //Undo all the evil that has been done to our coordinate system
       if (stream->rotationsOverride)
         {
            unflipRotationAxis(
                              &planetRotAbsoluteF[0],
                              &planetRotAbsoluteF[1],
                              &planetRotAbsoluteF[2],
                              stream->rotationsXYZ[0] ,
                              stream->rotationsXYZ[1] ,
                              stream->rotationsXYZ[2]
                              );
         }

       planetRotAbsolute[0] = (double) planetRotAbsoluteF[0];
       planetRotAbsolute[1] = (double) planetRotAbsoluteF[1];
       planetRotAbsolute[2] = (double) planetRotAbsoluteF[2];

       planetRotAbsolute[0] -= stream->rotationsOffset[0];
       planetRotAbsolute[1] -= stream->rotationsOffset[1];
       planetRotAbsolute[2] -= stream->rotationsOffset[2];
       planetRotAbsolute[0] =  planetRotAbsolute[0] / stream->scaleWorld[3];
       planetRotAbsolute[1] =  planetRotAbsolute[1] / stream->scaleWorld[4];
       planetRotAbsolute[2] =  planetRotAbsolute[2] / stream->scaleWorld[5];

       euler2QuaternionsInternal(planetQuatAbsolute , planetRotAbsolute,1);

       planetQuatAbsolute[0]=(-1) * planetQuatAbsolute[0];
       //planetQuatAbsolute[3]=(-1) * planetQuatAbsolute[3];

       if ( pointFromRelationWithObjectToAbsolute_PosXYZQuaternionXYZW(satPosAbsolute,planetPosAbsolute,planetQuatAbsolute,satPosRelative) )
       {
           stream->object[satteliteObj].frame[pos].x = (float) satPosAbsolute[0];
           stream->object[satteliteObj].frame[pos].y = (float) satPosAbsolute[1];
           stream->object[satteliteObj].frame[pos].z = (float) satPosAbsolute[2];
       }
    }
 return 1;
}
#else
    //There is literally no good reason to go from rotation -> quaternion -> 3x3 -> quaternion -> rotation this could be optimized
    //==================================================================================
    double satPosAbsolute[4]={0};
    satPosAbsolute[0] = (double) stream->object[satteliteObj].frame[frameNumber].x;
    satPosAbsolute[1] = (double) stream->object[satteliteObj].frame[frameNumber].y;
    satPosAbsolute[2] = (double) stream->object[satteliteObj].frame[frameNumber].z;
    satPosAbsolute[3] = 1.0;
    //==================================================================================
    double planetPosAbsolute[4]={0};
    planetPosAbsolute[0] = (double) stream->object[planetObj].frame[frameNumber].x;
    planetPosAbsolute[1] = (double) stream->object[planetObj].frame[frameNumber].y;
    planetPosAbsolute[2] = (double) stream->object[planetObj].frame[frameNumber].z;
    planetPosAbsolute[3] = 1.0;

    double planetRotAbsolute[4]={0};
    planetRotAbsolute[0] = (double) stream->object[planetObj].frame[frameNumber].rot1;
    planetRotAbsolute[1] = (double) stream->object[planetObj].frame[frameNumber].rot2;
    planetRotAbsolute[2] = (double) stream->object[planetObj].frame[frameNumber].rot3;
    //==================================================================================


    double satPosRelative[4]={0};
    pointFromAbsoluteToRelationWithObject_PosXYZRotationXYZ(1,satPosRelative,planetPosAbsolute,planetRotAbsolute,satPosAbsolute);

    unsigned int pos=0;
    for (pos=frameNumber+1; pos<frameNumber+duration; pos++)
    {
       planetPosAbsolute[0] = (double) stream->object[planetObj].frame[pos].x;
       planetPosAbsolute[1] = (double) stream->object[planetObj].frame[pos].y;
       planetPosAbsolute[2] = (double) stream->object[planetObj].frame[pos].z;
       planetPosAbsolute[3] = 1.0;

       planetRotAbsolute[0] = (double) stream->object[planetObj].frame[pos].rot1;
       planetRotAbsolute[1] = (double) stream->object[planetObj].frame[pos].rot2;
       planetRotAbsolute[2] = (double) stream->object[planetObj].frame[pos].rot3;

       if ( pointFromRelationWithObjectToAbsolute_PosXYZRotationXYZ(satPosAbsolute,planetPosAbsolute,planetRotAbsolute,satPosRelative) )
       {
           stream->object[satteliteObj].frame[pos].x = (float) satPosAbsolute[0];
           stream->object[satteliteObj].frame[pos].y = (float) satPosAbsolute[1];
           stream->object[satteliteObj].frame[pos].z = (float) satPosAbsolute[2];
       }
    }
 return 1;

#endif // USE_QUATERNIONS_FOR_ORBITING

}



int objectsCollide(struct VirtualStream * newstream,unsigned int atTime,unsigned int objIDA,unsigned int objIDB)
{
  float posA[7]={0}; float scaleA_X,scaleA_Y,scaleA_Z;
  float posB[7]={0}; float scaleB_X,scaleB_Y,scaleB_Z;

  calculateVirtualStreamPos(newstream,objIDA,atTime,posA,&scaleA_X,&scaleA_Y,&scaleA_Z);
  calculateVirtualStreamPos(newstream,objIDB,atTime,posB,&scaleB_X,&scaleB_Y,&scaleB_Z);

  float distance =  calculateDistanceTra(posA[0],posA[1],posA[2],posB[0],posB[1],posB[2]);
  fprintf(stderr,"Distance %u from %u = %f\n",objIDA,objIDB,distance);
  if ( distance > 0.3 ) { return 0;}

  return 1;
}



int normalizeQuaternionsTJP(double *qX,double *qY,double *qZ,double *qW)
{
#if USE_FAST_NORMALIZATION
      // Works best when quat is already almost-normalized
      double f = (double) (3.0 - (((*qX) * (*qX)) + ( (*qY) * (*qY) ) + ( (*qZ) * (*qZ)) + ((*qW) * (*qW)))) / 2.0;
      *qX *= f;
      *qY *= f;
      *qZ *= f;
      *qW *= f;
#else
      double sqrtDown = (double) sqrt(((*qX) * (*qX)) + ( (*qY) * (*qY) ) + ( (*qZ) * (*qZ)) + ((*qW) * (*qW)));
      double f = (double) 1 / sqrtDown;
       *qX *= f;
       *qY *= f;
       *qZ *= f;
       *qW *= f;
#endif // USE_FAST_NORMALIZATION
  return 1;
}

void quaternions2Euler(double * euler,double * quaternions,int quaternionConvention)
{
    double qX,qY,qZ,qW;

    euler[0]=0.0; euler[1]=0.0; euler[2]=0.0;

    switch (quaternionConvention)
     {
       case 0  :
       qW = quaternions[0];
       qX = quaternions[1];
       qY = quaternions[2];
       qZ = quaternions[3];
       break;

       case 1 :
       qX = quaternions[0];
       qY = quaternions[1];
       qZ = quaternions[2];
       qW = quaternions[3];
       break;

       default :
       fprintf(stderr,"Unhandled quaternion order given (%u) \n",quaternionConvention);
       break;
     }

  //http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
  //e1 Roll  - rX: rotation about the X-axis
  //e2 Pitch - rY: rotation about the Y-axis
  //e3 Yaw   - rZ: rotation about the Z-axis

  //Shorthand to go according to http://graphics.wikia.com/wiki/Conversion_between_quaternions_and_Euler_angles
  double q0=qW , q1 = qX , q2 = qY , q3 = qZ;
  double q0q1 = (double) q0*q1 , q2q3 = (double) q2*q3;
  double q0q2 = (double) q0*q2 , q3q1 = (double) q3*q1;
  double q0q3 = (double) q0*q3 , q1q2 = (double) q1*q2;


  double eXDenominator = ( 1.0 - 2.0 * (q1*q1 + q2*q2) );
  if (eXDenominator == 0.0 ) { fprintf(stderr,"Gimbal lock detected , cannot convert to euler coordinates\n"); return; }
  double eYDenominator = ( 1.0 - 2.0 * ( q2*q2 + q3*q3) );
  if (eYDenominator == 0.0 ) { fprintf(stderr,"Gimbal lock detected , cannot convert to euler coordinates\n"); return; }


  /* arctan and arcsin have a result between −π/2 and π/2. With three rotations between −π/2 and π/2 you can't have all possible orientations.
     We need to replace the arctan by atan2 to generate all the orientations. */
  /*eX*/ euler[0] = atan2( (2.0 *  (q0q1 + q2q3)) , eXDenominator ) ;
  /*eY*/ euler[1] = asin( 2.0 * (q0q2 - q3q1));
  /*eZ*/ euler[2] = atan2( (2.0 * (q0q3 + q1q2)) ,  eYDenominator );

  //Our output is in radians so we convert it to degrees for the user

  //Go from radians back to degrees
  euler[0] = (euler[0] * 180) / PI;
  euler[1] = (euler[1] * 180) / PI;
  euler[2] = (euler[2] * 180) / PI;

}


int flipRotationAxis(float * rotX, float * rotY , float * rotZ , int where2SendX , int where2SendY , int where2SendZ)
{
  #if PRINT_LOAD_INFO
   fprintf(stderr,"Had rotX %f rotY %f rotZ %f \n",*rotX,*rotY,*rotZ);
   fprintf(stderr,"Moving 0 to %u , 1 to %u , 2 to %u \n",where2SendX,where2SendY,where2SendZ);
  #endif

  float tmpX = *rotX;
  float tmpY = *rotY;
  float tmpZ = *rotZ;
  //-----------------------------------------
  if (where2SendX==0) { *rotX=tmpX; } else
  if (where2SendX==1) { *rotY=tmpX; } else
  if (where2SendX==2) { *rotZ=tmpX; }

  if (where2SendY==0) { *rotX=tmpY; } else
  if (where2SendY==1) { *rotY=tmpY; } else
  if (where2SendY==2) { *rotZ=tmpY; }

  if (where2SendZ==0) { *rotX=tmpZ; } else
  if (where2SendZ==1) { *rotY=tmpZ; } else
  if (where2SendZ==2) { *rotZ=tmpZ; }
  //-----------------------------------------

  #if PRINT_LOAD_INFO
   fprintf(stderr,"Now have rotX %f rotY %f rotZ %f \n",*rotX,*rotY,*rotZ);
  #endif

  return 1;
}



int unflipRotationAxis(float * rotX, float * rotY , float * rotZ , int where2SendX , int where2SendY , int where2SendZ)
{
  #warning "Is   unflipRotationAxis correct ? "
  #if PRINT_LOAD_INFO
   fprintf(stderr,"Had rotX %f rotY %f rotZ %f \n",*rotX,*rotY,*rotZ);
   fprintf(stderr,"Moving 0 to %u , 1 to %u , 2 to %u \n",where2SendX,where2SendY,where2SendZ);
  #endif

  float tmpX = 0;
  float tmpY = 0;
  float tmpZ = 0;
  //-----------------------------------------
  if (where2SendX==0) { tmpX=*rotX; } else
  if (where2SendX==1) { tmpX=*rotY; } else
  if (where2SendX==2) { tmpX=*rotZ; }

  if (where2SendY==0) { tmpY=*rotX; } else
  if (where2SendY==1) { tmpY=*rotY; } else
  if (where2SendY==2) { tmpY=*rotZ; }

  if (where2SendZ==0) { tmpZ=*rotX; } else
  if (where2SendZ==1) { tmpZ=*rotY; } else
  if (where2SendZ==2) { tmpZ=*rotZ; }
  //-----------------------------------------

  *rotX=tmpX;
  *rotY=tmpY;
  *rotZ=tmpZ;

  #if PRINT_LOAD_INFO
   fprintf(stderr,"Now have rotX %f rotY %f rotZ %f \n",*rotX,*rotY,*rotZ);
  #endif

  return 1;
}








int fillPosWithNull(float * pos,float * scaleX ,float * scaleY,float * scaleZ)
{
    #if PRINT_DEBUGGING_INFO
    fprintf(stderr,"Returning null frame for obj %u \n",ObjID);
    #endif

    pos[0]=0.0;
    pos[1]=0.0;
    pos[2]=0.0;
    pos[3]=0.0;
    pos[4]=0.0;
    pos[5]=0.0;
    pos[6]=0.0;
    *scaleX = 1.0;
    *scaleY = 1.0;
    *scaleZ = 1.0;

    return 1;
}


int fillPosWithLastFrame(struct VirtualStream * stream,ObjectIDHandler ObjID,float * pos,float * scaleX,float * scaleY,float * scaleZ )
{
   if (stream==0) { fprintf(stderr,"Cannot fill position on empty stream \n"); return 0; }
   if (pos==0) { fprintf(stderr,"Cannot fill position on empty position \n"); return 0; }
   if (ObjID>=stream->numberOfObjects) { fprintf(stderr,"Trying to add position for a non existing object\n"); return 0; }

   if (stream->object[ObjID].frame==0)
    {
      #if PRINT_WARNING_INFO
       fprintf(stderr,"Cannot Access frames for object %u \n",ObjID);
      #endif
      return 0;
    }

    #if PRINT_DEBUGGING_INFO
    fprintf(stderr,"Returning frame %u \n",FrameIDToReturn);
    #endif

    unsigned int FrameIDToReturn = stream->object[ObjID].numberOfFrames;
    if (FrameIDToReturn>0) { --FrameIDToReturn; } //We have FrameIDToReturn frames so we grab the last one ( FrameIDToReturn -1 )
    pos[0]=stream->object[ObjID].frame[FrameIDToReturn].x;
    pos[1]=stream->object[ObjID].frame[FrameIDToReturn].y;
    pos[2]=stream->object[ObjID].frame[FrameIDToReturn].z;
    pos[3]=stream->object[ObjID].frame[FrameIDToReturn].rot1;
    pos[4]=stream->object[ObjID].frame[FrameIDToReturn].rot2;
    pos[5]=stream->object[ObjID].frame[FrameIDToReturn].rot3;
    pos[6]=stream->object[ObjID].frame[FrameIDToReturn].rot4;
    *scaleX=stream->object[ObjID].frame[FrameIDToReturn].scaleX;
    *scaleY=stream->object[ObjID].frame[FrameIDToReturn].scaleY;
    *scaleZ=stream->object[ObjID].frame[FrameIDToReturn].scaleZ;
    return 1;
}

/*
int fillPosWithLastFrameD(struct VirtualStream * stream,ObjectIDHandler ObjID,double * pos,double * scale )
{
   if (stream->object[ObjID].frame==0)
    {
      #if PRINT_WARNING_INFO
      fprintf(stderr,"Cannot Access frames for object %u \n",ObjID);
      #endif
      return 0;
    }

    #if PRINT_DEBUGGING_INFO
    fprintf(stderr,"Returning frame %u \n",FrameIDToReturn);
    #endif

    unsigned int FrameIDToReturn = stream->object[ObjID].numberOfFrames;
    if (FrameIDToReturn>0) { --FrameIDToReturn; } //We have FrameIDToReturn frames so we grab the last one ( FrameIDToReturn -1 )
    pos[0]=(double) stream->object[ObjID].frame[FrameIDToReturn].x;
    pos[1]=(double) stream->object[ObjID].frame[FrameIDToReturn].y;
    pos[2]=(double) stream->object[ObjID].frame[FrameIDToReturn].z;
    pos[3]=(double) stream->object[ObjID].frame[FrameIDToReturn].rot1;
    pos[4]=(double) stream->object[ObjID].frame[FrameIDToReturn].rot2;
    pos[5]=(double) stream->object[ObjID].frame[FrameIDToReturn].rot3;
    pos[6]=(double) stream->object[ObjID].frame[FrameIDToReturn].rot4;
    *scale=stream->object[ObjID].frame[FrameIDToReturn].scale;
    return 1;
}*/


int fillPosWithFrame(struct VirtualStream * stream,ObjectIDHandler ObjID,unsigned int FrameIDToReturn,float * pos,float * scaleX,float * scaleY,float * scaleZ)
{
   if (stream->object[ObjID].frame==0)
    {
      #if PRINT_WARNING_INFO
      fprintf(stderr,"Cannot Access frames for object %u \n",ObjID);
      #endif
      return 0;
    }

    #if PRINT_DEBUGGING_INFO
    fprintf(stderr,"Returning frame %u \n",FrameIDToReturn);
    #endif

    if (FrameIDToReturn >= stream->object[ObjID].MAX_numberOfFrames )
     {
         fprintf(stderr,"fillPosWithFrame asked to return frame out of bounds ( %u / %u / %u Max ) \n",FrameIDToReturn,stream->object[ObjID].numberOfFrames,stream->object[ObjID].MAX_numberOfFrames);
         return 0;
     }

    pos[0]=stream->object[ObjID].frame[FrameIDToReturn].x;
    pos[1]=stream->object[ObjID].frame[FrameIDToReturn].y;
    pos[2]=stream->object[ObjID].frame[FrameIDToReturn].z;
    pos[3]=stream->object[ObjID].frame[FrameIDToReturn].rot1;
    pos[4]=stream->object[ObjID].frame[FrameIDToReturn].rot2;
    pos[5]=stream->object[ObjID].frame[FrameIDToReturn].rot3;
    pos[6]=stream->object[ObjID].frame[FrameIDToReturn].rot4;
    *scaleX=stream->object[ObjID].frame[FrameIDToReturn].scaleX;
    *scaleY=stream->object[ObjID].frame[FrameIDToReturn].scaleY;
    *scaleZ=stream->object[ObjID].frame[FrameIDToReturn].scaleZ;
    return 1;
}


int fillPosWithInterpolatedFrame(struct VirtualStream * stream,ObjectIDHandler ObjID,float * pos,float * scaleX,float * scaleY,float * scaleZ,
                                 unsigned int PrevFrame,unsigned int NextFrame , unsigned int time )
{
   if (stream->object[ObjID].frame==0)
    {
      #if PRINT_WARNING_INFO
      fprintf(stderr,"Cannot Access interpolated frames for object %u \n",ObjID);
      #endif
      return 0;
    }

   if (PrevFrame==NextFrame)
    {
       return fillPosWithFrame(stream,ObjID,PrevFrame,pos,scaleX,scaleY,scaleZ);
    }


    #if PRINT_DEBUGGING_INFO
    fprintf(stderr,"Interpolating frames @  %u , between %u and %u \n",time,PrevFrame,NextFrame);
    #endif
    float interPos[7]={0};
    float interScale;

    unsigned int MAX_stepTime= stream->object[ObjID].frame[NextFrame].time - stream->object[ObjID].frame[PrevFrame].time;
    if (MAX_stepTime == 0 ) { MAX_stepTime=1; }
    unsigned int our_stepTime= time - stream->object[ObjID].frame[PrevFrame].time;


    interPos[0]=(float) ( stream->object[ObjID].frame[NextFrame].x-stream->object[ObjID].frame[PrevFrame].x ) * our_stepTime / MAX_stepTime;
    interPos[0]+=stream->object[ObjID].frame[PrevFrame].x;

    interPos[1]=(float) ( stream->object[ObjID].frame[NextFrame].y-stream->object[ObjID].frame[PrevFrame].y ) * our_stepTime / MAX_stepTime;
    interPos[1]+=stream->object[ObjID].frame[PrevFrame].y;

    interPos[2]=(float) ( stream->object[ObjID].frame[NextFrame].z-stream->object[ObjID].frame[PrevFrame].z ) * our_stepTime / MAX_stepTime;
    interPos[2]+=stream->object[ObjID].frame[PrevFrame].z;

    interPos[3]=(float) ( stream->object[ObjID].frame[NextFrame].rot1-stream->object[ObjID].frame[PrevFrame].rot1 ) * our_stepTime / MAX_stepTime;
    interPos[3]+=stream->object[ObjID].frame[PrevFrame].rot1;

    interPos[4]=(float) ( stream->object[ObjID].frame[NextFrame].rot2-stream->object[ObjID].frame[PrevFrame].rot2 ) * our_stepTime / MAX_stepTime;
    interPos[4]+=stream->object[ObjID].frame[PrevFrame].rot2;

    interPos[5]=(float) ( stream->object[ObjID].frame[NextFrame].rot3-stream->object[ObjID].frame[PrevFrame].rot3 ) * our_stepTime / MAX_stepTime;
    interPos[5]+=stream->object[ObjID].frame[PrevFrame].rot3;

    interPos[6]=(float) ( stream->object[ObjID].frame[NextFrame].rot4-stream->object[ObjID].frame[PrevFrame].rot4 ) * our_stepTime / MAX_stepTime;
    interPos[6]+=stream->object[ObjID].frame[PrevFrame].rot4;

    interScale = (float) ( stream->object[ObjID].frame[NextFrame].scaleX -stream->object[ObjID].frame[PrevFrame].scaleX ) * our_stepTime / MAX_stepTime;
    interScale += stream->object[ObjID].frame[PrevFrame].scaleX;
    *scaleX=interScale;

    interScale = (float) ( stream->object[ObjID].frame[NextFrame].scaleY -stream->object[ObjID].frame[PrevFrame].scaleY ) * our_stepTime / MAX_stepTime;
    interScale += stream->object[ObjID].frame[PrevFrame].scaleY;
    *scaleY=interScale;

    interScale = (float) ( stream->object[ObjID].frame[NextFrame].scaleZ -stream->object[ObjID].frame[PrevFrame].scaleZ ) * our_stepTime / MAX_stepTime;
    interScale += stream->object[ObjID].frame[PrevFrame].scaleZ;
    *scaleZ=interScale;


    pos[0]=interPos[0]; pos[1]=interPos[1]; pos[2]=interPos[2];
    pos[3]=interPos[3]; pos[4]=interPos[4]; pos[5]=interPos[5];
    pos[6]=interPos[6];

    #if PRINT_DEBUGGING_INFO
    fprintf(stderr,"ok \n");
    #endif

    return 1;
}






int calculateVirtualStreamPos(struct VirtualStream * stream,ObjectIDHandler ObjID,unsigned int timeAbsMilliseconds,float * pos,float * scaleX,float * scaleY,float * scaleZ)
{
   if (stream==0) { fprintf(stderr,"calculateVirtualStreamPos called with null stream\n"); return 0; }
   if (stream->object==0) { fprintf(stderr,"calculateVirtualStreamPos called with null object array\n"); return 0; }
   if (stream->numberOfObjects<=ObjID) { fprintf(stderr,"calculateVirtualStreamPos ObjID %u is out of bounds (%u)\n",ObjID,stream->numberOfObjects); return 0; }
   if (stream->object[ObjID].frame == 0 )  { fprintf(stderr,"calculateVirtualStreamPos ObjID %u does not have a frame array allocated\n",ObjID); return 0; }
   if (stream->object[ObjID].numberOfFrames == 0 ) { fprintf(stderr,"calculateVirtualStreamPos ObjID %u has 0 frames\n",ObjID); return 0; }


   if (stream->autoRefresh != 0 )
    {
         //Check for refreshed version ?
       if (stream->autoRefresh < timeAbsMilliseconds-stream->lastRefresh )
          {
            unsigned long current_size = getFileSize(stream->filename);
            if (current_size != stream->fileSize)
             {
              refreshVirtualStream(stream);
              stream->lastRefresh = timeAbsMilliseconds;
             }
          }
    }

   unsigned int FrameIDToReturn = 0;
   unsigned int FrameIDLast = 0;
   unsigned int FrameIDNext = 0;


   /*!OK , Two major cases here..! The one is a simple Next frame getter , the second is a more complicated interpolated frame getter..! */
   if ( (stream->object[ObjID].MAX_numberOfFrames == 0 ) )
   {
       fprintf(stderr,"Returning Null position for ObjID %u\n",ObjID);
       fillPosWithNull(/*stream,ObjID,*/pos,scaleX,scaleY,scaleZ);
       return 1;
   } else
   if  ( (stream->ignoreTime) || (stream->object[ObjID].MAX_numberOfFrames == 1 ) )
   {
    //We might want to ignore time and just return frame after frame on each call!
    //Also if we only got one frame for the object there is no point in trying to interpolate time etc.. so just handle things here..
    if ( stream->object[ObjID].lastFrame +1 >= stream->object[ObjID].MAX_numberOfFrames ) { stream->object[ObjID].lastFrame  = 0; }
    FrameIDToReturn = stream->object[ObjID].lastFrame;
    ++stream->object[ObjID].lastFrame;

    fillPosWithFrame(stream,ObjID,FrameIDToReturn,pos,scaleX,scaleY,scaleZ);
    fprintf(stderr,"fillPosWithFrame %u => ( %0.2f %0.2f %0.2f , %0.2f %0.2f %0.2f)\n",FrameIDToReturn,pos[0],pos[1],pos[2],pos[3],pos[4],pos[5]);

    FrameIDLast = FrameIDToReturn;
    FrameIDNext = FrameIDToReturn+1;

    if ( FrameIDNext >= stream->object[ObjID].numberOfFrames )
     { //We 've reached the end of the stream so the last frame should truncate 0
       stream->object[ObjID].lastFrame=0;
     }

     return 1;

   } /*!END OF SIMPLE FRAME GETTER*/
   else
   { /*!START OF INTERPOLATED FRAME GETTER*/
     //fprintf(stderr,"interpolated position for ObjID %u\n",ObjID);
     //This is the case when we respect time , we will pick two frames and interpolate between them
     if ( timeAbsMilliseconds > stream->object[ObjID].MAX_timeOfFrames )
     {
       //This means we have passed the last frame.. so lets find out where we really are..
       if (stream->object[ObjID].MAX_timeOfFrames == 0 ) {
                                                           //If max time of frames is 0 then our time is also zero ( since it never goes over max )
                                                           //fprintf(stderr,"timeAbsMilliseconds can not be something more than zero");
                                                           timeAbsMilliseconds=0;
                                                         } else
                                                         { timeAbsMilliseconds = timeAbsMilliseconds % stream->object[ObjID].MAX_timeOfFrames; }
       //timeAbsMilliseconds should contain a valid value now somewhere from 0->MAX_timeOfFrames
     }

     #if PRINT_DEBUGGING_INFO
     fprintf(stderr,"Object %u has %u frames , lets search where we are \n",ObjID,stream->object[ObjID].numberOfFrames);
     #endif

     //We scan all the frames to find out the "last one" and the "next one"
     unsigned int i =0;
     for ( i=0; i <stream->object[ObjID].MAX_numberOfFrames-1; i++ )
      {
       if (( stream->object[ObjID].frame[i].time <= timeAbsMilliseconds )
                 &&
           ( timeAbsMilliseconds <= stream->object[ObjID].frame[i+1].time )  )
            {
               //This is the "next" frame!
               FrameIDLast = i;
               FrameIDNext = i+1;
               //This should be handled by raw response to zero elemetn :P
              break;
            }
      }

    //We now have our Last and Next frame , all that remains is extracting the
    //interpolated time between them..!
    return fillPosWithInterpolatedFrame(stream,ObjID,pos,scaleX,scaleY,scaleZ,FrameIDLast,FrameIDNext,timeAbsMilliseconds);

   } /*!END OF INTERPOLATED FRAME GETTER*/

    return 0;
}



int calculateVirtualStreamPosAfterTime(struct VirtualStream * stream,ObjectIDHandler ObjID,unsigned int timeAfterMilliseconds,float * pos,float * scaleX,float * scaleY, float * scaleZ)
{
   stream->object[ObjID].lastCalculationTime+=timeAfterMilliseconds;
   return calculateVirtualStreamPos(stream,ObjID,stream->object[ObjID].lastCalculationTime,pos,scaleX,scaleY,scaleZ);
}


int getVirtualStreamLastPosF(struct VirtualStream * stream,ObjectIDHandler ObjID,float * pos,float * scaleX,float * scaleY,float * scaleZ)
{
    return fillPosWithLastFrame(stream,ObjID,pos,scaleX,scaleY,scaleZ);
}



