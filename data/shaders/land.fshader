#ifdef USE_SHADOWS
uniform sampler2DShadow shadow;	// shadow map DEPTH_COMPONENT
#endif
uniform sampler2D mainmap;
uniform sampler2D detailmap;
uniform sampler2D arenamap;
uniform float showarena;

varying vec3 normal,lightDir;

void main()
{
	// add exp fog
	float fog = exp2(-gl_Fog.density * gl_FogFragCoord * 3.0 * 1.442695);
	float fog_factor = clamp(fog, 0.0, 1.0);

    // Look up the diffuse color and shadow states for each light source.
	float s0 = 1.0;
#ifdef USE_SHADOWS
	s0 = shadow2DProj(shadow, gl_TexCoord[2]).r;
#endif

	// compute the dot product between normal and normalized lightdir
	vec3 n = normalize(normal);
	float NdotL = max(dot(n,normalize(lightDir)),0.0) * s0;

	// Light color
	vec4 ambient = gl_LightSource[1].ambient; //* gl_FrontMaterial.ambient;
	vec4 diffuse = gl_LightSource[1].diffuse; //* gl_FrontMaterial.diffuse;
	vec4 lightcolor = (diffuse * NdotL + ambient);

    // Compute the final pixel color
	vec4 groundColor = texture2D(mainmap, gl_TexCoord[0].xy);
	vec4 detailColor = texture2D(detailmap, gl_TexCoord[1].xy);
	vec3 finalColor =
		((groundColor.rgb * 3.5) + detailColor.rgb) / 4.0 * lightcolor.rgb;
		
	vec4 arenaColor = vec4(0.0, 0.0, 0.0, 0.0);
	if (showarena != 0.0) arenaColor  = texture2D(arenamap, gl_TexCoord[0].xy);		
	if (arenaColor.a != 0.0) finalColor = mix(finalColor, arenaColor.xyz, 0.5);
			
	gl_FragColor = vec4(mix(vec3(gl_Fog.color), finalColor, fog_factor), 1.0);
}
