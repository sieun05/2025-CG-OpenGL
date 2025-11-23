#include "헤더.h"
#include "WindowToNDC.h"
#include "shader_func.h"
#include "Cube.h"
#include "Pyramid.h"
#include "LightIndicator.h"  // 조명 표시기 헤더 추가

// shader_func.h에서 선언된 변수들을 extern으로 참조
extern GLuint shaderProgramID;

void InitBuffer();

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int value);

// 전역 변수 정의
glm::mat4 gProjection(1.0f);
glm::mat4 gView(1.0f);
glm::mat4 gModel(1.0f);
GLint uMVP_loc = -1;

// 조명 관련 변수들
GLint uModel_loc = -1;
GLint uView_loc = -1;
GLint uProjection_loc = -1;
GLint uLightPos_loc = -1;
GLint uLightColor_loc = -1;
GLint uViewPos_loc = -1;
GLint uUseLighting_loc = -1;

// 조명 설정 (요청하신 (1.0f, 0.0f, 0.0f) 위치)
glm::vec3 lightPos(1.0f, 0.0f, 0.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);  // 흰색 조명
glm::vec3 viewPos(3.0f, 2.0f, 3.0f);     // 카메라 위치
int useLighting = 1;                       // 조명 사용 여부

// 조명 거리 조절 관련 변수들
float lightDistance = 2.0f;               // 객체로부터의 거리 (초기값)
float lightMinDistance = 0.5f;            // 최소 거리
float lightMaxDistance = 5.0f;            // 최대 거리
float lightDistanceStep = 0.1f;           // 거리 조절 단계

// y축 회전 애니메이션 변수들
float yRotationAngle = 0.0f;              // 현재 y축 회전 각도
bool yRotationEnabled = false;             // y축 회전 활성화 여부
int yRotationDirection = 1;                // 회전 방향 (1: 양의 방향, -1: 음의 방향)

// 조명 공전 관련 변수들
bool lightOrbitEnabled = false;            // 조명 공전 활성화 여부
int lightOrbitDirection = 1;               // 공전 방향 (1: 양의 방향, -1: 음의 방향)
float lightOrbitAngle = 0.0f;              // 현재 조명 공전 각도
float lightOrbitRadius = 2.0f;             // 공전 반지름 (lightDistance와 동기화)
float lightOrbitHeight = 0.0f;             // 조명의 y축 높이
glm::vec3 lightOrbitCenter(0.0f, 0.0f, 0.0f); // 공전 중심점

// 조명 표시기 표시 여부
bool showLightIndicator = true;            // 조명 위치 표시 여부

// 궤도 원 그리기를 위한 변수들
GLuint VAO_orbit = 0;
GLuint VBO_orbit = 0;
const int orbitSegments = 100;             // 원을 구성하는 선분 수

// 조명 거리 업데이트 함수
void UpdateLightDistance()
{
    // 현재 조명이 원점에서 벗어나 있으면 방향을 계산
    glm::vec3 lightDirection = glm::normalize(lightPos - lightOrbitCenter);
    
    // 새로운 조명 위치 = 중심점 + (방향 * 거리)
    lightPos = lightOrbitCenter + lightDirection * lightDistance;
    
    // 공전 반지름도 조명 거리와 동기화
    lightOrbitRadius = lightDistance;
    
    // 공전 중인 경우 현재 각도를 유지하면서 반지름만 변경
    if (lightOrbitEnabled) {
        lightPos.x = lightOrbitCenter.x + lightOrbitRadius * cos(glm::radians(lightOrbitAngle));
        lightPos.z = lightOrbitCenter.z + lightOrbitRadius * sin(glm::radians(lightOrbitAngle));
        lightPos.y = lightOrbitHeight;
    }
}

// 궤도 원 초기화 함수 (동적으로 업데이트 가능하게 수정)
void UpdateOrbitCircle()
{
    std::vector<glm::vec3> orbitPoints;
    
    // 원 위의 점들 생성 (현재 lightOrbitRadius 사용)
    for (int i = 0; i <= orbitSegments; i++) {
        float angle = 2.0f * M_PI * i / orbitSegments;
        float x = lightOrbitCenter.x + lightOrbitRadius * cos(angle);
        float z = lightOrbitCenter.z + lightOrbitRadius * sin(angle);
        orbitPoints.push_back(glm::vec3(x, lightOrbitHeight, z));
    }
    
    // 기존 VAO가 없으면 생성
    if (VAO_orbit == 0) {
        glGenVertexArrays(1, &VAO_orbit);
        glGenBuffers(1, &VBO_orbit);
    }
    
    // VAO, VBO 업데이트
    glBindVertexArray(VAO_orbit);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_orbit);
    glBufferData(GL_ARRAY_BUFFER, orbitPoints.size() * sizeof(glm::vec3), 
                 orbitPoints.data(), GL_DYNAMIC_DRAW);  // DYNAMIC_DRAW로 변경
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    
    glBindVertexArray(0);
}

// 기존 InitOrbitCircle 함수를 UpdateOrbitCircle로 대체
void InitOrbitCircle()
{
    UpdateOrbitCircle();
}

// 궤도 원 그리기 함수
void DrawOrbitCircle()
{
    if (!lightOrbitEnabled) return;
    
    // 조명이 꺼져있을 때만 궤도를 그리기 (조명이 켜져있으면 보기 어려움)
    if (useLighting == 0) {
        glUseProgram(shaderProgramID);
        
        // 궤도는 단순한 흰색 선으로 그리기
        glm::mat4 orbitModel = glm::mat4(1.0f);
        glm::mat4 orbitMVP = gProjection * gView * orbitModel;
        glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(orbitMVP));
        
        // 조명 비활성화하고 단순 색상으로 그리기
        glUniform1i(uUseLighting_loc, 0);
        
        glBindVertexArray(VAO_orbit);
        glLineWidth(2.0f);
        glDrawArrays(GL_LINE_STRIP, 0, orbitSegments + 1);
        glLineWidth(1.0f);
        glBindVertexArray(0);
        
        // 조명 설정 복원
        glUniform1i(uUseLighting_loc, useLighting);
    }
}

// y축 회전 각도 업데이트 함수
void UpdateYRotation()
{
    const float rotationSpeed = 2.0f; // 회전 속도 (도/프레임)
    
    if (yRotationEnabled) {
        yRotationAngle += rotationSpeed * yRotationDirection;
        
        // 360도를 넘으면 0으로 리셋
        if (yRotationAngle >= 360.0f) {
            yRotationAngle -= 360.0f;
        }
        else if (yRotationAngle < 0.0f) {
            yRotationAngle += 360.0f;
        }
    }
}

// 조명 공전 업데이트 함수
void UpdateLightOrbit()
{
    const float orbitSpeed = 1.5f; // 공전 속도 (도/프레임)
    
    if (lightOrbitEnabled) {
        lightOrbitAngle += orbitSpeed * lightOrbitDirection;
        
        // 360도를 넘으면 0으로 리셋
        if (lightOrbitAngle >= 360.0f) {
            lightOrbitAngle -= 360.0f;
        }
        else if (lightOrbitAngle < 0.0f) {
            lightOrbitAngle += 360.0f;
        }
        
        // 조명 위치 업데이트 (현재 lightOrbitRadius 사용)
        lightPos.x = lightOrbitCenter.x + lightOrbitRadius * cos(glm::radians(lightOrbitAngle));
        lightPos.z = lightOrbitCenter.z + lightOrbitRadius * sin(glm::radians(lightOrbitAngle));
        lightPos.y = lightOrbitHeight;
    }
}

// y축 회전 행렬 적용 함수
glm::mat4 ApplyYRotation(const glm::mat4& baseModel)
{
    if (yRotationAngle == 0.0f) {
        return baseModel;
    }
    
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f),
        glm::radians(yRotationAngle),
        glm::vec3(0.0f, 1.0f, 0.0f)); // y축 벡터
    
    return baseModel * rotationMatrix;
}

void AfterMakeShaders() 
{
	glUseProgram(shaderProgramID);
	
	// 기존 uniform 변수들
	uMVP_loc = glGetUniformLocation(shaderProgramID, "uMVP");
	if (uMVP_loc < 0) { printf("uMVP get error\n"); exit(1); }
	
	// 조명 관련 uniform 변수들
	uModel_loc = glGetUniformLocation(shaderProgramID, "uModel");
	uView_loc = glGetUniformLocation(shaderProgramID, "uView");
	uProjection_loc = glGetUniformLocation(shaderProgramID, "uProjection");
	uLightPos_loc = glGetUniformLocation(shaderProgramID, "uLightPos");
	uLightColor_loc = glGetUniformLocation(shaderProgramID, "uLightColor");
	uViewPos_loc = glGetUniformLocation(shaderProgramID, "uViewPos");
	uUseLighting_loc = glGetUniformLocation(shaderProgramID, "uUseLighting");
	
	if (uModel_loc < 0) printf("uModel get error\n");
	if (uView_loc < 0) printf("uView get error\n");
	if (uProjection_loc < 0) printf("uProjection get error\n");
	if (uLightPos_loc < 0) printf("uLightPos get error\n");
	if (uLightColor_loc < 0) printf("uLightColor get error\n");
	if (uViewPos_loc < 0) printf("uViewPos get error\n");
	if (uUseLighting_loc < 0) printf("uUseLighting get error\n");
	
	glUseProgram(0);
}

//--- 메인 함수
void main(int argc, char** argv)
{
	GLint width, height;

	width = 800;
	height = 800;
	//--- 윈도우생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Example25 - Lighting");

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	//--- 세이더읽어와서세이더프로그램만들기
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();
	AfterMakeShaders();

	glutReshapeFunc(Reshape);
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(16, Timer, 1);

	InitBuffer();
	glutMainLoop();
}

GLvoid Timer(int value)
{
	// y축 회전 업데이트
	UpdateYRotation();
	
	// 조명 공전 업데이트
	UpdateLightOrbit();
	
	glutPostRedisplay();
	glutTimerFunc(16, Timer, 1);
}

void InitBuffer()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	InitCubeBuffer();
	InitPyramidBuffer();
	InitOrbitCircle();      // 궤도 원 초기화
	InitLightIndicator();   // 조명 표시기 초기화 추가
}

//--- 출력 콜백함수
GLvoid drawScene()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--- 렌더링 파이프라인에 세이더 불러우기
	glUseProgram(shaderProgramID);

	// View 설정
	viewPos = glm::vec3(3.0f, 2.0f, 3.0f);
	gView = glm::mat4(1.0f);
	gView = glm::lookAt(
		viewPos,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	// 조명 관련 uniform 변수들 설정
	glUniform3fv(uLightPos_loc, 1, glm::value_ptr(lightPos));
	glUniform3fv(uLightColor_loc, 1, glm::value_ptr(lightColor));
	glUniform3fv(uViewPos_loc, 1, glm::value_ptr(viewPos));
	glUniform1i(uUseLighting_loc, useLighting);
	glUniformMatrix4fv(uView_loc, 1, GL_FALSE, glm::value_ptr(gView));
	glUniformMatrix4fv(uProjection_loc, 1, GL_FALSE, glm::value_ptr(gProjection));

	// 궤도 원 그리기 (객체보다 먼저)
	DrawOrbitCircle();

	// Cube 그리기
	if(drawCube) {
		gModel = glm::mat4(1.0f);
		gModel = ApplyYRotation(gModel); // y축 회전 적용
		glUniformMatrix4fv(uModel_loc, 1, GL_FALSE, glm::value_ptr(gModel));
		
		glm::mat4 MVP = gProjection * gView * gModel;
		glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(MVP));
		
		DrawCube(gModel, gView, gProjection, uMVP_loc);
	}
	
	if (drawPyramid){
		gModel = glm::mat4(1.0f);
		gModel = ApplyYRotation(gModel); // y축 회전 적용
		glUniformMatrix4fv(uModel_loc, 1, GL_FALSE, glm::value_ptr(gModel));
		
		glm::mat4 MVP = gProjection * gView * gModel;
		glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(MVP));
		
		DrawPyramid(gModel, gView, gProjection, uMVP_loc);
	}
	
	// 조명 표시기 그리기 (조명이 켜져있고 표시 옵션이 활성화되어 있을 때)
	if (showLightIndicator && useLighting == 1) {
		// 조명 표시기는 조명 없이 그리기 (자체적으로 밝게 표시)
		glUniform1i(uUseLighting_loc, 0);
		DrawLightIndicator(lightPos, gView, gProjection, uMVP_loc);
		// 조명 설정 복원
		glUniform1i(uUseLighting_loc, useLighting);
	}
	
	glutSwapBuffers();
}

//--- 다시그리기콜백함수
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glUseProgram(shaderProgramID);

	float aspect = (h == 0) ? 1 : (float)w / (float)h;
	
	gProjection = glm::perspective(
		glm::radians(45.0f),
		aspect,
		0.1f,
		100.0f
	);

	glEnable(GL_DEPTH_TEST);
	glUseProgram(0);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'n':
		drawPyramid = !drawPyramid;
		drawCube = !drawCube;
		printf("%s 그리기\n", drawCube ? "정육면체" : "사각뿔");
		break;
	case 'l':  // l키로 조명 켜기/끄기
		useLighting = (useLighting == 1) ? 0 : 1;
		printf("조명 %s\n", (useLighting == 1) ? "켜기" : "끄기");
		break;
	case 'i':  // i키로 조명 표시기 켜기/끄기
		showLightIndicator = !showLightIndicator;
		printf("조명 위치 표시 %s\n", showLightIndicator ? "켜기" : "끄기");
		break;
	case 'z':  // z키로 조명을 객체에 가깝게 이동
		if (lightDistance > lightMinDistance) {
			lightDistance -= lightDistanceStep;
			if (lightDistance < lightMinDistance) {
				lightDistance = lightMinDistance;
			}
			UpdateLightDistance();
			UpdateOrbitCircle();  // 궤도 원 크기 업데이트
			printf("조명 거리: %.1f (가깝게)\n", lightDistance);
		}
		break;
	case 'Z':  // Z키로 조명을 객체에서 멀게 이동
		if (lightDistance < lightMaxDistance) {
			lightDistance += lightDistanceStep;
			if (lightDistance > lightMaxDistance) {
				lightDistance = lightMaxDistance;
			}
			UpdateLightDistance();
			UpdateOrbitCircle();  // 궤도 원 크기 업데이트
			printf("조명 거리: %.1f (멀게)\n", lightDistance);
		}
		break;
	case 'y':  // y키로 y축 회전 시작/정지
		yRotationEnabled = !yRotationEnabled;
		if (yRotationEnabled) {
			yRotationDirection = 1; // 양의 방향 회전
			printf("y축 양의 방향 회전 시작\n");
		}
		else {
			printf("y축 회전 정지\n");
		}
		break;
	case 'Y':  // Shift+y키로 y축 음의 방향 회전
		yRotationEnabled = !yRotationEnabled;
		if (yRotationEnabled) {
			yRotationDirection = -1; // 음의 방향 회전
			printf("y축 음의 방향 회전 시작\n");
		}
		else {
			printf("y축 회전 정지\n");
		}
		break;
	case 'r':  // r키로 조명 양의 방향 공전 시작/정지
		lightOrbitEnabled = !lightOrbitEnabled;
		if (lightOrbitEnabled) {
			lightOrbitDirection = 1; // 양의 방향 공전
			printf("조명 양의 방향 공전 시작 (반지름: %.1f)\n", lightOrbitRadius);
		}
		else {
			// 공전 정지 시 현재 위치에서 정지 (초기 위치로 복원하지 않음)
			printf("조명 공전 정지 (현재 위치: %.2f, %.2f, %.2f)\n", lightPos.x, lightPos.y, lightPos.z);
		}
		break;
	case 'R':  // Shift+r키로 조명 음의 방향 공전
		lightOrbitEnabled = !lightOrbitEnabled;
		if (lightOrbitEnabled) {
			lightOrbitDirection = -1; // 음의 방향 공전
			printf("조명 음의 방향 공전 시작 (반지름: %.1f)\n", lightOrbitRadius);
		}
		else {
			// 공전 정지 시 현재 위치에서 정지 (초기 위치로 복원하지 않음)
			printf("조명 공전 정지 (현재 위치: %.2f, %.2f, %.2f)\n", lightPos.x, lightPos.y, lightPos.z);
		}
		break;
	case 'c':  // c키로 초기 상태로 리셋
		yRotationAngle = 0.0f;
		yRotationEnabled = false;
		yRotationDirection = 1;
		lightOrbitEnabled = false;
		lightOrbitAngle = 0.0f;
		lightOrbitDirection = 1;
		lightDistance = 2.0f;  // 초기 거리로 리셋
		lightPos = glm::vec3(1.0f, 0.0f, 0.0f);  // 초기 위치로 리셋
		lightOrbitRadius = lightDistance;  // 궤도 반지름도 리셋
		UpdateOrbitCircle();  // 궤도 원 크기 업데이트
		useLighting = 1;
		showLightIndicator = true;
		drawCube = true;
		drawPyramid = false;
		printf("초기 상태로 리셋 (조명 거리: %.1f)\n", lightDistance);
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}

	glutPostRedisplay();
}