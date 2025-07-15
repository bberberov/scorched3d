varying vec3 normal,lightDir;

void main()
{	
	vec4 vertex = gl_ModelViewMatrix * gl_Vertex;
	normal = normalize(gl_NormalMatrix * gl_Normal);
	
	/* compute the light's direction */
	vec4 ecPos = gl_ModelViewMatrix * gl_Vertex;
	vec3 aux = vec3(gl_LightSource[1].position-ecPos);
	lightDir = normalize(aux);
		
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_TexCoord[1] = gl_MultiTexCoord2;
    gl_TexCoord[2] = gl_TextureMatrix[2] * vertex;

    gl_Position = ftransform();
    gl_FogFragCoord = max(gl_Position.z - 350.0, 0.0);
} 
