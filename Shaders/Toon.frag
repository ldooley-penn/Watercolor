#version 410 core

in vec3 wPos;
in vec3 wNormal;

const vec3 lightDir = vec3(1.0, -1.0, -1.0);
const float e = 2.718;
const float layers = 6.0;

vec3 darkenColor(vec3 C, float d){
    return C - (C - C * C) * (d - 1.0);
}

void main(){
    float intensity = clamp(dot(wNormal, -lightDir), 0.0, 1.0);

    float discretizedIntensity = ceil(layers * pow(e, intensity) / e) / layers;

    float darkenFactor = (1.0 - discretizedIntensity) * 3.0;

    vec3 baseColor = vec3(1.0, 0.5, 0.5);

    vec3 toonColor = darkenColor(baseColor, darkenFactor);

    gl_FragColor = vec4(toonColor, 1.0);
}