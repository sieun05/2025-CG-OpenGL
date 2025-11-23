#pragma once
#define _CRT_SECURE_NO_WARNINGS 
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

// M_PI 상수 정의 (일부 컴파일러에서 정의되지 않을 수 있음)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 기본 구조체 및 타입 정의
struct RGBA { float r, g, b, a; };

// 전역 변수 선언 (shaderProgramID는 shader_func.h에서 정의됨)
extern glm::mat4 gProjection;
extern glm::mat4 gView;
extern glm::mat4 gModel;
extern GLint uMVP_loc;
