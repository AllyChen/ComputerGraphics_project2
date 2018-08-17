#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>

#include <Windows.h>
#include <mmsystem.h>

using namespace std;
//#include <cmath>

#include "vgl.h"
#include "LoadShaders.h"
#include "objloader.hpp"
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;



#define PARTSNUM 18
#define BODY 0
#define LEFTSHOUDER 1
#define ULEFTARM 2
#define DLEFTARM 3
#define LEFTHAND 4

void updateModels();

void init();

void ChangeSize(int w,int h);
void display();
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button,int state,int x,int y);

void menuEvents(int option);
void ActionMenuEvents(int option);
void ModeMenuEvents(int option);
void ShaderMenuEvents(int option);

void idle(int dummy);

mat4 translate(float x,float y,float z);
mat4 scale(float x,float y,float z);
mat4 rotate(float angle,float x,float y,float z);

void Obj2Buffer();
void load2Buffer( char* obj,int);

void updateObj(int);
void resetObj(int);

bool isFrame;

GLuint VAO;
GLuint VBO;
GLuint uVBO;
GLuint nVBO;
GLuint mVBO;
GLuint UBO;
GLuint VBOs[PARTSNUM];
GLuint uVBOs[PARTSNUM];
GLuint nVBOs[PARTSNUM];
GLuint program;
int pNo;

float angles[PARTSNUM];
float position = 0.0;
float angle = 0.0;
float eyeAngley = 0.0;
float eyedistance = 20.0;
float size = 1;
GLfloat movex,movey;
GLint MatricesIdx;
GLuint ModelID;

int vertices_size[PARTSNUM];
int uvs_size[PARTSNUM];
int normals_size[PARTSNUM];
int materialCount[PARTSNUM];

std::vector<std::string> mtls[PARTSNUM];//use material
std::vector<unsigned int> faces[PARTSNUM];//face count
map<string,vec3> KDs;//mtl-name&Kd
map<string,vec3> KSs;//mtl-name&Ks

mat4 Projection ;
mat4 View;
mat4 Model;
mat4 Models[PARTSNUM];

#define leftHand 0
#define rightHand 1
#define leftFoot 2
#define rightFoot 3

#define IDLE 0
#define WALK 1
#define GUARD 2
#define THINK 3
#define BEAR 4
#define HUG 5

#define OPEN 1
#define CLOSE 0
#define PIXEL 2

int mode;
int action;
int shader;

#define A_BODY 0
#define A_HEAD 1

#define A_LEFT_SHOULDER 2
#define A_LEFT_HAND 3
#define A_LEFT_DOWNARM 13

#define A_RIGHT_SHOULDER 4
#define A_RIGHT_HAND 5
#define A_RIGHT_DOWNARM 14

#define A_CLOTHES 6

#define A_PANTS 7

#define A_LEFT_UP_LEG 8
#define A_LEFT_DOWN_LEG 9

#define A_RIGHT_UP_LEG 10
#define A_RIGHT_DOWN_LEG 11

#define A_SHIELD 12

#define ELEMENTS 15
#define MOVE 4

float PandaAnglesDirection[ELEMENTS][3] = { 0 };
float PandaAngles[ELEMENTS] = { 0 };
float PandaTranslate_Y[ELEMENTS] = { 0 };
float PandaTranslate_Z[ELEMENTS] = { 0 };

float IdleAngles[ELEMENTS] = { 0 };

// body / head / L_shoulder / L_upArm / R_shoulder 
// R_upArm / clothes / pants / L_upLeg / L_downLeg 
// R_upLeg / R_downLeg / shield  / L_downArm / R_downArm

float WalkAnglesDirection[ELEMENTS][3] = {
	{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },
	{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },
	{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 }
};

float WalkAngles[MOVE][ELEMENTS] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{0, 10, 30, 0, -30, 0, 0, 0, -40, -20, 30, 20, 0, 15, -15 },
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0	},
	{ 0, -10, -30, 0, 30, 0, 0, 0, 40, 20, -30, -20, 0, -15, 15 }
};

float WalkTranslate_Y[MOVE][ELEMENTS] = { 0 };

float WalkTranslate_Z[MOVE][ELEMENTS] = { 0 };

float GuardAngles[MOVE][ELEMENTS] = {
	{ 0, 0, 0, 0, -50, 0, 0, 0, -40, 50, 0, 50, 0, 0, -25 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -25 },
	{ 0, 0, 0, 0, -50, 0, 0, 0, -40, 50, 0, 50, 0, 0, -25 },
	{ 0, 0, 0, 0, -50, 0, 0, 0, -40, 50, 0, 50, 0, 0, -60 }
};

float GuardTranslate_Y[MOVE][ELEMENTS] = {
	{ -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

float GuardTranslate_Z[MOVE][ELEMENTS] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5, 0, 0, 0, 0, 0 }
};

float ThinkAngles[MOVE][ELEMENTS] = {
	{ 0, 30, -50, 0, -50, 0, 0, 0, -40, 50, -40, 50, 0, 15, -15 },
	{ 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, -15 },
	{ 0, 30, -50, 0, -50, 0, 0, 0, -40, 50, -40, 50, 0, 15, -15 },
	{ 0, 30, -50, 0, -50, 0, 0, 0, -40, 50, -40, 50, 0, 60, -60 }
};

float ThinkTranslate_Y[MOVE][ELEMENTS] = { 
	{ -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } 
};

float ThinkTranslate_Z[MOVE][ELEMENTS] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5, 0, 0.5, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5, 0, 0.5, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5, 0, 0.5, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.5, 0, 0.5, 0, 0, 0 }
};

float BearAngles[MOVE][ELEMENTS] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, -50, 0, 50, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, -50, 0, 50, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, -50, 0, 50, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, -50, 0, 50, 0, 0, 0, 0 }
};

float BearTranslate_Y[MOVE][ELEMENTS] = {
	{ -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

float HugAngles[MOVE][ELEMENTS] = {
	{ 0, 0, -60, 0, -60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, -70, 0, -70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, -70, 0, -70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, -70, 0, -70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

float HugTranslate_Z[MOVE][ELEMENTS] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

float DeltaAngles[ELEMENTS] = { 0 };
float DeltaTranslate_Y[ELEMENTS] = { 0 };
float DeltaTranslate_Z[ELEMENTS] = { 0 };

int Cal_moveTime = 0; // calculate the move time
int MoveNum = 0; //the number of move
int moveTime[MOVE] = { 40, 40, 40, 40 }; // how many time of every move

void Timer (int time);

float rotateShader_t = 0;
GLuint t_ID;
GLuint textureID;
GLuint LoadTexture(string fileName);