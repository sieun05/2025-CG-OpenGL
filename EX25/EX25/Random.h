#pragma once
#include <random>

std::random_device rd;  // 시드 생성기
std::mt19937 gen(rd()); // 메르센 트위스터 엔진
std::uniform_int_distribution<> Cube_face_dis(0, 5); // 0~5 사이 균등 분포
std::uniform_int_distribution<> Pyramid_face_dis(0, 3); // 0~5 사이 균등 분포