#include <SDL.h>
#include <SDL_opengles2.h>
#include <GLES3/gl3.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp> // tut5
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "texture.h"

/** Encapsulates the data for a single vertex.
 * Must match the vertex shader's input.
 */
typedef struct Vertex_s {
	float position[3];
	float texCoord[2];
	float normal[3];
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

/**
 * Creates the Index Buffer Object (IBO) containing
 * 
 * the given indices.
 * 
 * @param indices pointer to the array of indices
 * @param numIndices the number of indices in the array
 */
 GLuint iboCreate(GLushort *indices, GLuint numIndices) {
	// Create the Index Buffer Object
	GLuint ibo;
	int nBuffers = 1;
	glGenBuffers(nBuffers, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	
	// Copy the index data in, and deactivate
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0])*numIndices, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	// Check for problems
	GLenum err = glGetError();
	if(err != GL_NO_ERROR) {
		// Failed
		glDeleteBuffers(nBuffers, &ibo);
		SDL_Log("Creating IBO Failed, code %u\n", err);
		ibo = 0;
	}
	
	return ibo;
}

/** Frees the IBO
 * 
 * @param vbo the IBO's name.
 */
 void iboFree(GLuint ibo){
	 glDeleteBuffers(1, &ibo);
}

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
	// Enable and set up the depth buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	// Clear to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepthf(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
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
	
	// Get other uniform locations
	
	GLint mvMatLoc = glGetUniformLocation(shaderProg, "mvMat");
	if (mvMatLoc < 0) {
		SDL_Log("ERROR: Couldn't get mvMat's location.");
		return EXIT_FAILURE;
	}
	
	GLint normalMatLoc = glGetUniformLocation(shaderProg, "normalMat");
	if (normalMatLoc < 0) {
		SDL_Log("ERROR: Couldn't get normalMat's location.");
		return EXIT_FAILURE;
	}
	
	GLint projMatLoc = glGetUniformLocation(shaderProg, "projMat");
	if (projMatLoc < 0) {
		SDL_Log("ERROR: Couldn't get projMat's location.");
		return EXIT_FAILURE;
	}
	
	GLint lightPosLoc = glGetUniformLocation(shaderProg, "lightPos");
	if (lightPosLoc < 0) {
		SDL_Log("ERROR: Couldn't get lightPos' location.");
		return EXIT_FAILURE;
	}
	
	GLint ambientColLoc = glGetUniformLocation(shaderProg, "ambientCol");
	if (ambientColLoc < 0) {
		SDL_Log("ERROR: Couldn't get ambientCol's location.");
		return EXIT_FAILURE;
	}
	
	GLint diffuseColLoc = glGetUniformLocation(shaderProg, "diffuseCol");
	if (diffuseColLoc < 0) {
		SDL_Log("ERROR: Couldn't get diffuseCol's location.");
		return EXIT_FAILURE;
	}
	
	//Create the 3D cube
	
	float cubeSize_2 = 100.0f / 2.0f; // Half the cube's size
	const Vertex vertices[] = {
		// Front face
		{{-cubeSize_2, -cubeSize_2, cubeSize_2},{0.0f, 0.0f},{0.0f,0.0f,1.0f}},
		{{cubeSize_2, -cubeSize_2, cubeSize_2},{1.0f, 0.0f},{0.0f,0.0f,1.0f}},
		{{cubeSize_2, cubeSize_2, cubeSize_2},{1.0f, 1.0f},{0.0f,0.0f,1.0f}},
		{{-cubeSize_2, cubeSize_2, cubeSize_2},{0.0f, 1.0f},{0.0f,0.0f,1.0f}},
		// Back face
		{{cubeSize_2, -cubeSize_2, -cubeSize_2},{0.0f, 0.0f},{0.0f,0.0f,-1.0f}},
		{{-cubeSize_2, -cubeSize_2, -cubeSize_2},{1.0f, 0.0f},{0.0f,0.0f,-1.0f}},
		{{-cubeSize_2, cubeSize_2, -cubeSize_2},{1.0f, 1.0f},{0.0f,0.0f,-1.0f}},
		{{cubeSize_2, cubeSize_2, -cubeSize_2},{0.0f, 1.0f},{0.0f,0.0f,-1.0f}},
		// Left face
		{{-cubeSize_2, -cubeSize_2, -cubeSize_2},{0.0f, 0.0f},{-1.0f,0.0f,0.0f}},
		{{-cubeSize_2, -cubeSize_2, cubeSize_2},{1.0f, 0.0f},{-1.0f,0.0f,0.0f}},
		{{-cubeSize_2, cubeSize_2, cubeSize_2},{1.0f, 1.0f},{-1.0f,0.0f,0.0f}},
		{{-cubeSize_2, cubeSize_2, -cubeSize_2},{0.0f, 1.0f},{-1.0f,0.0f,0.0f}},
		// Right face
		{{cubeSize_2, -cubeSize_2, cubeSize_2},{0.0f, 0.0f},{1.0f,0.0f,0.0f}},
		{{cubeSize_2, -cubeSize_2, -cubeSize_2},{1.0f, 0.0f},{1.0f,0.0f,0.0f}},
		{{cubeSize_2, cubeSize_2, -cubeSize_2},{1.0f, 1.0f},{1.0f,0.0f,0.0f}},
		{{cubeSize_2, cubeSize_2, cubeSize_2},{0.0f, 1.0f},{1.0f,0.0f,0.0f}},
		// Top face
		{{cubeSize_2, cubeSize_2, -cubeSize_2},{0.0f, 0.0f},{0.0f,1.0f,0.0f}},
		{{-cubeSize_2, cubeSize_2, -cubeSize_2},{1.0f, 0.0f},{0.0f,1.0f,0.0f}},
		{{-cubeSize_2, cubeSize_2, cubeSize_2},{1.0f, 1.0f},{0.0f,1.0f,0.0f}},
		{{cubeSize_2, cubeSize_2, cubeSize_2},{0.0f, 1.0f},{0.0f,1.0f,0.0f}},
		// Bottom face
		{{-cubeSize_2, -cubeSize_2, -cubeSize_2},{0.0f, 0.0f},{0.0f,-1.0f,0.0f}},
		{{cubeSize_2, -cubeSize_2, -cubeSize_2},{1.0f, 0.0f},{0.0f,-1.0f,0.0f}},
		{{cubeSize_2, -cubeSize_2, cubeSize_2},{1.0f, 1.0f},{0.0f,-1.0f,0.0f}},
		{{-cubeSize_2, -cubeSize_2, cubeSize_2},{0.0f, 1.0f},{0.0f,-1.0f,0.0f}}
		};
	
	GLsizei vertSize = sizeof(vertices[0]);
	GLsizei numVertices = sizeof(vertices)/vertSize;
	GLuint triangleVBO = vboCreate(vertices, numVertices);
	if(!triangleVBO) {
		//Failes. Error message has already been printed, so just quit
		return EXIT_FAILURE;
	}
	
	// Generate the index array
	
	const GLsizei vertsPerSide = 4;
	const GLsizei numSides = 6;
	const GLsizei indicesPerSide = 6;
	const GLsizei numIndices = indicesPerSide *numSides;
	GLushort indices[numIndices];
	GLuint i = 0;
	for (GLushort j = 0; j < numSides; ++j) {
		GLushort sideBaseIdx = j * vertsPerSide;
		indices[i++] = sideBaseIdx + 0;
		indices[i++] = sideBaseIdx + 1;
		indices[i++] = sideBaseIdx + 2;
		indices[i++] = sideBaseIdx + 2;
		indices[i++] = sideBaseIdx + 3;
		indices[i++] = sideBaseIdx + 0;
	}
	
	GLuint ibo = iboCreate(indices, numIndices);
	if(!ibo){
		// Failed. Error message has already been printed, so just quit
		return EXIT_FAILURE;
	}
	
	// Bind the index array for use
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	
	// Set up for rendering the triangle (activate the VBO)
	
	GLuint positionIdx = 0; // Position is vertex attribute 0
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glVertexAttribPointer(positionIdx, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid *) 0);
	glEnableVertexAttribArray(positionIdx); // Changed 2 for 3 in tutorial4 for the Cube
	
	GLuint texCoordIdx = 1; // TexCoord is vertex attribute 1
	glVertexAttribPointer(texCoordIdx, 2, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (const GLvoid*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(texCoordIdx);
	
	GLuint normalIdx = 2; // Normal is vertex attribute 2
	glVertexAttribPointer(normalIdx, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(normalIdx);
	
	// Set the object's pose
	
	glm::mat4 modelMat = glm::rotate(glm::mat4(1.0f), (float)M_PI / 4, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMat = glm::rotate(modelMat, (float)M_PI / 4, glm::vec3(0.0f, 1.0f, 0.0f));
	
	// Set up the camera
	
	// NOTE: OpenGL cameras look down the negative z-axis
	float camPosX = 0.0f;
	float camPosY = 0.0f;
	float camPosZ = 150.0f;
	glm::mat4 viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(-camPosX, -camPosY, -camPosZ));
	
	glm::mat4 projMat = glm::perspective(glm::radians(60.0f), (float)DISP_WIDTH / (float)DISP_HEIGHT, 1.0f, 1000.0f);
	
	// Set up the light
	
	// NOTE: AmbientCol and diffuseCol are the combined colour of both the light and the object's material properties
	glm::vec3 lightPos(camPosX + 50.0f, camPosY + 80.0f, camPosZ);
	glm::vec3 ambientCol(0.15f, 0.15f, 0.15f);
	glm::vec3 diffuseCol(1.2f, 1.2f, 1.2f);
	
	// Upload the shader uniforms
	glm::mat4 mvMat = viewMat * modelMat;
	glm::mat4 normalMat = glm::inverseTranspose(mvMat);
	glUniformMatrix4fv(mvMatLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(normalMat));
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
	glUniform3fv(ambientColLoc, 1, glm::value_ptr(ambientCol));
	glUniform3fv(diffuseColLoc, 1, glm::value_ptr(diffuseCol));
	
	// Now draw!
	
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, (GLvoid*)0);
	
	// Update the window
	
	SDL_GL_SwapWindow(window);
	
	// Cleanup
	
	vboFree(triangleVBO);
	triangleVBO = 0;
	shaderProgDestroy(shaderProg);
	shaderProg = 0;
	texDestroy(texture); // Delete texture
	texture = 0;
	iboFree(ibo);
	ibo = 0;
	
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
