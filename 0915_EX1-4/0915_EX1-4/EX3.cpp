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
	Rect(float x, float y, float x_size, float y_size, RGB color)
		: x{ x }, y{ y }, x_size{ x_size }, y_size{ y_size }, color { color }
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

			if (abs(x - r.x) < x_max_size and abs(y - r.y) < y_max_size or
				abs(x - r.x) < x_max_size and abs(y - r.y) < y_max_size or
				abs(x - r.x) < x_max_size and abs(y - r.y) < y_max_size or
				abs(x - r.x) < x_max_size and abs(y - r.y) < y_max_size) {

				return i;
			}
		}
		return -1;
	}

	bool In_rect(float mx, float my) {
		if (abs(x - mx) < x_size and abs(y - my) < y_size){
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
		glRectf(r.x - r.x_size, r.y - r.y_size, r.x + r.x_size, r.y + r.y_size);
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y)
{
	if (rect.size() <= 0) return;

	mp = WindowToNDC(x, y);

	for (int i{}; i < rect.size(); i++) {
		const Rect& r = rect[i];

		if (abs(mp.x - r.x) < r.x_size and abs(mp.y - r.y) < r.y_size) {
			rect_in = true;
			rect_index = i;
		}
	}

	if (state == GLUT_UP) {
		switch (button) {
		case GLUT_LEFT_BUTTON:
		{
			rect_in = false;
			cout << "선택rect: " << rect_index << ", 충돌rect: " << rect[rect_index].Crash_rect(rect) << endl;

			int i = rect[rect_index].Crash_rect(rect);

			if (i >= 0) {
				float r_x_size = max(rect[rect_index].x_size, rect[i].x_size);
				float r_y_size = max(rect[rect_index].y_size, rect[i].y_size);
				Vec2 rp = WindowToNDC(x, y);

				if (rect_index < i) {
					rect.erase(rect.begin() + i);
					rect.erase(rect.begin() + rect_index);
				}
				else {
					rect.erase(rect.begin() + rect_index);
					rect.erase(rect.begin() + i);
				}
				

				RGB rgb = { r_float(gen), r_float(gen), r_float(gen) };
				rect.push_back({ rp.x, rp.y, r_x_size, r_y_size, rgb });
			}
			break;
		}
		case GLUT_RIGHT_BUTTON:
		{
			if (rect.size() >= 30) return;

			mp = WindowToNDC(x, y);

			cout << 333 << endl;
			
			for (int i{}; i < rect.size(); i++) {
				Rect& r = rect[i];

				if (r.In_rect(mp.x, mp.y)) {

					float x_size{ r_float3(gen) };
					float y_size{ r_float3(gen) };
					RGB rgb{ r_float(gen) ,r_float(gen) ,r_float(gen) };
					rect.push_back({ mp.x - x_size, mp.y, x_size, y_size, rgb });

					x_size = { r_float3(gen) };
					y_size ={ r_float3(gen) };
					rgb = { r_float(gen) ,r_float(gen) ,r_float(gen) };
					rect.push_back({ mp.x + x_size, mp.y, x_size, y_size, rgb });

					rect.erase(rect.begin() + i);

					return;
				}
			}
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
		if (rect.size() < 10) {
			float x{ r_float2(gen) };
			float y{ r_float2(gen) };
			float x_size{ r_float3(gen) };
			float y_size{ r_float3(gen) };
			RGB rgb{ r_float(gen) ,r_float(gen) ,r_float(gen) };
			rect.push_back({x, y, x_size, y_size, rgb});
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
