#pragma once
#include "헤더.h"
#include "Cube.h"

extern bool mazeGenerated;

// 미로 생성 관련 함수들
void GenerateMaze();           // 미로 생성 (r 키용)
void ResetMaze();              // 미로 초기화 (모든 큐브 복원)
bool IsMazeGenerated();        // 미로가 생성되었는지 확인

// 미로 생성 알고리즘 관련 함수들
void CreateConnectedMaze();    // 연결된 미로 생성 (DFS 기반)
void CreatePathMaze();         // 경로 기반 미로 생성 (백업용)
void CreateRandomMaze();       // 랜덤 미로 생성
void AddRandomPaths();         // 추가 경로 생성
void EnsureStartAndEnd();      // 시작점과 끝점 보장
bool IsValidPosition(int x, int z, int width, int height);  // 유효한 위치 확인