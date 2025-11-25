#version 410 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 wPos;
out vec3 wNormal;

void main(){
    wPos = (modelMatrix * vec4(vPos, 1.0)).xyz;
    wNormal = (modelMatrix * vec4(vNormal, 0.0)).xyz;
    gl_Position = projectionMatrix * viewMatrix * vec4(wPos, 1.0);
}