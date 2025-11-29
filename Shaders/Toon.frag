#version 410 core

in vec3 wPos;
in vec3 wNormal;

const vec3 lightDir = vec3(1.0, 1.0, -1.0);

void main(){
    float intensity = clamp(dot(wNormal, lightDir), 0.0, 1.0);
    gl_FragColor = vec4(vec3(intensity), 1.0);
}