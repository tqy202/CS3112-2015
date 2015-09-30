#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Game.h"
#include <math.h>

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif


SDL_Window* displayWindow;


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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	GLuint blank = LoadTexture("blank.png");

	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	projectionMatrix.setOrthoProjection(-1.7777f, 1.7777f, -1.0f, 1.0f, -1.0f, 1.0f);
	float portWidth = 640;
	float portHeight = 320;

	float lastFrameTicks = 0.0f;
	float ballPosx = .0f;
	float ballPosy = 0.0f;
	float ballAngle = 180.0f;
	float ballSpeed = 2.0f;
	float paddlePos1 = 0.0f;
	float paddlePos2 = 0.0f;
	float paddleSpeed = 0.0015f;
	float server = 0.0f;


	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		const Uint8 *keys = SDL_GetKeyboardState(NULL); 
//player1 input
		if (keys[SDL_SCANCODE_W]){
			paddlePos1 += paddleSpeed;
			if (paddlePos1 + 0.2 >= 1)
				paddlePos1 = 1 - 0.2;
		}
		else if (keys[SDL_SCANCODE_S]){
			paddlePos1 += -1 * paddleSpeed;
			if (paddlePos1 - 0.2 <= -1)
			paddlePos1 = -1 + 0.2;
		}
		
//player2 input		
		if (keys[SDL_SCANCODE_O]){
			paddlePos2 += paddleSpeed;
			if (paddlePos2 + 0.2 >= 1)
				paddlePos2 = 1 - 0.2;
		}
		else if (keys[SDL_SCANCODE_L]){
			paddlePos2 += -1 * paddleSpeed;
			if (paddlePos2 - 0.2 <= -1)
				paddlePos2 = -1 + 0.2;
		}

//update
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		if (abs(ballAngle) - 90 <= 20 && abs(ballAngle) -90 >= -20)
			ballSpeed = 4;
		else
			ballSpeed = 2;

		float dist = elapsed*ballSpeed;
		ballPosx += cos(ballAngle * PI / 180)*dist;
		ballPosy += sin(ballAngle * PI / 180)*dist;		



//collison test
		if (ballPosy >= 1){
			ballAngle = -1 * ballAngle;
			ballPosy = 1;
		}
		else if (ballPosy <= -1){
			ballAngle = -1 * ballAngle;
			ballPosy = -1;
		}

		if (ballPosx <= -1.5 && ballPosx >= -1.6 && ballPosy <= paddlePos1 + 0.2 && ballPosy >= paddlePos1 - 0.2){
			if (ballPosy > paddlePos1 + 0.1 || ballPosy < paddlePos1 - 0.1){
				if (ballAngle > 0)
					ballAngle = 180 - ballAngle + 20;
				else
					ballAngle = -180 - ballAngle - 20;
			}
			else if (ballPosy > paddlePos1 + 0.175 || ballPosy < paddlePos1 - 0.175){
				if (ballAngle > 0)
					ballAngle = 180 - ballAngle + 15;
				else
					ballAngle = -180 - ballAngle - 15;
			}
			else {
				if (ballAngle > 0)
					ballAngle = 180 - ballAngle;
				else
					ballAngle = -180 - ballAngle;
			}
			ballPosx = -1.5;
		}
		else if (ballPosx >= 1.5 && ballPosx <= 1.6 && ballPosy <= paddlePos2 + 0.2 && ballPosy >= paddlePos2 - 0.2){
			if (ballPosy > paddlePos2 + 0.1 || ballPosy < paddlePos2 - 0.1){
				if (ballAngle > 0)
					ballAngle = 180 - ballAngle - 20;
				else
					ballAngle = -180 - ballAngle + 20;
			}
			else if (ballPosy > paddlePos2 + 0.175 || ballPosy < paddlePos2 - 0.175){
				if (ballAngle > 0)
					ballAngle = 180 - ballAngle - 15;
				else
					ballAngle = -180 - ballAngle + 15;
			}
			else {
				if (ballAngle > 0)
					ballAngle = 180 - ballAngle;
				else
					ballAngle = -180 - ballAngle;
			}
			ballPosx = 1.5;
		}

//victory condition test
		if (ballPosx <= -1.8){
			ballPosx = 0;
			ballPosy = 0;
			ballAngle = 0;
			paddlePos1 = 0;
			paddlePos2 = 0;
			server = 1;
		}
		else if (ballPosx >= 1.8){
			ballPosx = 0;
			ballPosy = 0;
			ballAngle = 180;
			paddlePos1 = 0;
			paddlePos2 = 0;
			server = -1;
		}


//render
		glViewport(0, 0, portWidth, portHeight);
		float verticesBall[] = { -0.03f, -0.03f, 
								0.03f, 0.03f, 
								-0.03f, 0.03f, 
								0.03f, 0.03f, 
								-0.03f, -0.03f, 
								0.03f, -0.03f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesBall);
		glEnableVertexAttribArray(program.positionAttribute);

		float texCoordsBall[] = { 0.0, 1.0f, 
								1.0f, 0.0f, 
								0.0f, 0.0f, 
								1.0, 0.0, 
								0.0, 1.0, 
								1.0, 1.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordsBall);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, blank);

		modelMatrix.identity();
		modelMatrix.Translate(ballPosx,ballPosy,0.0);
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		float verticesPad1[] = { -0.1f, -0.2f,
			0.0f, 0.2f,
			-0.1f, 0.2f,
			0.0f, 0.2f,
			-0.1f, -0.2f,
			0.0f, -0.2f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesPad1);
		glEnableVertexAttribArray(program.positionAttribute);

		float texCoordsPad1[] = { 0.0, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			1.0, 0.0,
			0.0, 1.0,
			1.0, 1.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordsPad1);
		glEnableVertexAttribArray(program.texCoordAttribute);


		modelMatrix.identity();
		modelMatrix.Translate(-1.5, paddlePos1, 0.0);
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		float verticesPad2[] = { 0.1f, -0.2f,
			0.0f, 0.2f,
			0.1f, 0.2f,
			0.0f, 0.2f,
			0.1f, -0.2f,
			0.0f, -0.2f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesPad2);
		glEnableVertexAttribArray(program.positionAttribute);

		float texCoordsPad2[] = { 0.0, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			1.0, 0.0,
			0.0, 1.0,
			1.0, 1.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordsPad2);
		glEnableVertexAttribArray(program.texCoordAttribute);
		

		modelMatrix.identity();
		modelMatrix.Translate(1.5, paddlePos2, 0.0);
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		if (server){
			float verticesfl[] = { -0.025f, 0.0f,
				0.025f, 0.1f,
				-0.025f, 0.1f,
				0.025f, 0.1f,
				-0.025f, 0.0f,
				0.0f, 0.0f };
			glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesfl);
			glEnableVertexAttribArray(program.positionAttribute);

			float texCoordsfl[] = { 0.0, 1.0f,
				1.0f, 0.0f,
				0.0f, 0.0f,
				1.0, 0.0,
				0.0, 1.0,
				1.0, 1.0 };
			glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordsfl);
			glEnableVertexAttribArray(program.texCoordAttribute);


			modelMatrix.identity();
			modelMatrix.Translate(server, -1, 0.0);
			program.setModelMatrix(modelMatrix);
			program.setProjectionMatrix(projectionMatrix);
			program.setViewMatrix(viewMatrix);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			glDisableVertexAttribArray(program.positionAttribute);
			glDisableVertexAttribArray(program.texCoordAttribute);
		}

		SDL_GL_SwapWindow(displayWindow);
		glClear(GL_COLOR_BUFFER_BIT);

	}


	/*Game app;
	while (!app.UpdateAndRender()) {}*/

	//SDL_Quit();
	return 0;
}
