#pragma once
#include "헤더.h"

extern bool drawPyramid;

// 삼각뿔 관련 전역 변수
extern GLuint VAO_Pyramid;
extern GLuint VBO_Pyramid[2];
extern GLuint EBO_Pyramid;

void InitPyramidBuffer();
void DrawPyramid(const glm::mat4& baseModel, const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation);
