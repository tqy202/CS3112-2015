#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>


#include "ShaderProgram.h"
#include <vector>
#include <list>
#include <algorithm>
#include <cstdlib>
#include <time.h>
#include <string>

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define FIXED_TIMESTEP 0.0166666f 
#define MAX_TIMESTEPS 6 
float timeLeftOver = 0.0f;

SDL_Window* displayWindow;
enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_GAME_OVER };

std::vector<Entity> entities;
std::vector<Texture> textures;

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

class Texture{
public:
	Texture(const char *image_path, int countX = 1, int countY = 1) :
		spriteCountX(countX), spriteCountY(countY)
	{
		sheet = LoadTexture(image_path);
	}
	~Texture(){}
	int spriteCountX;
	int spriteCountY;
	GLuint sheet;

};

class Entity{
public:
	Entity(float x1, float y1, float h, float w, float s, int tex, int index, Texture &sheet, bool sta = false) :
		x(x1), y(y1), height(h), width(w), speed(s), texture(tex), stat(sta) {
		matrix.identity();
		float u = (float)(((int)index) % sheet.spriteCountX) / (float)sheet.spriteCountX; 
		float v = (float)(((int)index) / sheet.spriteCountX) / (float)sheet.spriteCountY;
		float spriteWidth = 1.0 / (float)sheet.spriteCountX;
		float spriteHeight = 1.0 / (float)sheet.spriteCountY;
		
		vertices[0]= -1 * width;
		vertices[1]= -1 * height;
		vertices[2]= width;
		vertices[3]= height;
		vertices[4]= -1 * width;
		vertices[5]= height;
		vertices[6]= width;
		vertices[7]= height;
		vertices[8]= -1 * width;
		vertices[9]= -1 * height;
		vertices[10]= width;
		vertices[11]= -1 * height;

		vertices[0]= u;
		vertices[1]= v + spriteHeight; 
		vertices[2]= u + spriteWidth;
		vertices[3]= v;
		vertices[4]= u;
		vertices[5]= v;
		vertices[6]= u + spriteWidth;
		vertices[7]= v;
		vertices[8]= u;
		vertices[9]= v + spriteHeight; 
		vertices[10]=u + spriteWidth;
		vertices[11]=v + spriteHeight;
	}
	~Entity(){
		delete texCoords;
		delete vertices;
	}
	//private:
	float x;
	float y;
	float speed;
	float height;
	float width;
	float vertices[12];
	GLfloat texCoords[12];
	bool stat;
	int texture;
	Matrix matrix;
};

void updateGame(){
	
}



int main(int argc, char *argv[])
{
//setup
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	SDL_Event event;
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	projectionMatrix.setOrthoProjection(-2.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);

	float portWidth = 640;
	float portHeight = 360;

	int state = STATE_MAIN_MENU;
	float lastFrameTicks = 0.0f;
	bool done = false;


	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		const Uint8 *keys = SDL_GetKeyboardState(NULL); 

//input
		switch (state) {
			case STATE_MAIN_MENU:
					break;
			case STATE_GAME_LEVEL:
				break;
			case STATE_GAME_OVER:
				break;
		}
//update

		switch (state) { 
			case STATE_MAIN_MENU:
				break; 
			case STATE_GAME_LEVEL:
				break;
			case STATE_GAME_OVER:
				break;
		}

//render
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, portWidth, portHeight);

		switch (state) {
			case STATE_MAIN_MENU:
				break;
			case STATE_GAME_LEVEL:
				break;
			case STATE_GAME_OVER:
				break;
		}


		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}