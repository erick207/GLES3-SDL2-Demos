// texture.cpp
//
// See header file for details

#include "texture.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengles2.h>

/** Sets the swizzling for a texture colour channel from an SDL colour mask.
 * 
 * @param channel the texture channel to set (e.g., GL_TEXTURE_SWIZZLE_R)
 * @param mask the SDL colour channel mask (e.g., texSurf->format->Rmask)
 */

bool sdlToGLSwizzle(GLenum channel, Uint32 mask) {
	GLint swizzle;
	switch(mask) {
		case 0x000000FF:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			swizzle = GL_ALPHA;
#else
			swizzle = GL_RED;
#endif
		break;
		case 0x0000FF00:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			swizzle = GL_BLUE;
#else
			swizzle = GL_GREEN;
#endif
		break;
		case 0x00FF0000:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			swizzle = GL_GREEN;
#else
			swizzle = GL_BLUE;
#endif
		break;
		case 0xFF000000:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			swizzle = GL_ALPHA;
#else
			swizzle = GL_RED;
#endif
		break;
		default:
			SDL_Log("Unrecongnized colour channel mask 0x%08X", mask);
			return false;
	} //end switch(mask)
	
	glTexParameteri(GL_TEXTURE_2D, channel, swizzle);
	return true;
}

GLuint texLoad(const char *filename) {
	
	// Make sure the JPEG and PNG image loaders are present (don't know what file type we'll get).
	int flags = IMG_INIT_JPG | IMG_INIT_PNG;
	if((IMG_Init(flags) & flags) == 0) {
		
		// Failed :-(
		SDL_Log("ERROR: Texture loading failed. Couldn't get JPEG and PNG loaders. \n");
		return 0;
	}
	
	// Load the image
	SDL_Surface *texSurf = IMG_Load(filename);
	if(!texSurf) {
		SDL_Log("Loading image %s failed with error: %s", filename, IMG_GetError());
		return 0;
	}
	
	// Determine the format
	// NOTE: Only supporting 24 and 32-bit images
	GLenum format;
	GLenum type = GL_UNSIGNED_BYTE;
	switch (texSurf->format->BytesPerPixel) {
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			SDL_Log("Can't load image %s; it isn't a 24/32-bit image\n", filename);
			SDL_FreeSurface(texSurf);
			texSurf = NULL;
			return 0;
	}
	
	// Create the texture
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, format, texSurf->w,
		texSurf->h, 0, format, type, texSurf->pixels); // what's the deal with 'type'???
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		
		// Failed
		glDeleteBuffers(1, &texture);
		texture = 0;
		SDL_FreeSurface(texSurf);
		texSurf = NULL;
		SDL_Log("Creating texture %s failed, code %u\n", filename, err);
		return 0;
	}
	
	// Set up texture swizzling to match the image's channel order
	bool success = sdlToGLSwizzle(GL_TEXTURE_SWIZZLE_R, texSurf->format->Rmask);
	success &= sdlToGLSwizzle(GL_TEXTURE_SWIZZLE_G, texSurf->format->Gmask);
	success &= sdlToGLSwizzle(GL_TEXTURE_SWIZZLE_G, texSurf->format->Bmask);
	if(format == GL_RGBA) {
		success &= sdlToGLSwizzle(GL_TEXTURE_SWIZZLE_G, texSurf->format->Amask);
	}
	if(!success) {
		SDL_Log("Couldn't set up swizzling for texture %s\n", filename);
		glDeleteBuffers(1, &texture);
		texture = 0;
		SDL_FreeSurface(texSurf);
		texSurf = NULL;
		return 0;
	}
	
	// Set up the filtering
	// NOTE: Failure to do this may result in no texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	// Cleanup
	SDL_FreeSurface(texSurf);
	texSurf = NULL;
	
	return texture;
}

void texDestroy(GLuint texName) {
	
	glDeleteTextures(1, &texName);
}


