varying vec3 eyedir;

void main() {
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    //gl_Position.z = 5.0; // Always places the skybox at the far end of the frustum
    eyedir = normalize(-gl_Vertex.xyz);
}
