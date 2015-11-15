#include "ClassDemoApp.h"
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_FreeSurface(surface);
	return textureID;
}
ClassDemoApp::ClassDemoApp() {
	Setup();
}
void ClassDemoApp::Setup() {
	// SDL and OpenGL initialization
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	lastFrameTicks = 0.0f;
	glViewport(0, 0, 800, 600);
	program = new ShaderProgram(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
	projectionMatrix.setOrthoProjection(-1.33f, 1.33f, -1.0f, 1.0f, -1.0f, 1.0f);
	bool done = false;
	fontTexture = LoadTexture("pixel_font.png");
	spriteSheet = LoadTexture("sprites.png");
	int state = 0;
	amAlive = true;
	score = 0;
	damage = 0;
	timesincelastfire = 0.0f;
	player = new Entity(0.0f, -0.85f, 0.15f, 0.1f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float initialYPosition = 0.7f;
	for (float i = -0.88f; i < .88f; i += .16f){
		aliensSprites.push_back(new Entity(i, initialYPosition, 0.20f, 0.15f));
		aliensSprites.push_back(new Entity(i, initialYPosition - .11f, 0.20f, 0.15f));
		aliensSprites.push_back(new Entity(i, initialYPosition - .22f, 0.20f, 0.15f));
		aliensSprites.push_back(new Entity(i, initialYPosition - .33f, 0.20f, 0.15f));
		aliensSprites.push_back(new Entity(i, initialYPosition - .44f, 0.20f, 0.15f));
	}
	for (Entity* anAlien : aliensSprites){
		anAlien->velocityXPos = 0.3f;
	}
}
ClassDemoApp::~ClassDemoApp() {
	delete player;
	for (GLuint i = 0; i < playerBullets.size(); i++) { delete playerBullets[i]; }
	for (GLuint i = 0; i < aliensSprites.size(); i++) { delete aliensSprites[i]; }
	SDL_Quit();
}
void ClassDemoApp::DrawText(int fontTexture, std::string text, float size, float spacing) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (size_t i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void ClassDemoApp::Render() {
	program->setModelMatrix(modelMatrix);
	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);
	glClear(GL_COLOR_BUFFER_BIT);
	switch (state) {
	case STATE_MENU:
		RenderMenu();
		break;
	case STATE_GAME:
		RenderGame();
		break;
	case STATE_END:
		if (amAlive){ 
			win(); 
		}
		else{ 
			lose(); 
		}
		break;
	}
	SDL_GL_SwapWindow(displayWindow);
}

void ClassDemoApp::ProcessEvents() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
	}
}

void ClassDemoApp::Update(float elapsed) {
	if (keys[SDL_SCANCODE_SPACE] && state == 0){
		state = 1;
	}
	else if (keys[SDL_SCANCODE_SPACE] && state == 2){
		state = 0;
		Setup();
	}
	if (state == 1){
		if (keys[SDL_SCANCODE_LEFT] && player->xpos > -1.25){
			player->xpos -= elapsed * .5f;
		}
		else if (keys[SDL_SCANCODE_RIGHT] && player->xpos < 1.25){
			player->xpos += elapsed * .5f;
		}
		Entity* alien = NULL;
		for (size_t i = 0; i < aliensSprites.size(); i++){
			if (aliensSprites[i] != NULL){
				alien = aliensSprites[i];
				break;
			}
		}
		if (alien == NULL){
			state++;
		}
		else{
			if (alien->velocityXPos< 0 && (alien->xpos < -1.255)){
				for (Entity* j : aliensSprites){
					j->velocityXPos = 0.3f;
					j->ypos -= .05f;
				}
			}
			else if (alien->velocityXPos > 0){
				for (size_t i = 0; i < aliensSprites.size(); i++){
					if (aliensSprites[i] != NULL){
						alien = aliensSprites[i];
					}
				}
				if (alien->xpos > 1.255){
					for (Entity* h : aliensSprites){
						h->velocityXPos = -0.3f;
						h->ypos -= .05f;
					}
				}
			}
		}
		timesincelastfire += elapsed;
		if (keys[SDL_SCANCODE_SPACE]){
			if (timesincelastfire > 0.8){
				//then make a new bullet
				playerBullets.push_back(new Entity(player->xpos, (player->ypos + player->iheight), .03f, player->iheight));
				// then we take the most recent bullet and make it have a velocity in the y direction so it can move upwards
				playerBullets[playerBullets.size() - 1]->velocityYPos = 0.6f;
				//reset the time since the last bullet fired so you can check for that elapsed time again
				timesincelastfire = 0.0f;
			}
		}
		for (size_t i = 0; i < playerBullets.size(); i++){
			if (playerBullets[i]->ypos > 1.3){
				// when the bullet goes out of the screen, let's delete them.
				delete playerBullets[i];
				playerBullets.erase(playerBullets.begin() + i);
			}
		}
		for (size_t i = 0; i < playerBullets.size(); i++){
			Entity* k = playerBullets[i];
			for (size_t j = 0; j < aliensSprites.size(); j++){
				Entity* a = aliensSprites[j];
				// collision detection
				if (!(
					(k->ypos - (k->iheight / 2)) >(a->ypos + (a->iheight / 2)) ||
					(k->ypos + (k->iheight / 2)) < (a->ypos - (a->iheight / 2)) ||
					(k->xpos - (k->iwidth / 2)) > (a->xpos + (a->iwidth / 2)) ||
					(k->xpos + (k->iwidth / 2)) < (a->xpos - (a->iwidth / 2))
					))
				{
					delete playerBullets[i];
					playerBullets.erase(playerBullets.begin() + i);
					delete aliensSprites[j];
					aliensSprites.erase(aliensSprites.begin() + j);
					break;
				}
			
			}
		}
		//update the aliens
		for (Entity* t : aliensSprites){
			t->Update(elapsed);
		}
		//update the bullets
		for (Entity* t : playerBullets){
			t->Update(elapsed);
		}
	}
	// move things based on time passed
	// check for collisions and respond to them
}
bool ClassDemoApp::UpdateAndRender() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	ProcessEvents();
	Update(elapsed);
	Render();
	return done;
}

void ClassDemoApp::RenderMenu(){
	modelMatrix.identity();
	modelMatrix.Translate(-0.69f, 0.7f, 0.0);
	DrawText(fontTexture, "My SpaceInvaders Game", 0.13f, 0.0f);
}

void ClassDemoApp::RenderGame(){
	modelMatrix.identity();
	for (size_t i = 0; i < aliensSprites.size(); i++) {
		aliensSprites[i]->Render(program);
	}
	for (Entity* y : playerBullets){
		y->Render(program);
	}
	player->Render(program);
}
void ClassDemoApp::win(){
	modelMatrix.identity();
	modelMatrix.Translate(-0.6f, 0.4f, 0.0);
	DrawText(fontTexture, "You win", 0.13f, 0.0f);
}
void ClassDemoApp::lose(){
	modelMatrix.identity();
	modelMatrix.Translate(-0.6f, 0.4f, 0.0);
	DrawText(fontTexture, "You lose", 0.13f, 0.0f);
}