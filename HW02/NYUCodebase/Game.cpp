#include "Game.h"

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

Game::Game() :
lastFrameTicks(0.0f), paddleProp(4), done(false)
{ 
	Setup(); 
} 
void Game::Setup(){
	// SDL and OpenGL initialization 
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	program = &ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	blank = LoadTexture("blank.png");
	//textures.push_back(blank);

	projectionMatrix.setOrthoProjection(-1.7777f, 1.7777f, -1.0f, 1.0f, -1.0f, 1.0f);
	portWidth = 640;
	portHeight = 320;

	//lastFrameTicks = 0.0f;
	/*
	ballProp[0] = 0.0f; //ballPosx = 0.0f;
	ballProp[1] = 0.0f; //ballPosy = 0.0f;
	ballProp[2] = 180.0f; //ballAngle = 180.0f;
	ballProp[3] = 1.0f; //ballSpeed = 10.0f;
	*/
	ballProp.push_back(0);
	ballProp.push_back(0);
	ballProp.push_back(180);
	ballProp.push_back(1);

}
Game::~Game() { 
	// SDL and OpenGL cleanup (joysticks, textures, etc). 
} 
void Game::Render() {  
	// clear, render and swap the window
	
	//ball render
	float verticesBall[] = { -0.03f, -0.03f,
		0.03f, 0.03f,
		-0.03f, 0.03f,
		0.03f, 0.03f,
		-0.03f, -0.03f,
		0.03f, -0.03f };
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, verticesBall);
	glEnableVertexAttribArray(program->positionAttribute);

	float texCoordsBall[] = { 0.0, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0 };
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordsBall);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, blank); //textures[0]);

	modelMatrix.identity();
	//modelMatrix.Translate(ballProp[0], ballProp[1], 0.0);
	program->setModelMatrix(modelMatrix);
	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
	/*
	float verticesPad1[] = { -0.1f, -0.3f,
		0.0f, 0.05f,
		-0.1f, 0.05f,
		0.0f, 0.05f,
		-0.1f, -0.3f,
		0.0f, -0.3f };
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, verticesPad1);
	glEnableVertexAttribArray(program->positionAttribute);

	float texCoordsPad1[] = { 0.0, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0 };
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordsPad1);
	glEnableVertexAttribArray(program->texCoordAttribute);


	modelMatrix.identity();
	modelMatrix.Translate(-1.5, 0, 0.0);
	program->setModelMatrix(modelMatrix);
	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);

	float verticesPad2[] = { 0.1f, -0.3f,
		0.0f, 0.05f,
		0.1f, 0.05f,
		0.0f, 0.05f,
		0.1f, -0.3f,
		0.0f, -0.3f };
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, verticesPad2);
	glEnableVertexAttribArray(program->positionAttribute);

	float texCoordsPad2[] = { 0.0, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0 };
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordsPad2);
	glEnableVertexAttribArray(program->texCoordAttribute);


	modelMatrix.identity();
	modelMatrix.Translate(1.5, 0, 0.0);
	program->setModelMatrix(modelMatrix);
	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
	*/
	SDL_GL_SwapWindow(displayWindow);
	glClear(GL_COLOR_BUFFER_BIT);

} 
void Game::ProcessEvents() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;         
		} 
		// check for input events     
	} 
} 
void Game::Update(float elapsed) { 
	// move things based on time passed
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	float dist = elapsed*ballProp[3];
	ballProp[0] += cos(ballProp[2] * PI / 180)*dist;
	ballProp[1] += sin(ballProp[2] * PI / 180)*dist;

	if (ballProp[0] <= -1.8){
		//right victory
		ballProp[0] = 0;
		ballProp[1] = 0;
		ballProp[2] = 0;
	}
	else if (ballProp[0] >= 1.8){
		ballProp[0] = 0;
		ballProp[1] = 0;
		ballProp[2] = 180;
	}

	// check for collisions and respond to them 
	if (ballProp[1] >= 1){
		ballProp[2] = -1 * ballProp[2];
		ballProp[1] = 1;
	}
	else if (ballProp[1] <= -1){
		ballProp[2] = -1 * ballProp[2];
		ballProp[1] = -1;
	}

} 
bool Game::UpdateAndRender() {
	float ticks = (float)SDL_GetTicks()/1000.0f;     
	float elapsed = ticks - lastFrameTicks;     
	lastFrameTicks = ticks;        
	ProcessEvents();     
	Update(elapsed);     
	Render();         
	return done; 
}