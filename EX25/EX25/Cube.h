#pragma once
#include "헤더.h"

// 정육면체 관련 버퍼 변수 선언
extern GLuint VAO_cube;
extern GLuint VBO_cube[3];  // [0]: 위치, [1]: 색상, [2]: 법선
extern GLuint EBO_cube;     // 인덱스 버퍼

extern bool drawCube;

// 정육면체 관련 함수 선언
void InitCubeBuffer();
void DrawCube(const glm::mat4& baseModel, const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation);