#version 330 core

in vec2 local;
in vec4 colour;
flat in float seed;
out vec4 fragmentColour;

uniform int smokePass;

float hashCell(vec2 cell) {
    return fract(sin(dot(cell,vec2(127.1,311.7)))*43758.5453);
}

float softNoise(vec2 point) {
    vec2 cell = floor(point);
    vec2 t = fract(point);
    t = t*t*(3.0-2.0*t);
    float a = mix(hashCell(cell),hashCell(cell+vec2(1,0)),t.x);
    float b = mix(hashCell(cell+vec2(0,1)),hashCell(cell+vec2(1,1)),t.x);
    return mix(a,b,t.y);
}

void main() {
    float distanceFromCenter = length(local);
    float edge = 1.0-smoothstep(0.76,1.0,distanceFromCenter);
    float density;
    if (smokePass!=0) {
        float cloud = softNoise(local*3.1+vec2(seed*21.0,seed*13.0));
        float outline = distanceFromCenter+(cloud-0.5)*0.30;
        density = (1.0-smoothstep(0.22,1.0,outline))*(0.56+0.44*cloud);
    } else {
        density = exp(-3.2*distanceFromCenter*distanceFromCenter);
    }
    fragmentColour = vec4(colour.rgb,colour.a*edge*density);
}
