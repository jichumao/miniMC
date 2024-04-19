#version 150

in vec4 fs_UV;
out vec4 color;

uniform sampler2D u_Texture;
uniform int u_Time;

vec2 random2(vec2 p) {
    return fract(sin(vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)))) * 43758.5453);
}

float worleyNoise(vec2 uv) {
    vec2 uvInt = floor(uv);
    vec2 uvFract = fract(uv);

    // initialize minimum distance
    float minDist = 1.0;

    // iterate over all neighbor cells
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            vec2 neighbor = vec2(float(x), float(y));

            // set a random point inside the current neighbor cell
            vec2 point = random2(uvInt + neighbor);

            // animate the point
            point = 0.5 + 0.5 * sin(u_Time * 0.025 + 6.25 * point);

            // compute the distance and the fragment, and store the minimum distance
            vec2 diff = neighbor + point - uvFract;
            float dist = length(diff);
            minDist = min(minDist, dist);
        }
    }
    return minDist;
}

float random1(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float mySmootherStep(float a, float b, float t) {
    t = t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
    return mix(a, b, t);
}

vec2 rotate(vec2 p, float deg) {
    float rad = deg * 3.14159265 / 180.0;
    return vec2(cos(rad) * p.x - sin(rad) * p.y,
                sin(rad) * p.x + cos(rad) * p.y);
}

float noise(vec2 uv) {
    vec2 uvFract = fract(uv);
    float ll = random1(floor(uv));
    float lr = random1(floor(uv) + vec2(1,0));
    float ul = random1(floor(uv) + vec2(0,1));
    float ur = random1(floor(uv) + vec2(1,1));

    float lerpXL = mySmootherStep(ll, lr, uvFract.x);
    float lerpXU = mySmootherStep(ul, ur, uvFract.x);

    return mySmootherStep(lerpXL, lerpXU, uvFract.y);
}

float fbm(vec2 uv) {
    float amp = 0.5;
    float freq = 8.0;
    float sum = 0.0;
    for(int i = 0; i < 6; i++) {
        sum += noise(uv * freq) * amp;
        amp *= 0.5;
        freq *= 2.0;
    }
    return sum;
}

vec2 fbmNoise(vec2 uv) {
    float x = fbm(uv) * 2.0 - 1.0;
    float y = fbm(rotate(uv, 60.0)) * 2.0 - 1.0;
    return vec2(x, y);
}


void main()
{
    // add red tinge
    vec4 baseColor = texture(u_Texture, fs_UV.xy);
    baseColor.r += (1 - baseColor.r) * 0.8;

    // diminish other colors
    baseColor.g *= 0.65;
    baseColor.b *= 0.3;

    // add light distortion
    vec2 distortion = fbmNoise(fs_UV.xy * 2.0) * 1.0;
    float noise = worleyNoise(4.0 * fs_UV.xy + distortion);

    baseColor.r += noise * 0.5;
    baseColor.g += noise * 0.33;
    color = baseColor;
}
