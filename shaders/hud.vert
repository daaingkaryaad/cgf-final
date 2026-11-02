#version 330 core
layout(location=0) in vec2 position;
layout(location=1) in vec4 colour;
uniform vec3 canvasSize;
out vec4 vertexColour;
void main() {
    gl_Position=vec4(position.x/canvasSize.x*2.0-1.0,1.0-position.y/canvasSize.y*2.0,0.0,1.0);
    vertexColour=colour;
}
