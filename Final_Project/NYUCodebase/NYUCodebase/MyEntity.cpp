#include "MyEntity.h"

void Entity::Render(ShaderProgram *program){
	float vertex[] = { xpos - (.5 * iwidth), ypos + (.5 * iheight),
		xpos - (.5 * iwidth), ypos - (.5 * iheight),
		xpos + (.5 * iwidth), ypos - (.5 * iheight),
		xpos + (.5 * iwidth), ypos - (.5 * iheight),
		xpos + (.5 * iwidth), ypos + (.5 * iheight),
		xpos - (.5 * iwidth), ypos + (.5 * iheight)
	};
	//float texCoordData[] = {};
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertex);
	glEnableVertexAttribArray(program->positionAttribute);
	//glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData);
	//glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program->positionAttribute);
	//glDisableVertexAttribArray(program->texCoordAttribute);

}
Entity::Entity(float x, float y, float width, float height){
	xpos = x;
	ypos = y;
	iwidth = width;
	iheight = height;
	velocityXPos = 0.0f;
	velocityYPos = 0.0f;
}
void Entity::Update(float elapsed){
	xpos += velocityXPos * elapsed;
	ypos += velocityYPos * elapsed;

}