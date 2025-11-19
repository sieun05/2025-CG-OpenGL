#include "헤더.h"
#include "WindowToNDC.h"
#include "shader_func.h"

#include "Axes.h"
#include "ground.h"
#include "Cube.h"
#include "minimap.h"
#include "maze.h"
#include "player.h"

void InitBuffer();

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);
GLvoid SpecialKeyDown(int key, int x, int y);
GLvoid SpecialKeyUp(int key, int x, int y);

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

// 카메라 시점 모드
enum CameraMode {
    THIRD_PERSON = 0,    // 3인칭 시점 (기본)
    FIRST_PERSON = 1     // 1인칭 시점
};
CameraMode currentCameraMode = THIRD_PERSON;

// 육면체 애니메이션 토글
bool cubeUpDownAnimation = false;

// 미로 생성 토글


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
	glutSpecialFunc(SpecialKeyDown);    // 화살표 등 특수키 눌림 처리
	glutSpecialUpFunc(SpecialKeyUp);    // 특수키 떼었을 때 처리
	glutTimerFunc(16, Timer, 1); // 약 60FPS로 타이머 시작

	InitBuffer();
	
	// 사용자 안내 메시지 출력
	printf("=== Moving Mountain Maze ===\n");
	printf("키 조작:\n");
	printf("G: 직육면체 지형 생성 (가로/세로 입력)\n");
	printf("r: 미로 생성 (일부 큐브 삭제하여 경로 생성)\n");
	printf("s: 플레이어 생성 (미로의 빈 공간에 배치)\n");
	printf("c: 전체 초기화 (처음 상태로 리셋)\n");
	cout << endl;
	
	printf("화살표 키: 플레이어 이동 (위/아래/좌/우)\n");
	printf("1: 1인칭 시점 (플레이어 눈높이에서 보기)\n");
	printf("3: 3인칭 시점 (위에서 전체 보기)\n");
	printf("+: 플레이어 속도 증가\n");
	printf("-: 플레이어 속도 감소\n");
	cout << endl;

	printf("m: 큐브 위아래 스케일 애니메이션 시작\n");
	printf("M: 큐브 위아래 스케일 애니메이션 정지\n");
	printf("v: 모든 큐브 높이 1.0f로 평준화 / 원래 높이로 복원\n");
	printf("y: 카메라 Y축 양의 방향 회전 시작/정지 (3인칭 모드만)\n");
	printf("Y: 카메라 Y축 음의 방향 회전 시작/정지 (3인칭 모드만)\n");
	printf("O: 직교 투영\n");
	printf("P: 원근 투영\n");
	printf("Z/z: 카메라 거리 조절 (3인칭 모드만)\n");
	printf("Q: 종료\n");
	printf("=============================\n");
	
	glutMainLoop();
}

GLvoid Timer(int value)
{
	// 카메라 회전 업데이트
	UpdateCameraRotation();
	
	// 플레이어 업데이트
	UpdatePlayer();
	
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
	InitPlayer();  // 플레이어 초기화
	
	// 사용자 입력은 별도로 처리 (프로그램 시작 후)
}

//--- 출력 콜백함수
GLvoid drawScene()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // 어두운 배경색
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//GL_DEPTH_BUFFER_BIT 깊이에 따른 은면제거

	//--- 렌더링 파이프라인에 세이더 불러우기
	glUseProgram(shaderProgramID);

	// --- View: 카메라 모드에 따라 시점 설정
	gView = glm::mat4(1.0f);
	
	if (currentCameraMode == FIRST_PERSON && playerActive) {
		// 1인칭 시점: 플레이어 위치에서 앞쪽을 바라봄
		extern Player player;
		
		glm::vec3 cameraPos = player.position;
		cameraPos.y += 0.4f;  // 플레이어 눈높이 (약간 위쪽)
		
		// 플레이어가 바라보는 방향 (기본적으로 Z축 음의 방향)
		glm::vec3 frontDirection(0.0f, 0.0f, -1.0f);
		glm::vec3 lookAt = cameraPos + frontDirection;
		
		gView = glm::lookAt(
			cameraPos,                          // 카메라 위치 (플레이어 위치)
			lookAt,                            // 바라보는 지점 (앞쪽)
			glm::vec3(0.0f, 1.0f, 0.0f)        // 위쪽 방향 벡터
		);
	} else {
		// 3인칭 시점: 기존 카메라 (Y축 회전 적용)
		float cameraRadius = sqrt(18.0f * 18.0f + EYE_z * EYE_z);
		float radians = glm::radians(cameraYRotation);
		
		float cameraX = cameraRadius * sin(radians);
		float cameraY = 20.0f; // Y 높이는 고정
		float cameraZ = cameraRadius * cos(radians);
		
		gView = glm::lookAt(
			glm::vec3(cameraX, cameraY, cameraZ),  // 회전된 카메라 위치
			glm::vec3(0.0f, 0.0f, 0.0f),           // 바라보는 지점 (원점)
			glm::vec3(0.0f, 1.0f, 0.0f)            // 위쪽 방향 벡터
		);
	}

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
	
	//--- 플레이어 그리기 (1인칭 모드에서는 플레이어를 그리지 않음)
	if (currentCameraMode != FIRST_PERSON) {
		DrawPlayer(gView, gProjection, uMVP_loc);
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
		mazeGenerated = false; // 미로 생성 상태 초기화
		cubeUpDownAnimation = false;
		heightNormalized = false;
		cameraRotating = false;
		StopUpDownCubeAnimation();
		ResetPlayer();  // 플레이어 제거
		GetUserInput();
		break;
		
	case 'c':  // 전체 초기화 (Clear/Reset)
	case 'C':
		printf("프로그램을 초기 상태로 리셋합니다...\n");
		
		// 미로 상태 초기화
		mazeGenerated = false;
		if (gridWidth > 0 && gridHeight > 0) {
			ResetMaze();  // 미로 초기화
		}
		
		// 큐브 애니메이션 정리
		cubeUpDownAnimation = false;
		heightNormalized = false;
		StopUpDownCubeAnimation();
		
		// 카메라 초기화
		cameraRotating = false;
		cameraYRotation = 0.0f;
		rotationDirection = 1;
		currentCameraMode = THIRD_PERSON;
		EYE_z = 18.0f;
		
		// 투영 초기화 (원근 투영)
		gProjection = glm::perspective(
			glm::radians(45.0f),  // 시야각 45도
			aspect,               // 종횡비
			0.1f,                 // 근평면
			100.0f                // 원평면
		);
		
		// 플레이어 초기화
		ResetPlayer();
		
		// 격자 제거 (빈 상태로)
		gridWidth = 0;
		gridHeight = 0;
		cubeGrid.clear();
		break;
		
	case 's':  // 플레이어 생성 (Start player)
	case 'S':
		CreatePlayer();
		break;
		
	case '1':  // 1인칭 시점으로 변환
		currentCameraMode = FIRST_PERSON;
		cameraRotating = false;  // 1인칭 모드에서는 Y축 회전 정지
		printf("카메라 시점: 1인칭 모드\n");
		if (!playerActive) {
			printf("1인칭 시점을 사용하려면 먼저 플레이어를 생성하세요 (s 키)\n");
		}
		break;
		
	case '3':  // 3인칭 시점으로 변환
		currentCameraMode = THIRD_PERSON;
		printf("카메라 시점: 3인칭 모드\n");
		break;
		
	case '+':  // 플레이어 속도 증가
		IncreasePlayerSpeed();
		break;
		
	case '-':  // 플레이어 속도 감소
		DecreasePlayerSpeed();
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
	case 'y':  // Y축 양의 방향 회전 시작/정지 (3인칭 모드에서만)
		if (currentCameraMode == THIRD_PERSON) {
			cameraRotating = !cameraRotating;
			if (cameraRotating) {
				rotationDirection = 1;
				printf("카메라 Y축 양의 방향 회전 시작\n");
			}
			else {
				printf("카메라 Y축 회전 정지\n");
			}
		} else {
			printf("Y축 회전은 3인칭 모드에서만 사용 가능합니다\n");
		}
		break;
		
	case 'Y':  // Y축 음의 방향 회전 시작/정지 (3인칭 모드에서만)
		if (currentCameraMode == THIRD_PERSON) {
			cameraRotating = !cameraRotating;
			if (cameraRotating) {
				rotationDirection = -1;
				printf("카메라 Y축 음의 방향 회전 시작\n");
			}
			else {
				printf("카메라 Y축 회전 정지\n");
			}
		} else {
			printf("Y축 회전은 3인칭 모드에서만 사용 가능합니다\n");
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
		//카메라를 원점에서 더 멀리 (3인칭 모드에서만)
		if (currentCameraMode == THIRD_PERSON) {
			EYE_z++;
			if (EYE_z > 40.0f) EYE_z = 40.0f;
		} else {
			printf("카메라 거리 조절은 3인칭 모드에서만 사용 가능합니다\n");
		}
		break;
		
	case 'Z':
		//카메라를 원점에서 더 가깝게 (3인칭 모드에서만)
		if (currentCameraMode == THIRD_PERSON) {
			EYE_z--;
			if (EYE_z < 0.0f) EYE_z = 0.0f;
		} else {
			printf("카메라 거리 조절은 3인칭 모드에서만 사용 가능합니다\n");
		}
		break;
	case 'r':
		if (not mazeGenerated) {
			cubeUpDownAnimation = false;
			ResetPlayer();  // 플레이어 제거
			GenerateMaze(); // 미로 생성
		}
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}

	glutPostRedisplay();
}

// 특수키 눌림 (화살표 등)
GLvoid SpecialKeyDown(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		moveLeft = true;
		break;
	case GLUT_KEY_RIGHT:
		moveRight = true;
		break;
	case GLUT_KEY_UP:
		moveUp = true;
		break;
	case GLUT_KEY_DOWN:
		moveDown = true;
		break;
	}
}

// 특수키 떼었을 때
GLvoid SpecialKeyUp(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		moveLeft = false;
		break;
	case GLUT_KEY_RIGHT:
		moveRight = false;
		break;
	case GLUT_KEY_UP:
		moveUp = false;
		break;
	case GLUT_KEY_DOWN:
		moveDown = false;
		break;
	}
}