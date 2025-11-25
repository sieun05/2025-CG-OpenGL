#pragma once
#include <vector>
#include "헤더.h"
#include "Random.h"

struct Cube {
	Cube() = default;

	void DrawCube(glm::mat4 gProjection, glm::mat4 gView, GLuint uMVP_loc);
	void UpdateAnimation();

	float size = 1.0f;  // 한 변의 길이
	RGBA color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 기본 흰색
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // 위치
	
	bool animate[3] = { true, true, true }; // x, y, z축 회전 애니메이션 여부
	float xAngle = 0.0f; // x축 회전 각도
	float yAngle = 0.0f; // y축 회전 각도
	float zAngle = 0.0f; // z축 회전 각도

	float obj_yAngle = 0.0f; // 오브젝트 y축 회전 각도
};

// 정육면체 관련 전역 변수
extern GLuint VAO_cube;
extern GLuint VBO_cube[2];  // [0]: 정점, [1]: 색상
extern GLuint EBO_cube;     // 인덱스 버퍼

extern bool drawCube;
extern bool CubeWireDraw;

extern Cube cube;

// 정육면체 관련 함수 선언
void InitCubeBuffer();
void CreateCubes();
void DrawCubes(glm::mat4 gProjection, glm::mat4 gView, GLuint uMVP_loc);
void ChangeCubeColor(RGBA color);
void AnimateCubes();

