#include "head.h"
#include "WindowToNDC.h"

GLvoid drawScene(GLvoid);
int main(int argc, char** argv);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
//GLvoid Keyboard(unsigned char key, int x, int y);
//GLvoid Motion(int x, int y);
GLvoid TimerFunction(int value);

struct RGB { float r, g, b; };
struct Rect {
	Rect() = default;
	Rect(float x, float y, float x_size, float y_size, RGB color)
		: x{ x }, y{ y }, x_size{ x_size }, y_size{ y_size }, color{ color }
	{
	}
	Rect(float x, float y, float x_size, float y_size, RGB color, Direction dir)
		: x{ x }, y{ y }, x_size{ x_size }, y_size{ y_size }, color{ color }, dir{ dir }
	{
	}
	Rect(const Rect& r) = default;
	Rect& operator=(const Rect& p) = default;

	int Crash_rect(const vector<Rect>& rect)
	{
		for (int i{}; i < rect.size(); i++) {
			const Rect& r = rect[i];

			if (&r == this) continue;

			float x_max_size = r.x_size + x_size;
			float y_max_size = r.y_size + y_size;

			if (abs(x - r.x) < x_max_size and abs(y - r.y) < y_max_size) {
				return i;
			}
		}
		return -1;
	}

	bool In_rect(float mx, float my) {
		if (abs(x - mx) < x_size and abs(y - my) < y_size) {
			return true;
		}
		return false;
	}

	float x, y;
	float x_size, y_size;
	RGB color;
	Direction dir;
};

int windowID;
Vec2 mp;
vector<Rect> rect;
vector<vector<Rect>> anima_rect;
int animation[5]{0, };
int cnt{};

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

	for (int i{}; i < 5; i++) {
		float x = r_float2(gen);
		float y = r_float2(gen);
		float x_size = r_float3(gen);
		float y_size = r_float3(gen);
		RGB rgb = { r_float(gen), r_float(gen), r_float(gen) };
		rect.push_back({ x, y, x_size, y_size, rgb });
	}

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	//glutKeyboardFunc(Keyboard);
	//glutMotionFunc(Motion);
	glutTimerFunc(50, TimerFunction, 1);
	glutMainLoop();

	glutDestroyWindow(windowID);
	return 0;
}


GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (Rect& r : rect) {
		glColor3f(r.color.r, r.color.g, r.color.b);
		glRectf(r.x - r.x_size, r.y - r.y_size, r.x + r.x_size, r.y + r.y_size);
	}

	for (vector<Rect>& anima_rect : anima_rect) {
		for (Rect& r : anima_rect) {
			glColor3f(r.color.r, r.color.g, r.color.b);
			glRectf(r.x - r.x_size, r.y - r.y_size, r.x + r.x_size, r.y + r.y_size);
		}
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) {
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
		{
			cout << "Left Button: " << x << ", " << y << endl;
			mp = WindowToNDC(x, y);
			cout << "Left Button: " << mp.x << ", " << mp.y << endl;

			for (int i{}; i < rect.size(); i++) {
				Rect& r = rect[i];
				if (r.In_rect(mp.x, mp.y)) {
					cout << "Clicked Rect: " << r.x << ", " << r.y << endl;
					
					vector<Rect> temp_rect;
					temp_rect.push_back({ r.x - r.x_size / 2, r.y + r.y_size / 2, r.x_size / 2, r.y_size / 2, r.color, {-1, 1 } });
					temp_rect.push_back({ r.x + r.x_size / 2, r.y + r.y_size / 2, r.x_size / 2, r.y_size / 2, r.color, { 1, 1 } });
					temp_rect.push_back({ r.x - r.x_size / 2, r.y - r.y_size / 2, r.x_size / 2, r.y_size / 2, r.color, {-1, -1} });
					temp_rect.push_back({ r.x + r.x_size / 2, r.y - r.y_size / 2, r.x_size / 2, r.y_size / 2, r.color, { 1, -1} });

					animation[cnt++] = r_int_1_4(gen);
					//animation[cnt++] = 4;

					if (animation[cnt - 1] == 4) {
						temp_rect.push_back({ r.x - r.x_size / 2, r.y, r.x_size / 2, r.y_size / 2, r.color, {-1, 0 } });
						temp_rect.push_back({ r.x + r.x_size / 2, r.y, r.x_size / 2, r.y_size / 2, r.color, { 1, 0 } });
						temp_rect.push_back({ r.x, r.y - r.y_size / 2, r.x_size / 2, r.y_size / 2, r.color, {0, -1} });
						temp_rect.push_back({ r.x, r.y + r.y_size / 2, r.x_size / 2, r.y_size / 2, r.color, { 0, 1} });
					}

					anima_rect.push_back(temp_rect);

					rect.erase(rect.begin() + i);
					break;
				}
			}

			break;
		}
		case GLUT_RIGHT_BUTTON:
		{
			cout << "Right Button: " << x << ", " << y << endl;
			mp = WindowToNDC(x, y);

			break;
		}
		}
	}
	glutPostRedisplay();
}

GLvoid TimerFunction(int value)
{
	for (int i{}; i < anima_rect.size(); i++) {
		vector<Rect>& rect = anima_rect[i];
		if (animation[i] == 0) continue;

		for (int j{}; j < rect.size(); j++) {
			Rect& r = rect[j];

			switch (animation[i])
			{
			case 1:
			{
				if (j == 0)
					r.x += r.dir.dx * 0.01f;
				else if (j == 1)
					r.y += r.dir.dy * 0.01f;
				else if (j == 2)
					r.y += r.dir.dy * 0.01f;
				else if (j == 3)
					r.x += r.dir.dx * 0.01f;

				r.x_size -= 0.0006f;
				r.y_size -= 0.0008f;
				break;
			}
			case 2:
			{
				r.x += r.dir.dx * 0.01f;
				r.y += r.dir.dy * 0.01f;
				r.x_size -= 0.0006f;
				r.y_size -= 0.0008f;
				break;
			}
			case 3:
			{
				r.x += rect[0].dir.dx * 0.01f;
				r.y += rect[0].dir.dy * 0.01f;
				r.x_size -= 0.0006f;
				r.y_size -= 0.0008f;
				break;
			}
			case 4:
			{
				r.x += r.dir.dx * 0.01f;
				r.y += r.dir.dy * 0.01f;
				r.x_size -= 0.0006f;
				r.y_size -= 0.0008f;
				break;
			}
			}

			if (r.x_size <= 0.01 or r.y_size <= 0.01) {
				r.x_size = r.y_size = 0;
				animation[i] = (j == rect.size()-1) ? 0 : animation[i];
			}
		}
	}

	glutPostRedisplay();
	glutTimerFunc(50, TimerFunction, 1);
}