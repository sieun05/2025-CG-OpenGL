#include "헤더.h"

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid InitBuffer();

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);

//--- 필요한변수선언
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader;	//--- 버텍스세이더객체
GLuint fragmentShader;	//--- 프래그먼트 세이더객체

// 전역 변수
GLuint VAO[4], VBO[8]; // 4개 사분면용 VAO, 각각 위치와 색상 VBO
std::vector<Triangle> triangles[4]; // 각 사분면별 삼각형 저장
bool showAll = false; // 모든 삼각형 표시 여부

char* filetobuf(const char* file)	//쉐이더 파일을 읽어 문자열로 변환하는 유틸리티 함수
{
	FILE* fptr;
	long length;
	char* buf;

	fptr = fopen(file, "rb");

	if (!fptr)
		return NULL;

	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;

	return buf;
}

// 버퍼 업데이트 함수
void updateBuffer(int quadrant) {
	if (triangles[quadrant].empty()) return;

	std::vector<float> vertices;
	std::vector<float> colors;

	for (const auto& tri : triangles[quadrant]) {
		if (tri.visible) {
			for (int i = 0; i < 3; i++) {
				vertices.push_back(tri.vertices[i].x);
				vertices.push_back(tri.vertices[i].y);
				vertices.push_back(0.0f);

				colors.push_back(tri.color.r);
				colors.push_back(tri.color.g);
				colors.push_back(tri.color.b);
			}
		}
	}

	glBindVertexArray(VAO[quadrant]);

	// 위치 데이터
	glBindBuffer(GL_ARRAY_BUFFER, VBO[quadrant * 2]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	// 색상 데이터
	glBindBuffer(GL_ARRAY_BUFFER, VBO[quadrant * 2 + 1]);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
}

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
	glutCreateWindow("Example1");

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

	InitBuffer();
	glutMainLoop();
}

//--- 버텍스세이더객체만들기
void make_vertexShaders()
{
	GLchar* vertexSource;
	//--- 버텍스세이더읽어저장하고컴파일하기
	//--- filetobuf: 사용자정의 함수로 텍스트를읽어서문자열에저장하는함수

	vertexSource = filetobuf("vertex.glsl");

	//버텍스 쉐이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//쉐이더 코드를 쉐이더 객체에 넣기
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	//버텍스 쉐이더 컴파일하기
	glCompileShader(vertexShader);

	//컴파일이 제대로 되었는지 체크하기
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
//--- 프래그먼트세이더객체만들기
void make_fragmentShaders()
{
	GLchar* fragmentSource;
	//--- 프래그먼트세이더읽어저장하고컴파일하기
	fragmentSource = filetobuf("fragment.glsl");    // 프래그세이더 읽어오기

	//--- 프래그먼트세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- 쉐이더코드를쉐이더객체에넣기
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	//--- 프래그먼트세이더컴파일하기
	glCompileShader(fragmentShader);

	//--- 컴파일이제대로되었는지체크하기
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

GLuint make_shaderProgram()
{
	GLint result;
	GLchar errorLog[512]; // 이 부분을 배열로 변경
	GLuint shaderID;
	shaderID = glCreateProgram();

	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);
	glLinkProgram(shaderID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);

	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return -1;
	}

	glUseProgram(shaderID);
	return shaderID;
}


GLvoid InitBuffer()
{
	// VAO와 VBO 생성
	glGenVertexArrays(4, VAO);
	glGenBuffers(8, VBO);

	// 각 사분면별로 초기화
	for (int i = 0; i < 4; i++) {
		glBindVertexArray(VAO[i]);

		// 위치 버퍼
		glBindBuffer(GL_ARRAY_BUFFER, VBO[i * 2]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		// 색상 버퍼
		glBindBuffer(GL_ARRAY_BUFFER, VBO[i * 2 + 1]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(1);
	}
}

//--- 출력 콜백함수
GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//--- 렌더링 파이프라인에 세이더 불러오기
	glUseProgram(shaderProgramID);

	// 사분면 구분선 그리기 (선택사항)
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	glColor3f(0.7f, 0.7f, 0.7f);
	// 세로선
	glVertex2f(0.0f, -1.0f);
	glVertex2f(0.0f, 1.0f);
	// 가로선
	glVertex2f(-1.0f, 0.0f);
	glVertex2f(1.0f, 0.0f);
	glEnd();

	// 각 사분면별 삼각형 그리기
	for (int quadrant = 0; quadrant < 4; quadrant++) {
		if (!triangles[quadrant].empty()) {
			glBindVertexArray(VAO[quadrant]);

			int visibleCount = 0;
			for (const auto& tri : triangles[quadrant]) {
				if (tri.visible) visibleCount++;
			}

			if (visibleCount > 0) {
				glDrawArrays(GL_TRIANGLES, 0, visibleCount * 3);
			}
		}
	}

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
	case 'a':
	case 'A':
		// 도형을 면으로/선으로 그리기 토글
	{
		static bool wireframe = false;
		wireframe = !wireframe;
		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
	break;
	case 'c':
	case 'C':
		// 모든 도형이 삭제되고 새롭게 그려진다
		for (int i = 0; i < 4; i++) {
			triangles[i].clear();
		}
		break;
	case 'q':
	case 'Q':
		// 프로그램 종료
		exit(0);
		break;
	}
	glutPostRedisplay();
}

GLvoid Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// 윈도우 좌표를 NDC로 변환
		Vec2 ndc = WindowToNDC(x, y);

		// 사분면 판별
		int quadrant = getQuadrant(ndc.x, ndc.y);

		// 좌클릭: 해당 사분면의 기존 삼각형들을 모두 지우고 새로운 삼각형 생성
		triangles[quadrant].clear(); // 기존 삼각형 모두 삭제
		Triangle newTriangle = createTriangle(ndc.x, ndc.y);
		triangles[quadrant].push_back(newTriangle);
		updateBuffer(quadrant);

		glutPostRedisplay();
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		// 윈도우 좌표를 NDC로 변환
		Vec2 ndc = WindowToNDC(x, y);

		// 사분면 판별
		int quadrant = getQuadrant(ndc.x, ndc.y);

		// 새로운 삼각형 추가 (최대 4개)
		if (triangles[quadrant].size() < 4) {
			Triangle newTriangle = createTriangle(ndc.x, ndc.y);
			triangles[quadrant].push_back(newTriangle);
			updateBuffer(quadrant);
		}

		glutPostRedisplay();
	}
}
