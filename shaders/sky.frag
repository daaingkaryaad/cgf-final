#version 330 core
in vec2 uv;
out vec4 fragmentColour;

uniform float skyDarkness;
uniform vec3 darkHorizon;

float starHash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float starTwinkle(vec2 cell) {
    float phase = starHash(cell + vec2(5.0, 9.0)) * 6.2831853;
    return 0.85 + 0.15 * sin(phase);
}

void main() {
    vec3 horizon = mix(vec3(0.73,0.77,0.72),darkHorizon,skyDarkness);
    vec3 zenith = mix(vec3(0.28,0.47,0.57),vec3(0.018,0.028,0.065),skyDarkness);
    float horizonHeight = mix(0.15,0.54,skyDarkness);
    float skyBlend = smoothstep(horizonHeight, 1.0, uv.y);
    skyBlend = skyBlend * skyBlend * (3.0 - 2.0 * skyBlend);

    vec3 skyColor = mix(horizon, zenith, skyBlend);
    vec2 starGrid = uv * vec2(180.0, 100.0);
    vec2 cell = floor(starGrid);
    vec2 local = fract(starGrid);

    float randomValue = starHash(cell);
    vec2 starPosition = vec2(
    starHash(cell + 1.0),
    starHash(cell + 2.0)
    );

    float distanceToStar = length(local - starPosition);

    float star = step(0.997, randomValue);
    tar *= 1.0 - smoothstep(0.01, 0.045, distanceToStar);

    float starVisibility = smoothstep(0.35, 0.85, skyDarkness);

    float horizonFade = smoothstep(0.3, 0.75, uv.y);

    float finalStarVisibility = starVisibility * horizonFade;

    float twinkle = starTwinkle(cell);
    skyColor += vec3(0.9, 0.93, 1.0)
          * star * finalStarVisibility * twinkle;
    fragmentColour = vec4(skyColor, 1.0);
}
