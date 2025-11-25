#include "ground.h"

GLuint VAO_ground;
GLuint VBO_ground[2];
GLuint EBO_ground;

void InitGroundBuffer()
{
    const float size = 2.0f; // 정육면체 한 변의 절반 길이 (중심이 원점)

    const float ground_vertices[] = {
        -size, 0.0f, size,  // 0: 왼쪽 뒤
         size, 0.0f, size,  // 1: 오른쪽 뒤
		 size, 0.0f,  -size,  // 2: 오른쪽 앞
         -size, 0.0f,  -size,  // 2: 오른쪽 앞
    };

    const float ground_colors[] = {
        0.5f, 0.5f, 0.5f,  // 0
        0.5f, 0.5f, 0.5f,  // 1
        0.5f, 0.5f, 0.5f,  // 2
        0.5f, 0.5f, 0.5f,  // 3
	};

    const unsigned int ground_indices[] = {
        0, 1, 2,   2, 3, 0,
	};

    // VAO 생성 및 바인딩
    glGenVertexArrays(1, &VAO_ground);
    glBindVertexArray(VAO_ground);

    // VBO 생성
    glGenBuffers(2, VBO_ground);

    // 정점 위치 데이터
    glBindBuffer(GL_ARRAY_BUFFER, VBO_ground[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ground_vertices), ground_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 정점 색상 데이터
    glBindBuffer(GL_ARRAY_BUFFER, VBO_ground[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ground_colors), ground_colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // EBO (인덱스 버퍼) 설정
    glGenBuffers(1, &EBO_ground);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_ground);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ground_indices), ground_indices, GL_STATIC_DRAW);

    // 바인딩 해제
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void DrawGround()
{
    glBindVertexArray(VAO_ground);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // 36개 인덱스 (12개 삼각형)
    glBindVertexArray(0);
}

void changeGroundColor(RGBA newColor)
{
    float ground_colors[] = {
        newColor.r, newColor.g, newColor.b,  // 0
        newColor.r, newColor.g, newColor.b,  // 1
        newColor.r, newColor.g, newColor.b,  // 2
        newColor.r, newColor.g, newColor.b,  // 3
	};

    glBindBuffer(GL_ARRAY_BUFFER, VBO_ground[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ground_colors), ground_colors, GL_STATIC_DRAW);
}
