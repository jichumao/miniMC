#version 150

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform int u_Time;

in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_LightVec;

in vec4 fs_UV;

out vec4 out_Col;

float random1(vec3 p) {
    return fract(sin(dot(p,vec3(127.1, 311.7, 191.999)))
                 *43758.5453);
}

float mySmoothStep(float a, float b, float t) {
    t = smoothstep(0, 1, t);
    return mix(a, b, t);
}

float cubicTriMix(vec3 p) {
    vec3 pFract = fract(p);
    float llb = random1(floor(p) + vec3(0,0,0));
    float lrb = random1(floor(p) + vec3(1,0,0));
    float ulb = random1(floor(p) + vec3(0,1,0));
    float urb = random1(floor(p) + vec3(1,1,0));

    float llf = random1(floor(p) + vec3(0,0,1));
    float lrf = random1(floor(p) + vec3(1,0,1));
    float ulf = random1(floor(p) + vec3(0,1,1));
    float urf = random1(floor(p) + vec3(1,1,1));

    float mixLoBack     = mySmoothStep(llb, lrb, pFract.x);
    float mixHiBack     = mySmoothStep(ulb, urb, pFract.x);
    float mixLoFront    = mySmoothStep(llf, lrf, pFract.x);
    float mixHiFront    = mySmoothStep(ulf, urf, pFract.x);

    float mixLo = mySmoothStep(mixLoBack, mixLoFront, pFract.z);
    float mixHi = mySmoothStep(mixHiBack, mixHiFront, pFract.z);

    return mySmoothStep(mixLo, mixHi, pFract.y);
}

float fbm(vec3 p) {
    float amp = 0.5;
    float freq = 4.0;
    float sum = 0.0;
    for(int i = 0; i < 8; i++) {
        sum += cubicTriMix(p * freq) * amp;
        amp *= 0.5;
        freq *= 2.0;
    }
    return sum;
}

void main()
{
    //background texture
    vec4 texture_color = texture(u_Texture, fs_UV.xy);
    texture_color = texture_color * (0.5 * fbm(fs_Pos.xyz) + 0.5);

    vec4 water_color = vec4(0.2, 0.65, 0.95, 1.0);

    vec4 k = vec4(u_Time) * 0.025;
    k.xy = fs_UV.xy * 7.0;
    float val1 = length(0.5 - fract(k.xyw *= mat3(vec3(-2.0, -1.0, 0.0), vec3(3.0, -1.0, 1.0), vec3(1.0, -1.0, -1.0)) * 0.5));
    float val2 = length(0.5 - fract(k.xyw *= mat3(vec3(-2.0, -1.0, 0.0), vec3(3.0, -1.0, 1.0), vec3(1.0, -1.0, -1.0)) * 0.2));
    float val3 = length(0.5 - fract(k.xyw *= mat3(vec3(-2.0, -1.0, 0.0), vec3(3.0, -1.0, 1.0), vec3(1.0, -1.0, -1.0)) * 0.5));

    vec4 color = (pow(min(min(val1, val2), val3), 7.0) * 3.0) + texture_color * water_color * 1.7;
    out_Col = vec4(color.rgb, texture_color.a);
}
