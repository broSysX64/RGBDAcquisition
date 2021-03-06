#ifndef BVH_TO_CSV_H_INCLUDED
#define BVH_TO_CSV_H_INCLUDED


#include "../bvh_loader.h"
#include "../bvh_transform.h"
#include "../../../../../../tools/AmMatrix/simpleRenderer.h"

extern unsigned int invisibleJoints;
extern unsigned int   visibleJoints;
extern unsigned int filteredOutCSVBehindPoses;
extern unsigned int filteredOutCSVOutPoses;
extern unsigned int filteredOutCSVPoses;

int dumpBVHToCSVHeader(
                       struct BVH_MotionCapture * mc,
                       const char * filename2D,
                       const char * filename3D,
                       const char * filenameBVH
                      );


int dumpBVHToCSVBody(
                       struct BVH_MotionCapture * mc,
                       struct BVH_Transform * bvhTransform,
                       struct simpleRenderer * renderer,
                       unsigned int fID,
                       const char * filename2D,
                       const char * filename3D,
                       const char * filenameBVH,
                       unsigned int filterOutSkeletonsWithAnyLimbsBehindTheCamera,
                       unsigned int filterOutSkeletonsWithAnyLimbsOutOfImage,
                       unsigned int filterWeirdSkeletons,
                       unsigned int encodeRotationsAsRadians
                      );

#endif // BVH_TO_SVG_H_INCLUDED
