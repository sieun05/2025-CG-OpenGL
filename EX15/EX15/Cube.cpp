#include "Cube.h"

// 정육면체 관련 전역 변수 정의
GLuint VAO_cube = 0;
GLuint VBO_cube[2] = { 0, };
GLuint EBO_cube = 0;

//정육면체 그리기 T/F, 어떤 면 그릴지
bool drawCube = true;
int cubeFace = 6; // 0~5: 각 면, 6: 전체 면
bool randomCube = false;
int randomFace[2]{};

void InitCubeBuffer() {
    const float size = 0.5f; // 정육면체 한 변의 절반 길이 (중심이 원점)

    // 면별로 정점을 분리 (24개 정점 = 6면 × 4정점)
    // 모든 면을 바깥쪽에서 봤을 때 반시계방향으로 정의
    const float cube_vertices[] = {
        // 앞면 (z = +size) - 0~3 (바깥쪽에서 봤을 때 반시계방향)
        -size, -size,  size,  // 0: 왼쪽 아래
         size, -size,  size,  // 1: 오른쪽 아래
         size,  size,  size,  // 2: 오른쪽 위
        -size,  size,  size,  // 3: 왼쪽 위

        // 뒷면 (z = -size) - 4~7 (바깥쪽에서 봤을 때 반시계방향)
         size, -size, -size,  // 4: 오른쪽 아래 (뒤에서 보면 왼쪽)
        -size, -size, -size,  // 5: 왼쪽 아래 (뒤에서 보면 오른쪽)
        -size,  size, -size,  // 6: 왼쪽 위 (뒤에서 보면 오른쪽)
         size,  size, -size,  // 7: 오른쪽 위 (뒤에서 보면 왼쪽)

        // 왼쪽면 (x = -size) - 8~11 (바깥쪽에서 봤을 때 반시계방향)
        -size, -size, -size,  // 8: 뒤 아래
        -size, -size,  size,  // 9: 앞 아래
        -size,  size,  size,  // 10: 앞 위
        -size,  size, -size,  // 11: 뒤 위

        // 오른쪽면 (x = +size) - 12~15 (바깥쪽에서 봤을 때 반시계방향)
         size, -size,  size,  // 12: 앞 아래
         size, -size, -size,  // 13: 뒤 아래
         size,  size, -size,  // 14: 뒤 위
         size,  size,  size,  // 15: 앞 위

        // 아래면 (y = -size) - 16~19 (바깥쪽에서 봤을 때 반시계방향)
        -size, -size, -size,  // 16: 왼쪽 뒤
         size, -size, -size,  // 17: 오른쪽 뒤
         size, -size,  size,  // 18: 오른쪽 앞
        -size, -size,  size,  // 19: 왼쪽 앞

        // 위면 (y = +size) - 20~23 (바깥쪽에서 봤을 때 반시계방향)
        -size,  size,  size,  // 20: 왼쪽 앞
         size,  size,  size,  // 21: 오른쪽 앞
         size,  size, -size,  // 22: 오른쪽 뒤
        -size,  size, -size   // 23: 왼쪽 뒤
    };

    // 각 면마다 다른 색상 (24개 정점, 면별로 동일한 색상)
    const float cube_colors[] = {
        // 앞면 - 빨간색 (Red)
        1.0f, 0.0f, 0.0f,  // 0
        1.0f, 0.0f, 0.0f,  // 1
        1.0f, 0.0f, 0.0f,  // 2
        1.0f, 0.0f, 0.0f,  // 3

        // 뒷면 - 초록색 (Green)
        0.0f, 1.0f, 0.0f,  // 4
        0.0f, 1.0f, 0.0f,  // 5
        0.0f, 1.0f, 0.0f,  // 6
        0.0f, 1.0f, 0.0f,  // 7

        // 왼쪽면 - 파란색 (Blue)
        0.0f, 0.0f, 1.0f,  // 8
        0.0f, 0.0f, 1.0f,  // 9
        0.0f, 0.0f, 1.0f,  // 10
        0.0f, 0.0f, 1.0f,  // 11

        // 오른쪽면 - 노란색 (Yellow)
        1.0f, 1.0f, 0.0f,  // 12
        1.0f, 1.0f, 0.0f,  // 13
        1.0f, 1.0f, 0.0f,  // 14
        1.0f, 1.0f, 0.0f,  // 15

        // 아래면 - 자홍색 (Magenta)
        1.0f, 0.0f, 1.0f,  // 16
        1.0f, 0.0f, 1.0f,  // 17
        1.0f, 0.0f, 1.0f,  // 18
        1.0f, 0.0f, 1.0f,  // 19

        // 위면 - 청록색 (Cyan)
        0.0f, 1.0f, 1.0f,  // 20
        0.0f, 1.0f, 1.0f,  // 21
        0.0f, 1.0f, 1.0f,  // 22
        0.0f, 1.0f, 1.0f   // 23
    };

    // 인덱스 배열 (모든 면이 반시계방향으로 정의됨)
    const unsigned int cube_indices[] = {
        // 앞면 (Red) - 반시계방향
        0, 1, 2,   2, 3, 0,
        
        // 뒷면 (Green) - 반시계방향
        4, 5, 6,   6, 7, 4,
        
        // 왼쪽면 (Blue) - 반시계방향
        8, 9, 10,   10, 11, 8,
        
        // 오른쪽면 (Yellow) - 반시계방향
        12, 13, 14,   14, 15, 12,
        
        // 아래면 (Magenta) - 반시계방향
        16, 17, 18,   18, 19, 16,
        
        // 위면 (Cyan) - 반시계방향
        20, 21, 22,   22, 23, 20
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

void DrawCube()
{
    glBindVertexArray(VAO_cube);

    if (randomCube) {
        for (int i{}; i < 2; i++) {
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(randomFace[i] * 6 * sizeof(unsigned int)));
        }
    }
    else if(cubeFace >= 0 && cubeFace <= 5) {
        // 특정 면만 그리기 (6개 인덱스)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(cubeFace * 6 * sizeof(unsigned int)));
	}
	else
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // 36개 인덱스 (12개 삼각형)

    glBindVertexArray(0);
}