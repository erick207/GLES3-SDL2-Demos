#include <SDL.h>
#include <SDL_opengles2.h>
#include <GLES3/gl3.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "shader.h"
#include "texture.h"

/** Encapsulates the data for a single vertex.
 * Must match the vertex shader's input.
 */
typedef struct Vertex_s {
	float position[2];
	float texCoord[2];
}Vertex;

GLuint vboCreate(const Vertex *vertices, GLuint numVertices) {
	// Create the Vertex Buffer Object
	GLuint vbo;
	int nBuffers = 1;
	glGenBuffers(nBuffers, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Copy the vertex data in, and deactivate
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// Check for problems
	GLenum err = glGetError();
	if(err != GL_NO_ERROR) {
		// Failed
		glDeleteBuffers(nBuffers, &vbo);
		SDL_Log("Creating VBO failed, code %u\n", err);
		vbo = 0;
	}
	
	return vbo;
}

/** Frees the VBO.
 * 
 * @param vbo the VBO's name.
 */

void vboFree(GLuint vbo) {
	glDeleteBuffers(1, &vbo);
}

using namespace std;

const unsigned int DISP_WIDTH = 640;
const unsigned int DISP_HEIGHT = 480;

int SDL_main(int argc, char *args[]) {
	
	// The window
	SDL_Window *window = NULL;
	
	// The OpenGL context
	SDL_GLContext context = NULL;
	
	// IMPORTANT! These sets must go BEFORE SDL_Init
	// Request OpenGL ES 3.0
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	
	// Want double-buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		cout << "SDL could not initialize! SDL_Error:\n";
		return EXIT_FAILURE;
	}
	
	// Setup the exit hook
	atexit(SDL_Quit);
	

	
	// Create the window
	window = SDL_CreateWindow("GLES3+SDL2 Tutorial", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, DISP_WIDTH, DISP_HEIGHT,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if(!window){
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
				"Couldn't create the main window.", NULL);
			cout << "Couldn't create the main window.\n";
			return EXIT_FAILURE;
			}
		
		context = SDL_GL_CreateContext(window);
		if(!context) {
				SDL_ShowSimpleMessageBox(
					SDL_MESSAGEBOX_ERROR, "Error",
					"Couldn't create an OpenGL context.", NULL);
				cout << "Couldn't create an OpenGL context.\n";
				return EXIT_FAILURE;
			}
	// Clear to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Update the window
	SDL_GL_SwapWindow(window);
	

	// Load the shader program and set it for use
	
	GLuint shaderProg = shaderProgLoad("texture.vert", "texture.frag");
	
	if(!shaderProg){
		// Error messages already displayed...
		return EXIT_FAILURE;
	}
	glUseProgram(shaderProg);
	
	// Load the texture
	
	GLuint texture = texLoad("crate1_diffuse.png");
	if(!texture) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Couldn't load texture.", NULL);
		return EXIT_FAILURE;
	}
	
	// Bind the texture to unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	// Bind texSampler to unit 0
	
	GLint texSamplerUniformLoc = glGetUniformLocation(shaderProg, "texSampler");
	if (texSamplerUniformLoc < 0) {
		SDL_Log("ERROR: Couldn't get texSampler's location.");
		return EXIT_FAILURE;
	}
	glUniform1i(texSamplerUniformLoc, 0);
	
	//Create the triangle
	const Vertex vertices[] = {
		{{-0.9f, -0.9f}, {0.0f, 0.0f}},
		{{0.9f, -0.9f}, {1.0f, 0.0f}},
		{{0.9f, 0.9f}, {1.0f, 1.0f}},
		{{-0.9f, 0.9f}, {0.0f, 1.0f}}};
		
	GLsizei vertSize = sizeof(vertices[0]);
	GLsizei numVertices = sizeof(vertices)/vertSize;
	GLuint triangleVBO = vboCreate(vertices, numVertices);
	if(!triangleVBO) {
		//Failes. Error message has already been printed, so just quit
		return EXIT_FAILURE;
	}
	
	// Set up for rendering the triangle (activate the VBO)
	
	GLuint positionIdx = 0; // Position is vertex attribute 0
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glVertexAttribPointer(positionIdx, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) 0);
	glEnableVertexAttribArray(positionIdx);
	
	GLuint texCoordIdx = 1; // TexCoord is vertex attribute 1
	glVertexAttribPointer(texCoordIdx, 2, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (const GLvoid*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(texCoordIdx);
	// Now draw!
	
	glDrawArrays(GL_TRIANGLE_FAN, 0, numVertices);
	
	// Update the window
	
	SDL_GL_SwapWindow(window);
	
	// Cleanup
	
	vboFree(triangleVBO);
	triangleVBO = 0;
	shaderProgDestroy(shaderProg);
	shaderProg = 0;
	texDestroy(texture); // Delete texture
	texture = 0;
	
	// Wait for the user to quit
	bool quit = false;
	while (!quit) {
		SDL_Event event;
		if (SDL_WaitEvent(&event) != 0) {
				if (event.type == SDL_QUIT) {
					// User wants to quit
					quit = true;
				}
			}
	}
	
	return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
		return SDL_main(argc, argv);
	}
