#version 410 core

uniform sampler2D myTexture;
uniform sampler2D gradientTexture;
uniform float wobbleMagnitude;

in vec2 fUV;

vec4 getWobbledColor(){
    vec2 pixelSize = vec2(1.0) / vec2(textureSize(myTexture, 0));
    vec2 gradientTextureSize = vec2(textureSize(gradientTexture, 0));
    vec2 currentPixel = vec2(gl_FragCoord.x, gl_FragCoord.y);

    vec4 gradient = texture(gradientTexture, currentPixel/gradientTextureSize);
    float Gx = gradient.x;
    float Gy = gradient.y;

    return texture(myTexture, fUV + wobbleMagnitude * vec2(Gx, Gy));
}

void main(){
    gl_FragColor = getWobbledColor();
}