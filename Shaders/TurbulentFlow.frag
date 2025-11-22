#version 410 core

uniform sampler2D myTexture;

uniform vec2 turbulentFlowScale;
uniform float turbulentFlowIntensity;

in vec2 fUV;

vec3 darkenColor(vec3 C, float d){
    return C - (C - C * C) * (d - 1.0);
}

float fade(float t) {
	return ((6.0 * t - 15.0) * t + 10.0) * t * t * t;
}

float interpolate2D(float value00, float value01, float value10, float value11, vec2 point){
    float u = fade(point.x);
    float v = fade(point.y);

    float interpolateX0 = mix(value00, value10, u);
    float interpolateX1 = mix(value01, value11, u);
    return mix(interpolateX0, interpolateX1, v);
}

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

vec2 getGradient(vec2 gridPoint){
    float r1 = random(gridPoint);
    float r2 = random(gridPoint + vec2(0.5, 0.5));
    return vec2(r1, r2);
}

float perlinNoise(vec2 scale, vec2 offset){
    vec2 myTextureSize = textureSize(myTexture, 0);

    vec2 samplePoint = (fUV * myTextureSize + offset) / scale;
    vec2 grid00 = floor(samplePoint);
    vec2 grid11 = grid00 + vec2(1.0);
    vec2 grid10 = vec2(grid11.x, grid00.y);
    vec2 grid01 = vec2(grid00.x, grid11.y);

    vec2 grad00 = getGradient(grid00);
    vec2 grad11 = getGradient(grid11);
    vec2 grad10 = getGradient(grid10);
    vec2 grad01 = getGradient(grid01);

    vec2 toPoint00 = samplePoint - grid00;
    vec2 toPoint11 = samplePoint - grid11;
    vec2 toPoint10 = samplePoint - grid10;
    vec2 toPoint01 = samplePoint - grid01;

    float value00 = dot(grad00, toPoint00);
    float value11 = dot(grad11, toPoint11);
    float value10 = dot(grad10, toPoint10);
    float value01 = dot(grad01, toPoint01);

    return interpolate2D(value00, value01, value10, value11, samplePoint - grid00);
}

vec4 getTurbulentFlowColor(){
    vec2 perlinNoiseScale = turbulentFlowScale;
    float perlinNoiseIntensity = turbulentFlowIntensity;
    float noise = 0.0;
    for(int i = 0; i<10; i++){
        noise += perlinNoiseIntensity * perlinNoise(perlinNoiseScale, vec2(0.0));
        perlinNoiseIntensity *= 0.7f;
        perlinNoiseScale *= 0.7f;
    }

    vec3 textureColor = texture(myTexture, fUV).xyz;
    vec3 darkenedColor = darkenColor(textureColor, 1.0 + noise);
    return vec4(darkenedColor, 1.0);
}

void main(){
    gl_FragColor = getTurbulentFlowColor();
}