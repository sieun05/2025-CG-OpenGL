#include "헤더.h"
#include "WindowToNDC.h"
#include "shader_func.h"

#include "Axes.h"
#include "ground.h"

void InitBuffer();

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);
//GLvoid SpecialKeyDown(int key, int x, int y);
//GLvoid SpecialKeyUp(int key, int x, int y);

// 전역 변수 정의 (CommonHeaders.h에서 extern으로 선언된 것들)
// shaderProgramID는 shader_func.h에서 이미 정의됨
glm::mat4 gProjection(1.0f);
glm::mat4 gView(1.0f);
glm::mat4 gModel(1.0f);
GLint uMVP_loc = -1;

//직각투영/원근투영
float aspect = 1.0f;
bool perspectiveProjection = false;
float EYE_z = 18.0f;


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

	width = 1280;
	height = 800;
	//--- 윈도우생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );		//GLUT_DEPTH 깊이에 따른 은면제거
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Example17");

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
	glutPostRedisplay();
	glutTimerFunc(16, Timer, 1); // 약 60FPS로 타이머 시작
}

void InitBuffer()
{
	glEnable(GL_DEPTH_TEST); // 깊이버퍼 활성화
	glEnable(GL_CULL_FACE); // Face culling 비활성화 (ground가 보이도록)

	InitAxesBuffer();
	InitGroundBuffer();
}

//--- 출력 콜백함수
GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//GL_DEPTH_BUFFER_BIT 깊이에 따른 은면제거


	//--- 렌더링 파이프라인에 세이더 불러우기
	glUseProgram(shaderProgramID);

	// --- View: 카메라를 뒤쪽 위쪽에서 원점을 바라보도록 설정 (더 높은 위치에서)
	gView = glm::mat4(1.0f);
	gView = glm::lookAt(		//카메라 외부파라미터
		glm::vec3(18.0f, 20.0f, EYE_z),  // 카메라 위치를 더 높고 멀리 (x, y, z축이 모두 보이는 위치)
		glm::vec3(0.0f, 0.0f, 0.0f),  // 바라보는 지점 (원점) 							AT
		glm::vec3(0.0f, 1.0f, 0.0f)   // 위쪽 방향 벡터 					 			UP
	);


	DrawAxes();  // 좌표축 그리기

	//--- 모델 그리기
	DrawGround(gView, gProjection, uMVP_loc);

	glutSwapBuffers();
}

//--- 다시그리기콜백함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glUseProgram(shaderProgramID);

	aspect = (h == 0) ? 1 : (float)w / (float)h;
	
	//원근 투영 사용 (3D 효과를 더 잘 보여줌)
	gProjection = glm::perspective(
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
	case 'o':	//직각투영
		//perspectiveProjection = false;

		gProjection = glm::ortho(
			-10.0f * aspect, 10.0f * aspect,   // left, right
			-10.0f, 10.0f,                     // bottom, top
			0.1f, 100.0f                       // near, far
		);
		break;
	case 'p':	//원근투영
		//perspectiveProjection = true;

		gProjection = glm::perspective(
			glm::radians(45.0f),  // 시야각 45도	fovy
			aspect,               // 종횡비			aspect
			0.1f,                 // 근평면			-n
			100.0f                // 원평면			-f
		);
		break;
	case 'z':
		//카메라를 원점에서 더 멀리
		EYE_z = min(++EYE_z, 40.0f);
		break;
	case 'Z':
		//카메라를 원점에서 더 가깝게
		EYE_z = max(--EYE_z, 0.0f);
		break;

	case 'q':
	case 'Q':
		exit(0);
		break;
	}

	glutPostRedisplay();
}

//// 특수키 눌림 (화살표 등)
//GLvoid SpecialKeyDown(int key, int x, int y)
//{
//	switch (key) {
//	case GLUT_KEY_LEFT:
//		moveLeft = true;
//		break;
//	case GLUT_KEY_RIGHT:
//		moveRight = true;
//		break;
//	case GLUT_KEY_UP:
//		moveUp = true;
//		break;
//	case GLUT_KEY_DOWN:
//		moveDown = true;
//		break;
//	}
//}
//
//// 특수키 떼었을 때
//GLvoid SpecialKeyUp(int key, int x, int y)
//{
//	switch (key) {
//	case GLUT_KEY_LEFT:
//		moveLeft = false;
//		break;
//	case GLUT_KEY_RIGHT:
//		moveRight = false;
//		break;
//	case GLUT_KEY_UP:
//		moveUp = false;
//		break;
//	case GLUT_KEY_DOWN:
//		moveDown = false;
//		break;
//	}
//}//}//}//}