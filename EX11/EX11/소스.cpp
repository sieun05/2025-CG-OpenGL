#include "헤더.h"

GLvoid InitBuffer();
void updateBuffer();
GLvoid Timer(int value);
void createSpiral(float centerX, float centerY);

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);

// 전역 변수들
GLuint VAO, VBO[2]; // 하나의 VAO, 위치와 색상용 VBO
vector<float> spiralVertices;
vector<float> spiralColors;
float angle = 0.0f; // 회전 각도
bool isAnimating = false;
bool isClockwise = true; // 시계방향/반시계방향
bool drawAsPoints = true; // true: 점으로 그리기, false: 선으로 그리기
float backgroundColor[3] = {0.0f, 0.0f, 0.0f}; // 초기 배경색 (검정)
const int SPIRAL_POINTS = 200; // 각 스파이럴의 점 개수
const int CONNECTION_POINTS = 20; // 연결부 점 개수
const float SPIRAL_RADIUS = 0.15f; // 스파이럴 반지름
float spiralCenterX = 0.0f, spiralCenterY = 0.0f; // 스파이럴 중심점
bool spiralExists = false; // 스파이럴이 생성되었는지 여부

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
	glutCreateWindow("Example11 - Spiral Animation");

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
	glutMouseFunc(Mouse);
	glutTimerFunc(16, Timer, 1); // 약 60FPS로 타이머 시작

	InitBuffer();
	glutMainLoop();
}

GLvoid InitBuffer()
{
	// VAO, VBO 생성
	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);
	
	glBindVertexArray(VAO);
	
	// 위치 VBO (스파이럴 2개 + 연결부)
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, (SPIRAL_POINTS * 2 + CONNECTION_POINTS) * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	
	// 색상 VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, (SPIRAL_POINTS * 2 + CONNECTION_POINTS) * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
}

void createSpiral(float centerX, float centerY)
{
	spiralCenterX = centerX;
	spiralCenterY = centerY;
	spiralExists = true;
	isAnimating = true;
	
	spiralVertices.clear();
	spiralColors.clear();
	
	// 왼쪽 스파이럴: 중심에서 시작해서 바깥쪽으로 3바퀴
	for (int i = 0; i < SPIRAL_POINTS; i++) {
		float t = (float)i / SPIRAL_POINTS * 3.0f * 3.14159f; // 3바퀴 회전
		float radius = SPIRAL_RADIUS * t / (3.0f * 3.14159f); // 반지름이 점진적으로 증가
		
		float x = radius * cos(t) + centerX - SPIRAL_RADIUS; // 왼쪽에 위치
		float y = radius * sin(t) + centerY;
		
		spiralVertices.push_back(x);
		spiralVertices.push_back(y);
		spiralVertices.push_back(0.0f);
		
		// 색상 설정 (흰색)
		spiralColors.push_back(1.0f);
		spiralColors.push_back(1.0f);
		spiralColors.push_back(1.0f);
	}
	
	// 연결부: 왼쪽 스파이럴에서 오른쪽 스파이럴로 이어지는 선
	for (int i = 0; i < CONNECTION_POINTS; i++) {
		float progress = (float)i / (CONNECTION_POINTS - 1);
		float x = centerX - SPIRAL_RADIUS + (2.0f * SPIRAL_RADIUS * progress); // 왼쪽에서 오른쪽으로
		float y = centerY;
		
		spiralVertices.push_back(x);
		spiralVertices.push_back(y);
		spiralVertices.push_back(0.0f);
		
		// 색상 설정 (흰색)
		spiralColors.push_back(1.0f);
		spiralColors.push_back(1.0f);
		spiralColors.push_back(1.0f);
	}
	
	// 오른쪽 스파이럴: 바깥쪽에서 시작해서 중심으로 3바퀴
	for (int i = 0; i < SPIRAL_POINTS; i++) {
		float t = (float)i / SPIRAL_POINTS * 3.0f * 3.14159f; // 3바퀴 회전
		float radius = SPIRAL_RADIUS - (SPIRAL_RADIUS * t / (3.0f * 3.14159f)); // 반지름이 점진적으로 감소
		
		float x = radius * cos(-t) + centerX + SPIRAL_RADIUS; // 오른쪽에 위치, 반대방향 회전
		float y = radius * sin(-t) + centerY;
		
		spiralVertices.push_back(x);
		spiralVertices.push_back(y);
		spiralVertices.push_back(0.0f);
		
		// 색상 설정 (흰색)
		spiralColors.push_back(1.0f);
		spiralColors.push_back(1.0f);
		spiralColors.push_back(1.0f);
	}
	
	updateBuffer();
}

void updateBuffer()
{
	if (!spiralExists) return;
	
	// 회전된 스파이럴 좌표 계산
	vector<float> rotatedVertices;
	
	// 왼쪽 스파이럴: 중심에서 바깥쪽으로 3바퀴
	for (int i = 0; i < SPIRAL_POINTS; i++) {
		float t = (float)i / SPIRAL_POINTS * 3.0f * 3.14159f; // 3바퀴 회전
		float radius = SPIRAL_RADIUS * t / (3.0f * 3.14159f);
		
		float rotatedT = isClockwise ? t + angle : t - angle;
		float x = radius * cos(rotatedT) + spiralCenterX - SPIRAL_RADIUS;
		float y = radius * sin(rotatedT) + spiralCenterY;
		
		rotatedVertices.push_back(x);
		rotatedVertices.push_back(y);
		rotatedVertices.push_back(0.0f);
	}
	
	// 연결부: 왼쪽에서 오른쪽으로 이어지는 선 (회전하지 않음)
	for (int i = 0; i < CONNECTION_POINTS; i++) {
		float progress = (float)i / (CONNECTION_POINTS - 1);
		float x = spiralCenterX - SPIRAL_RADIUS + (2.0f * SPIRAL_RADIUS * progress);
		float y = spiralCenterY;
		
		rotatedVertices.push_back(x);
		rotatedVertices.push_back(y);
		rotatedVertices.push_back(0.0f);
	}
	
	// 오른쪽 스파이럴: 바깥쪽에서 중심으로 3바퀴
	for (int i = 0; i < SPIRAL_POINTS; i++) {
		float t = (float)i / SPIRAL_POINTS * 3.0f * 3.14159f; // 3바퀴 회전
		float radius = SPIRAL_RADIUS - (SPIRAL_RADIUS * t / (3.0f * 3.14159f));
		
		float rotatedT = isClockwise ? -t + angle : -t - angle; // 반대방향 회전
		float x = radius * cos(rotatedT) + spiralCenterX + SPIRAL_RADIUS;
		float y = radius * sin(rotatedT) + spiralCenterY;
		
		rotatedVertices.push_back(x);
		rotatedVertices.push_back(y);
		rotatedVertices.push_back(0.0f);
	}
	
	// VBO 업데이트
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, rotatedVertices.size() * sizeof(float), rotatedVertices.data());
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, spiralColors.size() * sizeof(float), spiralColors.data());
}

GLvoid Timer(int value)
{
	if (isAnimating && spiralExists) {
		angle += 0.05f;
		updateBuffer();
	}
	
	glutPostRedisplay();
	glutTimerFunc(16, Timer, 1);
}

//--- 출력 콜백함수
GLvoid drawScene()
{
	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (spiralExists) {
		//--- 렌더링 파이프라인에 세이더 불러오기
		glUseProgram(shaderProgramID);
		glBindVertexArray(VAO);
		
		if (drawAsPoints) {
			glPointSize(3.0f);
			glDrawArrays(GL_POINTS, 0, SPIRAL_POINTS * 2 + CONNECTION_POINTS);
		} else {
			glLineWidth(2.0f);
			// 연결된 하나의 8자 모양 스파이럴로 선으로 그리기
			glDrawArrays(GL_LINE_STRIP, 0, SPIRAL_POINTS * 2 + CONNECTION_POINTS);
		}
	}

	glutSwapBuffers();
}

//--- 다시그리기콜백함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// 마우스 좌표를 NDC로 변환
		Vec2 ndc = WindowToNDC(x, y);
		
		// 해당 위치에 스파이럴 생성
		createSpiral(ndc.x, ndc.y);
		glutPostRedisplay();
	}
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'p': case 'P':
		// 점으로 그리기 토글
		drawAsPoints = true;
		break;
	case 'l': case 'L':
		// 선으로 그리기 토글
		drawAsPoints = false;
		break;
	case '1': case '2': case '3': case '4': case '5':
		// 입력된 숫자만큼 초기 상태의 스파이럴 그리기
		{
			if (spiralExists) {
				int count = key - '0';
				angle = count * 0.5f;
				updateBuffer();
				isAnimating = false;
			}
		}
		break;
	case 'c': case 'C':
		// 방향 전환 (시계방향/반시계방향)
		isClockwise = !isClockwise;
		break;
	case 'b': case 'B':
		// 배경색 변경 (검정 <-> 빨강)
		if (backgroundColor[0] == 0.0f && backgroundColor[1] == 0.0f && backgroundColor[2] == 0.0f) {
			// 검정에서 빨강으로
			backgroundColor[0] = 1.0f;
			backgroundColor[1] = 0.0f;
			backgroundColor[2] = 0.0f;
		} else {
			// 빨강에서 검정으로
			backgroundColor[0] = 0.0f;
			backgroundColor[1] = 0.0f;
			backgroundColor[2] = 0.0f;
		}
		break;
	case 'r': case 'R':
		// 리셋: 스파이럴 제거하고 애니메이션 정지
		spiralExists = false;
		isAnimating = false;
		angle = 0.0f;
		spiralVertices.clear();
		spiralColors.clear();
		break;
	case 'q': case 'Q':
		// 프로그램 종료
		exit(0);
		break;
	}
	glutPostRedisplay();
}