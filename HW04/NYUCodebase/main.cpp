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

class Texture;
class Entity;

SDL_Window* displayWindow;
enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_GAME_OVER };
enum CollisionState { COLLISION_TOP, COLLISION_BOTTOM, COLLISION_LEFT, COLLISION_RIGHT };
float grav = 0.1;
float viewX;
float viewY;
Matrix projectionMatrix;
//Matrix modelMatrix;
Matrix viewMatrix;


std::list<Entity> nuts;
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
	Entity(float x1, float y1, float h, float w, float s, size_t tex, int index, Texture &sheet, bool sta = false) :
		x(x1), y(y1), height(h), width(w), speed(s), texture(tex), stat(sta) {
		matrix.identity();
		u = (float)(((int)index) % sheet.spriteCountX) / (float)sheet.spriteCountX; 
		v = (float)(((int)index) / sheet.spriteCountX) / (float)sheet.spriteCountY;
		spriteWidth = 1.0 / (float)sheet.spriteCountX;
		spriteHeight = 1.0 / (float)sheet.spriteCountY;

		collsion[0] = false;
		collsion[1] = false;
		collsion[2] = false;
		collsion[3] = false;
	}
	~Entity(){}
	//private:
	float x;
	float y;
	float u;
	float v;
	float spriteWidth;
	float spriteHeight;
	float speed;
	float height;
	float width;
	bool collsion[4];
	bool stat;
	size_t texture;
	Matrix matrix;
};

void gravity(Entity &ent){
	ent.y -= grav;
}

void collsionTest(Entity &ent){
	for (Entity &bast : nuts){
		if (ent.y + ent.height / 2 >= bast.y - bast.height / 2){
			ent.y = bast.y - bast.height / 2 - ent.height / 2;
			ent.collsion[COLLISION_RIGHT] = true;
		}
		else
			ent.collsion[COLLISION_RIGHT] = false;

		if (ent.y - ent.height / 2 <= bast.y + bast.height / 2){
			ent.y = bast.y + bast.height / 2 + ent.height / 2;
			ent.collsion[COLLISION_LEFT] = true;
		}
		else
			ent.collsion[COLLISION_LEFT] = false;

		if (ent.x + ent.width / 2 >= bast.x - bast.width / 2){
			ent.x = bast.x - bast.width / 2 - ent.width / 2;
			ent.collsion[COLLISION_TOP] = true;
		}
		else
			ent.collsion[COLLISION_TOP] = false;

		if (ent.x - ent.width / 2 <= bast.x + bast.width / 2){
			ent.x = ent.width / 2 + bast.x + bast.width / 2;
			ent.collsion[COLLISION_BOTTOM] = true;
		}
		else
			ent.collsion[COLLISION_BOTTOM] = false;

	}
}

void updateGame(){
	for (std::list<Entity>::iterator itr = nuts.begin(); itr != nuts.end(); itr++){
		if (itr->x + itr->width / 2 < viewX - 4.0 && itr->y + itr->height / 2 - viewY - 3.0)
			itr = nuts.erase(itr);
	}
	for (Entity &ent : entities){
		if (!ent.stat && !ent.collsion[COLLISION_BOTTOM])
			gravity(ent);
	}
	//if (entities.front()){
		if (entities.at(0).x > viewX){
			viewMatrix.Translate(viewX - entities.at(0).x, 0, 0);
			viewX = entities.at(0).x;
		}
		if (entities.at(0).y > viewY){
			viewMatrix.Translate(0, viewY - entities.at(0).y, 0);
			viewY = entities.at(0).y;
		}
	//}
	
}

void setupGame(){
	srand(time(NULL));
	viewMatrix.identity();
	viewX = 0;
	viewY = 0;
	entities.push_back(Entity(0,0, 0.1, 0.05, 0.1, 1, 0, textures.at(1), true));
	nuts.push_back(Entity(0.1, -0.1, 0.2, 1.0, 0, 1, 3, textures.at(1)));

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
	
	projectionMatrix.setOrthoProjection(-2.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);

	float portWidth = 640;
	float portHeight = 360;

	int state = STATE_GAME_LEVEL;
	float lastFrameTicks = 0.0f;
	bool done = false;

	textures.push_back(Texture("font1.png", 16, 16));
	textures.push_back(Texture("blanks.png", 2, 2));
	setupGame();

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
				if (keys[SDL_SCANCODE_D]){
					entities.at(0).x += entities.at(0).speed * elapsed;
				}
				else if (keys[SDL_SCANCODE_A]){
					entities.at(0).x = entities.at(0).x - entities.at(0).speed * elapsed;
				}
				break;
			case STATE_GAME_OVER:
				break;
		}
//update

		switch (state) { 
			case STATE_MAIN_MENU:
				break; 
			case STATE_GAME_LEVEL:
				//updateGame();
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
				for (Entity &ent : nuts){
					float vertices[] = { -1 * ent.width, -1 * ent.height, ent.width, ent.height, -1 * ent.width, 
						ent.height, ent.width, ent.height, -1 * ent.width, -1 * ent.height, ent.width, -1 * ent.height };

					GLfloat texCoords[] = { ent.u, ent.v + ent.spriteHeight, ent.u + ent.spriteWidth, ent.v, ent.u, 
						ent.v, ent.u + ent.spriteWidth, ent.v, ent.u, ent.v + ent.spriteHeight, ent.u + 
						ent.spriteWidth, ent.v + ent.spriteHeight };

					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
					glEnableVertexAttribArray(program.positionAttribute);
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
					glEnableVertexAttribArray(program.texCoordAttribute);
					glBindTexture(GL_TEXTURE_2D, textures[ent.texture].sheet);
					program.setModelMatrix(ent.matrix);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);
				}
				for (Entity &ent : entities){
					float vertices[] = { -1 * ent.width, -1 * ent.height, ent.width, ent.height, -1 * ent.width,
						ent.height, ent.width, ent.height, -1 * ent.width, -1 * ent.height, ent.width, -1 * ent.height };

					GLfloat texCoords[] = { ent.u, ent.v + ent.spriteHeight, ent.u + ent.spriteWidth, ent.v, ent.u,
						ent.v, ent.u + ent.spriteWidth, ent.v, ent.u, ent.v + ent.spriteHeight, ent.u +
						ent.spriteWidth, ent.v + ent.spriteHeight };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
					glEnableVertexAttribArray(program.positionAttribute);
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
					glEnableVertexAttribArray(program.texCoordAttribute);
					glBindTexture(GL_TEXTURE_2D, textures[ent.texture].sheet);
					program.setModelMatrix(ent.matrix);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);
				}
				break;
			case STATE_GAME_OVER:
				break;
		}


		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}