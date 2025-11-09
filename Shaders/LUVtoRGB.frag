#version 410 core

uniform sampler2D myTexture;

in vec2 fUV;

const vec3 XYZwhite = vec3(95.047, 100.0, 108.883);
const float e = 0.008856;
const float k = 903.3;

vec3 XYZtoRGB(vec3 XYZ){
    mat3 m = mat3(
        0.412453, 0.212671, 0.019334,
        0.357580, 0.715160, 0.119193,
        0.180423, 0.072169, 0.950227
    );

    return inverse(m) * XYZ;
}

vec2 XYZtoUVp(vec3 XYZ){
    float denom = XYZ.x + 15.0 * XYZ.y + 3.0 * XYZ.z;
    if(denom == 0.0){
        return vec2(0.0, 0.0);
    }

    vec2 nume = vec2(4.0 * XYZ.x, 9.0 * XYZ.y);

    return nume/denom;
}

vec3 LUVtoXYZ(vec3 LUV){
    float L = LUV.x;
    float U = LUV.y;
    float V = LUV.z;

    vec2 UV0 = XYZtoUVp(XYZwhite);

    float Y = XYZwhite.y;
    if(L > k * e){
        Y *= pow((L + 16.0)/116.0, 3.0);
    }
    else{
        Y *= L / k;
    }

    float a = (1.0/3.0) * ((52.0 * L)/(U + 13.0 * L * UV0.x) - 1.0);
    float b = -5.0 * Y;
    float c = -1.0/3.0;
    float d = Y * ((39.0 * L)/(V + 13.0 * L * UV0.y) - 5.0);

    float X = (d - b)/(a - c);
    float Z = X * a + b;

    return vec3(X, Y, Z);
}

vec3 LUVtoRGB(vec3 LUV){
    return XYZtoRGB(LUVtoXYZ(LUV));
}

void main(){
    vec4 luva = texture(myTexture, fUV);
    vec3 luv = luva.xyz;
    float a = luva.w;
    vec3 rgb = LUVtoRGB(luv);
    gl_FragColor = vec4(rgb, a);
    //gl_FragColor = luva;
}