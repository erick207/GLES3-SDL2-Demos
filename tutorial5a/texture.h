// texture.h

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <GLES3/gl3.h>

/** Loads a 2D texture from file.
 * 
 * @param filename name of the image file to load
 * 
 * @return GLuint the texture's name, or 0 if failed
 */

GLuint texLoad(const char *filename);

/** Deallocates a texture.
 */

void texDestroy(GLuint texName);

#endif
