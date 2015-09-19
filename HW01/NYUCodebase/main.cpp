#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include <io.h>

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

GLuint LoadTexture(const char *image_path) { 
	SDL_Surface *surface = IMG_Load(image_path);          
	GLuint textureID;    
	glGenTextures(1, &textureID);     
	glBindTexture(GL_TEXTURE_2D, textureID);          
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);          
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);     
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);         
	SDL_FreeSurface(surface);      
	return textureID; 
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	SDL_Event event;

	
	bool done = false;

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glUseProgram(program.programID);

	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);

	GLuint birdy1 = LoadTexture("flyFly1.png");
	GLuint birdy2 = LoadTexture("flyFly2.png");
	GLuint cloud = LoadTexture("cloud3.png");

	float lastFrameTicks = 0.0f;
	float round = 0.0f;
	bool alt = true;
	float position = -5;

	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, 640, 360);
		float ticks = (float)SDL_GetTicks() / 1000.0f; 
		float elapsed = ticks - lastFrameTicks; 
		lastFrameTicks = ticks;
	
		round += elapsed;
		if (round > 0.3f){
			alt = !alt;
			round = 0;
		}
		position += elapsed;
		if (position > 5)
			position = -5;

		float verticesc[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesc);
		glEnableVertexAttribArray(program.positionAttribute);


		float texCoordsc[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordsc);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, cloud);

		modelMatrix.identity();

		modelMatrix.Translate(position, 1.0, 0.0);
		modelMatrix.Scale(2.0, 1.0, 1.0);
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);

		glDisableVertexAttribArray(program.texCoordAttribute);

		float vertices1[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices1);
		glEnableVertexAttribArray(program.positionAttribute);


		float texCoords1[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords1);
		glEnableVertexAttribArray(program.texCoordAttribute);
	
		if (!alt)
			glBindTexture(GL_TEXTURE_2D, birdy2);
		else
			glBindTexture(GL_TEXTURE_2D, birdy1);
		
		modelMatrix.identity();

		modelMatrix.Translate(-1.0, 0.2, 0.0);
		modelMatrix.Scale(1.0, 0.8, 1.0);
		program.setModelMatrix(modelMatrix); 
		program.setProjectionMatrix(projectionMatrix); 
		program.setViewMatrix(viewMatrix);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);

		glDisableVertexAttribArray(program.texCoordAttribute);


		float vertices2[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
		glEnableVertexAttribArray(program.positionAttribute);


		float texCoords2[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
		glEnableVertexAttribArray(program.texCoordAttribute);

		if (alt)
			glBindTexture(GL_TEXTURE_2D, birdy2);
		else
			glBindTexture(GL_TEXTURE_2D, birdy1);
		
		modelMatrix.identity();
		modelMatrix.Translate(1.0, -0.1, 0.0);
		modelMatrix.Scale(1.0, 0.8, 1.0);
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);

		glDisableVertexAttribArray(program.texCoordAttribute);


		SDL_GL_SwapWindow(displayWindow);
		glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
		

	}

	SDL_Quit();
	return 0;
}
