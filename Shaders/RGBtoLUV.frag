#version 410 core

uniform sampler2D myTexture;

in vec2 fUV;

const vec3 XYZwhite = vec3(95.047, 100.0, 108.883);
const float e = 0.008856;
const float k = 903.3;

vec3 RGBtoXYZ(vec3 RGB){
    mat3 m = mat3(
        0.412453, 0.212671, 0.019334,
        0.357580, 0.715160, 0.119193,
        0.180423, 0.072169, 0.950227
    );

    return m * RGB;
}

vec2 XYZtoUVp(vec3 XYZ){
    float denominator = XYZ.x + 15.0 * XYZ.y + 3.0 * XYZ.z;
    if(denominator == 0.0){
        return vec2(0.0, 0.0);
    }

    vec2 numerator = vec2(4.0 * XYZ.x, 9.0 * XYZ.y);

    return numerator/denominator;
}

vec3 XYZtoLUV(vec3 XYZ){
    float yr = XYZ.y / XYZwhite.y;
    vec2 UVp = XYZtoUVp(XYZ);
    vec2 UVpr = XYZtoUVp(XYZwhite);

    float L = 0;
    if(yr > e){
        L = 116.0 * pow(yr, 1.0/3.0) - 16.0;
    } else{
        L = k * yr;
    }

    vec2 UV = 13.0 * L * (UVp - UVpr);

    return vec3(L, UV.x, UV.y);
}

vec3 RGBtoLUV(vec3 RGB){
    return XYZtoLUV(RGBtoXYZ(RGB));
}

void main(){
    vec4 rgba = texture(myTexture, fUV);
    vec3 rgb = rgba.xyz;
    float a = rgba.w;
    vec3 luv = RGBtoLUV(rgb);
    gl_FragColor = vec4(luv,a);
}