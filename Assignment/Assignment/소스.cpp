#include "헤더.h"
#include "WindowToNDC.h"
#include "shader_func.h"

#include "Axes.h"
#include "ground.h"
#include "Cube.h"
#include "minimap.h"

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

// 카메라 Y축 회전 관련 변수
float cameraYRotation = 0.0f;  // Y축 회전 각도
bool cameraRotating = false;   // 회전 중인지 여부
int rotationDirection = 1;     // 1: 양의 방향, -1: 음의 방향

// 육면체 애니메이션 토글
bool cubeUpDownAnimation = false;



void AfterMakeShaders() 
{
	glUseProgram(shaderProgramID);
	uMVP_loc = glGetUniformLocation(shaderProgramID, "uMVP");
	if (uMVP_loc < 0) { printf("uMVP get error\n"); exit(1); }
	glUseProgram(0);
}

// 카메라 Y축 회전 업데이트 함수
void UpdateCameraRotation()
{
	if (cameraRotating) {
		const float rotationSpeed = 1.0f; // 회전 속도 (도/프레임)
		cameraYRotation += rotationSpeed * rotationDirection;

		// 360도를 넘으면 0으로 리셋
		if (cameraYRotation >= 360.0f) {
			cameraYRotation -= 360.0f;
		}
		else if (cameraYRotation < 0.0f) {
			cameraYRotation += 360.0f;
		}
	}
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
	glutCreateWindow("Moving Mountain Maze");

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
	
	// 사용자 안내 메시지 출력
	printf("=== Moving Mountain Maze ===\n");
	printf("키 조작:\n");
	printf("G: 격자 생성 (가로/세로 입력)\n");
	printf("m: 큐브 위아래 스케일 애니메이션 시작\n");
	printf("M: 큐브 위아래 스케일 애니메이션 정지\n");
	printf("v: 모든 큐브 높이 1.0f로 평준화 / 원래 높이로 복원\n");
	printf("y: 카메라 Y축 양의 방향 회전 시작/정지\n");
	printf("Y: 카메라 Y축 음의 방향 회전 시작/정지\n");
	printf("O: 직교 투영\n");
	printf("P: 원근 투영\n");
	printf("Z/z: 카메라 거리 조절\n");
	printf("Q: 종료\n");
	printf("=============================\n");
	printf("먼저 바닥과 좌표축이 표시됩니다.\n");
	printf("'G' 키를 눌러 큐브 격자를 생성하세요.\n");
	printf("큐브들이 바닥 아래에서 솟아나는 애니메이션을 볼 수 있습니다!\n");
	printf("'m' 키로 큐브들이 위아래로 움직이는 애니메이션을 시작할 수 있습니다!\n");
	printf("'y/Y' 키로 카메라가 Y축을 중심으로 회전합니다!\n");
	printf("'v' 키로 모든 큐브의 높이를 평준화하거나 복원할 수 있습니다!\n");
	
	glutMainLoop();
}

GLvoid Timer(int value)
{
	// 카메라 회전 업데이트
	UpdateCameraRotation();
	
	glutPostRedisplay();
	glutTimerFunc(16, Timer, 1); // 약 60FPS로 타이머 시작
}

void InitBuffer()
{
	glEnable(GL_DEPTH_TEST); // 깊이버퍼 활성화
	glEnable(GL_CULL_FACE); // Face culling 활성화

	InitAxesBuffer();
	InitGroundBuffer();
	InitCubeBuffer();
	
	// 사용자 입력은 별도로 처리 (프로그램 시작 후)
}

//--- 출력 콜백함수
GLvoid drawScene()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // 어두운 배경색
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//GL_DEPTH_BUFFER_BIT 깊이에 따른 은면제거

	//--- 렌더링 파이프라인에 세이더 불러우기
	glUseProgram(shaderProgramID);

	// --- View: 카메라를 Y축 회전 적용하여 설정
	gView = glm::mat4(1.0f);
	
	// 카메라 위치 계산 (Y축 회전 적용)
	float cameraRadius = sqrt(18.0f * 18.0f + EYE_z * EYE_z); // 카메라와 원점 사이의 거리
	float radians = glm::radians(cameraYRotation);
	
	float cameraX = cameraRadius * sin(radians);
	float cameraY = 20.0f; // Y 높이는 고정
	float cameraZ = cameraRadius * cos(radians);
	
	gView = glm::lookAt(		//카메라 외부파라미터
		glm::vec3(cameraX, cameraY, cameraZ),  // 회전된 카메라 위치
		glm::vec3(0.0f, 0.0f, 0.0f),           // 바라보는 지점 (원점)
		glm::vec3(0.0f, 1.0f, 0.0f)            // 위쪽 방향 벡터
	);

	// 큐브 애니메이션 업데이트
	float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	UpdateCubeAnimations(currentTime);

	// === 메인 뷰 렌더링 ===
	DrawAxes(gView, gProjection, uMVP_loc);  // 좌표축 그리기

	//--- 모델 그리기
	DrawGround(gView, gProjection, uMVP_loc);
	
	//--- 모든 큐브 그리기 (격자가 생성된 경우에만)
	if (gridWidth > 0 && gridHeight > 0) {
		DrawAllCubes(gView, gProjection, uMVP_loc);
	}

	// === 미니맵 렌더링 ===
	// 현재 창 크기 가져오기
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int windowWidth = viewport[2];
	int windowHeight = viewport[3];
	
	DrawMinimap(windowWidth, windowHeight, uMVP_loc);

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
	case 'g':  // 격자 생성 (Grid 생성)
	case 'G':
		printf("격자를 생성합니다...\n");
		GetUserInput();
		break;
		
	case 'm':  // 위아래 스케일 애니메이션 시작 (소문자 m)
		if (not cubeUpDownAnimation) {
			cubeUpDownAnimation = true;
			UpDownCubeAnimation();
			heightNormalized = false; // 높이 평준화 해제
		}
		
		break;
	case 'M':  // 위아래 스케일 애니메이션 정지 (대문자 M)
		cubeUpDownAnimation = false;
		StopUpDownCubeAnimation();
		break;

	case 'v':
		heightNormalized = !heightNormalized;
		cubeUpDownAnimation = false;
		StopUpDownCubeAnimation();
		ToggleHeightNormalization();
		break;
	case 'y':  // Y축 양의 방향 회전 시작/정지
		cameraRotating = !cameraRotating;
		if (cameraRotating) {
			rotationDirection = 1;
			printf("카메라 Y축 양의 방향 회전 시작\n");
		}
		else {
			printf("카메라 Y축 회전 정지\n");
		}
		break;
		
	case 'Y':  // Y축 음의 방향 회전 시작/정지
		cameraRotating = !cameraRotating;
		if (cameraRotating) {
			rotationDirection = -1;
			printf("카메라 Y축 음의 방향 회전 시작\n");
		}
		else {
			printf("카메라 Y축 회전 정지\n");
		}
		break;
		
	case 'o':	//직각투영
		gProjection = glm::ortho(
			-10.0f * aspect, 10.0f * aspect,   // left, right
			-10.0f, 10.0f,                     // bottom, top
			0.1f, 100.0f                       // near, far
		);
		break;
		
	case 'p':	//원근투영
		gProjection = glm::perspective(
			glm::radians(45.0f),  // 시야각 45도	fovy
			aspect,               // 종횡비			aspect
			0.1f,                 // 근평면			-n
			100.0f                // 원평면			-f
		);
		break;
		
	case 'z':
		//카메라를 원점에서 더 멀리
		EYE_z++;
		if (EYE_z > 40.0f) EYE_z = 40.0f;
		break;
		
	case 'Z':
		//카메라를 원점에서 더 가깝게
		EYE_z--;
		if (EYE_z < 0.0f) EYE_z = 0.0f;
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
//}