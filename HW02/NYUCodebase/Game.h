#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include "ShaderProgram.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#ifndef PI
#define PI 3.14159265
#endif

GLuint LoadTexture(const char *image_path);

/*class Entity {

public:         
	void Draw();         
	float x;         
	float y;         
	float rotation;         
	int textureID;                  
	float width;         
	float height;         
	float speed;         
	float direction_x;         
	float direction_y; 
};*/

class Game { 
public:              
	Game();         
	~Game();              
	void Setup();       
	void ProcessEvents();           
	bool UpdateAndRender();              
	void Render();         
	void Update(float elapsed);          
private:              
	bool done;         
	float lastFrameTicks;
	float portWidth;
	float portHeight;

	float ballPosx;
	float ballPosy;
	float ballAngle;
	float ballSpeed;
	GLint blank;

	SDL_Window* displayWindow;       
	Matrix projectionMatrix;         
	Matrix modelMatrix;         
	Matrix viewMatrix;
	ShaderProgram *program;
	SDL_Event event;
	//std::vector<GLint> textures;
	std::vector<float> ballProp;
	std::vector<float> paddleProp;
	//vector<Entity> entities;
};