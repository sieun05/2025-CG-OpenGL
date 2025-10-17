#include "헤더.h"
#include "WindowToNDC.h"
#include "shader_func.h"
#include "Axes.h"
#include "Cube.h"
#include "Pyramid.h"
#include "Random.h"

void InitBuffer();

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);

// 전역 변수 정의 (CommonHeaders.h에서 extern으로 선언된 것들)
// shaderProgramID는 shader_func.h에서 이미 정의됨
glm::mat4 gProjection(1.0f);
glm::mat4 gView(1.0f);
glm::mat4 gModel(1.0f);
GLint uMVP_loc = -1;

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
	AfterMakeShaders();	//셰이더에서 uniform 변수 위치 얻기

	glutReshapeFunc(Reshape);
	glutDisplayFunc(drawScene);
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
	InitAxesBuffer();        // 좌표축 초기화
	InitCubeBuffer();        // 정육면체 초기화
	InitPyramidBuffer();     // 삼각뿔 초기화
}

//--- 출력 콜백함수
GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//GL_DEPTH_BUFFER_BIT 깊이에 따른 은면제거

	//--- 렌더링 파이프라인에 세이더 불러오기
	glUseProgram(shaderProgramID);

	// --- View: 카메라를 뒤쪽 위쪽에서 원점을 바라보도록 설정
	gView = glm::mat4(1.0f);
	gView = glm::lookAt(		//카메라 외부파라미터
		glm::vec3(3.0f, 2.0f, 3.0f),  // 카메라 위치 (x, y, z축이 모두 보이는 위치)	EYE
		glm::vec3(0.0f, 0.0f, 0.0f),  // 바라보는 지점 (원점) 						AT
		glm::vec3(0.0f, 1.0f, 0.0f)   // 위쪽 방향 벡터 					 		UP
	);

	// --- Model: 단위 행렬 (좌표축 자체는 회전시키지 않음)
	gModel = glm::mat4(1.0f);

	// --- 최종 MVP 계산 및 셰이더에 전달
	glm::mat4 MVP = gProjection * gView * gModel;
	glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(MVP));

	DrawAxes();  // 좌표축 그리기
	if(drawCube)	DrawCube();  // 정육면체 그리기
	if(drawPyramid)	DrawPyramid(); // 삼각뿔 그리기

	glutSwapBuffers();
}

//--- 다시그리기콜백함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glUseProgram(shaderProgramID);

	float aspect = (h == 0) ? 1 : (float)w / (float)h;
	
	// 원근 투영 사용 (3D 효과를 더 잘 보여줌)
	gProjection = glm::perspective(
		glm::radians(45.0f),  // 시야각 45도	fovy
		aspect,               // 종횡비			aspect
		0.1f,                 // 근평면			-n
		100.0f                // 원평면			-f
	);

	glUseProgram(0);

	glEnable(GL_DEPTH_TEST); // 깊이버퍼 활성화
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
		drawCube = true;
		drawPyramid = false;
		randomCube = false;
		cubeFace = (int)(key - '0');
		break;
	case 'r':
		drawCube = true;
		cubeFace = 6; // 전체 면
		break;
	case 'c':
		while (true) {
			randomFace[0] = Cube_face_dis(gen);
			randomFace[1] = Cube_face_dis(gen);
			if (randomFace[0] != randomFace[1])
				break;
		}
		drawCube = true;
		drawPyramid = false;
		randomCube = true;
		break;
	case '6':
	case '7':
	case '8':
	case '9':
		drawCube = false;
		drawPyramid = true;
		randomPyramid = false;

		pyramidFace = (int)(key - '0') - 6;
		break;
	case 't':
		drawCube = false;
		drawPyramid = true;
		randomPyramid = true;
		randomPyramidFace = Pyramid_face_dis(gen); // 0~3
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}

	glutPostRedisplay();
}