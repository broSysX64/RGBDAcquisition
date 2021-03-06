#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bvh_cut_paste.h"


int scanJointHierarchyUntilThisGroupEnds(
                                         struct BVH_MotionCapture * mc,
                                         BVHJointID jID,
                                         BVHJointID * jIDLastGroupJoint,
                                         unsigned int * numberOfChannelsContained
                                        )
{
  fprintf(stderr,"scanJointHierarchyUntilThisGroupEnds(%s): contains ",mc->jointHierarchy[jID].jointName);

  *jIDLastGroupJoint=jID;
  unsigned int targetHierarchyLevel=mc->jointHierarchy[jID].hierarchyLevel;
  *numberOfChannelsContained = mc->jointHierarchy[jID].loadedChannels;

  ++jID;//Start from the next joint..
  while (jID<mc->numberOfFrames)
  {
    if (targetHierarchyLevel>=mc->jointHierarchy[jID].hierarchyLevel)
    {
      fprintf(stderr,"all of them are %u \n",*numberOfChannelsContained);
      //We have reached the end..!
      *jIDLastGroupJoint=jID;
      return 1;
    } else
    {
      *numberOfChannelsContained+=mc->jointHierarchy[jID].loadedChannels;
      fprintf(stderr,"(%s-%u) ",mc->jointHierarchy[jID].jointName,mc->jointHierarchy[jID].loadedChannels);
      jID++;
    }
  }

 fprintf(stderr,"\n");
 return 0;
}



int checkIfJointsHaveSameGraphOutline(
                                       struct BVH_MotionCapture * mc,
                                       BVHJointID jIDA,
                                       BVHJointID jIDB,
                                       unsigned int * rangeOfJIDA,
                                       unsigned int * rangeOfJIDB,
                                       unsigned int * numberOfChannelsContainedJIDA,
                                       unsigned int * numberOfChannelsContainedJIDB
                                     )
{
  //We assume jIDA is before jIDB if now we fix it
  if (jIDA>jIDB)
  {
    BVHJointID tmp=jIDA;
    jIDA=jIDB;
    jIDB=tmp;
  }
  //----------------------------

  BVHJointID jIDALastJoint,jIDBLastJoint;

  //We see the range of each of the joint graphs
  if (
      (
        scanJointHierarchyUntilThisGroupEnds(
                                              mc,
                                              jIDA,
                                              &jIDALastJoint,
                                              numberOfChannelsContainedJIDA
                                            )
      ) &&
      (
        scanJointHierarchyUntilThisGroupEnds(
                                              mc,
                                              jIDB,
                                              &jIDBLastJoint,
                                              numberOfChannelsContainedJIDB
                                            )
      )
     )
     {
       unsigned int rA = (jIDALastJoint-jIDA);
       unsigned int rB = (jIDBLastJoint-jIDB);

       //If the last 2 arguments are not set to null we use them..!
       if (rangeOfJIDA!=0) { *rangeOfJIDA = rA; }
       if (rangeOfJIDB!=0) { *rangeOfJIDB = rB; }

       //If they are the same we can copy paste one to the other
       return ((rA==rB) && (*numberOfChannelsContainedJIDA==*numberOfChannelsContainedJIDB));
     }

  return 0;
}


float * allocateBufferThatCanContainJointAndChildren(
                                                      struct BVH_MotionCapture * mc,
                                                      BVHJointID jID
                                                    )
{
  unsigned int numberOfChannels=0;
  BVHJointID jIDLastJoint;
  if  (
        scanJointHierarchyUntilThisGroupEnds(
                                              mc,
                                              jID,
                                              &jIDLastJoint,
                                              &numberOfChannels
                                            )
      )
      {
        return (float *) malloc(sizeof(float) * (numberOfChannels));
      }
  return 0;
}



int copyJointAndChildrenToBuffer(
                                 struct BVH_MotionCapture * mc,
                                 float * buffer,
                                 BVHJointID jID,
                                 unsigned int rangeNumber,
                                 BVHFrameID  fID
                                )
{
  unsigned int mID = (fID * mc->numberOfValuesPerFrame) + mc->jointToMotionLookup[jID].jointMotionOffset;
  memcpy(
         buffer ,
         &mc->motionValues[mID],
         rangeNumber * sizeof(float)
        );

 return 1;
}

int copyBufferToJointAndChildren(
                                 struct BVH_MotionCapture * mc,
                                 float * buffer,
                                 BVHJointID jID,
                                 unsigned int rangeNumber,
                                 BVHFrameID  fID
                                )
{
  unsigned int mID = (fID * mc->numberOfValuesPerFrame) + mc->jointToMotionLookup[jID].jointMotionOffset;
  memcpy(
         &mc->motionValues[mID],
         buffer ,
         rangeNumber * sizeof(float)
        );

 return 1;
}







int bvh_GrowMocapFileByCopyingExistingMotions(
                                              struct BVH_MotionCapture * mc,
                                              unsigned int timesToRepeat
                                             )
{
  if (timesToRepeat==0)   { fprintf(stderr,"No times to repeat\n"); return 0; }
  float * newMotionValues = (float*) malloc(sizeof(float) * mc->motionValuesSize * (timesToRepeat+1) );
  if (newMotionValues==0) { fprintf(stderr,"No new motion values\n"); return 0; }

  float * oldMotionValues = mc->motionValues;
  float * ptr=newMotionValues;

  unsigned int r=0;
  for (r=0; r<timesToRepeat+1; r++)
  {
    memcpy(ptr,oldMotionValues,sizeof(float) * mc->motionValuesSize);
    ptr+=mc->motionValuesSize;
  }

 mc->numberOfFrames+=mc->numberOfFrames*timesToRepeat;
 mc->numberOfFramesEncountered+=mc->numberOfFrames;
 mc->motionValuesSize+=mc->motionValuesSize*timesToRepeat;
 mc->motionValues = newMotionValues;
 free(oldMotionValues);

 return 1;
}




int bvh_GrowMocapFileBySwappingJointAndItsChildren(
                                                     struct BVH_MotionCapture * mc,
                                                     const char * jointNameA,
                                                     const char * jointNameB,
                                                     int alsoIncludeOriginalMotion
                                                   )
{
  BVHJointID jIDA,jIDB;
  unsigned int rangeOfJIDA,rangeOfJIDB;
  unsigned int numberOfChannelsContainedJIDA,numberOfChannelsContainedJIDB;

  if (
       (bvh_getJointIDFromJointNameNocase(mc,jointNameA,&jIDA)) &&
       (bvh_getJointIDFromJointNameNocase(mc,jointNameB,&jIDB))
     )
  {
   fprintf(stderr,"We have resolved %s to %u and %s to %u\n",jointNameA,jIDA,jointNameB,jIDB);

   if (
        checkIfJointsHaveSameGraphOutline(
                                          mc,
                                          jIDA,
                                          jIDB,
                                          &rangeOfJIDA,
                                          &rangeOfJIDB,
                                          &numberOfChannelsContainedJIDA,
                                          &numberOfChannelsContainedJIDB
                                         )
      )
    {

     fprintf(stderr,"bvh_GrowMocapFileBySwappingJointAndItsChildren");
     unsigned int initialNumberOfFrames = mc->numberOfFrames;
     fprintf(stderr,"Initially had %u frames\n",initialNumberOfFrames);

     //----------------------------------------------------------------
     if (alsoIncludeOriginalMotion)
     {
      fprintf(stderr,"And we where asked to double them\n");
      if (
         bvh_GrowMocapFileByCopyingExistingMotions(
                                                   mc,
                                                   1
                                                  )
        )
      {
          //Successfully Grew buffer to alsoIncludeOriginalMotion
      } else
      {
        fprintf(stderr,"Could not grow our movement buffer to facilitate swapping\n");
        return 0;
      }
     }
     //----------------------------------------------------------------


       fprintf(stderr,"We now have %u frames\n",mc->numberOfFrames);
       float * temporaryMotionBufferA = allocateBufferThatCanContainJointAndChildren( mc, jIDA );
       float * temporaryMotionBufferB = allocateBufferThatCanContainJointAndChildren( mc, jIDB );
       if ( (temporaryMotionBufferA!=0) && (temporaryMotionBufferA!=0) )
       {
        BVHFrameID fID=0;
        for (fID=0; fID<initialNumberOfFrames; fID++)
         {
            if (
                (copyJointAndChildrenToBuffer(mc,temporaryMotionBufferA,jIDA,numberOfChannelsContainedJIDA,fID)) &&
                (copyJointAndChildrenToBuffer(mc,temporaryMotionBufferB,jIDB,numberOfChannelsContainedJIDB,fID))
               )
                {
                  if (
                      (copyBufferToJointAndChildren(mc,temporaryMotionBufferB,jIDA,numberOfChannelsContainedJIDA,fID)) &&
                      (copyBufferToJointAndChildren(mc,temporaryMotionBufferA,jIDB,numberOfChannelsContainedJIDB,fID))
                     )
                     {
                      //Success for mID
                     } else
                     { fprintf(stderr,"Error accessing and copying buffers back to joints %s/%s at frame %u\n",jointNameA,jointNameB,fID); }
                } else
                { fprintf(stderr,"Error accessing and copying joints %s/%s at frame %u to buffers\n",jointNameA,jointNameB,fID); }
         }
        free(temporaryMotionBufferA);
        free(temporaryMotionBufferB);
        return 1;
       } else
       { fprintf(stderr,"Could not allocate temporary buffer\n");  }
    } else
    { fprintf(stderr,"Joints %s and %s do not have the same hierarchy graph outline and therefore cannot be swapped\n",jointNameA,jointNameB); }
  } else
  {
    fprintf(stderr,"Could not resolve %s and %s , maybe they got internally renamed?\n",jointNameA,jointNameB);
    fprintf(stderr,"Full list of joints is : \n");
    unsigned int jID=0;
     for (jID=0; jID<mc->jointHierarchySize; jID++)
      {
        fprintf(stderr,"   joint %u = %s\n",jID,mc->jointHierarchy[jID].jointName);
      }
  }

 fprintf(stderr,"Errors occured during bvh_GrowMocapFileByMirroringJointAndItsChildren\n");
 return 0;
}










