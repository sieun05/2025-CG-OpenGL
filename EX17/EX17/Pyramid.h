#pragma once
#include "헤더.h"

extern bool drawPyramid;

// 삼각뿔 관련 전역 변수
extern GLuint VAO_Pyramid;
extern GLuint VBO_Pyramid[2];
extern GLuint EBO_Pyramid;

// 사각뿔 애니메이션 관련 변수들
extern bool pyramidAllFacesOpenEnabled;    // o키: 모든 면들이 함께 열기/닫기
extern float pyramidAllFacesOpenAngle;

extern bool pyramidEachFaceOpenEnabled;    // r키: 각 면이 하나씩 열기/닫기
extern float pyramidFaceOpenAngles[4];     // [0]: 앞면, [1]: 오른쪽면, [2]: 뒷면, [3]: 왼쪽면
extern int currentOpeningFace;             // 현재 열리고 있는 면

// 새로운 애니메이션 배열
extern bool animation_pyramid[2];          // [0]: o키, [1]: r키

void InitPyramidBuffer();
void DrawPyramid(const glm::mat4& baseModel, const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation);
void UpdatePyramidAnimations();  // 애니메이션 업데이트 함수