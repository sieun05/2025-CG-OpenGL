#include <iostream>
#include <random>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

//윈도우 좌표값 -> 오픈지엘 좌표값(-1~ 1, y 위쪽이 양의방향)으로 바꾸는 함수 만들어서 학기내내 사용

//#include <gl/glm/glm.hpp>
//#include <gl/glm/>
//#include <gl/glm/gtc/matrix_transform.hpp>

using namespace std;

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<float> r_float(0, 1);

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid TimerFunction(int value);

int windowID;
float r{ 1}, g{ 1 }, b{ 1 };
bool timer_flag=false;

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
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(500, TimerFunction, 1);
	glutMainLoop();

	glutDestroyWindow(windowID);
	return 0;
}

GLvoid drawScene() 
{
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'c':
		r = 0.0f;
		g = 1.0f;
		b = 1.0f;
		break;
	case 'm':
		r = 1.0f;
		g = 0.0f;
		b = 1.0f;
		break;
	case'y':
		r = 1.0f;
		g = 1.0f;
		b = 0.0f;
		break;
	case 'a':
		r = r_float(gen);
		g = r_float(gen);
		b = r_float(gen);
		break;
	case 'w':
		r = 1.0f;
		b = 1.0f;
		g = 1.0f;
		break;
	case 'k':
		r = 0.0f;
		b = 0.0f;
		g = 0.0f;
		break;
	case 't':
		timer_flag = true;
		break;
	case 's':
		timer_flag = false;
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

GLvoid TimerFunction(int value)
{
	if (timer_flag) {
		r = r_float(gen);
		g = r_float(gen);
		b = r_float(gen);

		glutPostRedisplay();
	}
	glutTimerFunc(500, TimerFunction, 1);
}
