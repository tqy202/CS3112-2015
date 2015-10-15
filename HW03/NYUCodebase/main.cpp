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

class Entity{
public:
	Entity(float x1, float y1, float h, float w, float s, int tex) :
		x(x1), y(y1), height(h), width(w), speed(s), texture(tex)
	{}
	~Entity(){}
	//private:
	float x;
	float y;
	float speed;
	float height;
	float width;
	int texture;
};
class Enemy{
public:
	Enemy(float x1, float y1, float h, float w, float s, int tex, int rShot) :
		x(x1), y(y1), height(h), width(w), speed(s), texture(tex), shotTimer(rShot)
	{}
	~Enemy(){}
//private:
	float x;
	float y;
	float speed;
	float height;
	float width;
	int shotTimer;
	int texture;
};
class Bullet{
public:
	Bullet(float x1, float y1, float h, float w, float s, int tex) :
		x(x1), y(y1), height(h), width(w), speed(s), texture(tex)
	{}
	~Bullet(){}
//private:
	float x;
	float y;
	float speed;
	float height;
	float width;
	int texture;
};
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
void buildGame(std::list<Enemy> &enemies, float size = 0.1){
	float xdiv = 1.0 / 5.0;
	srand(time(NULL));
	for (int y = 5; y > 0; y--){
		for (int x = -5; x < 6; x++){
			float x1 = x;
			float y1 = y;
			enemies.push_back(Enemy(x1 * xdiv, 1 - y1/7, size, size, 0.2, 1, rand() % 1000));
		}
	}
}

void updateGameLevel(std::list<Enemy> &enemies, std::list<Bullet> &bullets, std::list<Bullet> &pBullets, bool &reverse, float &minY, float elasped){
	
	minY = 3.0;
	for (std::list<Bullet>::iterator itr = bullets.begin(); itr != bullets.end();){
		itr->y += itr->speed *elasped;
		if (itr->y >= 1)
			itr = bullets.erase(itr);
		else
			itr++;
	}
	if (reverse){
		for (Enemy &x : enemies){
			x.y = x.y - 0.1;
			x.speed = x.speed * -1;
			if (x.speed < 0)
				x.speed = -1 * (0.2 + ((55 - (float)enemies.size()) / 100));
			else
				x.speed = (0.2 + ((55 - (float)enemies.size()) / 100));
			minY = std::min(minY, x.y - x.height / 2);
			if (!x.shotTimer){
				bullets.push_back(Bullet(x.x, x.y - x.height / 2, 0.02, 0.02, -0.75, 0));
				x.shotTimer = 900;
			}
			else
				x.shotTimer -= elasped;
		}
		reverse = false;
	}
	else{
		for (Enemy &x : enemies){
			x.x += x.speed * elasped;
			if (x.x >= 1.9 || x.x <= -1.9)
				reverse = true;
			minY = std::min(minY, x.y - x.height / 2);
			if (!x.shotTimer){
				bullets.push_back(Bullet(x.x, x.y - x.height / 2, 0.02, 0.02, -0.75, 0));
				x.shotTimer = 900;
			}
			else
				x.shotTimer -= elasped;
		}
	}
	
	for (std::list<Bullet>::iterator itr = pBullets.begin(); itr != pBullets.end();){
		itr->y += itr->speed *elasped;
		if (itr->y > 0.9)
			itr = pBullets.erase(itr);
		else
			itr++;
	}
}


bool checkECollison(std::list<Bullet> &bullets, Entity &player){
	float pTop = player.y + player.height / 2;
	float pBot = player.y - player.height / 2;
	float pRight = player.x + player.width / 2;
	float pLeft = player.x - player.width / 2;
	for (std::list<Bullet>::iterator itrE = bullets.begin(); itrE != bullets.end(); itrE++){
		float left = itrE->x - itrE->width / 2;
		float right = itrE->x + itrE->width / 2;
		float top = itrE->y + itrE->height / 2;
		float bot = itrE->y - itrE->height / 2;
		if (bot <= pTop && bot >= pBot && left >= pLeft && right <= pRight){
			bullets.erase(itrE);
			return true;
		}
	}
	return false;
}

bool checkPCollison(std::list<Bullet> &pBullets, std::list<Enemy> &enemies, std::list<Bullet> &bullets, int &score){
	for (std::list<Bullet>::iterator itr = pBullets.begin(); itr != pBullets.end();){
		float left = itr->x - itr->width / 2;
		float right = itr->x + itr->width / 2;
		float top = itr->y + itr->height / 2;
		float bot = itr->y - itr->height / 2;
		bool collison = false;
			for (std::list<Enemy>::iterator itrE = enemies.begin(); itrE != enemies.end(); itrE++){
				float leftE = itrE->x - itrE->width / 2;
				float rightE = itrE->x + itrE->width / 2;
				float topE = itrE->y + itrE->height / 2;
				float botE = itrE->y - itrE->height / 2;
				if (botE <= top && rightE >= left && leftE <= right && topE >= bot){
					enemies.erase(itrE);
					itr = pBullets.erase(itr);
					score++;
					collison = true;
					break;
				}
			}
		if (!collison){
			for (std::list<Bullet>::iterator itrE = bullets.begin(); itrE != bullets.end(); itrE++){
				float leftE = itrE->x - itrE->width / 2;
				float rightE = itrE->x + itrE->width / 2;
				float topE = itrE->y + itrE->height / 2;
				float botE = itrE->y - itrE->height / 2;
				if (botE <= top && rightE >= left && leftE <= right){
					bullets.erase(itrE);
					itr = pBullets.erase(itr);
					collison = true;
					break;
				}
			}
		}
		if (!collison)
			itr++;
	}
	if (!enemies.size())
		return true;
	return false;
}

void update(std::list<Enemy> &enemies, std::list<Bullet> &bullets, std::list<Bullet> &pBullets, Entity &player, int &lives, int &lifeIndex, int &state, bool &reverse, int &score, float elapsed){
	float minY = 3.0;
	updateGameLevel(enemies, bullets, pBullets, reverse, minY, elapsed);
	if (minY <= -0.9)
		state = STATE_GAME_OVER;
	if (pBullets.size()){
		if (checkPCollison(pBullets, enemies, bullets, score)){
			buildGame(enemies);
		}
	}
	if (bullets.size()){
		if (checkECollison(bullets, player)){
			lives--;
			lifeIndex--;
			if (!lives)
				state = STATE_GAME_OVER;
		}
	}
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

	std::vector<GLuint> textures;
	textures.push_back(LoadTexture("blankW.png"));
	textures.push_back(LoadTexture("blankR.png"));
	textures.push_back(LoadTexture("blankG.png"));
	textures.push_back(LoadTexture("blankB.png"));
	textures.push_back(LoadTexture("font1.png"));

	std::list<Enemy> enemies;
	std::list<Bullet> bullets;
	std::list<Bullet> pBullets;
	Entity player(0.0f, -0.9f, 0.1f, 0.1f, 1.0f, 2);

	int maxPBullets = 5;
	int maxEBullets = -1;
	int lives = 5;
	bool reverse = false;
	int score = 0;
	

	int spriteCountX = 16; 
	int spriteCountY = 16; 
	int lifeIndex = '5';

	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			else if (event.type == SDL_KEYDOWN) {
				switch (state) {
				case STATE_MAIN_MENU:
					state = STATE_GAME_LEVEL;
					buildGame(enemies);
					lives = 3;
					score = 0;
					lifeIndex = '3';
					break;
				case STATE_GAME_LEVEL:
					if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
						if (pBullets.size() < maxPBullets)
							pBullets.push_back(Bullet(player.x, player.y + player.height / 2, 0.02, 0.02, 0.75, 3));
					}
					break;
				case STATE_GAME_OVER:
					if (event.key.keysym.scancode == SDL_SCANCODE_RETURN){
						state = STATE_MAIN_MENU;
						bullets.clear();
						enemies.clear();
						pBullets.clear();
					}
					break;
				}    
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
					player.x += player.speed * elapsed;
					if (player.x + player.width / 2 >= 2.0)
						player.x = 2.0 - player.width / 2;
				}
				else if (keys[SDL_SCANCODE_A]){
					player.x = player.x - player.speed * elapsed;
					if (player.x - player.width / 2 <= -2.0)
						player.x = -2.0 + player.width / 2;
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
				while (elapsed >= FIXED_TIMESTEP) {
					elapsed -= FIXED_TIMESTEP;
					update(enemies, bullets, pBullets, player, lives, lifeIndex, state, reverse, score, FIXED_TIMESTEP);
				}
				if (elapsed <= FIXED_TIMESTEP)
					update(enemies, bullets, pBullets, player, lives, lifeIndex, state, reverse, score, elapsed);
				break;
			case STATE_GAME_OVER:
				break;
		}

//render
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, portWidth, portHeight);

		switch (state) {
			case STATE_MAIN_MENU:
				if (true){
					std::string line1 = "Space";
					std::string line2 = "Invaders";
					for (int x = 0; x < line1.size(); x++){
						int index = line1.at(x);
						float height = 0.15;
						float width = 0.15;
						float g = ((float)x + 1) * 2.0 / ((float) line1.size() + 1)  - 1.0;
						float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX; float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
						float spriteWidth = 1.0 / (float)spriteCountX;
						float spriteHeight = 1.0 / (float)spriteCountY;

						float vertices[] = { -1 * width, -1 * height,
							width, height,
							-1 * width, height,
							width, height,
							-1 * width, -1 * height,
							width, -1 * height };
						glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
						glEnableVertexAttribArray(program.positionAttribute);

						GLfloat texCoords[] = { u, v + spriteHeight, u + spriteWidth, v, u, v, u + spriteWidth, v, u, v + spriteHeight, u + spriteWidth, v + spriteHeight };
						glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
						glEnableVertexAttribArray(program.texCoordAttribute);
						glBindTexture(GL_TEXTURE_2D, textures[4]);

						modelMatrix.identity();
						modelMatrix.Translate(g, .11, 0.0);
						program.setModelMatrix(modelMatrix);
						program.setProjectionMatrix(projectionMatrix);
						program.setViewMatrix(viewMatrix);

						glDrawArrays(GL_TRIANGLES, 0, 6);
						glDisableVertexAttribArray(program.positionAttribute);
						glDisableVertexAttribArray(program.texCoordAttribute);
					}
					for (int x = 0; x < line2.size(); x++){
						int index = line2.at(x);
						float height = 0.15;
						float width = 0.15;
						float g = ((float)x + 1) * 2.0 / ((float) line2.size() + 1) - 1.0;
						float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX; float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
						float spriteWidth = 1.0 / (float)spriteCountX;
						float spriteHeight = 1.0 / (float)spriteCountY;

						float vertices[] = { -1 * width, -1 * height,
							width, height,
							-1 * width, height,
							width, height,
							-1 * width, -1 * height,
							width, -1 * height };
						glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
						glEnableVertexAttribArray(program.positionAttribute);

						GLfloat texCoords[] = { u, v + spriteHeight, u + spriteWidth, v, u, v, u + spriteWidth, v, u, v + spriteHeight, u + spriteWidth, v + spriteHeight };
						glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
						glEnableVertexAttribArray(program.texCoordAttribute);
						glBindTexture(GL_TEXTURE_2D, textures[4]);

						modelMatrix.identity();
						modelMatrix.Translate(g, -0.11, 0.0);
						program.setModelMatrix(modelMatrix);
						program.setProjectionMatrix(projectionMatrix);
						program.setViewMatrix(viewMatrix);

						glDrawArrays(GL_TRIANGLES, 0, 6);
						glDisableVertexAttribArray(program.positionAttribute);
						glDisableVertexAttribArray(program.texCoordAttribute);
					}
				}
				break;
			case STATE_GAME_LEVEL:
				for (const Enemy &x : enemies){
					float height = x.height/2;
					float width = x.width / 2;
					float vertices[] = { -1 * width, -1 * height,
						width, height,
						-1 * width, height,
						width, height,
						-1 * width, -1 * height,
						width, -1 * height };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
					glEnableVertexAttribArray(program.positionAttribute);

					float texCoords[] = { 0.0, 1.0f,
						1.0f, 0.0f,
						0.0f, 0.0f,
						1.0, 0.0,
						0.0, 1.0,
						1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
					glEnableVertexAttribArray(program.texCoordAttribute);

					glBindTexture(GL_TEXTURE_2D, textures[x.texture]);

					modelMatrix.identity();
					modelMatrix.Translate(x.x, x.y, 0.0);
					program.setModelMatrix(modelMatrix);
					program.setProjectionMatrix(projectionMatrix);
					program.setViewMatrix(viewMatrix);

					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);
				}
				for (Bullet x : bullets){
					float height = x.height / 2;
					float width = x.width / 2;
					float vertices[] = { -1 * width, -1 * height,
						width, height,
						-1 * width, height,
						width, height,
						-1 * width, -1 * height,
						width, -1 * height };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
					glEnableVertexAttribArray(program.positionAttribute);

					float texCoords[] = { 0.0, 1.0f,
						1.0f, 0.0f,
						0.0f, 0.0f,
						1.0, 0.0,
						0.0, 1.0,
						1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
					glEnableVertexAttribArray(program.texCoordAttribute);

					glBindTexture(GL_TEXTURE_2D, textures[x.texture]);

					modelMatrix.identity();
					modelMatrix.Translate(x.x, x.y, 0.0);
					program.setModelMatrix(modelMatrix);
					program.setProjectionMatrix(projectionMatrix);
					program.setViewMatrix(viewMatrix);

					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);
				}
				for (Bullet x : pBullets){
					float height = x.height / 2;
					float width = x.width / 2;
					float vertices[] = { -1 * width, -1 * height,
						width, height,
						-1 * width, height,
						width, height,
						-1 * width, -1 * height,
						width, -1 * height };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
					glEnableVertexAttribArray(program.positionAttribute);

					float texCoords[] = { 0.0, 1.0f,
						1.0f, 0.0f,
						0.0f, 0.0f,
						1.0, 0.0,
						0.0, 1.0,
						1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
					glEnableVertexAttribArray(program.texCoordAttribute);

					glBindTexture(GL_TEXTURE_2D, textures[x.texture]);

					modelMatrix.identity();
					modelMatrix.Translate(x.x, x.y, 0.0);
					program.setModelMatrix(modelMatrix);
					program.setProjectionMatrix(projectionMatrix);
					program.setViewMatrix(viewMatrix);

					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);
				}
				if (lives > 0){
					float height = player.height / 2;
					float width = player.width / 2;
					float vertices[] = { -1 * width, -1 * height,
						width, height,
						-1 * width, height,
						width, height,
						-1 * width, -1 * height,
						width, -1 * height };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
					glEnableVertexAttribArray(program.positionAttribute);

					float texCoords[] = { 0.0, 1.0f,
						1.0f, 0.0f,
						0.0f, 0.0f,
						1.0, 0.0,
						0.0, 1.0,
						1.0, 1.0 };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
					glEnableVertexAttribArray(program.texCoordAttribute);

					glBindTexture(GL_TEXTURE_2D, textures[player.texture]);

					modelMatrix.identity();
					modelMatrix.Translate(player.x, player.y, 0.0);
					program.setModelMatrix(modelMatrix);
					program.setProjectionMatrix(projectionMatrix);
					program.setViewMatrix(viewMatrix);

					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);


					
					float u = (float)(((int)lifeIndex) % spriteCountX) / (float)spriteCountX; float v = (float)(((int)lifeIndex) / spriteCountX) / (float)spriteCountY;
					float spriteWidth = 1.0 / (float)spriteCountX;
					float spriteHeight = 1.0 / (float)spriteCountY;

					float heightL = 0.1;
					float widthL = 0.1;
					float verticesL[] = { -1 * widthL, -1 * heightL,
						widthL, heightL,
						-1 * widthL, heightL,
						widthL, heightL,
						-1 * widthL, -1 * heightL,
						widthL, -1 * heightL };
					glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesL);
					glEnableVertexAttribArray(program.positionAttribute);

					GLfloat texCoordsL[] = { u, v + spriteHeight, u + spriteWidth, v, u, v, u + spriteWidth, v, u, v + spriteHeight, u + spriteWidth, v + spriteHeight };
					glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordsL);
					glEnableVertexAttribArray(program.texCoordAttribute);
					

					glBindTexture(GL_TEXTURE_2D, textures[4]);

					modelMatrix.identity();
					modelMatrix.Translate(-1.95, 0.95, 0.0);
					program.setModelMatrix(modelMatrix);
					program.setProjectionMatrix(projectionMatrix);
					program.setViewMatrix(viewMatrix);

					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(program.positionAttribute);
					glDisableVertexAttribArray(program.texCoordAttribute);
				}
				break;
			case STATE_GAME_OVER:
				if (score){
					std::string num = std::to_string(score);
					if (num.size() < 7)
						num.insert(0, 7 - num.size(), '0');
					for (int x = 0; x < 7; x++){
						int index = num.at(x);
						float height = 0.15;
						float width = 0.15;
						float g = ((float)x + 1) * 2.5 / 8.0 - 1.25;
						float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX; float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
						float spriteWidth = 1.0 / (float)spriteCountX;
						float spriteHeight = 1.0 / (float)spriteCountY;

						float vertices[] = { -1 * width, -1 * height,
							width, height,
							-1 * width, height,
							width, height,
							-1 * width, -1 * height,
							width, -1 * height };
						glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
						glEnableVertexAttribArray(program.positionAttribute);

						GLfloat texCoords[] = { u, v + spriteHeight, u + spriteWidth, v, u, v, u + spriteWidth, v, u, v + spriteHeight, u + spriteWidth, v + spriteHeight };
						glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
						glEnableVertexAttribArray(program.texCoordAttribute);
						glBindTexture(GL_TEXTURE_2D, textures[4]);

						modelMatrix.identity();
						modelMatrix.Translate(g, 0, 0.0);
						program.setModelMatrix(modelMatrix);
						program.setProjectionMatrix(projectionMatrix);
						program.setViewMatrix(viewMatrix);

						glDrawArrays(GL_TRIANGLES, 0, 6);
						glDisableVertexAttribArray(program.positionAttribute);
						glDisableVertexAttribArray(program.texCoordAttribute);
					}
				}
				else{
					for (int x = 1; x <= 7; x++){
						int index = '0';
						float height = 0.15;
						float width = 0.15;
						float g = (float) x * 2.5 / 8.0 - 1.25;
						float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX; float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
						float spriteWidth = 1.0 / (float)spriteCountX;
						float spriteHeight = 1.0 / (float)spriteCountY;

						float vertices[] = { -1 * width, -1 * height,
							width, height,
							-1 * width, height,
							width, height,
							-1 * width, -1 * height,
							width, -1 * height };
						glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
						glEnableVertexAttribArray(program.positionAttribute);

						GLfloat texCoords[] = { u, v + spriteHeight, u + spriteWidth, v, u, v, u + spriteWidth, v, u, v + spriteHeight, u + spriteWidth, v + spriteHeight };
						glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
						glEnableVertexAttribArray(program.texCoordAttribute);
						glBindTexture(GL_TEXTURE_2D, textures[4]);

						modelMatrix.identity();
						modelMatrix.Translate(g, 0, 0.0);
						program.setModelMatrix(modelMatrix);
						program.setProjectionMatrix(projectionMatrix);
						program.setViewMatrix(viewMatrix);

						glDrawArrays(GL_TRIANGLES, 0, 6);
						glDisableVertexAttribArray(program.positionAttribute);
						glDisableVertexAttribArray(program.texCoordAttribute);
					}
				}
				break;
		}


		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}