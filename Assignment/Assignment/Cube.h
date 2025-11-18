#pragma once
#include "헤더.h"

struct Cube {
	float sizeX;		// 큐브의 X축 크기
	float sizeZ;		// 큐브의 Z축 크기
	float height;		// 큐브의 높이 (Y축)
	RGBA color;
	glm::vec3 position;	// 큐브의 위치 (x, y, z)
	
	// 애니메이션 관련 변수들
	float targetY;		// 목표 Y 위치 (최종 위치)
	float currentY;		// 현재 Y 위치 (애니메이션용)
	bool isAnimating;	// 애니메이션 중인지 여부
	float animStartTime;// 애니메이션 시작 시간
	float animDelay;	// 애니메이션 시작 지연 시간
	
	// Y축 스케일 애니메이션 관련 변수들
	float originalHeight;	// 원본 높이
	float currentScale;		// 현재 Y축 스케일
	float baseScale;		// 애니메이션 시작 시의 기준 스케일
	bool isScaleAnimating;	// 스케일 애니메이션 중인지 여부
	float scaleAnimPhase;	// 애니메이션 위상 (사인파용)
};

// 큐브 관련 전역 변수
extern GLuint VAO_cube;
extern GLuint VBO_cube[2];  // [0]: 정점, [1]: 색상
extern GLuint EBO_cube;     // 인덱스 버퍼

extern std::vector<std::vector<Cube>> cubeGrid;  // 2D 큐브 격자
extern int gridWidth, gridHeight;  // 격자 크기
extern float animationStartTime;    // 전체 애니메이션 시작 시간
extern bool animationActive;        // 애니메이션 활성 상태

extern bool UpDownAnimationActive; // 위아래 애니메이션 활성 상태
extern float upDownAnimationTime;  // 위아래 애니메이션 시간

extern bool heightNormalized;      // 높이 평준화 상태 (true: 1.0f 높이, false: 원래 높이)

// 큐브 관련 함수들
void InitCubeBuffer();
void CreateCubeGrid(int width, int height);
void DrawCube(const Cube& cube, const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation);
void DrawAllCubes(const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation);
void GetUserInput();  // 사용자 입력 받기
void UpdateCubeAnimations(float currentTime);  // 애니메이션 업데이트
void StartCubeAnimations();  // 애니메이션 시작

void UpDownCubeAnimation(); // 위아래 높이 변화 애니메이션 시작
void StopUpDownCubeAnimation(); // 위아래 높이 변화 애니메이션 정지
void UpdateUpDownAnimation(float currentTime); // 위아래 애니메이션 업데이트

void ToggleHeightNormalization(); // 높이 평준화 토글 (v 키용)