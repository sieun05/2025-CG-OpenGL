#pragma once
#include "헤더.h"
#include "Cube.h"
#include "player.h"

// 미니맵 관련 함수들
void DrawMinimap(int windowWidth, int windowHeight, GLint mvpLocation);
void DrawMinimapGround(const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation);
void DrawMinimapCubes(const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation);
void DrawMinimapPlayer(const glm::mat4& view, const glm::mat4& projection, GLint mvpLocation);