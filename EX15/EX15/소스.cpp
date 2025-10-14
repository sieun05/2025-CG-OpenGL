#include "헤더.h"

void InitBuffer();

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);

glm::mat4 gProjection(1.0f);
glm::mat4 gView(1.0f);
glm::mat4 gModel(1.0f);

GLint uMVP_loc = -1;

GLuint VAO_axes = 0, VBO_axes[2] = { 0, };
void InitAxesBuffer() {
	const float L = 2.0f;

	// [x,y,z, r,g,b] * 6개 정점 (GL_LINES이므로 2개 = 1개 선분)
	const float axes_vertices[] = {
		// X axis (red)
		-L, 0.0f, 0.0f,   
		 L, 0.0f, 0.0f,   

		 // Y axis (blue)
		  0.0f, -L, 0.0f, 
		  0.0f,  L, 0.0f, 

		  // Z axis (green
		   0.0f, 0.0f, -L,
		   0.0f, 0.0f,  L,
	};

	const float axes_colors[] = {
		// X axis (red)
		1.0f, 0.0f, 0.0f, // start point
		1.0f, 0.0f, 0.0f, // end point

		// Y axis (blue)
		0.0f, 0.0f, 1.0f, // start point
		0.0f, 0.0f, 1.0f, // end point

		// Z axis (green)
		0.0f, 1.0f, 0.0f, // start point
		0.0f, 1.0f, 0.0f, // end point
	};

	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glGenBuffers(2, VBO_axes);

	// positions -> location=0
	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes_vertices), axes_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// colors -> location=1
	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes_colors), axes_colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void DrawAxes()
{
	glBindVertexArray(VAO_axes);
	glLineWidth(3.0f);
	glDrawArrays(GL_LINES, 0, 6); // 6개의 정점 = 3개의 선분
	glBindVertexArray(0);
}

void AfterMakeShaders() 
{
	glUseProgram(shaderProgramID);
	uMVP_loc = glGetUniformLocation(shaderProgramID, "uMVP");
	if (uMVP_loc < 0) { printf("uMVP get error\n"); exit(1); }
	glUseProgram(0);
}

//--- 메인 함수
void main(int argc, char** argv)
//--- 윈도우출력하고콜백함수설정
{
	GLint width, height;

	width = 800;
	height = 800;
	//--- 윈도우생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );		//GLUT_DEPTH 깊이에 따른 은면제거
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Example15");

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	//--- 세이더읽어와서세이더프로그램만들기: 사용자정의함수호출
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();
	AfterMakeShaders();	//uniform?

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(16, Timer, 1); // 약 60FPS로 타이머 시작

	InitBuffer();
	glutMainLoop();
}

GLvoid Timer(int value)
{
	glutTimerFunc(16, Timer, 1); // 약 60FPS로 타이머 시작
}

void InitBuffer()
{
	glEnable(GL_DEPTH_TEST); // 깊이버퍼 활성화
	InitAxesBuffer();
}

//--- 출력 콜백함수
GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//GL_DEPTH_BUFFER_BIT 깊이에 따른 은면제거

	//--- 렌더링 파이프라인에 세이더 불러오기
	glUseProgram(shaderProgramID);

	// --- View: 카메라 살짝 뒤로
	gView = glm::translate(gView, glm::vec3(0, 0, -4.0f));

	// --- Model: 등각 회전 (X 35.264°, Y 45°)
	gModel = glm::mat4(1.0f);
	gModel = glm::rotate(gModel, glm::radians(35.264f), glm::vec3(1, 0, 0));
	gModel = glm::rotate(gModel, glm::radians(45.0f), glm::vec3(0, 1, 0));

	// --- 최종 MVP
	glm::mat4 MVP = gProjection * gView * gModel;

	DrawAxes();

	glutSwapBuffers();
}

//--- 다시그리기콜백함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glUseProgram(shaderProgramID);

	float viewHalf = 3.0f;

	float aspect = (h == 0) ? 1 : (float)w / (float)h;
	float left = -viewHalf * aspect;
	float right = viewHalf * aspect;
	float bottom = -viewHalf;
	float top = viewHalf;

	gProjection = glm::ortho(left, right, bottom, top, -50.0f, 50.0f);

	glUseProgram(0);

	glEnable(GL_DEPTH_TEST); // 깊이버퍼 활성화
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