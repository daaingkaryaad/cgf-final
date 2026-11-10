#version 330 core
in vec3 worldPosition;
in vec3 worldNormal;
in vec2 uv;
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
uniform float skyDarkness;
uniform vec3 darkHorizon;

vec3 phong(vec3 n, vec3 v, vec3 l, vec3 albedo, vec3 lightColour) {
    float diffuse = max(dot(n,l),0.0);
    float specular = diffuse > 0.0 ? pow(max(dot(reflect(-l,n),v),0.0),48.0) : 0.0;
    return lightColour * (albedo*diffuse + vec3(specularStrength*specular));
}

void main() {
    vec3 albedo = baseColour;
    if (textureRepeat > 0.0)
        albedo *= texture(surfaceTexture,uv*textureRepeat).rgb;
    vec3 n = normalize(worldNormal);
    vec3 v = normalize(eyePosition-worldPosition);
    vec3 colour = 0.29*albedo;
    colour += phong(n,v,normalize(sunDirection),albedo,vec3(0.78,0.73,0.60));
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
    // blend distant terrain into the dark horizon, keeping the nearby rocket lit.
    vec3 highAltitude = mix(colour,darkHorizon,smoothstep(30.0,180.0,distanceToEye));
    colour = mix(daylight,highAltitude,skyDarkness);
    fragmentColour = vec4(clamp(colour,0.0,1.0),1.0);
}
