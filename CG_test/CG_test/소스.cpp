#include "헤더.h"
#include "WindowToNDC.h"
#include "shader_func.h"
#include "Axes.h"
#include "Cube.h"
#include "Pyramid.h"
#include "ground.h"

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
	glutCreateWindow("Example16");

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
	//glutSpecialFunc(SpecialKeyDown);    // 화살표 등 특수키 눌림 처리
	//glutSpecialUpFunc(SpecialKeyUp);
	glutTimerFunc(16, Timer, 1); // 약 60FPS로 타이머 시작

	InitBuffer();
	glutMainLoop();
}

GLvoid Timer(int value)
{
	AnimateCubes(); // 정육면체 애니메이션 업데이트

	glutPostRedisplay();
	glutTimerFunc(16, Timer, 1); // 약 60FPS로 타이머 시작
}

void InitBuffer()
{
	glEnable(GL_DEPTH_TEST); // 깊이버퍼 활성화
	//glEnable(GL_CULL_FACE);
	InitAxesBuffer();        // 좌표축 초기화
	InitCubeBuffer();        // 정육면체 초기화
	InitPyramidBuffer();     // 삼각뿔 초기화
	InitGroundBuffer();	  // 바닥 초기화

	CreateCubes();          // 정육면체 객체들 생성
}

//--- 출력 콜백함수
GLvoid drawScene()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//GL_DEPTH_BUFFER_BIT 깊이에 따른 은면제거


	//--- 렌더링 파이프라인에 세이더 불러우기
	glUseProgram(shaderProgramID);

	// --- View: 카메라를 뒤쪽 위쪽에서 원점을 바라보도록 설정
	gView = glm::mat4(1.0f);
	gView = glm::lookAt(		//카메라 외부파라미터
		glm::vec3(10.0f, 7.0f, 10.0f),  // 카메라 위치 (x, y, z축이 모두 보이는 위치)	EYE
		glm::vec3(0.0f, 0.0f, 0.0f),  // 바라보는 지점 (원점) 							AT
		glm::vec3(0.0f, 1.0f, 0.0f)   // 위쪽 방향 벡터 					 			UP
	);
	//gView = glm::rotate(gView, glm::radians(cameraYAngle), glm::vec3(0.0f, 1.0f, 0.0f)); // x축으로 -20도 회전

	// 좌표축 그리기 (회전 없이)
	gModel = glm::mat4(1.0f); // 단위 행렬
	glm::mat4 MVP_axes = gProjection * gView * gModel;
	glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(MVP_axes));
	DrawAxes();  // 좌표축 그리기

	// Cube 그리기 (x축 회전 적용)
	if(drawCube) {
		DrawCubes(gProjection, gView, uMVP_loc);  // 정육면체 그리기
	}
	
	//if (drawPyramid){
	//	gModel = glm::mat4(1.0f);
	//	// translation 먼저 설정하고 rotation을 적용 -> 오브젝트가 자신의 중심에서 회전하고 이동됨
	//	gModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	//	glm::mat4 MVP_pyramid = gProjection * gView * gModel;
	//	glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(MVP_pyramid));
	//	DrawPyramid(); // 삼각뿔 그리기
	//}

	gModel = glm::mat4(1.0f);
	glm::mat4 MVP_ground = gProjection * gView * gModel;
	glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(MVP_ground));
	DrawGround(); // 바닥 그리기

	glutSwapBuffers();
}

//--- 다시그리기콜백함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glUseProgram(shaderProgramID);

	float aspect = (h == 0) ? 1 : (float)w / (float)h;
	
	// 원근 투영 사용 (3D 효과를 더 잘 보여줌)
	gProjection = glm::perspective(		//orthographic: 직교투영, perspective: 원근투영
		glm::radians(45.0f),  // 시야각 45도	fovy
		aspect,               // 종횡비			aspect
		0.1f,                 // 근평면			-n
		100.0f                // 원평면			-f
	);

	glUseProgram(0);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'o':
		drawCube = !drawCube;
		drawPyramid = !drawPyramid;
		break;
	case 'p':
		changeGroundColor({ 0.0f, 1.0f, 0.0f, 1.0f }); // 바닥 색상을 녹색으로 변경
		break;
	case 'w':
	case 'W':
		CubeWireDraw = CubeWireDraw ? false : true;
		PyramidWireDraw = PyramidWireDraw ? false : true;
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}