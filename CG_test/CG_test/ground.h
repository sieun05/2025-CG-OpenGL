#pragma once
#include "Çì´õ.h"

extern GLuint VAO_ground;
extern GLuint VBO_ground[2];
extern GLuint EBO_ground;

void InitGroundBuffer();
void DrawGround();
void changeGroundColor(RGBA newColor);