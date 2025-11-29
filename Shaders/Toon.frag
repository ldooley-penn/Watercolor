#version 410 core

in vec3 wPos;
in vec3 wNormal;

void main(){
    gl_FragColor = vec4(abs(wNormal), 1.0);
}