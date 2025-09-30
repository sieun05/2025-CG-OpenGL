#include "head.h"
#include "WindowToNDC.h"

GLvoid drawScene(GLvoid);
int main(int argc, char** argv);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Motion(int x, int y);
//GLvoid TimerFunction(int value);

struct RGB { float r, g, b; };
struct Rect {
	Rect() = default;
	Rect(float x, float y, float x_size, float y_size, RGB color)
		: x{ x }, y{ y }, x_size{ x_size }, y_size{ y_size }, color{ color }
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
};

int windowID;

vector<Rect> rect;
Rect erase_rect;
Vec2 mp;
bool erase_flag = false;

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

	for (int i{}; i < 30; i++) {
		float x = r_float2(gen);
		float y = r_float2(gen);
		float x_size = 0.012f;
		float y_size = 0.016f;
		RGB rgb = { r_float(gen), r_float(gen), r_float(gen) };
		rect.push_back({ x, y, x_size, y_size, rgb });
	}

	float x_size = 0.024f;
	float y_size = 0.036f;
	RGB rgb = { 0, 0, 0 };

	erase_rect = { mp.x, mp.y, x_size, y_size, rgb };

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutMotionFunc(Motion);
	//glutTimerFunc(50, TimerFunction, 1);
	glutMainLoop();

	glutDestroyWindow(windowID);
	return 0;
}


GLvoid drawScene()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (Rect& r : rect) {
		glColor3f(r.color.r, r.color.g, r.color.b);
		glRectf(r.x - r.x_size, r.y - r.y_size, r.x + r.x_size, r.y + r.y_size);
	}

	if (erase_flag) {
		glColor3f(erase_rect.color.r, erase_rect.color.g, erase_rect.color.b);
		glRectf(erase_rect.x - erase_rect.x_size, erase_rect.y - erase_rect.y_size,
			erase_rect.x + erase_rect.x_size, erase_rect.y + erase_rect.y_size);
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y) 
{
	switch (key)
	{
	case 'r': 
		rect.clear();
		erase_rect.x_size = 0.024f;
		erase_rect.y_size = 0.036f;

		for (int i{}; i < 30; i++) {
			float x = r_float2(gen);
			float y = r_float2(gen);
			float x_size = 0.012f;
			float y_size = 0.016f;
			RGB rgb = { r_float(gen), r_float(gen), r_float(gen) };
			rect.push_back({ x, y, x_size, y_size, rgb });
		}

		glutPostRedisplay();
		break;
	}
}

GLvoid Mouse(int button, int state, int x, int y) {
	//if (rect.size() >= 5) return;

	if (state == GLUT_DOWN) {
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
		{
			cout << "Left Button: " << x << ", " << y << endl;

			mp = WindowToNDC(x, y);

			erase_flag = true;

			float x_size = erase_rect.x_size;
			float y_size = erase_rect.y_size;
			RGB rgb = { 0, 0, 0 };

			erase_rect = { mp.x, mp.y, x_size, y_size, rgb };

			break;
		}
		case GLUT_RIGHT_BUTTON:
		{
			cout << "Right Button: " << x << ", " << y << endl;

			if (rect.size() >= 40) break;

			mp = WindowToNDC(x, y);

			float x_size = 0.012f;
			float y_size = 0.016f;
			RGB rgb = { r_float(gen), r_float(gen), r_float(gen) };
			rect.push_back({ mp.x, mp.y, x_size, y_size, rgb });

			erase_rect.x_size -= 0.0012f;
			erase_rect.y_size -= 0.0016f;

			break;
		}
		}
		glutPostRedisplay();
	}
	else if (state == GLUT_UP) {
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
		{
			erase_flag = false;
		}
		}
	}
}

GLvoid Motion(int x, int y)
{
	Vec2 mmp = WindowToNDC(x, y);
	if (erase_flag) {
		erase_rect.x += mmp.x - mp.x;
		erase_rect.y += mmp.y - mp.y;

		mp.x = mmp.x;
		mp.y = mmp.y;

		int erase_index = erase_rect.Crash_rect(rect);

		if (erase_index != -1) {
			Rect crashed_rect = rect[erase_index];

			erase_rect.x_size += 0.0012f;
			erase_rect.y_size += 0.0016f;
			erase_rect.color = { crashed_rect.color.r, crashed_rect.color.g, crashed_rect.color.b };

			rect.erase(rect.begin() + erase_index);
			
		}
	}
	glutPostRedisplay();
}

//GLvoid TimerFunction(int value)
//{
//	if (not timer_flag) {
//		if (key_3_flag or key_4_flag) timer_flag = true;
//
//		glutTimerFunc(1000, TimerFunction, 1);
//		return;
//	}
//
//	if (key_1_flag) {
//		for (Rect& r : rect) {
//			r.prev_pos = { r.x, r.y };
//			r.prev_dir = { r.dir.dx, r.dir.dy };
//
//			r.x += r.dir.dx * r.speed;
//			r.y += r.dir.dy * r.speed;
//
//			if (r.x - r.x_size <= -1) {
//				r.dir.dx = 1;
//			}
//			if (r.x + r.x_size >= 1) {
//				r.dir.dx = -1;
//			}
//			if (r.y - r.y_size <= -1) {
//				r.dir.dy = 1;
//			}
//			if (r.y + r.y_size >= 1) {
//				r.dir.dy = -1;
//			}
//		}
//	}
//	if (key_2_flag) {
//		for (Rect& r : rect) {
//			r.prev_pos = { r.x, r.y };
//			r.prev_dir = { r.dir.dx, r.dir.dy };
//
//			r.x += r.dir.dx * r.speed;
//			if (r.x - r.x_size <= -1) {
//				r.dir.dx = 1;
//
//				r.y += r.dir.dy * r.y_size;
//
//				if (r.y - r.y_size <= -1) {
//					r.dir.dy = 1;
//				}
//				if (r.y + r.y_size >= 1) {
//					r.dir.dy = -1;
//				}
//			}
//			if (r.x + r.x_size >= 1) {
//				r.dir.dx = -1;
//
//				r.y += r.dir.dy * r.y_size;
//
//				if (r.y - r.y_size <= -1) {
//					r.dir.dy = 1;
//				}
//				if (r.y + r.y_size >= 1) {
//					r.dir.dy = -1;
//				}
//			}
//
//		}
//	}
//	if (key_3_flag and delay_time == 4) {
//		for (Rect& r : rect) {
//			r.x_size += r_bool(gen) ? 0.02f : -0.02;
//			r.y_size += r_bool(gen) ? 0.02f : -0.02;
//
//			if (r.x_size < 0.02f) r.x_size = 0.02f;
//			if (r.y_size < 0.02f) r.y_size = 0.02f;
//			if (r.x_size > 0.1f) r.x_size = 0.1f;
//			if (r.y_size > 0.1f) r.y_size = 0.1f;
//		}
//	}
//	if (key_4_flag and delay_time == 4) {
//		for (Rect& r : rect) {
//			r.color = { r_float(gen), r_float(gen), r_float(gen) };
//		}
//	}
//
//	delay_time++;
//	if (delay_time > 4) delay_time = 0;
//
//	glutPostRedisplay();
//	glutTimerFunc(50, TimerFunction, 1);
//}
