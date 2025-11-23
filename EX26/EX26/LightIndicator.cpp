#include "LightIndicator.h"

// 조명 표시기 관련 변수 정의
GLuint VAO_lightIndicator = 0;
GLuint VBO_lightIndicator[2] = { 0, };  // [0]: 위치, [1]: 색상
GLuint EBO_lightIndicator = 0;          // 인덱스 버퍼 추가

void InitLightIndicator() {
    // 작은 구형 표시기를 위한 정점 데이터 (간단한 정육면체 형태)
    const float size = 0.05f; // 작은 크기

    // 정육면체 정점 (원점 중심)
    const float indicator_vertices[] = {
        // 앞면 (z = +size)
        -size, -size,  size,  // 0
         size, -size,  size,  // 1
         size,  size,  size,  // 2
        -size,  size,  size,  // 3

        // 뒷면 (z = -size)
         size, -size, -size,  // 4
        -size, -size, -size,  // 5
        -size,  size, -size,  // 6
         size,  size, -size,  // 7

         // 좌측면 (x = -size)
         -size, -size, -size,  // 8
         -size, -size,  size,  // 9
         -size,  size,  size,  // 10
         -size,  size, -size,  // 11

         // 우측면 (x = +size)
          size, -size,  size,  // 12
          size, -size, -size,  // 13
          size,  size, -size,  // 14
          size,  size,  size,  // 15

          // 아래면 (y = -size)
          -size, -size, -size,  // 16
           size, -size, -size,  // 17
           size, -size,  size,  // 18
          -size, -size,  size,  // 19

          // 윗면 (y = +size)
          -size,  size,  size,  // 20
           size,  size,  size,  // 21
           size,  size, -size,  // 22
          -size,  size, -size   // 23
    };

    // 밝은 노란색 (조명 표시용)
    const float indicator_colors[] = {
        // 앞면 - 밝은 노란색
        1.0f, 1.0f, 0.0f,  // 0
        1.0f, 1.0f, 0.0f,  // 1
        1.0f, 1.0f, 0.0f,  // 2
        1.0f, 1.0f, 0.0f,  // 3

        // 뒷면 - 밝은 노란색
        1.0f, 1.0f, 0.0f,  // 4
        1.0f, 1.0f, 0.0f,  // 5
        1.0f, 1.0f, 0.0f,  // 6
        1.0f, 1.0f, 0.0f,  // 7

        // 좌측면 - 밝은 노란색
        1.0f, 1.0f, 0.0f,  // 8
        1.0f, 1.0f, 0.0f,  // 9
        1.0f, 1.0f, 0.0f,  // 10
        1.0f, 1.0f, 0.0f,  // 11

        // 우측면 - 밝은 노란색
        1.0f, 1.0f, 0.0f,  // 12
        1.0f, 1.0f, 0.0f,  // 13
        1.0f, 1.0f, 0.0f,  // 14
        1.0f, 1.0f, 0.0f,  // 15

        // 아래면 - 밝은 노란색
        1.0f, 1.0f, 0.0f,  // 16
        1.0f, 1.0f, 0.0f,  // 17
        1.0f, 1.0f, 0.0f,  // 18
        1.0f, 1.0f, 0.0f,  // 19

        // 윗면 - 밝은 노란색
        1.0f, 1.0f, 0.0f,  // 20
        1.0f, 1.0f, 0.0f,  // 21
        1.0f, 1.0f, 0.0f,  // 22
        1.0f, 1.0f, 0.0f   // 23
    };

    // 인덱스 배열 (정육면체의 각 면을 반시계 방향으로 정의)
    const unsigned int indicator_indices[] = {
        // 앞면
        0, 1, 2,   2, 3, 0,
        // 뒷면
        4, 5, 6,   6, 7, 4,
        // 좌측면
        8, 9, 10,   10, 11, 8,
        // 우측면
        12, 13, 14,   14, 15, 12,
        // 아래면
        16, 17, 18,   18, 19, 16,
        // 윗면
        20, 21, 22,   22, 23, 20
    };

    // VAO 생성 및 바인딩
    glGenVertexArrays(1, &VAO_lightIndicator);
    glBindVertexArray(VAO_lightIndicator);

    // VBO 생성 (2개: 위치, 색상)
    glGenBuffers(2, VBO_lightIndicator);

    // 정점 위치 버퍼
    glBindBuffer(GL_ARRAY_BUFFER, VBO_lightIndicator[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(indicator_vertices), indicator_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 정점 색상 버퍼
    glBindBuffer(GL_ARRAY_BUFFER, VBO_lightIndicator[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(indicator_colors), indicator_colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // EBO (인덱스 버퍼) 생성
    glGenBuffers(1, &EBO_lightIndicator);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_lightIndicator);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicator_indices), indicator_indices, GL_STATIC_DRAW);

    // 바인딩 해제
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void DrawLightIndicator(const glm::vec3& lightPos, const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation)
{
    // 조명 위치에 작은 표시기 그리기
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);  // 조명 위치로 이동

    // MVP 행렬 계산 및 전송
    glm::mat4 mvp = projection * view * model;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

    // VAO 바인딩 후 그리기
    glBindVertexArray(VAO_lightIndicator);
    
    // 인덱스를 사용하여 정육면체 그리기 (36개 인덱스 = 12개 삼각형 = 6개 면)
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
}