#version 330 core
//--- in_Position: attribute index 0
//--- in_Color: attribute index 1

layout (location = 0) in vec3 in_Position; //--- 위치 변수: attribute position 0
layout (location = 1) in vec3 in_Color; //--- 컬러 변수: attribute position 1
layout (location = 2) in vec3 in_Normal;  // 법선 벡터 추가

uniform mat4 uMVP; //--- MVP 행렬
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

// 조명 관련 uniform 변수들
uniform vec3 uLightPos;     // 조명 위치
uniform vec3 uLightColor;   // 조명 색상
uniform vec3 uViewPos;      // 카메라 위치
uniform int uUseLighting;   // 조명 사용 여부 (bool -> int 변경)

out vec3 out_Color; //--- 프래그먼트 셰더로 전달
out vec3 FragPos;     // 월드 좌표계의 프래그먼트 위치
out vec3 Normal;      // 월드 좌표계의 법선 벡터
out vec3 LightPos;    // 조명 위치
out vec3 ViewPos;     // 카메라 위치
out vec3 LightColor;  // 조명 색상
flat out int UseLighting;  // int varying은 반드시 flat이어야 함

void main()
{
    gl_Position = uMVP * vec4(in_Position, 1.0);
    
    // 월드 좌표계에서의 프래그먼트 위치
    FragPos = vec3(uModel * vec4(in_Position, 1.0));
    
    // 법선 벡터를 월드 좌표계로 변환 (비균등 스케일링 고려)
    Normal = mat3(transpose(inverse(uModel))) * in_Normal;
    
    // 다른 데이터들을 fragment shader로 전달
    out_Color = in_Color;
    LightPos = uLightPos;
    ViewPos = uViewPos;
    LightColor = uLightColor;
    UseLighting = uUseLighting;
}