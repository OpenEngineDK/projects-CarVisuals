uniform samplerCube skybox;

varying vec3 eyedir;

void main() {
    gl_FragColor = textureCube(skybox, eyedir);
}
