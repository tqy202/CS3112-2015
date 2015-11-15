#include "SheetSprite.h"
//this is for drawing our arrays
void SheetSprite::Draw() {
	glBindTexture(GL_TEXTURE_2D, textureID);
	GLfloat texCoords[] = {
		u, v + height,
		u + width, v,
		u, v,
		u + width, v,
		u, v + height,
		u + width, v + height
	};
	float aspect = width / height;
	float vertices[] = {
		-0.5f * size * aspect, -0.5f * size,
		0.5f * size * aspect, 0.5f * size,
		-0.5f * size * aspect, 0.5f * size,
		0.5f * size * aspect, 0.5f * size,
		-0.5f * size * aspect, -0.5f * size,
		0.5f * size * aspect, -0.5f * size };
}

Texture::Texture(GLuint &tex) :
texture(tex)
{}
Texture::Texture(GLuint &tex, unsigned int height, unsigned int width):
texture(tex)
{
	
}