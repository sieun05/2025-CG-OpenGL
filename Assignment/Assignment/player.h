#pragma once
#include "헤더.h"
#include "Cube.h"

struct Player {
    glm::vec3 position;      // 플레이어 위치
    glm::vec3 size;          // 플레이어 크기 (작은 큐브)
    RGBA color;              // 플레이어 색상 (빨간색)
    bool isActive;           // 플레이어 활성 상태
    
    // 이동 관련
    float moveSpeed;         // 이동 속도
    float minSpeed;          // 최소 속도
    float maxSpeed;          // 최대 속도
    bool canMove[4];         // 방향별 이동 가능 여부 [상, 하, 좌, 우]
};

// 플레이어 관련 전역 변수
extern Player player;
extern bool playerActive;

// 이동 상태 변수
extern bool moveUp, moveDown, moveLeft, moveRight;

// 플레이어 관련 함수들
void InitPlayer();                    // 플레이어 초기화
void CreatePlayer();                  // 플레이어 생성 (s 키용)
void UpdatePlayer();                  // 플레이어 업데이트
void DrawPlayer(const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation);  // 플레이어 렌더링
void MovePlayer(int direction);       // 플레이어 이동 (0:상, 1:하, 2:좌, 3:우)
bool CanMoveToPosition(float x, float z);  // 해당 위치로 이동 가능한지 확인
glm::vec2 FindEmptyPosition();        // 빈 공간(높이 0) 찾기
void ResetPlayer();                   // 플레이어 제거

// 속도 조절 함수들
void IncreasePlayerSpeed();           // 플레이어 속도 증가 (+ 키용)
void DecreasePlayerSpeed();           // 플레이어 속도 감소 (- 키용)
void ResetPlayerSpeed();              // 플레이어 속도 초기화