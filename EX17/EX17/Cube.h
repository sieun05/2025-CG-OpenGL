#pragma once
#include "헤더.h"

// 정육면체 관련 전역 변수
extern GLuint VAO_cube;
extern GLuint VBO_cube[2];  // [0]: 정점, [1]: 색상
extern GLuint EBO_cube;     // 인덱스 버퍼

extern bool drawCube;
extern bool animation_cube[4]; // [0]: t키, [1]: f키, [2]: s키, [3]: b키

// 큐브 애니메이션 관련 변수들
extern float topFaceRotationAngle;

extern float frontFaceOpenAngle;

extern float sideFaceRotationAngle;

extern float backFaceScale;


// 정육면체 관련 함수 선언
void InitCubeBuffer();
void DrawCube(const glm::mat4& baseModel, const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation);
void UpdateCubeAnimations();  // 애니메이션 업데이트 함수