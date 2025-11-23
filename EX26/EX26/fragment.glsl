#version 330 core

//--- out_Color: 버텍스 세이더에서 입력받는 색상 값
//--- FragColor: 출력할 색상의 값으로 프레임 버퍼로 전달 됨.

in vec3 out_Color; //--- 버텍스 세이더에게서 전달 받음
in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;
in vec3 ViewPos;
in vec3 LightColor;
flat in int UseLighting;  // int varying은 반드시 flat이어야 함

out vec4 FragColor; //--- 색상 출력


void main()
{
    if (UseLighting == 0) {  // int 비교로 변경
        // 조명을 사용하지 않는 경우 기본 색상만 출력
        FragColor = vec4(out_Color, 1.0);
        return;
    }
    
    // Phong 조명 모델 구현
    vec3 color = out_Color;
    
    // 1. Ambient lighting (환경광)
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * LightColor;
    
    // 2. Diffuse lighting (확산광)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * LightColor;
    
    // 3. Specular lighting (정반사광)
    float specularStrength = 0.5;
    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * LightColor;
    
    // 최종 색상 계산
    vec3 result = (ambient + diffuse + specular) * color;
    FragColor = vec4(result, 1.0);
}