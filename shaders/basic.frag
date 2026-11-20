#version 330 core
in vec3 worldPosition;
in vec3 worldNormal;
in vec2 uv;
in vec4 sunPosition;
out vec4 fragmentColour;

uniform vec3 baseColour;
uniform vec3 eyePosition;
uniform vec3 sunDirection;
uniform vec3 padLightPosition;
uniform int padLightOn;
uniform int emissive;
uniform float specularStrength;
uniform float textureRepeat;
uniform sampler2D surfaceTexture;
uniform sampler2D sunDepth;
uniform float skyDarkness;
uniform vec3 darkHorizon;

vec3 phong(vec3 n, vec3 v, vec3 l, vec3 albedo, vec3 lightColour) {
    float diffuse = max(dot(n,l),0.0);
    float specular = diffuse > 0.0 ? pow(max(dot(reflect(-l,n),v),0.0),48.0) : 0.0;
    return lightColour * (albedo*diffuse + vec3(specularStrength*specular));
}

float sunShadow(vec3 normal, vec3 lightDirection) {
    vec3 projected=sunPosition.xyz/sunPosition.w*0.5+0.5;
    if (projected.z<=0.0 || projected.z>=1.0 ||
        projected.x<=0.0 || projected.x>=1.0 || projected.y<=0.0 || projected.y>=1.0) return 0.0;
    float bias=max(0.00015,0.00065*(1.0-max(dot(normal,lightDirection),0.0)));
    vec2 texel=1.0/vec2(textureSize(sunDepth,0));
    float shadow=0.0;
    for (int x=-1;x<=1;++x) for (int y=-1;y<=1;++y) {
        float depth=texture(sunDepth,projected.xy+vec2(x,y)*texel).r;
        shadow+=projected.z-bias>depth ? 1.0 : 0.0;
    }
    vec2 edge=min(projected.xy,1.0-projected.xy);
    return shadow/9.0*smoothstep(0.0,0.06,min(edge.x,edge.y));
}

void main() {
    vec3 albedo = baseColour;
    if (textureRepeat > 0.0)
        albedo *= texture(surfaceTexture,uv*textureRepeat).rgb;
    vec3 n = normalize(worldNormal);
    vec3 v = normalize(eyePosition-worldPosition);
    vec3 colour = 0.29*albedo;
    vec3 sun=normalize(sunDirection);
    colour += (1.0-sunShadow(n,sun))*phong(n,v,sun,albedo,vec3(0.78,0.73,0.60));
    if (padLightOn != 0) {
        vec3 delta = padLightPosition-worldPosition;
        float distanceToLight = length(delta);
        float attenuation = 1.0/(1.0+0.035*distanceToLight+0.009*distanceToLight*distanceToLight);
        colour += attenuation*phong(n,v,normalize(delta),albedo,vec3(1.35,1.11,0.66));
    }
    if (emissive != 0) colour = baseColour;
    float distanceToEye = length(eyePosition-worldPosition);
    float dayFog = 1.0-exp(-0.0016*distanceToEye);
    vec3 daylight = mix(colour,vec3(0.69,0.73,0.68),dayFog);
    vec3 highAltitude = mix(colour,darkHorizon,smoothstep(30.0,180.0,distanceToEye));
    colour = mix(daylight,highAltitude,skyDarkness);
    fragmentColour = vec4(clamp(colour,0.0,1.0),1.0);
}
