#version 330 core
in vec2 uv;
out vec4 fragmentColour;
void main() {
    vec3 horizon=vec3(0.73,0.77,0.72);
    vec3 zenith=vec3(0.28,0.47,0.57);
    fragmentColour=vec4(mix(horizon,zenith,smoothstep(0.15,1.0,uv.y)),1.0);
}
