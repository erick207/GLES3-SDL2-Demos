// shader.h

#ifndef __SHADER_H__
#define __SHADER_H__

#include <GLES3/gl3.h>

/** Loads a vertex and fragment shader from disk and compiles (& links) them
 * into a shader program.
 * 
 * This will print any errors to the console.
 * 
 * @param vertFilename filename for the vertex shader.
 * @param fragFilename filename for the shader's filename.
 * 
 * @return GLuint the shader program's ID, or 0 if failed.
 */

GLuint shaderProgLoad(const char *vertFilename, const char *fragFilename);

/** Destroys a shader program.
 */

void shaderProgDestroy(GLuint shaderProg);


#endif
