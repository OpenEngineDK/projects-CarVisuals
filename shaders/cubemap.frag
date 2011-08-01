uniform samplerCube environment;

varying vec3 normal, eyeDir;
varying vec3 lightDir;
varying vec3 cubeReflect; // in view space

const vec3 grayscale = vec3(0.2989, 0.5870, 0.1140);
//const vec4 diffuseColor = vec4(1.0, 0.03, 0.0, 1.0);
//const vec4 diffuseColor = vec4(0.0, 1.0, 0.0, 1.0);
//const vec4 diffuseColor = vec4(0.3, 0.3, 1.0, 1.0);
const vec4 diffuseColor = vec4(0.643137, 0.735228, 0.800000, 1.000000);
const vec4 specularColor = vec4(0.5, 0.5, 0.5, 1.0);
const float shininess = 1.245731;

void main() {

    vec3 n = normalize(normal);
    vec3 l = normalize(lightDir);
    vec3 e = normalize(eyeDir);
    
    vec4 diffuseLight = 0.333 * (textureCube(environment, cubeReflect, 0.0) +
                                 textureCube(environment, cubeReflect, 2.0) +
                                 textureCube(environment, cubeReflect, 4.0));
    
    // Highligh intense colors
    vec4 envColor = textureCube(environment, cubeReflect, 3.0);
    float intensity = dot(grayscale, envColor.xyz);
    intensity = pow(intensity, shininess);
    vec4 specular = envColor * intensity;

    gl_FragColor = diffuseColor * diffuseLight + specularColor * specular;
}
