#include <iostream>
#include "Pyramid.h"

// 사각뿔 관련 전역 변수 정의
GLuint VAO_Pyramid = 0;
GLuint VBO_Pyramid[2] = { 0, };
GLuint EBO_Pyramid = 0;

// 사각뿔 그리기 제어 변수
bool drawPyramid = false;
bool PyramidWireDraw = false;

void InitPyramidBuffer()
{
    const float size = 0.5f; // 사각뿔 크기 조절용 변수

    const float Pyramid_vertices[] = {
        // 밑면 (사각형) - 4개 정점
         -size, -size, -size,  // 0: 왼쪽 뒤
         -size, -size,  size,  // 1: 왼쪽 앞
          size, -size,  size,  // 2: 오른쪽 앞
          size, -size, -size,  // 3: 오른쪽 뒤

          // 앞면 삼각형 (z = +size) - 3개 정점
          -size, -size,  size,  // 4: 왼쪽 앞
           size, -size,  size,  // 5: 오른쪽 앞  
           0.0f,  size,  0.0f,  // 6: 꼭짓점

           // 오른쪽면 삼각형 (x = +size) - 3개 정점
            size, -size,  size,  // 7: 오른쪽 앞
            size, -size, -size,  // 8: 오른쪽 뒤
            0.0f,  size,  0.0f,  // 9: 꼭짓점

            // 뒷면 삼각형 (z = -size) - 3개 정점
             size, -size, -size,  // 10: 오른쪽 뒤
            -size, -size, -size,  // 11: 왼쪽 뒤
             0.0f,  size,  0.0f,  // 12: 꼭짓점

             // 왼쪽면 삼각형 (x = -size) - 3개 정점
             -size, -size, -size,  // 13: 왼쪽 뒤
             -size, -size,  size,  // 14: 왼쪽 앞
              0.0f,  size,  0.0f   // 15: 꼭짓점

    };

    const float Pyramid_colors[] = {
        // 밑면 - 보라색
         1.0f, 0.0f, 1.0f,  // 0
         1.0f, 0.0f, 1.0f,  // 1
         1.0f, 0.0f, 1.0f,  // 2
         1.0f, 0.0f, 1.0f,  // 3

         // 앞면 - 빨간색
         1.0f, 0.0f, 0.0f,  // 4
         1.0f, 0.0f, 0.0f,  // 5
         1.0f, 0.0f, 0.0f,  // 6

         // 오른쪽면 - 초록색
         0.0f, 1.0f, 0.0f,  // 7
         0.0f, 1.0f, 0.0f,  // 8
         0.0f, 1.0f, 0.0f,  // 9

         // 뒷면 - 파란색
         0.0f, 0.0f, 1.0f,  // 10
         0.0f, 0.0f, 1.0f,  // 11
         0.0f, 0.0f, 1.0f,  // 12

         // 왼쪽면 - 노란색
         1.0f, 1.0f, 0.0f,  // 13
         1.0f, 1.0f, 0.0f,  // 14
         1.0f, 1.0f, 0.0f   // 15
    };

    const unsigned int Pyramid_indices[] = {
        // 밑면 (사각형) - 아래에서 올려다봤을 때 반시계방향
        0, 2, 1,   0, 3, 2,

        // 측면들 - 바깥쪽에서 봤을 때 반시계방향
        4, 5, 6,    // 앞면
        7, 8, 9,    // 오른쪽면
        10, 11, 12, // 뒷면
        13, 14, 15  // 왼쪽면
    };

    // VAO 생성 및 바인딩
    glGenVertexArrays(1, &VAO_Pyramid);
    glBindVertexArray(VAO_Pyramid);

    // VBO 생성
    glGenBuffers(2, VBO_Pyramid);

    // 정점 위치 데이터
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Pyramid[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Pyramid_vertices), Pyramid_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 정점 색상 데이터
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Pyramid[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Pyramid_colors), Pyramid_colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // EBO (인덱스 버퍼) 설정
    glGenBuffers(1, &EBO_Pyramid);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Pyramid);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Pyramid_indices), Pyramid_indices, GL_STATIC_DRAW);

    // 바인딩 해제
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void DrawPyramid()
{

    glBindVertexArray(VAO_Pyramid);
    if(PyramidWireDraw)
        for (int i = 0; i < 18; i += 3)
        {
            glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void*)(i * sizeof(unsigned int)));
        }
    else
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0); // 18개 인덱스 (6개 삼각형)
    
    glBindVertexArray(0);
}
