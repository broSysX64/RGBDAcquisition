
#if USE_GLEW
#include <GL/glew.h>


#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))


// Here we decide which of the two versions we want to use
// If your systems supports both, choose to uncomment USE_OPENGL32
// otherwise choose to uncomment USE_OPENGL21
// GLView cna also help you decide before running this program:
//
// FOR MACOSX only, please use OPENGL32 for AntTweakBar to work properly
//
#define USE_OPENGL32


#define USE_SIMPLE_SHADERS 0


// GLFW lib
// http://www.glfw.org/documentation.html
#ifdef USE_OPENGL32
    #ifndef _WIN32
        #define GLFW_INCLUDE_GL3
        #define USE_GL3
        #define GLFW_NO_GLU
        #define GL3_PROTOTYPES 1
    #endif
#endif

#endif



#include <GL/gl.h>
#include <GL/glx.h>    /* this includes the necessary X headers */
#include <GL/glu.h>
#include <GL/glut.h>


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ogl_shader_pipeline_renderer.h"

#include "../../tools.h"

#define NORMAL   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */



GLuint buffer;
unsigned int verticeCount;


int         windowWidth=640, windowHeight=480;
GLuint      skyboxProgram=0 , skyboxVAO=0;
GLuint      program=0;
GLuint      vao=0;
GLuint      bufferVao=0;
GLuint      bufferSkyboxVao=0;

 GLuint vPosition , vNormal , vColor , vTexture , lightPositionLocation  , materialColorLocation;
 GLuint fogLocation  , modelViewMatrixLocation  , modelViewProjectionMatrixLocation  , normalTransformationLocation;
 GLuint lightColorLocation , hdrColorLocation , lightMaterialsLocation , texture1Location , normalTextureLocation , skyboxLocation , textureStrengthLocation ;







int startShaderOGLRendering(struct rendererConfiguration * config)
{

  if (checkOpenGLError(__FILE__, __LINE__)) { fprintf(stderr,"OpenGL error while initializing scene\n"); }
  glEnable(GL_DEPTH_TEST); /* enable depth buffering */
  glDepthFunc(GL_LESS);    /* pedantic, GL_LESS is the default */
  glDepthMask(GL_TRUE);
  glClearDepth(1.0);       /* pedantic, 1.0 is the default */

  //HQ settings
  glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  if (checkOpenGLError(__FILE__, __LINE__)) { fprintf(stderr,"OpenGL error while initializing HQ settings\n"); }

  /* frame buffer clears should be to black */
  glClearColor(0.0, 0.0, 0.0, 0.0);

  /* set up projection transform */
  glMatrixMode(GL_PROJECTION);

  updateProjectionMatrix();
  if (checkOpenGLError(__FILE__, __LINE__))
     { fprintf(stderr,"OpenGL error after updating projection matrix\n"); }

  /* establish initial viewport */
  /* pedantic, full window size is default viewport */


  #warning "GL_COLOR does not even exist"
  //glEnable(GL_COLOR);
  //if (checkOpenGLError(__FILE__, __LINE__)) { fprintf(stderr,"OpenGL error after enabling color \n"); }
  glEnable(GL_COLOR_MATERIAL);
  if (checkOpenGLError(__FILE__, __LINE__)) { fprintf(stderr,"OpenGL error after enabling color material\n"); }

  #if USE_LIGHTS
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHTING);
   if (checkOpenGLError(__FILE__, __LINE__)) { fprintf(stderr,"OpenGL error after enabling lighting\n"); }
   glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   if (checkOpenGLError(__FILE__, __LINE__)) { fprintf(stderr,"OpenGL error after setting up lights\n"); }

   GLenum faces=GL_FRONT;//GL_FRONT_AND_BACK;
   glMaterialfv(faces, GL_AMBIENT,    mat_ambient);
   glMaterialfv(faces, GL_DIFFUSE,    mat_diffuse);
   glMaterialfv(faces, GL_SPECULAR,   mat_specular);
   glMaterialfv(faces, GL_SHININESS,   mat_shininess); // <- this was glMateriali
   if (checkOpenGLError(__FILE__, __LINE__)) { fprintf(stderr,"OpenGL error after setting up Front/Back lights\n"); }
  #else
   fprintf(stderr,"Please note that lighting is disabled via the USE_LIGHTS precompiler define\n");
  #endif // USE_LIGHTS


  if ( ( config->selectedFragmentShader != 0) || ( config->selectedVertexShader != 0 ) )
  {
      config->loadedShader = loadShader(config->selectedVertexShader,config->selectedFragmentShader);
  }

  //This is not needed -> :P  glCullFace(GL_FRONT_AND_BACK);
  //Enable Culling
  if (config->doCulling)
  {
   glFrontFace(GL_CCW); //GL_CW / GL_CCW
     if (checkOpenGLError(__FILE__, __LINE__)) { fprintf(stderr,"OpenGL error glFrontFace(GL_CCW); \n"); }
   glCullFace(GL_BACK);
     if (checkOpenGLError(__FILE__, __LINE__)) { fprintf(stderr,"OpenGL error glCullFace(GL_BACK); \n"); }
   glEnable(GL_CULL_FACE);
    if (checkOpenGLError(__FILE__, __LINE__)) { fprintf(stderr,"OpenGL error glEnable(GL_CULL_FACE); \n"); }
  }
 return 1;
}




















void doOGLShaderDrawCalllist(
                              float * vertices ,       unsigned int numberOfVertices ,
                              float * normals ,         unsigned int numberOfNormals ,
                              float * textureCoords ,  unsigned int numberOfTextureCoords ,
                              float * colors ,         unsigned int numberOfColors ,
                              unsigned int * indices , unsigned int numberOfIndices
                             )
{
/*


void pushObjectToBufferData(
                             unsigned int * verticeCount ,
                             const float * vertices , unsigned int numberOfVertices ,
                             const float * normals , unsigned int numberOfNormals ,
                             const float * colors , unsigned int numberOfColors ,
                             const float * texcoords , unsigned int numberOfTextureCoords ,
                             int generateNewBuffer ,
                             GLuint buffer
                           )
{*/

#if USE_GLEW

    glBindBuffer( GL_ARRAY_BUFFER, buffer );        checkOpenGLError(__FILE__, __LINE__);



    verticeCount+=(unsigned int ) numberOfVertices/(3*sizeof(float));
    fprintf(stderr,GREEN "Will DrawArray(GL_TRIANGLES,0,%u) - %u \n" NORMAL ,verticeCount,numberOfVertices);
    fprintf(stderr,GREEN "Pushing %u vertices (%u bytes) and %u normals (%u bytes) and %u colors and %u texture coords as our object \n" NORMAL ,numberOfVertices/sizeof(float),numberOfVertices,numberOfNormals/sizeof(float),numberOfNormals,numberOfColors,numberOfTextureCoords);

  int generateNewBuffer=1;
  if (generateNewBuffer)
   {
    glBufferData( GL_ARRAY_BUFFER, numberOfVertices + numberOfNormals  + numberOfColors + numberOfTextureCoords ,NULL, GL_STREAM_DRAW ); checkOpenGLError(__FILE__, __LINE__);

    glBufferSubData( GL_ARRAY_BUFFER, 0                                      , numberOfVertices , vertices );                  checkOpenGLError(__FILE__, __LINE__);
    glBufferSubData( GL_ARRAY_BUFFER, numberOfVertices                         , numberOfNormals  , normals );                   checkOpenGLError(__FILE__, __LINE__);

    if ( (colors!=0) && (numberOfColors!=0) )
    {
     glBufferSubData( GL_ARRAY_BUFFER, numberOfVertices + numberOfNormals , numberOfColors , colors );                     checkOpenGLError(__FILE__, __LINE__);
    }
    if ( (textureCoords!=0) && (numberOfTextureCoords!=0) )
    {
     glBufferSubData( GL_ARRAY_BUFFER, numberOfVertices + numberOfNormals + numberOfColors, numberOfTextureCoords , textureCoords ); checkOpenGLError(__FILE__, __LINE__);
    }
   }


    vPosition = glGetAttribLocation( program, "vPosition" );                                   checkOpenGLError(__FILE__, __LINE__);
    glEnableVertexAttribArray( vPosition );                                                    checkOpenGLError(__FILE__, __LINE__);
    glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );             checkOpenGLError(__FILE__, __LINE__);

     vNormal = glGetAttribLocation( program, "vNormal" );                                      checkOpenGLError(__FILE__, __LINE__);
     glEnableVertexAttribArray( vNormal );                                                     checkOpenGLError(__FILE__, __LINE__);
     glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(numberOfVertices) ); checkOpenGLError(__FILE__, __LINE__);


    if ( (colors!=0) && (numberOfColors!=0) )
    {
     vColor = glGetAttribLocation( program, "vColor" );
     glEnableVertexAttribArray( vColor );
     glVertexAttribPointer( vColor, 3, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET( numberOfVertices + numberOfNormals ) );
     checkOpenGLError(__FILE__, __LINE__);
    }


    textureStrengthLocation = glGetUniformLocation(program, "textureStrength");  checkOpenGLError(__FILE__, __LINE__);
    if ( (textureCoords!=0) && (numberOfTextureCoords!=0) )
    {
     vTexture = glGetAttribLocation( program, "vTexture" );
     glEnableVertexAttribArray( vTexture );
     glVertexAttribPointer( vTexture, 2, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET( numberOfVertices + numberOfNormals + numberOfColors) );
     checkOpenGLError(__FILE__, __LINE__);

     //textureStrength[0]=1.0;
    } else
    { /*textureStrength[0]=0.0;*/ }



#endif // USE_GLEW


}



int startOGLShaderPipeline()
{
#if USE_GLEW
#warning "Using GLEW"
  glewInit();
  fprintf(stderr,"Using GLEW %s \n",glewGetString(GLEW_VERSION));

  if (GLEW_VERSION_3_2)
    {
      fprintf(stderr,"Yay! OpenGL 3.2 is supported and GLSL 1.5!\n");
    }

	if (glewIsSupported("GL_ARB_vertex_buffer_object"))   { fprintf(stderr,"ARB VBO's are supported\n");  } else
    if (glewIsSupported("GL_APPLE_vertex_buffer_object")) { fprintf(stderr,"APPLE VBO's are supported\n");} else
		                                                  { fprintf(stderr,"VBO's are not supported,program will not run!!!\n"); }


	if (glewIsSupported("GL_ARB_vertex_array_object"))    { fprintf(stderr,"ARB VAO's are supported\n"); } else
	//this is the name of the extension for GL2.1 in MacOSX
    if (glewIsSupported("GL_APPLE_vertex_array_object"))  { fprintf(stderr,"APPLE VAO's are supported\n"); } else
		                                                  { fprintf(stderr,"VAO's are not supported, program will not run!!!\n"); }


    fprintf(stderr,"Vendor: %s \n",glGetString (GL_VENDOR) );
    fprintf(stderr,"Renderer: %s \n",glGetString (GL_RENDERER) );
    fprintf(stderr,"Version: %s \n",glGetString (GL_VERSION) );
#endif
}
