#pragma once
#include "헤더.h"

extern bool drawPyramid;
extern int pyramidFace; // 0~3: 각 면, 4: 밑면, 5: 전체 면
extern bool randomPyramid;
extern int randomPyramidFace;


// 삼각뿔 관련 전역 변수
extern GLuint VAO_Pyramid;
extern GLuint VBO_Pyramid[2];
extern GLuint EBO_Pyramid;

void InitPyramidBuffer();
void DrawPyramid();