#version 410 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vUV;

void main(){
    gl_Position = vec4(vPos.x, vPos.y, vPos.z, 1.0);
}