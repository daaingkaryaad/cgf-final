#version 330 core
in vec2 uv;
out vec4 fragmentColour;

uniform float skyDarkness;
uniform vec3 darkHorizon;

void main() {
    vec3 horizon = mix(vec3(0.73,0.77,0.72),darkHorizon,skyDarkness);
    vec3 zenith = mix(vec3(0.28,0.47,0.57),vec3(0.018,0.028,0.065),skyDarkness);
    float horizonHeight = mix(0.15,0.54,skyDarkness);
    float skyBlend = smoothstep(horizonHeight, 1.0, uv.y);
    skyBlend = skyBlend * skyBlend * (3.0 - 2.0 * skyBlend);

    vec3 skyColor = mix(horizon, zenith, skyBlend);
    fragmentColour = vec4(skyColor, 1.0);
}
