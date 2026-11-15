#version 330 core

layout(location=0) in vec3 center;
layout(location=1) in float radius;
layout(location=2) in vec4 tint;
layout(location=3) in vec2 style;

uniform mat4 view;
uniform mat4 projection;
uniform int smokePass;

out vec2 local;
out vec4 colour;
flat out float seed;

const vec2 corners[6] = vec2[6](vec2(-1,-1),vec2(1,-1),vec2(1,1),
                                vec2(-1,-1),vec2(1,1),vec2(-1,1));

void main() {
    local = corners[gl_VertexID];
    float c = cos(style.x), s = sin(style.x);
    vec2 offset = mat2(c,s,-s,c)*local*radius;
    if (smokePass==0) offset.y *= 1.45;
    vec4 eye = view*vec4(center,1.0);
    eye.xy += offset;
    gl_Position = projection*eye;
    colour = tint;
    seed = style.y;
}
