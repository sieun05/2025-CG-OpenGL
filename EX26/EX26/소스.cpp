#include "헤더.h"
#include "shader_func.h"
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

// 조명 설정 (화면 앞쪽에 위치)
glm::vec3 lightPos(1.0f, 1.0f, 2.0f);        // 화면 앞쪽에 위치
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);      // 흰색 조명
glm::vec3 viewPos(3.0f, 2.0f, 3.0f);         // 카메라 위치
int useLighting = 1;                          // 조명 사용 여부

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

// 구 렌더링을 위한 변수들
GLuint VAO_sphere = 0;
GLuint VBO_sphere[3] = { 0, };  // [0]: 위치, [1]: 색상, [2]: 법선
GLuint EBO_sphere = 0;
std::vector<glm::vec3> sphereVertices;
std::vector<glm::vec3> sphereColors;
std::vector<glm::vec3> sphereNormals;
std::vector<unsigned int> sphereIndices;

// 조명 색상 변경을 위한 변수들
int currentLightColorIndex = 0;
glm::vec3 lightColors[3] = {
    glm::vec3(1.0f, 1.0f, 1.0f),  // 흰색
    glm::vec3(1.0f, 0.0f, 0.0f),  // 빨간색
    glm::vec3(0.0f, 0.0f, 1.0f)   // 파란색
};

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

// 구 생성 함수
void GenerateSphere(float radius, int stacks, int sectors) {
    sphereVertices.clear();
    sphereColors.clear();
    sphereNormals.clear();
    sphereIndices.clear();

    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f / radius;
    float s, t;

    float sectorStep = 2 * M_PI / sectors;
    float stackStep = M_PI / stacks;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stacks; ++i) {
        stackAngle = M_PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            sectorAngle = j * sectorStep;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            sphereVertices.push_back(glm::vec3(x, y, z));

            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            sphereNormals.push_back(glm::vec3(nx, ny, nz));

            // 기본 색상 (회색)
            sphereColors.push_back(glm::vec3(0.7f, 0.7f, 0.7f));
        }
    }

    int k1, k2;
    for (int i = 0; i < stacks; ++i) {
        k1 = i * (sectors + 1);
        k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                sphereIndices.push_back(k1);
                sphereIndices.push_back(k2);
                sphereIndices.push_back(k1 + 1);
            }

            if (i != (stacks - 1)) {
                sphereIndices.push_back(k1 + 1);
                sphereIndices.push_back(k2);
                sphereIndices.push_back(k2 + 1);
            }
        }
    }
}

// 구 초기화 함수
void InitSphere() {
    GenerateSphere(0.5f, 20, 36);

    glGenVertexArrays(1, &VAO_sphere);
    glBindVertexArray(VAO_sphere);

    glGenBuffers(3, VBO_sphere);

    // 위치 데이터
    glBindBuffer(GL_ARRAY_BUFFER, VBO_sphere[0]);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(glm::vec3), 
                 sphereVertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 색상 데이터
    glBindBuffer(GL_ARRAY_BUFFER, VBO_sphere[1]);
    glBufferData(GL_ARRAY_BUFFER, sphereColors.size() * sizeof(glm::vec3), 
                 sphereColors.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 법선 데이터
    glBindBuffer(GL_ARRAY_BUFFER, VBO_sphere[2]);
    glBufferData(GL_ARRAY_BUFFER, sphereNormals.size() * sizeof(glm::vec3), 
                 sphereNormals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 인덱스 버퍼
    glGenBuffers(1, &EBO_sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_sphere);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), 
                 sphereIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

// 구 그리기 함수
void DrawSphere(const glm::vec3& position, const glm::vec3& scale = glm::vec3(1.0f)) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);

    glUniformMatrix4fv(uModel_loc, 1, GL_FALSE, glm::value_ptr(model));
    
    glm::mat4 mvp = gProjection * gView * model;
    glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, glm::value_ptr(mvp));

    glBindVertexArray(VAO_sphere);
    glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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
    // 조명 공전이 활성화되어 있으면 조명 위치 업데이트
    if (lightOrbitEnabled) {
        lightOrbitAngle += lightOrbitDirection * 2.0f; // 초당 약 120도 회전
        if (lightOrbitAngle >= 360.0f) lightOrbitAngle -= 360.0f;
        if (lightOrbitAngle < 0.0f) lightOrbitAngle += 360.0f;
        
        // 새로운 조명 위치 계산
        lightPos.x = lightOrbitCenter.x + lightOrbitRadius * cos(glm::radians(lightOrbitAngle));
        lightPos.z = lightOrbitCenter.z + lightOrbitRadius * sin(glm::radians(lightOrbitAngle));
        lightPos.y = lightOrbitHeight;
    }

    glutPostRedisplay();
    glutTimerFunc(16, Timer, 1);
}

void InitBuffer()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	InitOrbitCircle();      // 궤도 원 초기화
	InitLightIndicator();   // 조명 표시기 초기화 추가
	InitSphere();           // 구 초기화 추가
}

//--- 출력 콜백함수
GLvoid drawScene()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--- 렌더링 파이프라인에 세이더 불러우기
	glUseProgram(shaderProgramID);

	// View 설정
	viewPos = glm::vec3(8.0f, 0.0f, 8.0f);
	gView = glm::mat4(1.0f);
	gView = glm::lookAt(
		viewPos,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	// 조명 색상 업데이트 (현재 선택된 색상 사용)
	lightColor = lightColors[currentLightColorIndex];

    // 조명 관련 uniform 변수들 설정
    glUniform3fv(uLightPos_loc, 1, glm::value_ptr(lightPos));
    glUniform3fv(uLightColor_loc, 1, glm::value_ptr(lightColor));
    glUniform3fv(uViewPos_loc, 1, glm::value_ptr(viewPos));
    glUniform1i(uUseLighting_loc, useLighting);
    glUniformMatrix4fv(uView_loc, 1, GL_FALSE, glm::value_ptr(gView));
    glUniformMatrix4fv(uProjection_loc, 1, GL_FALSE, glm::value_ptr(gProjection));

    // 궤도 원 그리기 (객체보다 먼저)
    DrawOrbitCircle();

    // 3개의 구 일직선으로 배치 (Y축은 모두 0.0으로 동일하게, X축만 변경)
    // 1. 왼쪽 구
    DrawSphere(glm::vec3(-1.5f, 0.0f, 0.0f), glm::vec3(0.8f));
    
    // 2. 중앙 구 (화면 중앙)
    DrawSphere(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    
    // 3. 오른쪽 구
    DrawSphere(glm::vec3(1.5f, 0.0f, 0.0f), glm::vec3(0.8f));

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
	
	case 'c':
	case 'C':
        // 조명 색상 변경 (3가지 색상 순환)
        currentLightColorIndex = (currentLightColorIndex + 1) % 3;
        printf("조명 색상 변경: ");
        if (currentLightColorIndex == 0) printf("흰색\n");
        else if (currentLightColorIndex == 1) printf("빨간색\n");
        else printf("파란색\n");
        break;
        
    case 'r':
        // 조명을 중심 구의 Y축에 대하여 양의 방향으로 회전
        if (!lightOrbitEnabled) {
            lightOrbitEnabled = true;
            lightOrbitRadius = glm::length(lightPos - lightOrbitCenter);
            lightOrbitAngle = atan2(lightPos.z - lightOrbitCenter.z, 
                                   lightPos.x - lightOrbitCenter.x) * 180.0f / M_PI;
            lightOrbitHeight = lightPos.y;
            printf("조명 공전 시작 (양의 방향)\n");
        }
        lightOrbitDirection = 1;
        break;
        
    case 'R':
        // 조명을 중심 구의 Y축에 대하여 음의 방향으로 회전
        if (!lightOrbitEnabled) {
            lightOrbitEnabled = true;
            lightOrbitRadius = glm::length(lightPos - lightOrbitCenter);
            lightOrbitAngle = atan2(lightPos.z - lightOrbitCenter.z, 
                                   lightPos.x - lightOrbitCenter.x) * 180.0f / M_PI;
            lightOrbitHeight = lightPos.y;
            printf("조명 공전 시작 (음의 방향)\n");
        }
        lightOrbitDirection = -1;
        break;
	
	case 'q':
	case 'Q':
		printf("프로그램을 종료합니다.\n");
		exit(0);
		break;
		
	default:
		break;
	}

	glutPostRedisplay();
}