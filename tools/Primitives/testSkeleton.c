#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "skeleton.h"
#include "nao_geometry.h"

float visualizationScale = 3.0;
int frames =0;




void broadcastHTTPSkeleton(struct skeletonHuman * skeletonFound,const char * whereto)
{
  char cmd[4096]={0};
  char part[1024]={0};
  unsigned int i=0;

  for (i=0; i<HUMAN_SKELETON_PARTS; i++)
  {
       snprintf(part,1024,"%0.2f_%0.2f_%0.2f_",
                skeletonFound->joint[i].x ,
                skeletonFound->joint[i].y ,
                skeletonFound->joint[i].z );
       strcat(cmd,part);
  }

  fprintf(stderr,"Will execute : \n");
  fprintf(stderr,"%s%s \n",whereto,cmd);

  char runStrCmd[4096]={0};
  snprintf(runStrCmd,4096,"wget -qO- \"%s%s\" &> /dev/null &",whereto,cmd);
  i=system(runStrCmd);

  if (i==0) { fprintf(stderr,"Success\n"); } else
            { fprintf(stderr,"Error\n");   }
}





static int printout3DSkeleton(const char * filename ,struct skeletonHuman *  sk,int frameNum)
{

  unsigned int i=0;

  fprintf(stderr,YELLOW "printout3DSkeleton(%s,%u)\n" NORMAL,filename,frameNum);

  FILE * fp=0;

  if (frameNum==0)  { fp = fopen(filename,"w"); } else
                    { fp = fopen(filename,"a"); }
  if (fp!=0)
  {
   if (frameNum==0)
   {
   fprintf(fp,"#This is a simple trajectory file..! \n");
   fprintf(fp,"#You can render it with this tool :\n");
   fprintf(fp,"#https://github.com/AmmarkoV/RGBDAcquisition/tree/master/opengl_acquisition_shared_library/opengl_depth_and_color_renderer\n");

   fprintf(fp,"AUTOREFRESH(1500)\n");
   fprintf(fp,"BACKGROUND(20,20,20)\n");
   fprintf(fp,"MOVE_VIEW(1)\n");
   fprintf(fp,"OBJECTTYPE(joint,sphere)\n\n");


   fprintf(fp,"Bring our world to the MBV coordinate system\n");
   fprintf(fp,"SCALE_WORLD(-0.01,-0.01,0.01)\n");
   fprintf(fp,"MAP_ROTATIONS(-1,-1,1,zxy)\n");
   fprintf(fp,"OFFSET_ROTATIONS(0,0,0)\n");
   fprintf(fp,"EMULATE_PROJECTION_MATRIX(519.460494 , 0.0 , 324.420168 , 0.0 , 519.118667 , 229.823479 , 0 , 1)\n");
   fprintf(fp,"MODELVIEW_MATRIX(1,0,0,0, 0,1,0,0 , 0,0,1,0 ,0,0,0,1)\n");
   fprintf(fp,"We are now on MBV WORLD !!\n");
   fprintf(fp,"--------------------------------------------------------------------------\n\n");
   for (i=0; i<HUMAN_SKELETON_PARTS; i++)
   {
    fprintf(fp,"OBJECT(%s,joint,255,255,0,0 ,0, 0.5,0.5,0.5 , )\n",jointNames[i]);
   }
   for (i=0; i<HUMAN_SKELETON_PARTS; i++)
   {
    fprintf(fp,"CONNECTOR(%s,%s,0,0,255,0,3.0)\n",jointNames[i],jointNames[humanSkeletonJointsRelationMap[i]]);
   }

   fprintf(fp,"\n\n\nINTERPOLATE_TIME(1)\n\n\n");
   }

   fprintf(fp,"POS(camera,%u,   -1.0,1.0, 2.0 , 0.0, 0.0,0.0,0.0 )\n",frameNum*100);

   for (i=0; i<HUMAN_SKELETON_PARTS; i++)
   {
     fprintf(fp,"POS(%s,%u,   %0.2f , %0.2f , %0.2f  , 00.0,0.0,0.0,0.0)\n",jointNames[i],frameNum*100,sk->joint[i].x,sk->joint[i].y,sk->joint[i].z);
   }
  fprintf(fp,"----------------------- \n\n\n");
  fclose(fp);
  }
 return 0;
}











int doSkeletonConversions( struct skeletonHuman * skel )
{
  //if ( !skeletonEmpty(skel))
  {
   updateSkeletonAngles(skel);
    printSkeletonHuman(skel);

   //updateSkeletonAnglesNAO(skel);
   fprintf(stderr,"doSkeletonConversions #%u ",frames);
   char filenameBuf[512]={0};
   snprintf(filenameBuf,512,"skel2D%u.svg",frames);
   if (!skeleton2DEmpty(skel))
   {
    visualizeSkeletonHuman(filenameBuf,  skel, visualizationScale);
   } else { fprintf(stderr,RED "Won't print out 2D scenes with skeletons for empty 2D skeleton info \n" NORMAL );}



   if (!skeleton3DEmpty(skel))
   {
   snprintf(filenameBuf,512,"skel3D.scene",frames);
   printout3DSkeleton(filenameBuf,skel,frames);
   } else { fprintf(stderr,RED "Won't print out 3D scenes with skeletons for empty 3D skeleton info \n" NORMAL );}


   if (!skeleton3DEmpty(skel))
   {
    broadcastHTTPSkeleton(skel,"http://127.0.0.1:8080/sk.html?sk=");
   }else { fprintf(stderr,RED "Won't broadcast 3D scenes for empty 3D skeleton info\n" NORMAL );}

   struct naoCommand nao={0};
   struct skeletonHuman sk={0};

   setNAOMotorsFromHumanSkeleton( &nao , &sk );
   snprintf(filenameBuf,512,"skel%u.skel",frames);
   //printoutNAOCommand( filenameBuf , &nao );
  }

   usleep(1000*1000);
   cleanSkeleton(skel);
   ++frames;
  return 1;
}





int printJointField ( struct skeletonHuman * skel )
{
  unsigned int i=0;

  printf("numberOfJoints: %u\n",HUMAN_SKELETON_PARTS);



  printf("joints2D: [");
  for (i=0; i<HUMAN_SKELETON_PARTS-1; i++)
  {
     printf("%0.4f,%0.4f,",skel->joint2D[i].x,skel->joint2D[i].y);
  }
  printf("%0.4f,%0.4f]\n",skel->joint2D[HUMAN_SKELETON_PARTS-1].x,skel->joint2D[HUMAN_SKELETON_PARTS-1].y);


  printf("joints3D: [");
  for (i=0; i<HUMAN_SKELETON_PARTS-1; i++)
  {
     printf("%0.4f,%0.4f,%0.4f,",skel->joint[i].x,skel->joint[i].y,skel->joint[i].z);
  }
  ++i;
  printf("%0.4f,%0.4f,%0.4f]\n",skel->joint[HUMAN_SKELETON_PARTS-1].x,skel->joint[HUMAN_SKELETON_PARTS-1].y,skel->joint[HUMAN_SKELETON_PARTS-1].z);




  printf("relativeJointAngle: [");
  for (i=0; i<HUMAN_SKELETON_PARTS-1; i++)
  {
     printf("%0.4f,%0.4f,%0.4f,",skel->relativeJointAngle[i].x,skel->relativeJointAngle[i].y,skel->relativeJointAngle[i].z);
  }
  ++i;
  printf("%0.4f,%0.4f,%0.4f]\n",skel->relativeJointAngle[HUMAN_SKELETON_PARTS-1].x,skel->relativeJointAngle[HUMAN_SKELETON_PARTS-1].y,skel->relativeJointAngle[HUMAN_SKELETON_PARTS-1].z);




  printf("timestamp: %u\n", skel->observationNumber);
  printf("---\n");
  return 1;
}


int parseJointField ( struct skeletonHuman * skel , char * var , char * val)
{
  if (strcmp(var,"numberOfJoints")==0) { if ( atoi(val) != HUMAN_SKELETON_PARTS ) { fprintf(stderr,"Incorrect number of joints \n"); } } else
  if (strcmp(var,"timestamp")==0)      { skel->observationNumber= atoi(val); } else
  if (strcmp(var,"joints3D")==0)
         {
           char * numStart = strchr(val , '[');
           char * numEnd = strchr(val , ',');
           if ( (numStart!=0) && (numEnd!=0) )
           {
            ++numStart; // Supposing we have [xyz.etc,
            *numEnd=0;

            unsigned int i=0;
            for (i=0; i<HUMAN_SKELETON_PARTS; i++)
            {
              //Grab Coordinate X
              skel->joint[i].x=atof(numStart);
              numStart=numEnd+1;
              numEnd = strchr(numStart , ',');
              if (numEnd!=0) { *numEnd=0; }

              //Grab Coordinate Y
              skel->joint[i].y=atof(numStart);
              numStart=numEnd+1;
              if ( i==HUMAN_SKELETON_PARTS-1 )  { numEnd = strchr(numStart , ']'); } else
                                                { numEnd = strchr(numStart , ','); }
              if (numEnd!=0) { *numEnd=0; }

              //Grab Coordinate Z
              skel->joint[i].z=atof(numStart);
              numStart=numEnd+1;
              numEnd = strchr(numStart , ',');
              if (numEnd!=0) { *numEnd=0; }
            }
           }
         } else
  if (strcmp(var,"joints2D")==0)
         {
           char * numStart = strchr(val , '[');
           char * numEnd = strchr(val , ',');
           if ( (numStart!=0) && (numEnd!=0) )
           {
            ++numStart; // Supposing we have [xyz.etc,
            *numEnd=0;

            unsigned int i=0;
            for (i=0; i<HUMAN_SKELETON_PARTS; i++)
            {
              //Grab Coordinate Y
              skel->joint2D[i].x=atof(numStart);
              numStart=numEnd+1;
              if ( i==HUMAN_SKELETON_PARTS-1 )  { numEnd = strchr(numStart , ']'); } else
                                                { numEnd = strchr(numStart , ','); }
              if (numEnd!=0) { *numEnd=0; }

              //Grab Coordinate Z
              skel->joint2D[i].y=atof(numStart);
              numStart=numEnd+1;
              numEnd = strchr(numStart , ',');
              if (numEnd!=0) { *numEnd=0; }
            }
           }
         } else
         {
           return 0;
         }


 return 1;
}


int parseJointList(const char * filename)
{
  struct skeletonHuman skel={0};

  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  FILE * fp = fopen(filename,"r");
  if (fp!=0)
  {

    char * line = NULL;
    size_t len = 0;

    while ((read = getline(&line, &len, fp)) != -1)
    {
       printf("Retrieved line of length %zu :\n", read);
       printf("%s", line);



      if (strstr(line,"---")!=0)
      {
           doSkeletonConversions( &skel );
           printJointField ( &skel );
      } else
      {
        char * varNameEnd = strchr(line , ':');
        if (varNameEnd!=0)
        {
         *varNameEnd=0;
         printf("VAR = %s\n", line);
         char * val = varNameEnd+1;
         printf("VAL = %s\n", val);

         parseJointField ( &skel , line ,  val  );
        }
       }
    }

    fclose(fp);
    if (line) { free(line); }
    return 1;
  }



  return 0;
}




int main(int argc, char *argv[])
{
    if (argc < 2 ) { fprintf(stderr,"Please give filename of joint list \n"); return 1; }

    printf("Running Converter on %s !\n",argv[1]);

    struct skeletonHuman defaultPose={0};
    fillWithDefaultSkeleton(&defaultPose);
    visualizeSkeletonHuman("defaultPose.svg", &defaultPose , visualizationScale );

    struct skeletonHuman defaultNAOPose={0};
    fillWithDefaultNAOSkeleton(&defaultNAOPose);
    visualizeSkeletonHuman("defaultNAOPose.svg", &defaultNAOPose , visualizationScale );

    parseJointList(argv[1]);
    return 0;
}
