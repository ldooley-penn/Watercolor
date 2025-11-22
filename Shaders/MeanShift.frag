#version 410 core

uniform sampler2D myTexture;

uniform vec2 myTextureSize;

uniform int spatialRadius;
uniform float colorRadius;
uniform int iterationCount;

in vec2 fUV;

struct CenterOfMass
{
    vec2 UV;
    vec3 Color;
};

CenterOfMass GetCenterOfMass(vec2 pixelSize, CenterOfMass searchCenter){
    CenterOfMass result = searchCenter;

    vec2 UVSum = vec2(0.0);
    vec3 ColorSum = vec3(0.0);
    float weight = 0.0;

    for(int dx = -spatialRadius; dx <= spatialRadius; dx++){
        for(int dy = -spatialRadius; dy <= spatialRadius; dy++){
            vec2 UVNeighbor = searchCenter.UV + pixelSize * vec2(float(dx), float(dy));
            float pixelDistance = length(vec2(float(dx), float(dy)));
            if(UVNeighbor.x >= 0.0 && UVNeighbor.x <= 1.0 && UVNeighbor.y >= 0.0 && UVNeighbor.y <= 1.0 && pixelDistance <= spatialRadius){
                vec3 neighborColor = texture(myTexture, UVNeighbor).xyz;
                if(length(neighborColor - searchCenter.Color) < colorRadius){
                    UVSum += UVNeighbor;
                    ColorSum += neighborColor;
                    weight += 1.0;
                }
            }
        }
    }

    if(weight == 0.0){
        return result;
    }

    result.UV = UVSum / weight;
    result.Color = ColorSum / weight;

    return result;
}

vec3 MeanShift(){
    vec2 pixelSize = vec2(1.0) / myTextureSize;

    CenterOfMass centerOfMass;
    centerOfMass.UV = fUV;
    centerOfMass.Color = texture(myTexture, fUV).xyz;

    for(int i = 0; i < iterationCount; i++){
        centerOfMass = GetCenterOfMass(pixelSize, centerOfMass);
    }

    return centerOfMass.Color;
}

void main(){
    gl_FragColor = vec4(MeanShift(), 1.0);
}