/** @file ogl_shader_pipeline_renderer.h
 *  @brief  This is the new way ( OpenGL3+ ) to render using a shader based pipeline..
 *  @bug    The shader based rendering is not support is not yet complete
 *  @author Ammar Qammaz (AmmarkoV)
 */

#ifndef OGL_SHADER_PIPELINE_RENDERER_H_INCLUDED
#define OGL_SHADER_PIPELINE_RENDERER_H_INCLUDED



#include "../ogl_rendering.h"


int startOGLShaderPipeline(struct rendererConfiguration * config);

int stopOGLShaderRendering(struct rendererConfiguration * config);

void doOGLShaderBoneDrawCalllist( float * pos , unsigned int * parentNode ,  unsigned int boneSizes);


/**
* @brief This call renders a mesh using shaders..!
* @ingroup Rendering

* @param Vertice data
* @param Number of Vertices

* @param Normal data
* @param Number of Normals

* @param Texture Coordinate data
* @param Number of Texture Coordinates

* @param Color data
* @param Number of Colors

* @param Indexing data for vertices
* @param Number of Indices
*/
void doOGLShaderDrawCalllist(
                              float * projectionMatrix ,
                              float * viewMatrix ,
                              float * modelMatrix ,
                              float * mvpMatrix ,
                              //-------------------------------------------------------
                              float * vertices ,       unsigned int numberOfVertices ,
                              float * normals ,         unsigned int numberOfNormals ,
                              float * textureCoords ,  unsigned int numberOfTextureCoords ,
                              float * colors ,         unsigned int numberOfColors ,
                              unsigned int * indices , unsigned int numberOfIndices
                             );



#endif // OGL_SHADER_PIPELINE_RENDERER_H_INCLUDED
