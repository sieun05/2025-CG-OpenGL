#pragma once
#define _CRT_SECURE_NO_WARNINGS 
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <random>

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h> 

#include "WindowToNDC.h"
#include "shader_func.h"

using namespace std;

//struct Vec2 { float x, y; };
struct RGBA { float r, g, b, a; };