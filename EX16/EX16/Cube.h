#pragma once
#include "헤더.h"

// 정육면체 관련 전역 변수
extern GLuint VAO_cube;
extern GLuint VBO_cube[2];  // [0]: 정점, [1]: 색상
extern GLuint EBO_cube;     // 인덱스 버퍼

extern bool drawCube;
extern bool CubeWireDraw;

// 정육면체 관련 함수 선언
void InitCubeBuffer();
void DrawCube();