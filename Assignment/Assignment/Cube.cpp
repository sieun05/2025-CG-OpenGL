#include "Cube.h"
#include <random>

GLuint VAO_cube;
GLuint VBO_cube[2];  // [0]: 정점, [1]: 색상  
GLuint EBO_cube;     // 인덱스 버퍼

std::vector<std::vector<Cube>> cubeGrid;
int gridWidth = 0, gridHeight = 0;
float animationStartTime = 0.0f;
bool animationActive = false;

void InitCubeBuffer()
{
    // 큐브의 8개 정점 (1x1x1 크기)
    const float cube_vertices[] = {
        // 앞면
        -0.5f, -0.5f,  0.5f,  // 0
         0.5f, -0.5f,  0.5f,  // 1
         0.5f,  0.5f,  0.5f,  // 2
        -0.5f,  0.5f,  0.5f,  // 3
        // 뒷면
        -0.5f, -0.5f, -0.5f,  // 4
         0.5f, -0.5f, -0.5f,  // 5
         0.5f,  0.5f, -0.5f,  // 6
        -0.5f,  0.5f, -0.5f   // 7
    };

    // 큐브의 색상 (각 정점마다)
    const float cube_colors[] = {
        0.8f, 0.8f, 0.8f,  // 0
        0.8f, 0.8f, 0.8f,  // 1
        0.9f, 0.9f, 0.9f,  // 2
        0.9f, 0.9f, 0.9f,  // 3
        0.7f, 0.7f, 0.7f,  // 4
        0.7f, 0.7f, 0.7f,  // 5
        0.8f, 0.8f, 0.8f,  // 6
        0.8f, 0.8f, 0.8f   // 7
    };

    // 큐브의 면을 구성하는 인덱스 (12개 삼각형)
    const unsigned int cube_indices[] = {
        // 앞면
        0, 1, 2,  2, 3, 0,
        // 뒷면
        4, 6, 5,  6, 4, 7,
        // 왼쪽면
        4, 0, 3,  3, 7, 4,
        // 오른쪽면
        1, 5, 6,  6, 2, 1,
        // 위면
        3, 2, 6,  6, 7, 3,
        // 아래면
        4, 5, 1,  1, 0, 4
    };

    // VAO 생성 및 바인딩
    glGenVertexArrays(1, &VAO_cube);
    glBindVertexArray(VAO_cube);

    // VBO 생성
    glGenBuffers(2, VBO_cube);

    // 정점 위치 데이터
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 정점 색상 데이터
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // EBO (인덱스 버퍼) 설정
    glGenBuffers(1, &EBO_cube);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_cube);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // 바인딩 해제
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GetUserInput()
{
    int width, height;
    
    printf("가로 칸 수를 입력하세요 (5-25): ");
    scanf("%d", &width);
    
    printf("세로 칸 수를 입력하세요 (5-25): ");
    scanf("%d", &height);
    
    // 범위 검사
    if (width < 5) width = 5;
    if (width > 25) width = 25;
    if (height < 5) height = 5;
    if (height > 25) height = 25;
    
    printf("격자 크기: %d x %d\n", width, height);
    
    CreateCubeGrid(width, height);
}

void CreateCubeGrid(int width, int height)
{
    gridWidth = width;
    gridHeight = height;
    
    // 기존 격자 클리어
    cubeGrid.clear();
    cubeGrid.resize(height);
    
    // 각 칸의 크기 계산 (바닥이 -10~10 = 20 크기)
    float cellSizeX = 20.0f / width;   // X축 셀 크기
    float cellSizeZ = 20.0f / height;  // Z축 셀 크기 (별도로 계산)
    
    // 난수 생성기 설정
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> heightDist(0.5f, 4.0f);  // 높이 0.5~4.0
    std::uniform_real_distribution<float> colorDist(0.6f, 1.0f);   // 색상 0.6~1.0
    std::uniform_real_distribution<float> delayDist(0.0f, 2.0f);   // 애니메이션 지연 시간 0~2초
    
    for (int z = 0; z < height; z++) {
        cubeGrid[z].resize(width);
        
        for (int x = 0; x < width; x++) {
            Cube& cube = cubeGrid[z][x];
            
            // 위치 설정 (바닥 중앙을 기준으로)
            cube.position.x = -10.0f + (x + 0.5f) * cellSizeX;
            cube.position.z = -10.0f + (z + 0.5f) * cellSizeZ;
            
            // 크기 설정 - 틈 없이 100%로 설정
            cube.sizeX = cellSizeX;  // X축 크기 (100% - 틈 없음)
            cube.sizeZ = cellSizeZ;  // Z축 크기 (100% - 틈 없음)
            cube.height = heightDist(gen);
            
            // 애니메이션 설정
            cube.targetY = cube.height * 0.5f;  // 목표 Y 위치 (바닥에서 시작)
            cube.currentY = -cube.height;       // 현재 Y 위치 (바닥 아래에서 시작)
            cube.position.y = cube.currentY;    // 초기 위치 설정
            cube.isAnimating = true;
            cube.animDelay = delayDist(gen);    // 랜덤 지연 시간
            
            // 랜덤 색상 설정
            cube.color.r = colorDist(gen);
            cube.color.g = colorDist(gen);
            cube.color.b = colorDist(gen);
            cube.color.a = 1.0f;
        }
    }
    
    printf("큐브 격자 생성 완료: %d x %d = %d개 큐브\n", width, height, width * height);
    
    // 애니메이션 시작
    StartCubeAnimations();
}

void StartCubeAnimations()
{
    animationStartTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;  // 현재 시간 (초)
    animationActive = true;
    printf("큐브 애니메이션 시작!\n");
}

void UpdateCubeAnimations(float currentTime)
{
    if (!animationActive) return;
    
    bool anyAnimating = false;
    float elapsedTime = currentTime - animationStartTime;
    
    for (int z = 0; z < gridHeight; z++) {
        for (int x = 0; x < gridWidth; x++) {
            Cube& cube = cubeGrid[z][x];
            
            if (cube.isAnimating) {
                // 지연 시간이 지났는지 확인
                if (elapsedTime >= cube.animDelay) {
                    float animTime = elapsedTime - cube.animDelay;
                    float animDuration = 1.5f;  // 애니메이션 지속 시간 (1.5초)
                    
                    if (animTime < animDuration) {
                        // 이징 함수 적용 (부드러운 움직임)
                        float t = animTime / animDuration;
                        float easedT = t * t * (3.0f - 2.0f * t);  // smoothstep 함수
                        
                        cube.currentY = -cube.height + (cube.targetY + cube.height) * easedT;
                        cube.position.y = cube.currentY;
                        anyAnimating = true;
                    } else {
                        // 애니메이션 완료
                        cube.currentY = cube.targetY;
                        cube.position.y = cube.currentY;
                        cube.isAnimating = false;
                    }
                } else {
                    anyAnimating = true;  // 아직 지연 시간 중
                }
            }
        }
    }
    
    // 모든 큐브의 애니메이션이 끝났으면 전체 애니메이션 종료
    if (!anyAnimating) {
        animationActive = false;
        printf("모든 큐브 애니메이션 완료!\n");
    }
}

void DrawCube(const Cube& cube, const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation)
{
    glBindVertexArray(VAO_cube);

    // 모델 행렬 생성 (위치, X/Z 크기, 높이 적용)
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, cube.position);
    model = glm::scale(model, glm::vec3(cube.sizeX, cube.height, cube.sizeZ));

    glm::mat4 mvp = projection * view * model;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

    // 큐브 그리기 (36개 정점 = 12개 삼각형)
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
}

void DrawAllCubes(const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation)
{
    for (int z = 0; z < gridHeight; z++) {
        for (int x = 0; x < gridWidth; x++) {
            DrawCube(cubeGrid[z][x], view, projection, mvpLocation);
        }
    }
}
