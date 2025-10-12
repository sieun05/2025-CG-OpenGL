#include "헤더.h"

GLvoid InitBuffer();
void updateBuffer();
GLvoid Timer(int value);

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);

// 삼각형 데이터
GLfloat triangle[9] = {
	0.0f, 0.1f, 0.0f,   // 위쪽 꼭짓점
	-0.1f, -0.1f, 0.0f,  // 왼쪽 아래 꼭짓점
	0.1f, -0.1f, 0.0f   // 오른쪽 아래 꼭짓점
};

GLfloat colors[9] = {
	1.0f, 0.0f, 0.0f,   // 빨간색
	0.0f, 1.0f, 0.0f,   // 초록색
	0.0f, 0.0f, 1.0f    // 파란색
};

// 삼각형 위치
GLfloat triangleX = 0.0f;
GLfloat triangleY = 0.0f;

// 움직임 상태 토글 변수들
bool jumpMovement = false;
bool zigzagMovement = false;
bool squareSpiralMovement = false;
bool circleSpiralMovement = false;

// 움직임 관련 변수들
bool moveRight = true;
float diagonalVelocityX = 1.0f;  // 대각선 이동의 X 방향 속도
float diagonalVelocityY = 1.0f;  // 대각선 이동의 Y 방향 속도
float spiralRadius = 0.1f;
float spiralAngle = 0.0f;
float circleAngle = 0.0f;
float circleRadius = 0.3f;

// 버퍼 ID
GLuint VAO, VBO[2];

//--- 메인 함수
void main(int argc, char** argv)
//--- 윈도우출력하고콜백함수설정
{
	GLint width, height;

	width = 800;
	height = 600;
	//--- 윈도우생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Example10 - Triangle Movement");

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	//--- 세이더읽어와서세이더프로그램만들기: 사용자정의함수호출
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(16, Timer, 1); // 약 60FPS로 타이머 시작

	InitBuffer();
	glutMainLoop();
}

GLvoid InitBuffer()
{
	// VAO 생성 및 바인딩
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// 위치 버퍼 생성
	glGenBuffers(1, &VBO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 색상 버퍼 생성
	glGenBuffers(1, &VBO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}

void updateBuffer()
{
	// 삼각형 위치 업데이트
	GLfloat updatedTriangle[9] = {
		triangleX, triangleY + 0.1f, 0.0f,      // 위쪽 꼭짓점
		triangleX - 0.1f, triangleY - 0.1f, 0.0f, // 왼쪽 아래 꼭짓점
		triangleX + 0.1f, triangleY - 0.1f, 0.0f  // 오른쪽 아래 꼭짓점
	};

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(updatedTriangle), updatedTriangle);
}

GLvoid Timer(int value)
{
	const float moveStep = 0.02f;

	// 1. 대각선 이동 (벽에 부딪히면 튕기기)
	if (jumpMovement) {
		triangleX += diagonalVelocityX * moveStep;
		triangleY += diagonalVelocityY * moveStep;

		// 화면 경계 검사 및 방향 변경
		if (triangleX > 0.9f) {
			triangleX = 0.9f;
			diagonalVelocityX = -diagonalVelocityX;  // X 방향 반전
		}
		if (triangleX < -0.9f) {
			triangleX = -0.9f;
			diagonalVelocityX = -diagonalVelocityX;  // X 방향 반전
		}
		if (triangleY > 0.9f) {
			triangleY = 0.9f;
			diagonalVelocityY = -diagonalVelocityY;  // Y 방향 반전
		}
		if (triangleY < -0.9f) {
			triangleY = -0.9f;
			diagonalVelocityY = -diagonalVelocityY;  // Y 방향 반전
		}
	}

	// 2. 좌우 지그재그 이동
	if (zigzagMovement) {
		if (moveRight) {
			triangleX += moveStep;
			if (triangleX > 0.8f) {
				triangleX = 0.8f;
				moveRight = false;
			}
		} else {
			triangleX -= moveStep;
			if (triangleX < -0.8f) {
				triangleX = -0.8f;
				moveRight = true;
			}
		}
	}

	// 3. 사각 스파이럴 이동
	if (squareSpiralMovement) {
		spiralAngle += 0.05f;
		if (spiralAngle >= 6.28f) {  // 2π
			spiralAngle = 0.0f;
			spiralRadius += 0.01f;
			if (spiralRadius > 0.7f) spiralRadius = 0.1f;
		}

		triangleX = spiralRadius * cos(spiralAngle);
		triangleY = spiralRadius * sin(spiralAngle);
	}

	// 4. 원 스파이럴 이동
	if (circleSpiralMovement) {
		circleAngle += 0.03f;
		if (circleAngle >= 6.28f) circleAngle = 0.0f;
		circleRadius += 0.001f;

		triangleX = circleRadius * cos(circleAngle);
		triangleY = circleRadius * sin(circleAngle);
	}

	// 움직임이 활성화된 경우에만 버퍼 업데이트
	if (jumpMovement || zigzagMovement || squareSpiralMovement || circleSpiralMovement) {
		updateBuffer();
		glutPostRedisplay();
	}

	glutTimerFunc(16, Timer, 1); // 다음 타이머 설정
}

//--- 출력 콜백함수
GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//--- 렌더링 파이프라인에 세이더 불러오기
	glUseProgram(shaderProgramID);

	// 삼각형 그리기
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glutSwapBuffers();
}

//--- 다시그리기콜백함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '1':  // 대각선 이동 토글
		jumpMovement = !jumpMovement;
		if (jumpMovement) {
			// 다른 움직임 비활성화
			zigzagMovement = false;
			squareSpiralMovement = false;
			circleSpiralMovement = false;
			// 초기 방향 설정 (우상향 대각선)
			triangleX = 0.4f;
			triangleY = 0.0f;

			diagonalVelocityX = 1.0f;
			diagonalVelocityY = 1.0f;
		}
		break;
	case '2':  // 좌우 지그재그 이동 토글
		zigzagMovement = !zigzagMovement;
		if (zigzagMovement) {
			// 다른 움직임 비활성화
			jumpMovement = false;
			squareSpiralMovement = false;
			circleSpiralMovement = false;
			// 초기 상태 설정
			moveRight = true;
		}
		break;
	case '3':  // 사각 스파이럴 이동 토글
		squareSpiralMovement = !squareSpiralMovement;
		if (squareSpiralMovement) {
			// 다른 움직임 비활성화
			jumpMovement = false;
			zigzagMovement = false;
			circleSpiralMovement = false;
			// 초기 상태 설정
			spiralRadius = 0.1f;
			spiralAngle = 0.0f;
		}
		break;
	case '4':  // 원 스파이럴 이동 토글
		circleSpiralMovement = !circleSpiralMovement;
		if (circleSpiralMovement) {
			// 다른 움직임 비활성화
			jumpMovement = false;
			zigzagMovement = false;
			squareSpiralMovement = false;
			// 초기 상태 설정
			circleAngle = 0.0f;
			spiralRadius = 0.1f;
		}
		break;
	case 's':
	case 'S':  // 모든 움직임 정지
		jumpMovement = false;
		zigzagMovement = false;
		squareSpiralMovement = false;
		circleSpiralMovement = false;
		break;
	case 'r':
	case 'R':  // 초기 위치로 리셋
		triangleX = 0.0f;
		triangleY = 0.0f;
		jumpMovement = false;
		zigzagMovement = false;
		squareSpiralMovement = false;
		circleSpiralMovement = false;
		updateBuffer();
		glutPostRedisplay();
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}
}