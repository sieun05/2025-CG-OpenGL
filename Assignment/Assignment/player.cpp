#include "player.h"
#include <random>

// 플레이어 관련 전역 변수
Player player;
bool playerActive = false;

// 이동 상태 변수
bool moveUp = false, moveDown = false, moveLeft = false, moveRight = false;

void InitPlayer()
{
    player.size = glm::vec3(0.25f, 0.4f, 0.25f);  // 작은 크기
    player.color = {1.0f, 0.0f, 0.0f, 1.0f};      // 빨간색
    player.moveSpeed = 2.0f;                       // 기본 이동 속도 (다시 올림)
    player.minSpeed = 0.5f;                        // 최소 속도
    player.maxSpeed = 8.0f;                        // 최대 속도
    player.isActive = false;
    playerActive = false;
    
    // 이동 상태 초기화
    moveUp = moveDown = moveLeft = moveRight = false;
}

void CreatePlayer()
{
    extern std::vector<std::vector<Cube>> cubeGrid;
    extern int gridWidth, gridHeight;
    
    if (gridWidth == 0 || gridHeight == 0) {
        printf("먼저 격자를 생성하세요 (G 키)\n");
        return;
    }
    
    // 빈 공간(높이 0인 곳) 찾기
    glm::vec2 emptyPos = FindEmptyPosition();
    
    if (emptyPos.x == -1 && emptyPos.y == -1) {
        printf("미로에 빈 공간이 없습니다. 먼저 미로를 생성하세요 (r 키)\n");
        return;
    }
    
    // 플레이어 위치 설정 (바닥 위에)
    player.position.x = emptyPos.x;
    player.position.y = player.size.y * 0.5f;  // 바닥 위에 배치
    player.position.z = emptyPos.y;
    player.isActive = true;
    playerActive = true;
    
    printf("플레이어가 생성되었습니다! 화살표 키로 이동하세요.\n");
    printf("위치: (%.1f, %.1f, %.1f)\n", player.position.x, player.position.y, player.position.z);
}

glm::vec2 FindEmptyPosition()
{
    extern std::vector<std::vector<Cube>> cubeGrid;
    extern int gridWidth, gridHeight;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // 삭제된 큐브들의 위치를 수집
    std::vector<glm::vec2> emptyPositions;
    
    for (int z = 0; z < gridHeight; z++) {
        for (int x = 0; x < gridWidth; x++) {
            if (cubeGrid[z][x].isDeleted) {
                // 삭제된 큐브의 실제 월드 좌표 계산
                emptyPositions.push_back(glm::vec2(cubeGrid[z][x].position.x, cubeGrid[z][x].position.z));
            }
        }
    }
    
    if (emptyPositions.empty()) {
        return glm::vec2(-1, -1);  // 빈 공간이 없음
    }
    
    // 랜덤하게 빈 공간 선택
    std::uniform_int_distribution<int> posDist(0, emptyPositions.size() - 1);
    int randomIndex = posDist(gen);
    
    return emptyPositions[randomIndex];
}

void UpdatePlayer()
{
    if (!playerActive || !player.isActive) return;
    
    // 이동 처리 - 각 축별로 개별 충돌 검사
    float deltaTime = 1.0f / 60.0f;  // 60 FPS 기준
    glm::vec3 velocity(0.0f);
    
    if (moveUp) {
        velocity.z -= player.moveSpeed;
    }
    if (moveDown) {
        velocity.z += player.moveSpeed;
    }
    if (moveLeft) {
        velocity.x -= player.moveSpeed;
    }
    if (moveRight) {
        velocity.x += player.moveSpeed;
    }
    
    // 대각선 이동 시 속도 정규화 (더 자연스러운 이동)
    float velocityLength = glm::length(velocity);
    if (velocityLength > player.moveSpeed) {
        velocity = glm::normalize(velocity) * player.moveSpeed;
    }
    
    glm::vec3 movement = velocity * deltaTime;
    glm::vec3 newPosition = player.position;
    
    // X축 이동 시도
    if (movement.x != 0.0f) {
        glm::vec3 testPos = newPosition;
        testPos.x += movement.x;
        if (CanMoveToPosition(testPos.x, testPos.z)) {
            newPosition.x = testPos.x;
        }
    }
    
    // Z축 이동 시도  
    if (movement.z != 0.0f) {
        glm::vec3 testPos = newPosition;
        testPos.z += movement.z;
        if (CanMoveToPosition(testPos.x, testPos.z)) {
            newPosition.z = testPos.z;
        }
    }
    
    player.position = newPosition;
}

bool CanMoveToPosition(float x, float z)
{
    extern std::vector<std::vector<Cube>> cubeGrid;
    extern int gridWidth, gridHeight;
    
    // 경계 체크 (플레이어 크기 고려)
    float playerRadius = max(player.size.x, player.size.z) * 0.5f;  // 플레이어 반지름
    float safetyMargin = 0.1f;  // 벽과의 안전 거리
    float totalMargin = playerRadius + safetyMargin;
    
    if (x - totalMargin < -10.0f || x + totalMargin > 10.0f || 
        z - totalMargin < -10.0f || z + totalMargin > 10.0f) {
        return false;
    }
    
    // 각 칸의 크기 계산
    float cellSizeX = 20.0f / gridWidth;
    float cellSizeZ = 20.0f / gridHeight;
    
    // 플레이어가 차지할 영역 계산 (플레이어 중심 기준으로 주변 셀들 확인)
    int minGridX = (int)((x - totalMargin + 10.0f) / cellSizeX);
    int maxGridX = (int)((x + totalMargin + 10.0f) / cellSizeX);
    int minGridZ = (int)((z - totalMargin + 10.0f) / cellSizeZ);
    int maxGridZ = (int)((z + totalMargin + 10.0f) / cellSizeZ);
    
    // 범위를 격자 내로 제한
    minGridX = max(0, minGridX);
    maxGridX = min(gridWidth - 1, maxGridX);
    minGridZ = max(0, minGridZ);
    maxGridZ = min(gridHeight - 1, maxGridZ);
    
    // 플레이어가 차지할 모든 셀이 비어있는지 확인
    for (int z_idx = minGridZ; z_idx <= maxGridZ; z_idx++) {
        for (int x_idx = minGridX; x_idx <= maxGridX; x_idx++) {
            // 해당 셀에 큐브가 있으면 (삭제되지 않은 큐브) 이동 불가
            if (!cubeGrid[z_idx][x_idx].isDeleted) {
                return false;
            }
        }
    }
    
    return true;
}

void DrawPlayer(const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation)
{
    if (!playerActive || !player.isActive) return;
    
    extern GLuint VAO_cube;  // 큐브와 같은 VAO 사용
    
    glBindVertexArray(VAO_cube);
    
    // 플레이어 모델 행렬 생성
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, player.position);
    model = glm::scale(model, player.size);
    
    glm::mat4 mvp = projection * view * model;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
    
    // 플레이어 그리기 (빨간색 작은 큐브)
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
}

void MovePlayer(int direction)
{
    if (!playerActive || !player.isActive) return;
    
    glm::vec3 movement(0.0f);
    
    switch (direction) {
    case 0: // 위 (Z축 음의 방향)
        movement.z = -player.moveSpeed;
        break;
    case 1: // 아래 (Z축 양의 방향)
        movement.z = player.moveSpeed;
        break;
    case 2: // 왼쪽 (X축 음의 방향)
        movement.x = -player.moveSpeed;
        break;
    case 3: // 오른쪽 (X축 양의 방향)
        movement.x = player.moveSpeed;
        break;
    }
    
    glm::vec3 newPosition = player.position + movement;
    
    if (CanMoveToPosition(newPosition.x, newPosition.z)) {
        player.position = newPosition;
        printf("플레이어 이동: (%.1f, %.1f, %.1f)\n", player.position.x, player.position.y, player.position.z);
    } else {
        printf("그 방향으로 이동할 수 없습니다 (벽이 있습니다)\n");
    }
}

void ResetPlayer()
{
    player.isActive = false;
    playerActive = false;
    moveUp = moveDown = moveLeft = moveRight = false;
    printf("플레이어가 제거되었습니다.\n");
}

void IncreasePlayerSpeed()
{
    if (!playerActive || !player.isActive) {
        printf("플레이어가 생성되지 않았습니다. 먼저 's' 키로 플레이어를 생성하세요.\n");
        return;
    }
    
    float speedIncrement = 0.5f;  // 속도 증가량
    player.moveSpeed += speedIncrement;
    
    // 최대 속도 제한
    if (player.moveSpeed > player.maxSpeed) {
        player.moveSpeed = player.maxSpeed;
        printf("플레이어 속도가 최대값에 도달했습니다! 현재 속도: %.2f\n", player.moveSpeed);
    } else {
        printf("플레이어 속도 증가! 현재 속도: %.2f\n", player.moveSpeed);
    }
}

void DecreasePlayerSpeed()
{
    if (!playerActive || !player.isActive) {
        printf("플레이어가 생성되지 않았습니다. 먼저 's' 키로 플레이어를 생성하세요.\n");
        return;
    }
    
    float speedDecrement = 0.5f;  // 속도 감소량
    player.moveSpeed -= speedDecrement;
    
    // 최소 속도 제한
    if (player.moveSpeed < player.minSpeed) {
        player.moveSpeed = player.minSpeed;
        printf("플레이어 속도가 최소값에 도달했습니다! 현재 속도: %.2f\n", player.moveSpeed);
    } else {
        printf("플레이어 속도 감소! 현재 속도: %.2f\n", player.moveSpeed);
    }
}

void ResetPlayerSpeed()
{
    if (!playerActive || !player.isActive) {
        printf("플레이어가 생성되지 않았습니다.\n");
        return;
    }
    
    player.moveSpeed = 2.0f;  // 기본 속도로 초기화
    printf("플레이어 속도 초기화! 현재 속도: %.2f\n", player.moveSpeed);
}