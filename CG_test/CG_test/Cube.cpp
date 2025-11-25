#include "Cube.h"

// 정육면체 관련 전역 변수 정의
GLuint VAO_cube = 0;
GLuint VBO_cube[2] = { 0, };
GLuint EBO_cube = 0;

//정육면체 그리기 T/F, 어떤 면 그릴지
bool drawCube = true;
bool CubeWireDraw = false;

vector<Cube> cubes;

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
void CreateCubes()
{
    cubes.resize(5);
    int i{1};
    for(Cube& c : cubes)
    {
		c.size = scale_dis(gen);
        c.color = {color_dis(gen), color_dis(gen), color_dis(gen), 1.0f};
        c.position = glm::vec3(position_dis(gen), 0.0f, position_dis(gen));
        //c.position = glm::vec3(0.0f, 0.0f, 0.0f);
	}
}
void DrawCubes(glm::mat4 gProjection, glm::mat4 gView, GLuint uMVP_loc)
{   
    for(Cube& c : cubes)
        c.DrawCube(gProjection, gView, uMVP_loc);
}

void ChangeCubeColor(RGBA color)
{
    float cube_colors[] = {
        color.r, color.g, color.b,  // 0
        color.r, color.g, color.b,  // 1
        color.r, color.g, color.b,  // 2
        color.r, color.g, color.b,  // 3

        color.r, color.g, color.b,  // 0
        color.r, color.g, color.b,  // 1
        color.r, color.g, color.b,  // 2
        color.r, color.g, color.b,  // 3

        color.r, color.g, color.b,  // 0
        color.r, color.g, color.b,  // 1
        color.r, color.g, color.b,  // 2
        color.r, color.g, color.b,  // 3

        color.r, color.g, color.b,  // 0
        color.r, color.g, color.b,  // 1
        color.r, color.g, color.b,  // 2
        color.r, color.g, color.b,  // 3

        color.r, color.g, color.b,  // 0
        color.r, color.g, color.b,  // 1
        color.r, color.g, color.b,  // 2
        color.r, color.g, color.b,  // 3

        color.r, color.g, color.b,  // 0
        color.r, color.g, color.b,  // 1
        color.r, color.g, color.b,  // 2
        color.r, color.g, color.b,  // 3
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);
}

void AnimateCubes()
{
    for(Cube& c : cubes)
		c.UpdateAnimation();
}


void Cube::DrawCube(glm::mat4 gProjection, glm::mat4 gView, GLuint uMVP_loc)
{
	//ChangeCubeColor(this->color);
    glBindVertexArray(VAO_cube);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(this->size));
    model = glm::rotate(model, glm::radians(this->xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(this->yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(this->zAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, this->position);

	model = glm::rotate(model, glm::radians(this->obj_yAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.5f)); // 앞면 이동
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, -0.5f)); // 앞면 이동


    glm::mat4 MVP_cube = gProjection * gView * model;
    glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(MVP_cube));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(this->size));
    model = glm::rotate(model, glm::radians(this->xAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(this->yAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(this->zAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, this->position);

    model = glm::rotate(model, glm::radians(this->obj_yAngle), glm::vec3(0.0f, 1.0f, 0.0f));

	MVP_cube = gProjection * gView * model;
	glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(MVP_cube));
    glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));

	//cout << "Cube position: (" << this->position.x << ", " << this->position.y << ", " << this->position.z << ")\n";

    //if (CubeWireDraw) {
    //    for (int i = 0; i < 36; i += 3)
    //    {
    //        glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void*)(i * sizeof(unsigned int)));
    //    }
    //}
    //else
    //    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // 36개 인덱스 (12개 삼각형)
    glBindVertexArray(0);
}

void Cube::UpdateAnimation()
{
    //if (this->animate[0]) // x축
    //    xAngle += 1.0f;
    //if (this->animate[1]) // y축
    //    yAngle += 1.0f;
    //if (this->animate[2]) // z축
    //    zAngle += 1.0f;

	obj_yAngle += 2.0f;

    this->position.y += 0.005f;
}
