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

using namespace std;

//struct Vec2 { float x, y; };
struct RGBA { float r, g, b, a; };

// 삼각형 데이터 구조체
struct Triangle {
    Vec2 vertices[3];
    RGBA color;
    bool visible;
};

// 랜덤 색상 생성
RGBA getRandomColor() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    return { dis(gen), dis(gen), dis(gen), 1.0f };
}

// 사분면 판별 함수
int getQuadrant(float x, float y) {
    if (x >= 0 && y >= 0) return 0; // 1사분면 (우상)
    if (x < 0 && y >= 0) return 1;  // 2사분면 (좌상)
    if (x < 0 && y < 0) return 2;   // 3사분면 (좌하)
    if (x >= 0 && y < 0) return 3;  // 4사분면 (우하)
    return 0;
}


// 삼각형 생성 함수
Triangle createTriangle(float centerX, float centerY) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> r_size(0.1f, 0.3f);

    Triangle tri;
    float size = r_size(gen);

    // 정삼각형 생성
    tri.vertices[0] = { centerX, centerY + size };           // 상단
    tri.vertices[1] = { centerX - size * 0.866f, centerY - size * 0.5f }; // 좌하
    tri.vertices[2] = { centerX + size * 0.866f, centerY - size * 0.5f }; // 우하

    tri.color = getRandomColor();
    tri.visible = true;

    return tri;
}