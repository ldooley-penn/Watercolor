#version 410 core

uniform sampler2D myTexture;
uniform sampler2D gradientTexture;
uniform vec2 wobbleMagnitude;
uniform vec2 gradientOffset;
uniform vec2 wobbleTextureScale;

in vec2 fUV;

vec4 getWobbledColor(){
    vec2 sampleTextureSize = vec2(textureSize(myTexture, 0));
    vec2 gradientTextureSize = vec2(textureSize(gradientTexture, 0));
    float gradientTextureAspectRatio = gradientTextureSize.x/gradientTextureSize.y;

    vec2 gradientUV = vec2(fUV.x, fUV.y * gradientTextureSize.x / sampleTextureSize.x) / wobbleTextureScale;

    vec2 gradient = texture(gradientTexture, gradientUV).xy + gradientOffset;

    vec2 sampleUV = fUV + wobbleMagnitude * gradient / sampleTextureSize;

    return texture(myTexture, sampleUV);
}

void main(){
    gl_FragColor = getWobbledColor();
}