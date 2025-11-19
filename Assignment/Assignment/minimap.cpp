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
    
    // 미니맵 플레이어 그리기
    DrawMinimapPlayer(minimapView, minimapProjection, mvpLocation);

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

            // 삭제된 큐브는 렌더링하지 않음 (미로에서 삭제된 경우)
            if (cube.isDeleted) {
                continue;
            }

            // 스케일이 0이면 큐브를 렌더링하지 않음 (삭제된 것으로 처리)
            if (cube.currentScale <= 0.0f) {
                continue;
            }

            glBindVertexArray(VAO_cube);

            // 미니맵용 모델 행렬 (실제 큐브 위치와 스케일 사용)
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cube.position);
            
            // 스케일 애니메이션 반영
            float finalHeight = (cube.originalHeight * cube.currentScale);
            model = glm::scale(model, glm::vec3(cube.sizeX, finalHeight, cube.sizeZ));

            glm::mat4 mvp = projection * view * model;
            glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }
}

void DrawMinimapPlayer(const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation)
{
    extern Player player;
    extern bool playerActive;
    
    // 플레이어가 활성화되지 않았으면 그리지 않음
    if (!playerActive || !player.isActive) {
        return;
    }
    
    extern GLuint VAO_cube;  // 큐브와 같은 VAO 사용
    
    glBindVertexArray(VAO_cube);
    
    // 미니맵용 플레이어 모델 행렬 생성
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, player.position);
    
    // 플레이어를 미니맵에서 더 잘 보이게 하기 위해 크기를 약간 키움
    glm::vec3 minimapPlayerSize = player.size;
    minimapPlayerSize.x *= 1.5f;  // X축 크기 1.5배
    minimapPlayerSize.y *= 2.0f;  // Y축 높이 2배 (더 잘 보이게)
    minimapPlayerSize.z *= 1.5f;  // Z축 크기 1.5배
    
    model = glm::scale(model, minimapPlayerSize);
    
    glm::mat4 mvp = projection * view * model;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
    
    // 플레이어 그리기 (빨간색 큐브)
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
}

