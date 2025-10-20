#include "Cube.h"

// 정육면체 관련 전역 변수 정의
GLuint VAO_cube = 0;
GLuint VBO_cube[2] = { 0, };
GLuint EBO_cube = 0;

//정육면체 그리기 T/F, 어떤 면 그릴지
bool drawCube = true;
bool animetion_cube[4]{ false };

// 큐브 애니메이션 관련 변수들 정의
float topFaceRotationAngle = 0.0f;

float frontFaceOpenAngle = 0.0f;

float sideFaceRotationAngle = 0.0f;

float backFaceScale = 1.0f;

// 새로운 애니메이션 배열 (소스.cpp와 호환)
bool animation_cube[4] = { false, false, false, false };  // [0]: t키, [1]: f키, [2]: s키, [3]: b키

void UpdateCubeAnimations()
{
    const float rotationSpeed = 3.0f;
    const float openSpeed = 2.0f;
    const float scaleSpeed = 0.02f;

    // t키: 윗면 z축을 중심으로 회전
    if (animation_cube[0]) {
        topFaceRotationAngle += rotationSpeed;
        if (topFaceRotationAngle >= 360.0f) {
            topFaceRotationAngle -= 360.0f;
        }
    }

    // f키: 앞면 열기/닫기 (0도에서 90도까지)
    if (animation_cube[1]) {
        if (frontFaceOpenAngle < 90.0f) {
            frontFaceOpenAngle += openSpeed;
            if (frontFaceOpenAngle >= 90.0f) {
                frontFaceOpenAngle = 90.0f;
            }
        }
    } else {
        if (frontFaceOpenAngle > 0.0f) {
            frontFaceOpenAngle -= openSpeed;
            if (frontFaceOpenAngle <= 0.0f) {
                frontFaceOpenAngle = 0.0f;
            }
        }
    }

    // s키: 옆면들 제자리에서 회전
    if (animation_cube[2]) {
        sideFaceRotationAngle += rotationSpeed;
        if (sideFaceRotationAngle >= 360.0f) {
            sideFaceRotationAngle -= 360.0f;
        }
    }

    // b키: 뒷면 스케일링 (1.0에서 0.0까지)
    if (animation_cube[3]) {
        if (backFaceScale > 0.0f) {
            backFaceScale -= scaleSpeed;
            if (backFaceScale <= 0.0f) {
                backFaceScale = 0.0f;
            }
        }
    } else {
        if (backFaceScale < 1.0f) {
            backFaceScale += scaleSpeed;
            if (backFaceScale >= 1.0f) {
                backFaceScale = 1.0f;
            }
        }
    }
}

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

void DrawCube(const glm::mat4& baseModel, const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation)
{
    glBindVertexArray(VAO_cube);
    
    // 각 면을 개별적으로 그리기
    
    // 1. 앞면 (Red) - f키: 위축을 기준으로 회전하여 열기/닫기
    glm::mat4 frontModel = baseModel;
    if (frontFaceOpenAngle > 0.0f) {
        // 앞면을 위쪽 모서리를 기준으로 x축 회전 (앞면의 위쪽 모서리는 y=0.5, z=0.5에 위치)
        frontModel = glm::translate(frontModel, glm::vec3(0.0f, 0.5f, 0.5f)); // 회전 중심(위쪽 모서리)으로 이동
        frontModel = glm::rotate(frontModel, glm::radians(-frontFaceOpenAngle), glm::vec3(1.0f, 0.0f, 0.0f)); // x축 회전
        frontModel = glm::translate(frontModel, glm::vec3(0.0f, -0.5f, -0.5f)); // 원래 위치로 복귀
    }
    glm::mat4 frontMVP = projection * view * frontModel;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(frontMVP));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

    // 2. 뒷면 (Green) - b키: 제자리에서 작아지면서 없어지기/커지면서 나타나기
    if (backFaceScale > 0.0f) {
        glm::mat4 backModel = baseModel;
        // 뒷면의 중심점(0, 0, -0.5)을 기준으로 스케일링
        backModel = glm::translate(backModel, glm::vec3(0.0f, 0.0f, -0.5f)); // 뒷면 중심으로 이동
        backModel = glm::scale(backModel, glm::vec3(backFaceScale, backFaceScale, backFaceScale)); // 스케일링
        backModel = glm::translate(backModel, glm::vec3(0.0f, 0.0f, 0.5f)); // 원래 위치로 복귀
        glm::mat4 backMVP = projection * view * backModel;
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(backMVP));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));
    }

    // 3. 왼쪽면 (Blue) - s키: 중점을 기준으로 제자리에서 회전
    glm::mat4 leftModel = baseModel;
    if (sideFaceRotationAngle > 0.0f) {
        // 면의 중심에서 x축 회전
        leftModel = glm::rotate(leftModel, glm::radians(sideFaceRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    glm::mat4 leftMVP = projection * view * leftModel;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(leftMVP));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(unsigned int)));

    // 4. 오른쪽면 (Yellow) - s키: 중점을 기준으로 제자리에서 회전
    glm::mat4 rightModel = baseModel;
    if (sideFaceRotationAngle > 0.0f) {
        // 면의 중심에서 x축 회전
        rightModel = glm::rotate(rightModel, glm::radians(sideFaceRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    }
    glm::mat4 rightMVP = projection * view * rightModel;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(rightMVP));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(unsigned int)));

    // 5. 아래면 (Magenta) - 변환 없음
    glm::mat4 bottomMVP = projection * view * baseModel;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(bottomMVP));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(unsigned int)));

    // 6. 위면 (Cyan) - t키: 윗면의 가운데 축(z축)을 중심으로 회전
    glm::mat4 topModel = baseModel;
    if (topFaceRotationAngle > 0.0f) {
        // 위면을 z축 중심으로 회전
        topModel = glm::rotate(topModel, glm::radians(topFaceRotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    glm::mat4 topMVP = projection * view * topModel;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(topMVP));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(unsigned int)));
        
    glBindVertexArray(0);
}