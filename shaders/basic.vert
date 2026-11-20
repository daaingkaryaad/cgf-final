#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 sunViewProjection;
out vec3 worldPosition;
out vec3 worldNormal;
out vec2 uv;
out vec4 sunPosition;

void main() {
    vec4 world = model * vec4(position,1.0);
    worldPosition = world.xyz;
    sunPosition = sunViewProjection * world;
    worldNormal = mat3(transpose(inverse(model))) * normal;
    uv = texcoord;
    gl_Position = projection * view * world;
}
