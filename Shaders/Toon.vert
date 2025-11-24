#version 410 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 wPos;
out vec3 wNormal;

void main(){
    wPos = (ModelMatrix * vec4(vPos, 1.0)).xyz;
    wNormal = (ModelMatrix * vec4(vNormal, 0.0)).xyz;
    gl_Position = ProjectionMatrix * ViewMatrix * vec4(wPos, 1.0);
}