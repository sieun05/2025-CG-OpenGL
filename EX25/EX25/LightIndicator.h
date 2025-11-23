#pragma once
#include "헤더.h"

// 조명 표시기 관련 변수 선언
extern GLuint VAO_lightIndicator;
extern GLuint VBO_lightIndicator[2];  // [0]: 위치, [1]: 색상
extern GLuint EBO_lightIndicator;     // 인덱스 버퍼 추가

// 조명 표시기 함수 선언
void InitLightIndicator();
void DrawLightIndicator(const glm::vec3& lightPos, const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation);