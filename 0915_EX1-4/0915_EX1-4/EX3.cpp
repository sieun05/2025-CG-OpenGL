#include "head.h"
#include "WindowToNDC.h"

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Motion(int x, int y);

struct RGB { float r, g, b; };
struct Rect {
	Rect() = default;
	Rect(float x, float y, float size, RGB color)
		: x{ x }, y{ y }, size{ size }, color{ color }
	{
	}
	Rect(const Rect& r) = default;
	Rect& operator=(const Rect& p) = default;

	bool Crash_rect(const vector<Rect>& rect)
	{
		for (const Rect& r : rect) {
			if (&r == this) continue;

			if (abs(x-size - r.x) < r.size and abs(y+size - r.y) < r.size or
				abs(x+size - r.x) < r.size and abs(y+size - r.y) < r.size or
				abs(x+size - r.x) < r.size and abs(y-size - r.y) < r.size or
				abs(x-size - r.x) < r.size and abs(y-size - r.y) < r.size) {
				return true;
			}
		}
		return false;
		
	}

	float x, y;
	float size;
	RGB color;
};

int windowID;
vector<Rect> rect;
int a_rect;
Vec2 mp;
int rect_index;
bool rect_in{ false };

int main(int argc, char** argv)
{
	//윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
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
	glutKeyboardFunc(Keyboard);
	glutMotionFunc(Motion);
	glutMainLoop();

	glutDestroyWindow(windowID);
	return 0;
}

GLvoid drawScene()
{
	glClearColor(1, 1, 1, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (Rect& r : rect) {
		glColor3f(r.color.r, r.color.g, r.color.b);
		glRectf(r.x - r.size, r.y - r.size, r.x + r.size, r.y + r.size);
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y)
{
	mp = WindowToNDC(x, y);

	for (int i{}; i < rect.size(); i++) {
		const Rect& r = rect[i];

		if (abs(mp.x - r.x) < r.size and abs(mp.y - r.y) < r.size) {
			rect_in = true;
			rect_index = i;
		}
	}

	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:
		{
			
			break;
		}
		case GLUT_RIGHT_BUTTON:
		{
			
			break;
		}
		}
	}
	else if (state == GLUT_UP) {
		switch (button) {
		case GLUT_LEFT_BUTTON:
		{
			rect_in = false;
			cout << rect[rect_index].Crash_rect(rect) << endl;
			break;
		}
		case GLUT_RIGHT_BUTTON:
		{

			break;
		}
		}
	}


	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y) 
{
	switch (key) {
	case 'a':
		if (a_rect < 10) {
			a_rect++;
			float x{ r_float2(gen) };
			float y{ r_float2(gen) };
			float size{ r_float3(gen) };
			RGB rgb{ r_float(gen) ,r_float(gen) ,r_float(gen) };
			rect.push_back({x, y, size, rgb});
		}
		break;
	}

	glutPostRedisplay();
}

GLvoid Motion(int x, int y)
{
	Vec2 mpp = WindowToNDC(x, y);


	if (rect_in) {
		rect[rect_index].x += (mpp.x - mp.x);
		rect[rect_index].y += (mpp.y - mp.y);
	}

	mp.x = mpp.x;
	mp.y = mpp.y;
	
	glutPostRedisplay();

}
