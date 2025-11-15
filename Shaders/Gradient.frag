#version 410 core

uniform sampler2D myTexture;

in vec2 fUV;

const float Gx[9] = float[](
        -1.0, 0.0, 1.0,
        -2.0, 0.0, 2.0,
        -1.0, 0.0, 1.0
    );

const float Gy[9] = float[](
        -1.0, -2.0, 1.0,
        0.0, 0.0, 0.0,
        1.0, 2.0, 1.0
    );

float ApplyKernel(float[9] kernel)
{
    vec2 pixelSize = vec2(1.0) / vec2(textureSize(myTexture, 0));
    float sum = 0.0;
    int index = 0;
    for(int dy = - 1; dy <= 1; dy++){
        for(int dx = -1; dx <= 1; dx++){
            vec2 UVNeighbor = fUV + pixelSize * vec2(float(dx), float(dy));
            sum += kernel[index] * texture(myTexture, UVNeighbor).r;
            index++;
        }
    }

    return sum;
}

void main(){
    float grad_x = ApplyKernel(Gx);
    float grad_y = ApplyKernel(Gy);
    gl_FragColor = vec4(grad_x, grad_y, 0.0, 1.0);
}