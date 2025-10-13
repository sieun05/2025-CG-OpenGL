#include "헤더.h"

GLvoid InitBuffer();
void updateBuffer();

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);

//--- 메인 함수
void main(int argc, char** argv)
//--- 윈도우출력하고콜백함수설정
{
	GLint width, height;

	width = 800;
	height = 600;
	//--- 윈도우생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );		//GLUT_DEPTH 깊이에 따른 은면제거
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
	//// VAO 생성 및 바인딩
	//glGenVertexArrays(1, &VAO);
	//glBindVertexArray(VAO);

	//// 위치 버퍼 생성
	//glGenBuffers(1, &VBO[0]);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_DYNAMIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(0);

	//// 색상 버퍼 생성
	//glGenBuffers(1, &VBO[1]);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(1);
}

void updateBuffer()
{
	//// 삼각형 위치 업데이트
	//GLfloat updatedTriangle[9] = {
	//	triangleX, triangleY + 0.1f, 0.0f,      // 위쪽 꼭짓점
	//	triangleX - 0.1f, triangleY - 0.1f, 0.0f, // 왼쪽 아래 꼭짓점
	//	triangleX + 0.1f, triangleY - 0.1f, 0.0f  // 오른쪽 아래 꼭짓점
	//};

	//glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(updatedTriangle), updatedTriangle);
}

GLvoid Timer(int value)
{
	glutTimerFunc(16, Timer, 1); // 약 60FPS로 타이머 시작
}

//--- 출력 콜백함수
GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//GL_DEPTH_BUFFER_BIT 깊이에 따른 은면제거

	//--- 렌더링 파이프라인에 세이더 불러오기
	glUseProgram(shaderProgramID);


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
	
	case 'q':
	case 'Q':
		exit(0);
		break;
	}
}