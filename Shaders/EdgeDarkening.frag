#version 410 core

uniform sampler2D myTexture;

uniform float edgeDarkeningMagnitude;

in vec2 fUV;

vec3 darkenColor(vec3 C, float d){
    return C - (C - C * C) * (d - 1.0);
}

float computeGradient(){
    vec2 myTextureSize = vec2(textureSize(myTexture, 0));
    vec2 pixelSize = vec2(1.0) / myTextureSize;
    vec3 gradient = (abs(texture(myTexture, fUV + vec2(pixelSize.x, 0.0)) - texture(myTexture, fUV - vec2(pixelSize.x, 0.0))) + abs(texture(myTexture, fUV + vec2(0.0, pixelSize.y)) - texture(myTexture, fUV - vec2(0.0, pixelSize.y)))).xyz;
    return (gradient.x + gradient.y + gradient.z)/3.0;
}

vec4 getEdgeDarkenedColor(){
    float gradient = computeGradient();
    vec3 textureColor = texture(myTexture, fUV).xyz;
    vec3 darkenedColor = darkenColor(textureColor, 1.0 + edgeDarkeningMagnitude * gradient);
    return vec4(darkenedColor, 1.0);
}

void main(){
    gl_FragColor = getEdgeDarkenedColor();
}