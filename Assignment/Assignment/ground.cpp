#include "ground.h"

GLuint VAO_ground;
GLuint VBO_ground[2];  // [0]: 정점, [1]: 색상
GLuint EBO_ground;     // 인덱스 버퍼

void InitGroundBuffer()
{
	const float size = 10.0f; // 바닥 크기를 더 크게
	const float color = 0.3f; // 색상을 더 밝게

	const float ground_vertices[] = {
		-size, 0.0f, -size,  // 0: 왼쪽 뒤
		 size, 0.0f, -size,  // 1: 오른쪽 뒤
		 size, 0.0f,  size,  // 2: 오른쪽 앞
		-size, 0.0f,  size   // 3: 왼쪽 앞
	};

	const float ground_colors[] = {
		color, color, color,
		color, color, color,
		color, color, color,
		color, color, color
	};

	// 반시계방향으로 인덱스 순서 변경 (face culling 대응)
	const unsigned int ground_indices[] = {
		0, 2, 1,  // 첫 번째 삼각형 (반시계방향)
		0, 3, 2   // 두 번째 삼각형 (반시계방향)
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

void DrawGround(const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation)
{
	glBindVertexArray(VAO_ground);

	glm::mat4 model = glm::mat4(1.0f); // 바닥은 변환 없음
	glm::mat4 mvp = projection * view * model;

	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
