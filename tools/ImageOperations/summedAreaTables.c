#include "summedAreaTables.h"
#include <stdio.h>
#include <stdlib.h>

unsigned int * generateSummedAreaTableRGB(unsigned char * source,  unsigned int sourceWidth , unsigned int sourceHeight )
{
 fprintf(stderr,"generateSummedAreaTable(%p , %u , %u )\n",source,sourceWidth , sourceHeight);
 if ( (sourceWidth>4000 ) && (sourceHeight>4000) )
 {
   fprintf(stderr,"generateSummedAreaTable cannot contain so big frame sizes \n");
   return 0;
 }

 unsigned int *  sat = (unsigned int * ) malloc(sizeof(unsigned int) * ((sourceWidth) * (sourceHeight) *3) );
 if (sat==0) { fprintf(stderr,"generateSummedAreaTable could not allocate table\n"); return 0; }

 unsigned int nextLineOffset = (sourceWidth*3);

 unsigned int * satOut = sat;
 unsigned char * inPtr = source;
 unsigned char * inLineLimit = inPtr + nextLineOffset;
 unsigned char * inLimit = inPtr + (sourceWidth*sourceHeight*3) ;

 //First pixel is just the source ( inPtr ) value , and we go forward
 unsigned int *outLeftPtr=satOut;
 *satOut=(unsigned int) (*inPtr); ++inPtr; ++satOut;
 *satOut=(unsigned int) (*inPtr); ++inPtr; ++satOut;
 *satOut=(unsigned int) (*inPtr); ++inPtr; ++satOut;

 //First horizontal line is special since it has no vertical additions , so we just sum up left and current elements
 while (inPtr<inLineLimit)
 {
   *satOut = (unsigned int) (*inPtr) + (*outLeftPtr); ++inPtr; ++satOut; ++outLeftPtr;
   *satOut = (unsigned int) (*inPtr) + (*outLeftPtr); ++inPtr; ++satOut; ++outLeftPtr;
   *satOut = (unsigned int) (*inPtr) + (*outLeftPtr); ++inPtr; ++satOut; ++outLeftPtr;
 }
 inLineLimit+=nextLineOffset;

 unsigned int *outUpPtr=sat , *outUpLeftPtr=sat;
 //Ready for the main loop
 outLeftPtr=satOut;
 while (inPtr<inLimit)
 {
    outLeftPtr=satOut;
    *satOut = (unsigned int) (*inPtr) + (*outUpPtr); ++inPtr; ++outUpPtr; ++satOut;
    *satOut = (unsigned int) (*inPtr) + (*outUpPtr); ++inPtr; ++outUpPtr; ++satOut;
    *satOut = (unsigned int) (*inPtr) + (*outUpPtr); ++inPtr; ++outUpPtr; ++satOut;

    if (inLineLimit>inLimit) { fprintf(stderr,"Border case\n");  inLineLimit=inLimit;  }
    while (inPtr<inLineLimit)
    {
      *satOut = (unsigned int) (*inPtr) + (*outLeftPtr) +  (*outUpPtr);
      *satOut -= (*outUpLeftPtr);
      ++inPtr; ++outUpPtr; ++outUpLeftPtr; ++outLeftPtr; ++satOut;

      *satOut = (unsigned int) (*inPtr) + (*outLeftPtr) +  (*outUpPtr);
      *satOut -= (*outUpLeftPtr);
      ++inPtr; ++outUpPtr; ++outUpLeftPtr; ++outLeftPtr; ++satOut;


      *satOut = (unsigned int) (*inPtr) + (*outLeftPtr) +  (*outUpPtr);
      *satOut -= (*outUpLeftPtr);
      ++inPtr; ++outUpPtr; ++outUpLeftPtr; ++outLeftPtr; ++satOut;
    }
    inLineLimit+=nextLineOffset;
    outUpLeftPtr=outUpPtr;
 }


 return sat;
}





unsigned int getSATSum(unsigned int * sourceSAT ,  unsigned int sourceWidth , unsigned int sourceHeight ,  unsigned int x, unsigned int y , unsigned int blockWidth , unsigned int blockHeight )
{
  unsigned int * sourceSATLimit = sourceSAT + (sourceHeight * sourceWidth);


  /*                            TOP
       -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
      L-  -  -  -  -  -  A  -  -  -  -  B  -  -  -  -  -  -R
      E-  -  -  -  -  -  -  - REQUEST-  -  -  -  -  -  -  -I
      F-  -  -  -  -  -  -  -   SUM  -  -  -  -  -  -  -  -G
      T-  -  -  -  -  -  C  -  -  -  -  D  -  -  -  -  -  -H
       -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -T
                               BOTTOM
      _____________
      D + A - B - C
  */
  unsigned int * A = sourceSAT + ( sourceWidth* (y+0) )           + (x+0);
  unsigned int * B = sourceSAT + ( sourceWidth* (y+0) )           + (x+blockWidth);
  unsigned int * C = sourceSAT + ( sourceWidth* (y+blockHeight) ) + (x+0);
  unsigned int * D = sourceSAT + ( sourceWidth* (y+blockHeight) ) + (x+blockWidth);

  if (
       (A>=sourceSATLimit) ||
       (B>=sourceSATLimit) ||
       (C>=sourceSATLimit) ||
       (D>=sourceSATLimit)
     ) { return 0; }


  unsigned int sum = *D + *A ;
  sum = sum - *B - *C;
  return sum;
}


int summedAreaTableTest()
{
  fprintf(stderr,"summedAreaTableTest()\n");
  unsigned char sourceRGB[]={
                             1,0,2 , 1,0,2 , 1,0,2 , 1,0,2 , 1,0,2 ,
                             1,0,2 , 1,0,2 , 1,0,2 , 1,0,2 , 1,0,2 ,
                             1,0,2 , 1,0,2 , 1,0,2 , 1,0,2 , 1,0,2 ,
                             1,0,2 , 1,0,2 , 1,0,2 , 1,0,2 , 1,0,2 ,
                             1,0,2 , 1,0,2 , 1,0,2 , 1,0,2 , 1,0,2 ,
                             1,0,2 , 1,0,2 , 1,0,2 , 1,0,2 , 1,0,2
                          } ;

 unsigned int * output = generateSummedAreaTableRGB(sourceRGB ,  5 , 5 );
 if (output==0)
 {
   fprintf(stderr,"generateSummedAreaTableRGB failed\n");
   return 0;
 }
 unsigned int x,y,z=0;

 fprintf(stderr,"Input\n");
 for (y=0; y<5; y++)
 { for (x=0; x<5; x++)
   {
    fprintf(stderr,"%u %u %u ",(unsigned int) sourceRGB[z],(unsigned int) sourceRGB[z+1],(unsigned int) sourceRGB[z+2]);
    z+=3;
   }
   fprintf(stderr,"\n");
 }
 fprintf(stderr,"\n\n\n\n\n\n");

 fprintf(stderr,"Output\n");
 z=0;
 for (y=0; y<5; y++)
 { for (x=0; x<5; x++)
   {
    fprintf(stderr,"%u %u %u ",output[z],output[z+1],output[z+2]);
    z+=3;
   }
   fprintf(stderr,"\n");
 }
 fprintf(stderr,"\n\n\n\n\n\n");

 free(output);

 return 1;
}
