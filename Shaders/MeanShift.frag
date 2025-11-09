#version 410 core

uniform sampler2D myTexture;

uniform vec2 myTextureSize;

in vec2 fUV;

const int kernelRadius = 5;

vec2 GetCenterOfMass(vec2 pixelSize, vec2 UV0){
    float sum = 0.0;
    vec2 comUV = vec2(0.0);
    for(int dx = -kernelRadius; dx <= kernelRadius; dx++){
        for(int dy = -kernelRadius; dy <= kernelRadius; dy++){
            vec2 uv = UV0 + pixelSize * vec2(float(dx), float(dy));
            if(uv.x >= 0 && uv.x <= 1 && uv.y >= 0 && uv.y <= 1){
                vec3 LUV = texture(myTexture, uv).xyz;
                float L = LUV.x;
                sum += L;
                comUV += L * uv;
            }
        }
    }

    if(sum == 0.0){
        return UV0;
    }

    return comUV / sum;
}

vec3 GetMeanValue(vec2 pixelSize, vec2 UV){
    vec3 sum = vec3(0.0);
    float weight = 0.0;
    for(int dx = -kernelRadius; dx <= kernelRadius; dx++){
        for(int dy = -kernelRadius; dy <= kernelRadius; dy++){
            vec2 uv = UV + pixelSize * vec2(float(dx), float(dy));
            if(uv.x >= 0 && uv.x <= 1 && uv.y >= 0 && uv.y <= 1){
                sum += texture(myTexture, uv).xyz;
                weight += 1.0;
            }
        }
    }

    if(weight == 0.0){
        return vec3(0.0);
    }

    return sum / weight;
}

vec3 MeanShift(){
    vec2 pixelSize = vec2(1.0) / myTextureSize;

    vec2 meanUV = fUV;
    for(int i = 0; i < 10; i++){
        meanUV = GetCenterOfMass(pixelSize, meanUV);
    }

    return GetMeanValue(pixelSize, meanUV);
}

void main(){
    gl_FragColor = vec4(MeanShift(), 1.0);
}