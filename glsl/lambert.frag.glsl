#version 150

uniform sampler2D u_Texture; // The texture to be read from by this shader

uniform sampler2D u_ShadowMap;

uniform int u_Time;

uniform vec4 u_Color; // The color with which to render this instance of geometry.

uniform vec4 u_SunDirection;

// These are the interpolated values out of the rasterizer, so you can't know
// their specific values without knowing the vertices that contributed to them
in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_Col;
in vec4 fs_UV;
in vec4 fs_UnhomShadowMapCoord;
in vec4 fs_ViewSpacePos;

// Output fragment color.
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

    float mixLoBack = mySmoothStep(llb, lrb, pFract.x);
    float mixHiBack = mySmoothStep(ulb, urb, pFract.x);
    float mixLoFront = mySmoothStep(llf, lrf, pFract.x);
    float mixHiFront = mySmoothStep(ulf, urf, pFract.x);

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

float computeShadowFactor(float bias) {
    vec3 homShadowMapCoord = fs_UnhomShadowMapCoord.xyz / fs_UnhomShadowMapCoord.w;

    float shadowMapDepth = texture(u_ShadowMap, homShadowMapCoord.xy).x;

    return shadowMapDepth < homShadowMapCoord.z - bias ? 0.7 : 1.;
}

vec3 computeDistanceFog(vec3 color) {
    return color;

    // float radius = 100.f;
    // float clampedZ = clamp(fs_Pos.z, 0.0f, radius);
    // return mix(color, vec3(1.0f, 1.0f, 1.0f), clampedZ/radius);
    // return mix(vec3(1.0f, 1.0f, 1.0f), color, clampedZ/radius);
    // float zNear = 0.f;
    // float zFar = 1000.f;
    // float linearZ = zNear * zFar / (zFar + gl_FragCoord.z * (zNear - zFar));
    // return mix(color, vec3(1.0f, 1.0f, 1.0f), linearZ);

    // float fog_maxdist = 200.0;
    // float fog_mindist = 0.1;
    // float dist = length(fs_ViewSpacePos.xyz);
    // float fog_factor = (fog_maxdist - dist) / (fog_maxdist - fog_mindist);
    // fog_factor = clamp(fog_factor, 0.0, 1.0);
    // // return mix(color, vec3(1.0f, 1.0f, 1.0f), gl_FragCoord.z/gl_FragCoord.w);


    // float fogFactor = (80 - dist)/(80 - 20);
    // fogFactor = clamp( fogFactor, 0.0, 1.0 );

    // //if you inverse color in glsl mix function you have to
    // //put 1.0 - fogFactor
    // return mix(vec3(1.0f, 1.0f, 1.0f), color, fogFactor);
}

void main() {
    vec4 textureColor;
    vec4 diffuseColor;
    float time = 0;

    // Sampler2D
    // no animation
    if(fs_UV.z == 0){
        textureColor = texture(u_Texture, fs_UV.xy);
        // set non-water block to 100% no-opaque
        textureColor.a = 2.0;
        diffuseColor = textureColor;
        // Material base color (before shading)
        //vec4 diffuseColor = fs_Col;
        diffuseColor = diffuseColor * (0.5 * fbm(fs_Pos.xyz) + 0.5);
    } else {
        // with animation (LAVA and WATER)
        time = u_Time * 0.025;
        vec2 animatedUV = fs_UV.xy;

        textureColor.a = 0.8;
        animatedUV.x *= 16.0;
        animatedUV.x -= 13.0;
        animatedUV.x = fract(animatedUV.x + time);
        animatedUV.x = (animatedUV.x + 13.0) / 16.0;

        textureColor = texture(u_Texture, animatedUV);
        diffuseColor = textureColor;
        diffuseColor = diffuseColor * (0.5 * fbm(fs_Pos.xyz) + 0.5);
    }

    float diffuseTerm = dot(normalize(fs_Nor), normalize(u_SunDirection));
    diffuseTerm = clamp(diffuseTerm, 0, 1);

    float ambientTerm = 0.2;

    float lightIntensity = diffuseTerm + ambientTerm;   //Add a small float value to the color multiplier
                                                        //to simulate ambient lighting. This ensures that faces that are not
                                                        //lit by our point light are not completely black.

    // float bias = max(0.05 * (1.0 - diffuseTerm), 0.005);
    float bias = 0.0004*tan(acos(diffuseTerm));
    float shadowFactor = computeShadowFactor(bias); 

    out_Col = vec4(computeDistanceFog(diffuseColor.rgb * lightIntensity * shadowFactor), diffuseColor.a);

    // out_Col = vec4(fs_ViewSpacePos.xyz, 1.0f);
}
