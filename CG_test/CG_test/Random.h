#pragma once
#include <random>

// 전역 변수 선언 (extern)
extern std::random_device rd;  // 시드 생성기
extern std::mt19937 gen; // 메르센 트위스터 엔진
extern std::uniform_int_distribution<> Cube_face_dis; // 0~5 사이 균등 분포
extern std::uniform_int_distribution<> Pyramid_face_dis; // 0~3 사이 균등 분포
extern std::uniform_real_distribution<> position_dis; // -5.0~5.0 사이 균등 분포
extern std::uniform_real_distribution<> scale_dis; // 0.5~2.0 사이 균등 분포
extern std::uniform_real_distribution<> color_dis; // 0.0~1.0 사이 균등 분포