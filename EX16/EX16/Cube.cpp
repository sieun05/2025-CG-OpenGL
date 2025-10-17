#include "Cube.h"

// 정육면체 관련 전역 변수 정의
GLuint VAO_cube = 0;
GLuint VBO_cube[2] = { 0, };
GLuint EBO_cube = 0;

//정육면체 그리기 T/F, 어떤 면 그릴지
bool drawCube = true;
bool CubeWireDraw = false;

void InitCubeBuffer() {
    const float size = 0.5f; // 정육면체 한 변의 절반 길이 (중심이 원점)

    // 면별로 정점을 분리 (24개 정점 = 6면 × 4정점)
    // 모든 면을 바깥쪽에서 봤을 때 반시계방향으로 정의
    const float cube_vertices[] = {
        -size, size, -size,
        -size, size, size,
        size, size, size,
        size, size, -size,
        -size, -size, size,
        size, -size, size,
        size, -size, -size,
        -size, -size, -size,
    };

    // 각 면마다 다른 색상 (24개 정점, 면별로 동일한 색상)
    const float cube_colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.0f,
        1.0f, 0.0f, 0.5f
    };

    // 인덱스 배열 (모든 면이 반시계방향으로 정의됨)
    const unsigned int cube_indices[] = {
	   1, 4, 5, 5, 2, 1,       // 앞쪽 면
	   0, 7, 4, 4, 1, 0,       // 왼쪽 면
	   0, 3, 6, 6, 7, 0,       // 뒤쪽 면
	   4, 7, 6, 6, 5, 4,        // 아래쪽 면
       0, 1, 2, 2, 3, 0,       // 위쪽 면
       2, 5, 6, 6, 3, 2,       // 오른쪽 면
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
    if (CubeWireDraw) {
        for(int i = 0; i < 36; i += 3)
        {
            glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void*)(i * sizeof(unsigned int)));
		}
    }
    else
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // 36개 인덱스 (12개 삼각형)
    glBindVertexArray(0);
}