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
//GLvoid SpecialKeyDown(int key, int x, int y);
//GLvoid SpecialKeyUp(int key, int x, int y);

// 전역 변수 정의 (CommonHeaders.h에서 extern으로 선언된 것들)
// shaderProgramID는 shader_func.h에서 이미 정의됨
glm::mat4 gProjection(1.0f);
glm::mat4 gView(1.0f);
glm::mat4 gModel(1.0f);
GLint uMVP_loc = -1;

// 은면제거 토글 변수
bool depthTestEnabled = true;
bool backfaceCullEnabled = true;

// y축 회전 애니메이션 변수
float yRotationAngle = 0.0f;
bool yRotationEnabled = false;
int yRotationDirection = 1; // 1: 양의 방향, -1: 음의 방향

void AfterMakeShaders() 
{
	glUseProgram(shaderProgramID);
	uMVP_loc = glGetUniformLocation(shaderProgramID, "uMVP");
	if (uMVP_loc < 0) { printf("uMVP get error\n"); exit(1); }
	glUseProgram(0);
}

// y축 회전 각도 업데이트 함수
void UpdateYRotation()
{
	const float rotationSpeed = 2.0f; // 회전 속도 (도/프레임)
	yRotationAngle += rotationSpeed * yRotationDirection;

	// 360도를 넘으면 0으로 리셋
	if (yRotationAngle >= 360.0f) {
		yRotationAngle -= 360.0f;
	}
	else if (yRotationAngle < 0.0f) {
		yRotationAngle += 360.0f;
	}
}

//y축 회전 행렬 적용 함수
glm::mat4 ApplyYRotation(const glm::mat4& baseModel)
{
	glm::mat4 rotationMatrix(1.0f);
	rotationMatrix = glm::rotate(glm::mat4(1.0f),
		glm::radians(yRotationAngle),
		glm::vec3(0.0f, 1.0f, 0.0f)); // y축 벡터
	return baseModel * rotationMatrix;
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
	if (yRotationEnabled) {
		UpdateYRotation();
	}
	UpdateCubeAnimations();
	UpdatePyramidAnimations();

	glutPostRedisplay();
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

	// 은면제거 상태에 따라 깊이 테스트 활성화/비활성화
	if (depthTestEnabled) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
	}

	// 뒷면제거 상태에 따라 컬링 활성화/비활성화 (glEnable 인자: GL_CULL_FACE)
	if (backfaceCullEnabled) {
		glEnable(GL_CULL_FACE);
	}
	else {
		glDisable(GL_CULL_FACE);
	}

	//--- 렌더링 파이프라인에 세이더 불러우기
	glUseProgram(shaderProgramID);

	// --- View: 카메라를 뒤쪽 위쪽에서 원점을 바라보도록 설정
	gView = glm::mat4(1.0f);
	gView = glm::lookAt(		//카메라 외부파라미터
		glm::vec3(-1.0f, 2.0f, 3.0f),  // 카메라 위치 (x, y, z축이 모두 보이는 위치)	EYE
		glm::vec3(0.0f, 0.0f, 0.0f),  // 바라보는 지점 (원점) 							AT
		glm::vec3(0.0f, 1.0f, 0.0f)   // 위쪽 방향 벡터 					 			UP
	);

	// 좌표축 그리기 (회전 없이)
	gModel = glm::mat4(1.0f); // 단위 행렬
	glm::mat4 MVP_axes = gProjection * gView * gModel;
	glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(MVP_axes));
	DrawAxes();  // 좌표축 그리기

	// Cube 그리기
	if(drawCube) {
		gModel = glm::mat4(1.0f);
		// translation 먼저 설정하고 rotation을 적용 -> 오브젝트가 자신의 중심에서 회전하고 이동됨
		gModel = ApplyYRotation(gModel);
		// DrawCube에 필요한 매개변수들을 전달
		DrawCube(gModel, gView, gProjection, uMVP_loc);  // 정육면체 그리기
	}
	
	if (drawPyramid){
		gModel = glm::mat4(1.0f);
		// translation 먼저 설정하고 rotation을 적용 -> 오브젝트가 자신의 중심에서 회전하고 이동됨
		gModel = ApplyYRotation(gModel);
		// DrawPyramid에 필요한 매개변수들을 전달
		DrawPyramid(gModel, gView, gProjection, uMVP_loc); // 사각뿔 그리기
	}
	glutSwapBuffers();
}

//--- 다시그리기콜백함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glUseProgram(shaderProgramID);

	float aspect = (h == 0) ? 1 : (float)w / (float)h;
	
	//원근 투영 사용 (3D 효과를 더 잘 보여줌)
	gProjection = glm::perspective(
		glm::radians(45.0f),  // 시야각 45도	fovy
		aspect,               // 종횡비			aspect
		0.1f,                 // 근평면			-n
		100.0f                // 원평면			-f
	);

	glEnable(GL_DEPTH_TEST); // 깊이버퍼 활성화
	glUseProgram(0);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 't':
		animation_cube[0] = !animation_cube[0];
		break;
	case 'f':
		animation_cube[1] = !animation_cube[1];
		break;
	case 's':
		animation_cube[2] = !animation_cube[2];
		break;
	case 'b':
		animation_cube[3] = !animation_cube[3];
		break;
	case 'o':
		// 사각뿔 모든 면들이 함께 열기/닫기
		animation_pyramid[0] = !animation_pyramid[0];
		printf("사각뿔 모든 면 함께 %s\n", animation_pyramid[0] ? "열기" : "닫기");
		break;
	case 'r':
		// 사각뿔 각 면이 하나씩 열기/닫기
		animation_pyramid[1] = !animation_pyramid[1];
		printf("사각뿔 각 면 하나씩 %s\n", animation_pyramid[1] ? "열기" : "닫기");
		break;

	case 'p':
		drawPyramid = !drawPyramid;
		drawCube = !drawCube;
		break;
	case 'h':
		// 은면제거 토글
		depthTestEnabled = !depthTestEnabled;
		printf("은면제거 %s\n", depthTestEnabled ? "활성화" : "비활성화");
		break;
	case 'u':
		//뒷면제거 토글
		backfaceCullEnabled = !backfaceCullEnabled;
		printf("뒷면제거 %s\n", depthTestEnabled ? "활성화" : "비활성화");
		break;
	case 'y':
		// y축 양의 방향 회전 시작/정지
		yRotationEnabled = !yRotationEnabled;
		if (yRotationEnabled) {
			yRotationDirection = 1;
			printf("y축 양의 방향 회전 시작\n");
		}
		else {
			printf("y축 회전 정지\n");
		}
		break;
	case 'Y':
		// y축 음의 방향 회전 시작/정지
		yRotationEnabled = !yRotationEnabled;
		if (yRotationEnabled) {
			yRotationDirection = -1;
			printf("y축 음의 방향 회전 시작\n");
		}
		else {
			printf("y축 회전 정지\n");
		}
		break;
	case 'c':
		drawPyramid = false;
		drawCube = true;

		yRotationAngle = 0.0f;
		yRotationEnabled = false;

		for(bool& anim : animation_cube) 
			anim = false;
		topFaceRotationAngle = 0.0f;
		frontFaceOpenAngle = 0.0f;
		sideFaceRotationAngle = 0.0f;
		backFaceScale = 1.0f;

		for (int i = 0; i < 2; i++) {
			animation_pyramid[i] = false;
		}
		pyramidAllFacesOpenAngle = 0.0f;
		for (int i = 0; i < 4; i++) {
			pyramidFaceOpenAngles[i] = 0.0f;
		}
		currentOpeningFace = 0;

		printf("초기 위치 및 회전으로 리셋\n");
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