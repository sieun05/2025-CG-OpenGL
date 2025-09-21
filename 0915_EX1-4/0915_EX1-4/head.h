#pragma once
#include <iostream>
#include <random>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <cmath>

using namespace std;

struct Vec2 { float x, y; };

struct Direction {
	Direction() = default;
	Direction(int dx, int dy) : dx{ dx }, dy{ dy } {}

	int dx, dy;
};

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<float> r_float(0, 1);
uniform_real_distribution<float> r_float2(-1, 1);
uniform_real_distribution<float> r_float3(0.05, 0.2);
uniform_real_distribution<float> r_float4(0.005, 0.01);
uniform_int_distribution<int> r_bool(0, 1);
uniform_int_distribution<int> r_int_0_100(0, 100);