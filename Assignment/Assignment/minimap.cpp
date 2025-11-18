#include "minimap.h"


void DrawMinimap(int windowWidth, int windowHeight, GLint mvpLocation)
{
    // 미니맵 크기와 위치 설정
    int minimapSize = 300;
    int minimapX = windowWidth - minimapSize;  // 오른쪽에서 10px 여백
    int minimapY = windowHeight - minimapSize; // 위쪽에서 10px 여백

    // 현재 뷰포트 저장
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // 미니맵용 뷰포트 설정
    glViewport(minimapX, minimapY, minimapSize, minimapSize);

    // 깊이 버퍼 클리어 (미니맵 영역만)
    glEnable(GL_SCISSOR_TEST);
    glScissor(minimapX, minimapY, minimapSize, minimapSize);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    // 탑뷰 카메라 설정 (위에서 아래로)
    glm::mat4 minimapView = glm::lookAt(
        glm::vec3(0.0f, 25.0f, 0.0f),  // 카메라 위치 (위에서)
        glm::vec3(0.0f, 0.0f, 0.0f),   // 바라보는 지점 (원점)
        glm::vec3(0.0f, 0.0f, -1.0f)   // 위쪽 방향 (Z축 음의 방향)
    );


    // 직교 투영 설정 (탑뷰용)
    //glm::mat4 minimapProjection = glm::ortho(
    //    -12.0f, 12.0f,  // left, right (바닥보다 약간 크게)
    //    -12.0f, 12.0f,  // bottom, top
    //    0.1f, 50.0f     // near, far
    //);

    // 원근 투영 설정 (탑뷰용)
    glm::mat4 minimapProjection = glm::perspective(
        glm::radians(60.0f),  // 시야각 60도 (넓은 시야각으로 더 많은 영역 표시)
        1.0f,                 // 종횡비 1:1 (정사각형 미니맵)
        0.1f,                 // 근평면
        50.0f                 // 원평면
    );

    // 미니맵 바닥 그리기
    DrawMinimapGround(minimapView, minimapProjection, mvpLocation);

    // 미니맵 큐브들 그리기
    if (gridWidth > 0 && gridHeight > 0) {
        DrawMinimapCubes(minimapView, minimapProjection, mvpLocation);
    }

    // 원래 뷰포트 복원
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void DrawMinimapGround(const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation)
{
    // ground.cpp의 DrawGround와 동일하지만 다른 view/projection 사용
    extern GLuint VAO_ground;

    glBindVertexArray(VAO_ground);

    glm::mat4 model = glm::mat4(1.0f); // 바닥은 변환 없음
    glm::mat4 mvp = projection * view * model;

    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void DrawMinimapCubes(const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation)
{
    for (int z = 0; z < gridHeight; z++) {
        for (int x = 0; x < gridWidth; x++) {
            const Cube& cube = cubeGrid[z][x];

            glBindVertexArray(VAO_cube);

            // 미니맵용 모델 행렬 (실제 큐브 위치 사용)
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cube.position);
            model = glm::scale(model, glm::vec3(cube.sizeX, cube.height, cube.sizeZ));

            glm::mat4 mvp = projection * view * model;
            glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }
}

