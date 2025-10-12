//--- 필요한헤더파일선언
#define _CRT_SECURE_NO_WARNINGS 
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <random>

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h> 

#include "WindowToNDC.h"

using namespace std;

//--- 아래5개함수는사용자정의함수임
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid InitBuffer();
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);

struct RGB { GLfloat r, g, b; };

//--- 도형 타입 열거형
enum ShapeType {
	SHAPE_POINT = 0,
	LINE = 1,
	TRIANGLE = 2,
	RECTANGLE = 3
};

//--- 도형 구조체
struct Shape {
	ShapeType type;
	Vec2 position; // 중심점
	RGB color;    // RGB 색상
	vector<Vec2> vertices; // 정점 좌표들
	GLuint vao, vbo[2];
	bool active; // 활성화 여부
};

//--- 전역변수선언
vector<Shape> shapes; // 도형들을 저장하는 벡터
const int MAX_SHAPES = 10; // 최대 도형 개수
ShapeType currentShapeType = SHAPE_POINT; // 현재 선택된 도형 타입
int selectedShapeIndex = -1; // 선택된 도형 인덱스 (-1: 없음)

//--- 필요한변수선언
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader;	//--- 버텍스세이더객체
GLuint fragmentShader;	//--- 프래그먼트 세이더객체

RGB getRandomColor() {
	RGB color;
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
	color.r = dis(gen);
	color.g = dis(gen);
	color.b = dis(gen);

	return color;
}

//--- 도형 생성 함수들
void createSHAPE_POINT(Vec2 pos) {		//점
	if (shapes.size() >= MAX_SHAPES) return;

	Shape shape;
	shape.type = SHAPE_POINT;
	shape.position = pos;
	shape.color = getRandomColor();
	shape.active = true;

	// 점을 작은 삼각형으로 표현
	float size = 0.02f;
	shape.vertices.push_back({ pos.x, pos.y + size });
	shape.vertices.push_back({ pos.x - size, pos.y - size });
	shape.vertices.push_back({ pos.x + size, pos.y - size });

	shapes.push_back(shape);
}


void createLine(Vec2 pos) {		//선
	if (shapes.size() >= MAX_SHAPES) return;

	Shape shape;
	shape.type = LINE;
	shape.position = pos;
	shape.color = getRandomColor();
	shape.active = true;

	// 선을 얇은 사각형으로 표현
	float length = 0.2f;
	float width = 0.01f;
	shape.vertices.push_back({ pos.x - length / 2, pos.y - width / 2 });
	shape.vertices.push_back({ pos.x + length / 2, pos.y - width / 2 });
	shape.vertices.push_back({ pos.x + length / 2, pos.y + width / 2 });
	shape.vertices.push_back({ pos.x - length / 2, pos.y - width / 2 });
	shape.vertices.push_back({ pos.x + length / 2, pos.y + width / 2 });
	shape.vertices.push_back({ pos.x - length / 2, pos.y + width / 2 });

	shapes.push_back(shape);
}

void createTriangle(Vec2 pos) {		//삼각형
	if (shapes.size() >= MAX_SHAPES) return;

	Shape shape;
	shape.type = TRIANGLE;
	shape.position = pos;
	shape.color = getRandomColor();
	shape.active = true;

	float size = 0.08f;
	shape.vertices.push_back({ pos.x, pos.y + size });
	shape.vertices.push_back({ pos.x - size, pos.y - size });
	shape.vertices.push_back({ pos.x + size, pos.y - size });

	shapes.push_back(shape);
}


void createRectangle(Vec2 pos) {
	if (shapes.size() >= MAX_SHAPES) return;

	Shape shape;
	shape.type = RECTANGLE;
	shape.position = pos;
	shape.color = getRandomColor();
	shape.active = true;

	float width = 0.12f;
	float height = 0.16f;

	// 두 개의 삼각형으로 사각형 구성
	shape.vertices.push_back({ pos.x - width / 2, pos.y - height / 2 });
	shape.vertices.push_back({ pos.x + width / 2, pos.y - height / 2 });
	shape.vertices.push_back({ pos.x + width / 2, pos.y + height / 2 });
	shape.vertices.push_back({ pos.x - width / 2, pos.y - height / 2 });
	shape.vertices.push_back({ pos.x + width / 2, pos.y + height / 2 });
	shape.vertices.push_back({ pos.x - width / 2, pos.y + height / 2 });

	shapes.push_back(shape);
}

//--- 도형 선택 함수 수정필요해보임 크기별로 고려해서 체크해야하지 않나?
int findShapeAtPosition(Vec2 pos) {
	for (int i = shapes.size() - 1; i >= 0; i--) {
		if (!shapes[i].active) continue;

		Vec2 diff = { pos.x - shapes[i].position.x, pos.y - shapes[i].position.y };
		float distance = sqrt(diff.x * diff.x + diff.y * diff.y);

		if (distance < 0.1f) { // 클릭 허용 범위
			return i;
		}
	}
	return -1;
}

//--- 도형 이동 함수
void moveShape(int shapeIndex, float dx, float dy) {
	if (shapeIndex < 0 || shapeIndex >= shapes.size() || !shapes[shapeIndex].active) return;

	shapes[shapeIndex].position.x += dx;
	shapes[shapeIndex].position.y += dy;

	// 모든 정점 이동
	for (auto& vertex : shapes[shapeIndex].vertices) {
		vertex.x += dx;
		vertex.y += dy;
	}
}


//--- 버퍼 업데이트 함수
void updateShapeBuffer(int shapeIndex) {
	if (shapeIndex < 0 || shapeIndex >= shapes.size()) return;

	Shape& shape = shapes[shapeIndex];

	// VAO, VBO 생성
	glGenVertexArrays(1, &shape.vao);
	glGenBuffers(2, shape.vbo);

	glBindVertexArray(shape.vao);

	// 정점 데이터
	std::vector<float> vertices;
	std::vector<float> colors;

	for (const auto& vertex : shape.vertices) {
		vertices.push_back(vertex.x);
		vertices.push_back(vertex.y);
		vertices.push_back(0.0f);

		colors.push_back(shape.color.r);
		colors.push_back(shape.color.g);
		colors.push_back(shape.color.b);
	}

	// 위치 VBO
	glBindBuffer(GL_ARRAY_BUFFER, shape.vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 색상 VBO
	glBindBuffer(GL_ARRAY_BUFFER, shape.vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

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
	//--- 버텍스세이더만들기
	make_fragmentShaders();
	//--- 프래그먼트세이더만들기
	shaderProgramID = make_shaderProgram();

	//--- 세이더프로그램만들기
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);

	//InitBuffer();
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
	if(!result)
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
	//make_vertexShaders(); //--- 버텍스 세이더 만들기
	//make_fragmentShaders(); //--- 프래그먼트 세이더 만들기

	GLint result;
	GLchar* errorLog = NULL;
	GLuint shaderID;
	shaderID = glCreateProgram();

	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);
	glLinkProgram(shaderID);
	//--- 세이더프로그램만들기
	//--- 세이더프로그램에버텍스세이더붙이기
	//--- 세이더프로그램에프래그먼트세이더붙이기
	//--- 세이더프로그램링크하기
	  
	//--- 세이더객체를세이더프로그램에링크했음으로,세이더객체자체는삭제가능
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);

	// ---세이더가 잘연결되었는지체크하기
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return -1;
	}

	glUseProgram(shaderID);
	return shaderID;
	//--- 만들어진세이더프로그램사용하기
   //--- 여러 개의세이더프로그램만들수있고, 그중한개의프로그램을사용하려면
   //--- glUseProgram 함수를 호출하여사용할특정프로그램을지정한다.
	//--- 사용하기직전에호출할수있다.
}

//--- 출력 콜백함수
GLvoid drawScene()
{
	//--- 콜백 함수: 그리기콜백함수
	GLfloat rColor, gColor, bColor;
	rColor = gColor = bColor = 1.0;

	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//--- 렌더링 파이프라인에 세이더 불러오기
	glUseProgram(shaderProgramID);

	// 모든 도형 그리기
	for (const auto& shape : shapes) {
		if (!shape.active) continue;

		glBindVertexArray(shape.vao);
		glDrawArrays(GL_TRIANGLES, 0, shape.vertices.size());
	}

	glutSwapBuffers();
}

//--- 다시그리기콜백함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	//--- 배경색을파랑색으로설정
   //--- 렌더링하기: 0번인덱스에서1개의버텍스를사용하여점그리기
   // 화면에출력하기
   //--- 콜백 함수: 다시 그리기 콜백 함수
}

//GLvoid InitBuffer()
//{
//	glGenVertexArrays(1, &vao); //--- VAO 를 지정하고 할당하기
//	glBindVertexArray(vao); //--- VAO를 바인드하기
//	glGenBuffers(2, vbo); //--- 2개의 VBO를 지정하고 할당하기
//
//	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
//	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
//	//--- 변수 diamond 에서 버텍스 데이터 값을 버퍼에 복사한다.
//	//--- triShape 배열의 사이즈: 9 * float
//	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), triShape, GL_STATIC_DRAW);
//	//--- 좌표값을 attribute 인덱스 0번에 명시한다: 버텍스 당 3* float
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//	//--- attribute 인덱스 0번을 사용가능하게 함
//	glEnableVertexAttribArray(0);
//	//--- 2번째 VBO를 활성화 하여 바인드 하고, 버텍스 속성 (색상)을 저장
//	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
//	//--- 변수 colors에서 버텍스 색상을 복사한다.
//	//--- colors 배열의 사이즈: 9 *float
//	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
//	//--- 색상값을 attribute 인덱스 1번에 명시한다: 버텍스 당 3*float
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
//	//--- attribute 인덱스 1번을 사용 가능하게 함.
//	glEnableVertexAttribArray(1);
//}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	float moveDistance = 0.05f;

	switch (key) {
	case 'p':
		currentShapeType = SHAPE_POINT;
		selectedShapeIndex = -1;
		std::cout << "점 그리기 모드" << std::endl;
		break;
	case 'q':
		currentShapeType = LINE;
		selectedShapeIndex = -1;
		std::cout << "선 그리기 모드" << std::endl;
		break;
	case 't':
		currentShapeType = TRIANGLE;
		selectedShapeIndex = -1;
		std::cout << "삼각형 그리기 모드" << std::endl;
		break;
	case 'r':
		currentShapeType = RECTANGLE;
		selectedShapeIndex = -1;
		std::cout << "사각형 그리기 모드" << std::endl;
		break;
	case 'w':
		if (selectedShapeIndex >= 0) {
			moveShape(selectedShapeIndex, 0, moveDistance);
			updateShapeBuffer(selectedShapeIndex);
			glutPostRedisplay();
		}
		break;
	case 'a':
		if (selectedShapeIndex >= 0) {
			moveShape(selectedShapeIndex, -moveDistance, 0);
			updateShapeBuffer(selectedShapeIndex);
			glutPostRedisplay();
		}
		break;
	case 's':
		if (selectedShapeIndex >= 0) {
			moveShape(selectedShapeIndex, 0, -moveDistance);
			updateShapeBuffer(selectedShapeIndex);
			glutPostRedisplay();
		}
		break;
	case 'd':
		if (selectedShapeIndex >= 0) {
			moveShape(selectedShapeIndex, moveDistance, 0);
			updateShapeBuffer(selectedShapeIndex);
			glutPostRedisplay();
		}
		break;
	case 'i':
		if (selectedShapeIndex >= 0) {
			moveShape(selectedShapeIndex, -moveDistance, moveDistance);
			updateShapeBuffer(selectedShapeIndex);
			glutPostRedisplay();
		}
		break;
	case 'j':
		if (selectedShapeIndex >= 0) {
			moveShape(selectedShapeIndex, moveDistance, moveDistance);
			updateShapeBuffer(selectedShapeIndex);
			glutPostRedisplay();
		}
		break;
	case 'k':
		if (selectedShapeIndex >= 0) {
			moveShape(selectedShapeIndex, -moveDistance, -moveDistance);
			updateShapeBuffer(selectedShapeIndex);
			glutPostRedisplay();
		}
		break;
	case 'l':
		if (selectedShapeIndex >= 0) {
			moveShape(selectedShapeIndex, moveDistance, -moveDistance);
			updateShapeBuffer(selectedShapeIndex);
			glutPostRedisplay();
		}
		break;
	case 'c':
		for (auto& shape : shapes) {
			if (shape.active) {
				glDeleteVertexArrays(1, &shape.vao);
				glDeleteBuffers(2, shape.vbo);
				shape.active = false;
			}
		}
		shapes.clear();
		selectedShapeIndex = -1;
		std::cout << "모든 도형 삭제됨" << std::endl;
		break;
	}
	glutPostRedisplay();
}

GLvoid Mouse(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN) {
		if(button == GLUT_LEFT_BUTTON) {
			Vec2 ndc = WindowToNDC(x, y);
			std::cout << "NDC 좌표: " << ndc.x << ", " << ndc.y << std::endl;

			// 이동 모드인지 확인
			if (selectedShapeIndex >= 0) {
				selectedShapeIndex = -1; // 선택 해제
				glutPostRedisplay();
				return;
			}

			// 도형 선택 모드
			int clickedShape = findShapeAtPosition(ndc);
			if (clickedShape >= 0) {
				selectedShapeIndex = clickedShape;
				std::cout << "도형 " << clickedShape << " 선택됨" << std::endl;
				glutPostRedisplay();
				return;
			}

			// 새 도형 생성
			switch (currentShapeType) {
			case SHAPE_POINT:
				createSHAPE_POINT(ndc);
				break;
			case LINE:
				createLine(ndc);
				break;
			case TRIANGLE:
				createTriangle(ndc);
				break;
			case RECTANGLE:
				createRectangle(ndc);
				break;
			}

			// 마지막 도형의 버퍼 업데이트
			if (!shapes.empty()) {
				updateShapeBuffer(shapes.size() - 1);
			}

			glutPostRedisplay();
		}
	}
}

// 유니폼 변수를 선언했으면 반드시 할당해서 사용해야한다 아니면 오류남 
// 메인에서 값을 바꾸면 다시 버터 데이터를 쉐이더에 보내주어야한다. (아직 변환을 안 배워서)