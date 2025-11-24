#version 410 core

uniform sampler2D myTexture;
uniform sampler2D paperTexture;

uniform float turbulentFlowScale;
uniform float turbulentFlowIntensity;

uniform float gaussianNoiseScale;
uniform float gaussianNoiseIntensity;

uniform float paperGrainScale;
uniform float paperGrainIntensity;

in vec2 fUV;

const float PI = 3.1415926;

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

float perlinNoise(float scale, vec2 offset){
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

float getTurbulentFlowDarkeningFactor(){
    float perlinNoiseScale = turbulentFlowScale;
    float perlinNoiseIntensity = turbulentFlowIntensity;
    float noise = 0.0;
    for(int i = 0; i<10; i++){
        noise += perlinNoiseIntensity * perlinNoise(perlinNoiseScale, vec2(0.0));
        perlinNoiseIntensity *= 0.7f;
        perlinNoiseScale *= 0.7f;
    }

    return noise;
}

float getPaperGrainDarkeningFactor(){
    vec2 myTextureSize = vec2(textureSize(myTexture, 0));
    vec2 paperTextureSize = vec2(textureSize(paperTexture, 0));

    vec2 paperUV = vec2(fUV.x, fUV.y * paperTextureSize.x / myTextureSize.x) / paperGrainScale;
    return paperGrainIntensity * texture(paperTexture, paperUV).r;
}

float gaussianRandom(vec2 seed){
    float r1 = random(seed);
    float r2 = random(seed + vec2(0.5, 0.5));
    // Box muller transform
    return sqrt(abs(-2.0 * log(max(0.001, r1)))) * cos(2.0 * PI * r2);
}

float getGaussianDarkeningFactor(){
    vec2 myTextureSize = textureSize(myTexture, 0);

    vec2 point = fUV * myTextureSize / vec2(gaussianNoiseScale);
    vec2 p00 = floor(point);
    vec2 p11 = p00 + vec2(1.0);
    vec2 p01 = p00 + vec2(0.0, 1.0);
    vec2 p10 = p00 + vec2(1.0, 0.0);

    float g00 = gaussianRandom(p00);
    float g11 = gaussianRandom(p11);
    float g01 = gaussianRandom(p01);
    float g10 = gaussianRandom(p10);

    float gaussian = interpolate2D(g00, g01, g10, g11, point - p00);
    return gaussianNoiseIntensity * gaussian;
}

void main(){
    vec3 textureColor = texture(myTexture, fUV).xyz;

    float darkeningFactor = 1.0;
    darkeningFactor += getTurbulentFlowDarkeningFactor();
    darkeningFactor += getPaperGrainDarkeningFactor();
    darkeningFactor += getGaussianDarkeningFactor();

    vec3 darkenedColor = darkenColor(textureColor, darkeningFactor);

    gl_FragColor = vec4(darkenedColor, 1.0);
}