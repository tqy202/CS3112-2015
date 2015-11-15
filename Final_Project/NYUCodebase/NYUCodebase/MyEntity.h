#include "ShaderProgram.h"
#include "SheetSprite.h"
#include <iostream>
class Entity {
public:
	Entity();
	Entity(float x, float y, float width, float height);
	void Update(float elapsed);
	void Render(ShaderProgram *program);
	//bool collidesWith(Entity *entity);
	SheetSprite* sprite;
	float xpos;
	float ypos;
	float iwidth;
	float iheight;
	float velocityXPos;
	float velocityYPos;
};