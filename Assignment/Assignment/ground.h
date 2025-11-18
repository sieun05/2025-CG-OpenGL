#pragma once
#include "헤더.h"

extern GLuint VAO_ground;
extern GLuint VBO_ground[2];  // [0]: 정점, [1]: 색상
extern GLuint EBO_ground;     // 인덱스 버퍼

void InitGroundBuffer();
void DrawGround(const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation);
