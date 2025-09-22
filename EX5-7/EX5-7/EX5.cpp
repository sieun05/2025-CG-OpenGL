#include "head.h"
#include "WindowToNDC.h"

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
//GLvoid Motion(int x, int y);
GLvoid TimerFunction(int value);

struct RGB { float r, g, b; };
struct Rect {
	Rect() = default;
	Rect(float x, float y, float x_size, float y_size, RGB color)
		: x{ x }, y{ y }, x_size{ x_size }, y_size{ y_size }, color{ color }
	{
		init_pos = { x, y };
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
	Vec2 init_pos;
	Direction dir;
	float speed;

	Vec2 prev_pos;
	Direction prev_dir;
};

int windowID;

vector<Rect> rect;
Vec2 mp;
bool key_1_flag{ false };
bool key_2_flag{ false };
bool key_3_flag{ false };
bool key_4_flag{ false };
bool key_5_flag{ false };
bool timer_flag{ false };

int delay_time;

int r_index;

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
	//glutMotionFunc(Motion);
	glutTimerFunc(50, TimerFunction, 1);
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

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y) {
	if (rect.size() >= 5) return;

	if (state == GLUT_DOWN) {
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
		{
			cout << "Left Button: " << x << ", " << y << endl;

			mp = WindowToNDC(x, y);
			float x_size = 0.06f;
			float y_size = 0.08f;
			RGB rgb = { r_float(gen), r_float(gen), r_float(gen) };

			rect.push_back({ mp.x, mp.y, x_size, y_size, rgb });

			break;
		}
		}
		glutPostRedisplay();
	}
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		if (not key_1_flag) {
			key_1_flag = true;
			key_2_flag = false;
			timer_flag = true;
			key_5_flag = false;

			for (Rect& r : rect) {
				r.dir.dx = r_bool(gen) ? 1 : -1;
				r.dir.dy = r_bool(gen) ? 1 : -1;

				r.speed = r_float4(gen);
			}
		}
		else {
			key_1_flag = false;
		}
		break;
	case '2':
		if (not key_2_flag) {
			key_2_flag = true;
			key_1_flag = false;
			timer_flag = true;
			key_5_flag = false;

			for (Rect& r : rect) {
				r.dir.dx = r_bool(gen) ? 1 : -1;
				r.dir.dy = r_bool(gen) ? 1 : -1;

				r.speed = r_float4(gen);
			}
		}
		else {
			key_2_flag = false;
		}
		break;
	case '3':
		key_3_flag = key_3_flag ? false : true;
		break;
	case '4':
		key_4_flag = key_4_flag ? false : true;
		break;
	case '5':
		if (not key_5_flag and (key_1_flag or key_2_flag)) {
			r_index = { static_cast<int>(r_int_0_100(gen) % (rect.size() - 1)) };
			key_5_flag = true;

			for (int i{}; i < rect.size(); i++) {
				if (i == r_index) continue;
				int prev_index;
				if (r_index != 0 and i == r_index + 1)
					prev_index = i - 2;
				else if (i == 0)
					prev_index = r_index;
				else
					prev_index = i - 1;

				Rect& r = rect[i];
				Rect& prev_r = rect[prev_index];

				r.x = prev_r.prev_pos.x - prev_r.prev_dir.dx * r.x_size * 2.5;
				if (key_1_flag)
					r.y = prev_r.prev_pos.y - prev_r.prev_dir.dy * r.y_size * 2.5;
				else
					r.y = prev_r.prev_pos.y;

				r.dir.dx = prev_r.prev_dir.dx;
				r.dir.dy = prev_r.prev_dir.dy;
				r.speed = prev_r.speed;

				r.prev_pos = { r.x, r.y };
				r.prev_dir = r.dir;
			}
		}
		else {
			key_1_flag = false;
			key_2_flag = false;
			key_5_flag = false;
		}
		break;
	case 's':
		timer_flag = false;
		break;
	case 'm':
		for (Rect& r : rect) {
			r.x = r.init_pos.x;
			r.y = r.init_pos.y;
		}
		break;
	case 'r':
		rect.clear();
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

GLvoid TimerFunction(int value)
{
	if (not timer_flag) {
		if (key_3_flag or key_4_flag) timer_flag = true;

		glutTimerFunc(1000, TimerFunction, 1);
		return;
	}

	if (key_1_flag) {
		for (Rect& r : rect) {
			r.prev_pos = { r.x, r.y };
			r.prev_dir = { r.dir.dx, r.dir.dy };

			r.x += r.dir.dx * r.speed;
			r.y += r.dir.dy * r.speed;

			if (r.x - r.x_size <= -1) {
				r.dir.dx = 1;
			}
			if (r.x + r.x_size >= 1) {
				r.dir.dx = -1;
			}
			if (r.y - r.y_size <= -1) {
				r.dir.dy = 1;
			}
			if (r.y + r.y_size >= 1) {
				r.dir.dy = -1;
			}
		}
	}
	if (key_2_flag) {
		for (Rect& r : rect) {
			r.prev_pos = { r.x, r.y };
			r.prev_dir = { r.dir.dx, r.dir.dy };

			r.x += r.dir.dx * r.speed;
			if (r.x - r.x_size <= -1) {
				r.dir.dx = 1;

				r.y += r.dir.dy * r.y_size;

				if (r.y - r.y_size <= -1) {
					r.dir.dy = 1;
				}
				if (r.y + r.y_size >= 1) {
					r.dir.dy = -1;
				}
			}
			if (r.x + r.x_size >= 1) {
				r.dir.dx = -1;

				r.y += r.dir.dy * r.y_size;

				if (r.y - r.y_size <= -1) {
					r.dir.dy = 1;
				}
				if (r.y + r.y_size >= 1) {
					r.dir.dy = -1;
				}
			}

		}
	}
	if (key_3_flag and delay_time == 4) {
		for (Rect& r : rect) {
			r.x_size += r_bool(gen) ? 0.02f : -0.02;
			r.y_size += r_bool(gen) ? 0.02f : -0.02;

			if (r.x_size < 0.02f) r.x_size = 0.02f;
			if (r.y_size < 0.02f) r.y_size = 0.02f;
			if (r.x_size > 0.1f) r.x_size = 0.1f;
			if (r.y_size > 0.1f) r.y_size = 0.1f;
		}
	}
	if (key_4_flag and delay_time == 4) {
		for (Rect& r : rect) {
			r.color = { r_float(gen), r_float(gen), r_float(gen) };
		}
	}

	delay_time++;
	if (delay_time > 4) delay_time = 0;

	glutPostRedisplay();
	glutTimerFunc(50, TimerFunction, 1);
}
