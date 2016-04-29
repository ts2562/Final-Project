/*
Timothy Seid
Final Project
Attempt at creating a multiplayer version of the T-Rex jumping game in chrome
example: apps.thecodepost.org/trex/trex.html
As of now this progress report is just single player just to test collision, obstacle spawning, and animations
Will have 3 stages: 1 normal stage, 1 stage with low gravity and 1 stage with obstacles spawning quickly
*/
#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include "SDL_opengles2.h"
#include "vector"
#include <math.h>
using namespace std;

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

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

class Player1 {
public:
	Player1();
	Player1(unsigned int textureID1, float u1, float v1, float width1, float height1, float
		size1) : textureID(textureID1), u(u1), v(v1), width(width1), height(height1), size(size1){
	};
	void Draw(ShaderProgram *p, int index, int spriteCountX, int spriteCountY);
	float getX1();
	float getX2();
	float getX3();
	float getY1();
	float getY2();
	float getY3();
	float size;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
	float velocity_y = 1.5f;
	float acceleration_y = 1.0f;
	float y;
	bool is_jumping = false;
	bool jump_max = false;
	bool isAlive = true;
	Matrix matrix;
};

class Enemy {
public:
	Enemy();
	Enemy(unsigned int textureID1, float u1, float v1, float width1, float height1, float
		size1) : textureID(textureID1), u(u1), v(v1), width(width1), height(height1), size(size1){};
	void Draw(ShaderProgram *p, int index, int spriteCountX, int spriteCountY);
	float getX1();
	float getX2();
	float getX3();
	float getY1();
	float getY2();
	float getY3();
	float size;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
	bool isActive = false;
	Matrix matrix;
	float movement;
	float initPos = 2.5f;
};

int index = 19;
int spriteCountX = 30;
int spriteCountY = 30;
float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
float spriteWidth = 1.0 / (float)spriteCountX;
float spriteHeight = 1.0 / (float)spriteCountY;

float Player1::getX1(){
	return 0.5f;
}
float Player1::getX2(){
	return -0.5f;
}
float Player1::getX3(){
	return 0.5f;
}
float Player1::getY1(){
	return 0.5f;
}
float Player1::getY2(){
	return -0.5f;
}
float Player1::getY3(){
	return -0.5f;
}


void Player1::Draw(ShaderProgram *program, int index, int spriteCountX, int spriteCountY) {
	glBindTexture(GL_TEXTURE_2D, textureID);
	float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
	float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
	float spriteWidth = 1.0 / (float)spriteCountX;
	float spriteHeight = 1.0 / (float)spriteCountY;
	GLfloat texCoords[] = {
		u, v + spriteHeight,
		u + spriteWidth, v,
		u, v,
		u + spriteWidth, v,
		u, v + spriteHeight,
		u + spriteWidth, v + spriteHeight
	};
	float vertices[] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f };
	// draw our arrays
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

//enemy coordinates
int indexE = 169;
int spriteCountXE = 30;
int spriteCountYE = 30;
float uE = (float)(((int)indexE) % spriteCountXE) / (float)spriteCountXE;
float vE = (float)(((int)indexE) / spriteCountXE) / (float)spriteCountYE;
float spriteWidthE = 1.0 / (float)spriteCountXE;
float spriteHeightE = 1.0 / (float)spriteCountYE;

float Enemy::getX1()
{
	return -0.5f + movement + initPos;
}
float Enemy::getX2()
{
	return  0.5f + movement + initPos;
}
float Enemy::getX3()
{
	return -0.5f + movement + initPos;
}
float Enemy::getY1()
{
	return -0.5f;
}
float Enemy::getY2()
{
	return 0.5f;
}
float Enemy::getY3()
{
	return 0.5f;
}
void Enemy::Draw(ShaderProgram *program, int index, int spriteCountX, int spriteCountY) {
	glBindTexture(GL_TEXTURE_2D, textureID);
	float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
	float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
	float spriteWidth = 1.0 / (float)spriteCountX;
	float spriteHeight = 1.0 / (float)spriteCountY;
	GLfloat texCoords[] = {
		u, v + spriteHeight,
		u + spriteWidth, v,
		u, v,
		u + spriteWidth, v,
		u, v + spriteHeight,
		u + spriteWidth, v + spriteHeight
	};
	float vertices[] = { -0.5f + movement + initPos, -0.5f, 0.5f + movement + initPos, 0.5f,
		-0.5f + movement + initPos, 0.5f, 0.5f + movement + initPos, 0.5f, -0.5f + movement + initPos,
		-0.5f, 0.5f + movement + initPos, -0.5};
	// draw our arrays
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}


void DrawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
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

enum GameState {STATE_MAIN_MENU, STATE_GAME_LEVEL};
int state;
void renderMenu(){
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	SDL_Event event;
	bool done = false;
#ifdef _WINDOWS
	glewInit();
#endif

	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	const Uint8 *key = SDL_GetKeyboardState(NULL);
	if (key[SDL_SCANCODE_SPACE]){
		state = STATE_GAME_LEVEL;
	}
	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(program.programID);

	GLuint font = LoadTexture("font2.png");
	string start = "SPACE INVADERS-CLOSE TO START";
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE){
				done = true;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		SDL_GL_SwapWindow(displayWindow);

		DrawText(&program, font, start, 0.3, 1.0);
	}

	SDL_Quit();
}

/*
GLfloat spriteUVs[] = { u, v,
u, v + spriteHeight,
u + spriteWidth, v + spriteHeight,
u + spriteWidth, v
};
*/


const int runAnimation[] = {20, 21, 26, 21};
const int numFrames = 4;
float animationElapsed = 0.0f;
float framesPerSecond = 10.0f;
int currentIndex = 0;

void renderGame()
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	SDL_Event event;
	bool done = false;
#ifdef _WINDOWS
	glewInit();
#endif

	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	GLuint spriteSheetTexture = LoadTexture("spritesheet_rgba.png");
	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(program.programID);
	float lastFrameTicks = 0.0f;
	float elap = 0.0f;
	Player1 mySprite = Player1(spriteSheetTexture, u, v,
		spriteWidth, spriteHeight, 0.3f);
	Enemy enemy = Enemy(spriteSheetTexture, uE, vE,
		spriteWidthE, spriteHeightE, 0.3f);

	float enemyMovement = 0.0f;

	float test2 = 3.0f;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		glBindTexture(GL_TEXTURE_2D, spriteSheetTexture);
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		
		//run animation
		animationElapsed += elapsed;
		if (animationElapsed > 1.0 / framesPerSecond) {
			currentIndex++;
			animationElapsed = 0.0;
			if (currentIndex > numFrames - 1) {
				currentIndex = 0;
			}
		}
		modelMatrix.identity();
		/*
		//elapsed will always be around the same value
		//rn check to see if holding the button down is the problem (1 tap for jump is ok)
		//"jumping" doesn't work at the moment
		if (mySprite.is_jumping == true){
			program.setModelMatrix(mySprite.matrix);
			mySprite.matrix.identity();
			if (mySprite.y < 1.5f){
				mySprite.y = mySprite.velocity_y * elapsed;
				mySprite.matrix.Translate(0, mySprite.y, 0);
			}
			if (mySprite.y >= 1.5f)
			{
				//elapsed = 0.0f;
				//mySprite.matrix.identity();
				mySprite.is_jumping = false;
			}
		}
		if (mySprite.is_jumping == false){
			if (keys[SDL_SCANCODE_S]){
				mySprite.is_jumping = true;
			}
		}
		if (keys[SDL_SCANCODE_D]){
			//mySprite.is_jumping = true;
			program.setModelMatrix(mySprite.matrix);
			mySprite.y = elapsed;
			mySprite.matrix.Translate(mySprite.y, 0, 0);
		}
		*/
		
		mySprite.Draw(&program, runAnimation[currentIndex], spriteCountX, spriteCountY);
		
		int test = ticks;
		int min = 0;
		int max = 200;
		int drawMin = 2;
		int drawMax = 5;
		//attempt at "randomizing" obstacles to jump over
		//also increases the speed of the obstacles spawning
		if ((test % (rand() % (drawMax - drawMin + 1) + drawMin)) == 0) //randomize the mod #
		{
			if ((min + (rand() % (int)(max - min + 1))) == 0){
				enemy.isActive = true;
			}
			
		}
		
		if ((test % 5) == 0){
			test2 += 0.0001f;
		}
		if (enemy.isActive == true){
			enemy.Draw(&program, indexE, spriteCountXE, spriteCountYE);
			//enemyMovement -= elapsed * 7.5f;
			enemyMovement -= elapsed * test2;
			enemy.movement = enemyMovement;
		}

		//hitbox scenarios note: will test more when jumping is implemented
		//also i don't know why it works sometimes and doesn't work other times
		//obstacles disappear for testing purposes. In the final game the player will have a lose animation
		//note:add conditional for mySprite.isAlive later
		if ((mySprite.getX3() == enemy.getX1() || mySprite.getX1() == enemy.getX3())
			&& mySprite.getY3() == enemy.getY1())
		{
			enemy.isActive = false;
			enemyMovement = 0.0f;
			enemy.movement = 0.0f;
		}

		if ((mySprite.getY2() == enemy.getY3() || mySprite.getY3() == enemy.getY2())
			&& mySprite.getX3() <= enemy.getX2())
		{
			enemy.isActive = false;
			enemyMovement = 0.0f;
			enemy.movement = 0.0f;
		}

		if (mySprite.getY3() <= enemy.getY3() && mySprite.getX3() == enemy.getX3())
		{
			enemy.isActive = false;
			enemyMovement = 0.0f;
			enemy.movement = 0.0f;
		}

		//point where the obstacle disappears
		if (enemyMovement <= -5.3f){
			enemy.isActive = false;
			enemyMovement = 0.0f;
			enemy.movement = 0.0f;
		}
		
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
}

//from an old hw note: didn't work last time
/*
void render(){
	const Uint8 *key = SDL_GetKeyboardState(NULL);
	if (key[SDL_SCANCODE_SPACE]){
		state = STATE_GAME_LEVEL;
	}
	switch (state){
	case STATE_MAIN_MENU:
		renderMenu();
	case STATE_GAME_LEVEL:
		renderGame();
	break;
	}
}
*/
int main(int argc, char *argv[])
{
	renderGame(); 
	return 0;
}
