varying vec3 normal, eyeDir;
varying vec3 lightDir;
varying vec3 cubeReflect; // in view space

mat3 Reduce(mat4 m) {
	mat3 result;
	
	result[0][0] = m[0][0]; 
	result[0][1] = m[0][1]; 
	result[0][2] = m[0][2]; 

	result[1][0] = m[1][0]; 
	result[1][1] = m[1][1]; 
	result[1][2] = m[1][2]; 
	
	result[2][0] = m[2][0]; 
	result[2][1] = m[2][1]; 
	result[2][2] = m[2][2]; 
	
	return result;
}

void main() {
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    // find view space position.
	eyeDir = -normalize((gl_ModelViewMatrix * gl_Vertex).xyz);
	
	// find view space normal.
    normal = normalize(gl_NormalMatrix * gl_Normal); 
		
	// calculate the reflection vector in view space.
    lightDir = cubeReflect = reflect(eyeDir, normal);

    // Return it to world space (@TODO need actual inverse worldspace matrix here)
    cubeReflect = cubeReflect * gl_NormalMatrix; // Reduce(gl_ModelViewMatrixInverse) * cubeReflect;

    //cubeReflect = reflect(normalize(gl_Vertex.xyz), gl_Normal);
}
