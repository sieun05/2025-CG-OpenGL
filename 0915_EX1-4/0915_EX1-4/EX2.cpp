#include "head.h"

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<float> r_float(0, 1);

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);

struct Vec2 { float x, y; };
struct RGB { float r, g, b; };

// winX, winY : 윈도우(또는 프레임버퍼) 기준 픽셀 좌표 (원점: 좌상단)
// pixelCenter : 픽셀 센터를 맞추려면 true (마우스 클릭 지점이 픽셀 중앙에 매핑됨)
Vec2 WindowToNDC(int winX, int winY, bool pixelCenter = true) {
	// 현재 glViewport 값 가져오기: {x, y, w, h}
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	const float vx = (float)vp[0];
	const float vy = (float)vp[1];
	const float vw = (float)vp[2];
	const float vh = (float)vp[3];

	// 픽셀 센터 보정(원하면 0.5 더함)
	const float px = pixelCenter ? (winX + 0.5f) : (float)winX;
	const float py = pixelCenter ? (winY + 0.5f) : (float)winY;

	// 윈도우 좌표(좌상단 원점, +y 아래) → 뷰포트 정규 [0,1]
	const float nx = (px - vx) / vw;          // 0(left) ~ 1(right)
	const float ny_down = (py - vy) / vh;     // 0(top)  ~ 1(bottom)

	// [0,1] → [-1,1], y축 뒤집기
	Vec2 ndc;
	ndc.x = nx * 2.0f - 1.0f;
	ndc.y = 1.0f - (ny_down * 2.0f);         // 위로 증가하도록 플립

	return ndc;
}

int windowID;
float r{ r_float(gen) }, g{ r_float(gen) }, b{ r_float(gen) };
bool timer_flag{ false };
float mx, my;
float rect_size[4]{0.5, 0.5, 0.5, 0.5};
RGB rect_color[4]{ {1, 0, 0}, {1, 1, 0}, {1, 0, 1}, {0, 1, 1} };
int rect_index;
bool rect_in{ false };

int main(int argc, char** argv)
{
	//윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(500, 500);
	windowID = glutCreateWindow("Example1");

	//GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}
	else {
		cout << "GLEW Initialized" << endl;
	}

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMainLoop();

	glutDestroyWindow(windowID);
	return 0;
}

GLvoid drawScene()
{
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(rect_color[0].r, rect_color[0].g, rect_color[0].b);
	glRectf(-0.5 - rect_size[0], 0.5 - rect_size[0], -0.5 + rect_size[0], 0.5 + rect_size[0]);

	glColor3f(rect_color[1].r, rect_color[1].g, rect_color[1].b);
	glRectf(0.5 - rect_size[1], 0.5 - rect_size[1], 0.5 + rect_size[1], 0.5 + rect_size[1]);

	glColor3f(rect_color[2].r, rect_color[2].g, rect_color[2].b);
	glRectf(-0.5 - rect_size[2], -0.5 - rect_size[2], -0.5 + rect_size[2], -0.5 + rect_size[2]);

	glColor3f(rect_color[3].r, rect_color[3].g, rect_color[3].b);
	glRectf(0.5 - rect_size[3], -0.5 - rect_size[3], 0.5 + rect_size[3], -0.5 + rect_size[3]);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y)
{
	Vec2 mp{};
	mp = WindowToNDC(x, y);
	mx = mp.x, my = mp.y;
	rect_in = false;

	if (mx < 0 and my > 0) {
		rect_index = 0;
		if (mx > -0.5 - rect_size[0] and mx < -0.5 + rect_size[0] and my > 0.5 - rect_size[0] and my < 1 + rect_size[0])
			rect_in = true;
	}
	else if (mx > 0 and my > 0) {
		rect_index = 1;
		if (mx > 0.5 - rect_size[1] and mx < 0.5 + rect_size[1] and my> 0.5 - rect_size[1] and my < 0.5 + rect_size[1])
			rect_in = true;
	}
	else if (mx < 0 and my < 0) {
		rect_index = 2;
		if (mx > -0.5 - rect_size[2] and mx < -0.5 + rect_size[2] and my >  -0.5 - rect_size[2] and my < -0.5 + rect_size[2])
			rect_in = true;
	}
	else if (mx > 0 and my < 0) {
		rect_index = 3;
		if (mx > 0.5 - rect_size[3] and mx < 0.5 + rect_size[3] and my > -0.5 - rect_size[3] and my < -0.5 + rect_size[3])
			rect_in = true;
	}

	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:
		{
			if (rect_in) {
				rect_color[rect_index] = {r_float(gen), r_float(gen), r_float(gen)};
			}
			else {
				r = r_float(gen);
				g = r_float(gen);
				b = r_float(gen);
			}
			break;
		}
		case GLUT_RIGHT_BUTTON:
		{
			if (rect_in) {
				if (rect_size[rect_index] > 0.2) {
					rect_size[rect_index] -= 0.1;
				}
			}
			else {
				if (rect_size[rect_index] < 0.5) {
					rect_size[rect_index] += 0.1;
				}
			}
			break;
		}
		}
	}
	
}