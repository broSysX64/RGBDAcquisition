#ifndef BVH_CUT_PASTE_H_INCLUDED
#define BVH_CUT_PASTE_H_INCLUDED


#include "../bvh_loader.h"

int checkIfJointsHaveSameGraphOutline(
                                       struct BVH_MotionCapture * mc,
                                       BVHJointID jIDA,
                                       BVHJointID jIDB,
                                       unsigned int * rangeOfJIDA,
                                       unsigned int * rangeOfJIDB,
                                       unsigned int * numberOfChannelsContainedJIDA,
                                       unsigned int * numberOfChannelsContainedJIDB
                                     );

float * allocateBufferThatCanContainJointAndChildren(
                                                  struct BVH_MotionCapture * mc,
                                                  BVHJointID jID
                                                 );

int copyJointAndChildrenToBuffer(
                                 struct BVH_MotionCapture * mc,
                                 float * buffer,
                                 BVHJointID jID,
                                 unsigned int rangeNumber,
                                 BVHFrameID fID
                                );

int copyBufferToJointAndChildren(
                                 struct BVH_MotionCapture * mc,
                                 float * buffer,
                                 BVHJointID jID,
                                 unsigned int rangeNumber,
                                 BVHFrameID  fID
                                );



int bvh_GrowMocapFileByCopyingExistingMotions(
                                              struct BVH_MotionCapture * mc,
                                              unsigned int timesToRepeat
                                             );

int bvh_GrowMocapFileBySwappingJointAndItsChildren(
                                                     struct BVH_MotionCapture * mc,
                                                     const char * jointNameA,
                                                     const char * jointNameB,
                                                     int alsoIncludeOriginalMotion
                                                   );

#endif
