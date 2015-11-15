#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <vector>
#include <string>
#include "SheetSprite.h"
#include "MyEntity.h"

enum GameState { STATE_MENU, STATE_GAME, STATE_END };
GLuint LoadTexture(const char *image_path);
class ClassDemoApp {
public:
	ClassDemoApp();
	~ClassDemoApp();
	void Setup();
	void ProcessEvents();
	bool UpdateAndRender();
	void Render();
	void Update(float elapsed);
	void DrawText(int fontTexture, std::string text, float size, float spacing);
private:
	float lastFrameTicks;
	SDL_Window* displayWindow;
	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	ShaderProgram *program;
	SDL_Event event;
	
	bool done;
	int state;
	int score;
	int damage;
	bool amAlive;
	Entity* player;
	float timesincelastfire;
	std::vector<Entity*> aliensSprites;
	std::vector<Entity*> Bullets;
	std::vector<GLuint*> textures;
	void RenderMenu();
	void RenderGame();
	void win();
	void lose();
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
};