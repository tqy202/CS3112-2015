#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>

class SheetSprite {
public:
	SheetSprite();
	SheetSprite(unsigned int textureID, float u, float v, float width, float height, float size);
	void Draw();
	float size;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
};

class Texture{
public:
	Texture(GLuint &tex);
	Texture(GLuint &tex, unsigned int height, unsigned int width);

private:
	std::vector<SheetSprite*> sheetSprites;
	GLuint texture;
};