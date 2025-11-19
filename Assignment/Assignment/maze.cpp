#include "maze.h"
#include <random>
#include <vector>
#include <stack>
#include <algorithm>

// 미로 생성 상태
bool mazeGenerated = false;

// 방향 정의 (상, 하, 좌, 우)
const int dx[] = {0, 0, -1, 1};
const int dz[] = {-1, 1, 0, 0};

// 미로 생성 함수
void GenerateMaze()
{
    // Cube.cpp의 전역 변수들 extern으로 접근
    extern std::vector<std::vector<Cube>> cubeGrid;
    extern int gridWidth, gridHeight;
    
    if (gridWidth == 0 || gridHeight == 0) {
        printf("먼저 격자를 생성하세요 (G 키)\n");
        return;
    }
    
    if (mazeGenerated) {
        // 이미 미로가 생성된 경우 초기화 후 새로운 미로 생성
        ResetMaze();
    }
    
    // DFS 기반 연결된 미로 생성
    CreateConnectedMaze();
    
    mazeGenerated = true;
    printf("연결된 미로가 생성되었습니다! 모든 경로가 연결되어 있습니다.\n");
}

void ResetMaze()
{
    extern std::vector<std::vector<Cube>> cubeGrid;
    extern int gridWidth, gridHeight;
    
    if (gridWidth == 0 || gridHeight == 0) {
        return;
    }
    
    // 모든 큐브를 다시 표시 (삭제된 큐브들 복원)
    for (int z = 0; z < gridHeight; z++) {
        for (int x = 0; x < gridWidth; x++) {
            Cube& cube = cubeGrid[z][x];
            
            // 삭제 상태 해제
            cube.isDeleted = false;
            
            // 큐브의 스케일을 1.0으로 설정하여 표시
            cube.currentScale = 1.0f;
            // 원래 Y 위치로 복원
            cube.position.y = cube.originalHeight * 0.5f;
        }
    }
    
    mazeGenerated = false;
    printf("미로가 초기화되었습니다. 모든 큐브가 복원되었습니다.\n");
}

bool IsMazeGenerated()
{
    return mazeGenerated;
}

void CreateConnectedMaze()
{
    extern std::vector<std::vector<Cube>> cubeGrid;
    extern int gridWidth, gridHeight;
    
    // 먼저 모든 내부를 벽으로 설정하고 외곽은 벽으로 유지
    for (int z = 0; z < gridHeight; z++) {
        for (int x = 0; x < gridWidth; x++) {
            Cube& cube = cubeGrid[z][x];
            
            // 가장자리는 벽으로 유지
            if (x == 0 || x == gridWidth - 1 || z == 0 || z == gridHeight - 1) {
                cube.isDeleted = false;
                cube.currentScale = 1.0f;
                cube.position.y = cube.originalHeight * 0.5f;
            } else {
                // 내부는 일단 모두 벽으로 설정
                cube.isDeleted = false;  
                cube.currentScale = 1.0f;
                cube.position.y = cube.originalHeight * 0.5f;
            }
        }
    }
    
    // 방문 체크용 배열
    std::vector<std::vector<bool>> visited(gridHeight, std::vector<bool>(gridWidth, false));
    
    // DFS로 미로 생성
    std::stack<std::pair<int, int>> stack;
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // 시작점 설정 (내부의 홀수 좌표에서 시작)
    int startX = 1;
    int startZ = 1;
    
    // 시작점을 경로로 만들고 스택에 추가
    cubeGrid[startZ][startX].isDeleted = true;
    cubeGrid[startZ][startX].currentScale = 0.0f;
    visited[startZ][startX] = true;
    stack.push({startX, startZ});
    
    while (!stack.empty()) {
        auto current = stack.top();
        int x = current.first;
        int z = current.second;
        
        // 현재 위치에서 갈 수 있는 방향들 찾기
        std::vector<int> directions;
        
        for (int dir = 0; dir < 4; dir++) {
            int nx = x + dx[dir] * 2;  // 2칸씩 이동 (벽-경로-벽 패턴)
            int nz = z + dz[dir] * 2;
            
            // 경계 확인 및 방문 체크
            if (IsValidPosition(nx, nz, gridWidth, gridHeight) && 
                nx > 0 && nx < gridWidth - 1 && nz > 0 && nz < gridHeight - 1 &&
                !visited[nz][nx]) {
                directions.push_back(dir);
            }
        }
        
        if (!directions.empty()) {
            // 랜덤하게 방향 선택
            std::shuffle(directions.begin(), directions.end(), gen);
            int chosenDir = directions[0];
            
            int nx = x + dx[chosenDir] * 2;
            int nz = z + dz[chosenDir] * 2;
            
            // 중간 벽과 목적지를 경로로 만들기
            int midX = x + dx[chosenDir];
            int midZ = z + dz[chosenDir];
            
            // 중간 위치 (벽 제거)
            cubeGrid[midZ][midX].isDeleted = true;
            cubeGrid[midZ][midX].currentScale = 0.0f;
            
            // 목적지 (경로)
            cubeGrid[nz][nx].isDeleted = true;
            cubeGrid[nz][nx].currentScale = 0.0f;
            visited[nz][nx] = true;
            
            stack.push({nx, nz});
        } else {
            // 더 이상 갈 곳이 없으면 백트래킹
            stack.pop();
        }
    }
    
    // 추가 경로 생성 (더 복잡한 미로를 위해)
    AddRandomPaths();
    
    // 시작점과 끝점 보장
    EnsureStartAndEnd();
}

void AddRandomPaths()
{
    extern std::vector<std::vector<Cube>> cubeGrid;
    extern int gridWidth, gridHeight;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> pathDist(0, 100);
    
    // 일부 벽을 랜덤하게 제거하여 추가 경로 생성 (20% 확률)
    for (int z = 1; z < gridHeight - 1; z++) {
        for (int x = 1; x < gridWidth - 1; x++) {
            if (!cubeGrid[z][x].isDeleted && pathDist(gen) < 20) {
                cubeGrid[z][x].isDeleted = true;
                cubeGrid[z][x].currentScale = 0.0f;
            }
        }
    }
}

void EnsureStartAndEnd()
{
    extern std::vector<std::vector<Cube>> cubeGrid;
    extern int gridWidth, gridHeight;
    
    if (gridWidth > 2 && gridHeight > 2) {
        // 시작점 (왼쪽 위 모서리 근처) - 이미 처리되었지만 확실히 하기 위해
        cubeGrid[1][1].isDeleted = true;
        cubeGrid[1][1].currentScale = 0.0f;
        
        // 끝점 (오른쪽 아래 모서리 근처)
        cubeGrid[gridHeight-2][gridWidth-2].isDeleted = true;
        cubeGrid[gridHeight-2][gridWidth-2].currentScale = 0.0f;
        
        // 끝점 주변에 경로 보장
        if (gridWidth > 3) {
            cubeGrid[gridHeight-2][gridWidth-3].isDeleted = true;
            cubeGrid[gridHeight-2][gridWidth-3].currentScale = 0.0f;
        }
        if (gridHeight > 3) {
            cubeGrid[gridHeight-3][gridWidth-2].isDeleted = true;
            cubeGrid[gridHeight-3][gridWidth-2].currentScale = 0.0f;
        }
    }
}

void CreatePathMaze()
{
    // 이제 CreateConnectedMaze()를 사용하므로 이 함수는 백업용으로만 유지
    CreateConnectedMaze();
}

void CreateRandomMaze()
{
    extern std::vector<std::vector<Cube>> cubeGrid;
    extern int gridWidth, gridHeight;
    
    // 단순한 랜덤 미로 생성 (백업용)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> deleteDist(0, 100);
    
    for (int z = 0; z < gridHeight; z++) {
        for (int x = 0; x < gridWidth; x++) {
            Cube& cube = cubeGrid[z][x];
            
            // 30% 확률로 큐브 삭제
            if (deleteDist(gen) < 30) {
                cube.isDeleted = true;
                cube.currentScale = 0.0f;
            } else {
                cube.isDeleted = false;
                cube.currentScale = 1.0f;
                cube.position.y = cube.originalHeight * 0.5f;
            }
        }
    }
}

bool IsValidPosition(int x, int z, int width, int height)
{
    return (x >= 0 && x < width && z >= 0 && z < height);
}