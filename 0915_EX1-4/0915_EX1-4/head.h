#pragma once
#include <iostream>
#include <random>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <cmath>

using namespace std;

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<float> r_float(0, 1);
uniform_real_distribution<float> r_float2(-1, 1);
uniform_real_distribution<float> r_float3(0.05, 0.2);