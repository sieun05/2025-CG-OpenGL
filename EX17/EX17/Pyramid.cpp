#include <iostream>
#include "Pyramid.h"

// 사각뿔 관련 전역 변수 정의
GLuint VAO_Pyramid = 0;
GLuint VBO_Pyramid[2] = { 0, };
GLuint EBO_Pyramid = 0;

// 사각뿔 그리기 제어 변수
bool drawPyramid = false;

// 사각뿔 애니메이션 관련 변수들 정의
bool pyramidAllFacesOpenEnabled = false;    // o키: 모든 면들이 함께 열기/닫기
float pyramidAllFacesOpenAngle = 0.0f;

bool pyramidEachFaceOpenEnabled = false;    // r키: 각 면이 하나씩 열기/닫기
float pyramidFaceOpenAngles[4] = { 0.0f, 0.0f, 0.0f, 0.0f };     // [0]: 앞면, [1]: 오른쪽면, [2]: 뒷면, [3]: 왼쪽면
int currentOpeningFace = 0;                 // 현재 열리고 있는 면

// 새로운 애니메이션 배열
bool animation_pyramid[2] = { false, false };  // [0]: o키, [1]: r키

void UpdatePyramidAnimations()
{
    const float rotationSpeed = 3.0f;
    const float openSpeed = 2.0f;

    // o키: 모든 면들이 함께 열기/닫기 (220도까지)
    static bool isOpening = false; // 현재 열리는 중인지 닫히는 중인지 상태 추적
    static bool wasKeyPressed = false; // 이전 프레임에서 키가 눌렸는지 추적
    
    // 키가 새로 눌렸는지 감지 (토글 감지)
    bool keyJustPressed = animation_pyramid[0] && !wasKeyPressed;
    
    if (keyJustPressed) {
        // 키가 새로 눌렸을 때 방향 결정
        if (pyramidAllFacesOpenAngle <= 0.0f) {
            isOpening = true; // 닫힌 상태에서 열기
        } else if (pyramidAllFacesOpenAngle >= 232.5f) {
            isOpening = false; // 열린 상태에서 닫기
        } else {
            // 중간 상태에서는 현재 진행 중인 방향 유지
            // 만약 애니메이션이 정지 상태라면 현재 각도에 따라 결정
            isOpening = pyramidAllFacesOpenAngle < 116.25f; // 절반 미만이면 열기, 이상이면 닫기
        }
    }
    
    // wasKeyPressed 업데이트는 키 처리 후에
    wasKeyPressed = animation_pyramid[0];
    
    if (animation_pyramid[0]) {
        if (isOpening) {
            // 열기 애니메이션
            if (pyramidAllFacesOpenAngle < 232.5f) {
                pyramidAllFacesOpenAngle += openSpeed;
                if (pyramidAllFacesOpenAngle >= 232.5f) {
                    pyramidAllFacesOpenAngle = 232.5f;
                }
            }
        } else {
            // 닫기 애니메이션
            if (pyramidAllFacesOpenAngle > 0.0f) {
                pyramidAllFacesOpenAngle -= openSpeed;
                if (pyramidAllFacesOpenAngle <= 0.0f) {
                    pyramidAllFacesOpenAngle = 0.0f;
                }
            }
        }
    }

    // r키: 각 면이 하나씩 번갈아 가며 열기/닫기 (90도까지)
    static bool rKeyWasPressed = false; // r키 이전 상태 추적
    static bool isClosingPhase = false; // 닫는 단계인지 추적
    static int closingFace = 0; // 현재 닫고 있는 면
    static bool allFacesOpened = false; // 모든 면이 열렸는지 추적
    
    bool rKeyJustPressed = animation_pyramid[1] && !rKeyWasPressed;
    
    if (rKeyJustPressed) {
        // r키가 새로 눌렸을 때
        if (!allFacesOpened && !isClosingPhase) {
            // 첫 번째 r키: 열기 시작
            isClosingPhase = false;
            currentOpeningFace = 0;
            allFacesOpened = false;
            // 모든 면 초기화
            for (int i = 0; i < 4; i++) {
                pyramidFaceOpenAngles[i] = 0.0f;
            }
        } else if (allFacesOpened && !isClosingPhase) {
            // 두 번째 r키: 닫기 시작
            isClosingPhase = true;
            closingFace = 3; // 마지막 면부터 닫기 시작 (역순)
        }
    }
    
    // rKeyWasPressed 업데이트는 키 처리 후에
    rKeyWasPressed = animation_pyramid[1];
    
    if (animation_pyramid[1]) {
        if (!isClosingPhase) {
            // 열기 단계
            // 현재 면이 90도까지 열렸으면 다음 면으로
            if (pyramidFaceOpenAngles[currentOpeningFace] >= 90.0f) {
                currentOpeningFace++;
                if (currentOpeningFace >= 4) {
                    // 모든 면이 열렸으면 열기 완료 (닫기 단계로 자동 전환하지 않음)
                    allFacesOpened = true;
                    currentOpeningFace = 3; // 인덱스 유지
                    // 열기 완료 후 애니메이션 정지
                    animation_pyramid[1] = false;
                }
            }
            
            // 현재 면 열기
            if (currentOpeningFace < 4 && pyramidFaceOpenAngles[currentOpeningFace] < 90.0f) {
                pyramidFaceOpenAngles[currentOpeningFace] += openSpeed;
                if (pyramidFaceOpenAngles[currentOpeningFace] >= 90.0f) {
                    pyramidFaceOpenAngles[currentOpeningFace] = 90.0f;
                }
            }
        } else {
            // 닫기 단계
            // 현재 면이 완전히 닫혔으면 이전 면으로
            if (pyramidFaceOpenAngles[closingFace] <= 0.0f && closingFace > 0) {
                closingFace--;
            }
            
            // 현재 면 닫기
            if (closingFace >= 0 && pyramidFaceOpenAngles[closingFace] > 0.0f) {
                pyramidFaceOpenAngles[closingFace] -= openSpeed;
                if (pyramidFaceOpenAngles[closingFace] <= 0.0f) {
                    pyramidFaceOpenAngles[closingFace] = 0.0f;
                    
                    // 모든 면이 닫혔으면 리셋 및 애니메이션 정지
                    if (closingFace == 0) {
                        isClosingPhase = false;
                        allFacesOpened = false;
                        currentOpeningFace = 0;
                        // 닫기 완료 후 애니메이션 정지
                        animation_pyramid[1] = false;
                    }
                }
            }
        }
    } else {
        // r키가 떼어졌을 때 - 현재 상태 유지 (자동으로 닫지 않음)
        // 필요시 여기서 추가 로직 구현 가능
    }
}

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

void DrawPyramid(const glm::mat4& baseModel, const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation)
{
    glBindVertexArray(VAO_Pyramid);
    
    // 1. 밑면 그리기 (항상 고정)
    glm::mat4 bottomMVP = projection * view * baseModel;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(bottomMVP));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

    // 2. 측면들 그리기 (애니메이션 적용)
    // o키가 활성화되어 있거나 각도가 0이 아니면 o키 애니메이션 적용
    bool useAllFacesAnimation = animation_pyramid[0] || (pyramidAllFacesOpenAngle > 0.0f);
    
    // 앞면 (빨간색) - 아래 모서리를 기준으로 x축 회전
    glm::mat4 frontModel = baseModel;
    float frontAngle = useAllFacesAnimation ? pyramidAllFacesOpenAngle : pyramidFaceOpenAngles[0];
    if (frontAngle > 0.0f) {
        // 앞면의 아래쪽 모서리(y=-0.5, z=0.5)를 기준으로 x축 회전
        frontModel = glm::translate(frontModel, glm::vec3(0.0f, -0.5f, 0.5f));
        frontModel = glm::rotate(frontModel, glm::radians(frontAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        frontModel = glm::translate(frontModel, glm::vec3(0.0f, 0.5f, -0.5f));
    }
    glm::mat4 frontMVP = projection * view * frontModel;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(frontMVP));
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));

    // 오른쪽면 (초록색) - 아래 모서리를 기준으로 z축 회전
    glm::mat4 rightModel = baseModel;
    float rightAngle = useAllFacesAnimation ? pyramidAllFacesOpenAngle : pyramidFaceOpenAngles[1];
    if (rightAngle > 0.0f) {
        // 오른쪽면의 아래쪽 모서리(x=0.5, y=-0.5)를 기준으로 z축 회전
        rightModel = glm::translate(rightModel, glm::vec3(0.5f, -0.5f, 0.0f));
        rightModel = glm::rotate(rightModel, glm::radians(-rightAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        rightModel = glm::translate(rightModel, glm::vec3(-0.5f, 0.5f, 0.0f));
    }
    glm::mat4 rightMVP = projection * view * rightModel;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(rightMVP));
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(9 * sizeof(unsigned int)));

    // 뒷면 (파란색) - 아래 모서리를 기준으로 x축 회전
    glm::mat4 backModel = baseModel;
    float backAngle = useAllFacesAnimation ? pyramidAllFacesOpenAngle : pyramidFaceOpenAngles[2];
    if (backAngle > 0.0f) {
        // 뒷면의 아래쪽 모서리(y=-0.5, z=-0.5)를 기준으로 x축 회전
        backModel = glm::translate(backModel, glm::vec3(0.0f, -0.5f, -0.5f));
        backModel = glm::rotate(backModel, glm::radians(-backAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        backModel = glm::translate(backModel, glm::vec3(0.0f, 0.5f, 0.5f));
    }
    glm::mat4 backMVP = projection * view * backModel;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(backMVP));
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(12 * sizeof(unsigned int)));

    // 왼쪽면 (노란색) - 아래 모서리를 기준으로 z축 회전
    glm::mat4 leftModel = baseModel;
    float leftAngle = useAllFacesAnimation ? pyramidAllFacesOpenAngle : pyramidFaceOpenAngles[3];
    if (leftAngle > 0.0f) {
        // 왼쪽면의 아래쪽 모서리(x=-0.5, y=-0.5)를 기준으로 z축 회전
        leftModel = glm::translate(leftModel, glm::vec3(-0.5f, -0.5f, 0.0f));
        leftModel = glm::rotate(leftModel, glm::radians(leftAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        leftModel = glm::translate(leftModel, glm::vec3(0.5f, 0.5f, 0.0f));
    }
    glm::mat4 leftMVP = projection * view * leftModel;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(leftMVP));
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(15 * sizeof(unsigned int)));

    glBindVertexArray(0);
}
