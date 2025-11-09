#version 410 core

uniform sampler2D myTexture;

in vec2 fUV;

void main(){
    gl_FragColor = texture(myTexture, fUV);
}