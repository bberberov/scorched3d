uniform sampler2DShadow shadow;
uniform sampler2D splat1map;
uniform sampler2D splat2map;
uniform sampler2D splattex;

varying vec3 normal,lightDir;

void main()
{
    // Look up the diffuse color and shadow states for each light source.
    float s0 = shadow2DProj(shadow, gl_TexCoord[3]).r;

	// compute the dot product between normal and normalized lightdir
	float NdotL = max(dot(normal,normalize(lightDir)),0.0) * s0;

	// Light color
	vec4 ambient = gl_LightSource[1].ambient; //* gl_FrontMaterial.ambient;
	vec4 diffuse = gl_LightSource[1].diffuse; //* gl_FrontMaterial.diffuse;
	vec4 lightcolor = (diffuse * NdotL + ambient);

    // Compute the final pixel color
	vec4 splat1 = texture2D(splat1map, gl_TexCoord[0].xy);
	vec4 splat2 = texture2D(splat2map, gl_TexCoord[0].xy);
	
	vec2 splatcoord = vec2(fract(gl_TexCoord[2].x) * 0.25, fract(gl_TexCoord[2].y) * 0.25);
	
	vec4 groundColor = vec4(0, 0, 0, 0);
	if (splat1.r > 0.0) groundColor += texture2D(splattex, vec2(splatcoord.x + 0.0412, splatcoord.y + 0.0412)) * splat1.r;
	if (splat1.g > 0.0) groundColor += texture2D(splattex, vec2(splatcoord.x + 0.3745, splatcoord.y + 0.0412)) * splat1.g;
	if (splat1.b > 0.0) groundColor += texture2D(splattex, vec2(splatcoord.x + 0.7080, splatcoord.y + 0.0412)) * splat1.b;
	if (splat1.a > 0.0) groundColor += texture2D(splattex, vec2(splatcoord.x + 0.0412, splatcoord.y + 0.3745)) * splat1.a;
	if (splat2.r > 0.0) groundColor += texture2D(splattex, vec2(splatcoord.x + 0.3745, splatcoord.y + 0.3745)) * splat2.r;
	if (splat2.g > 0.0) groundColor += texture2D(splattex, vec2(splatcoord.x + 0.7080, splatcoord.y + 0.3745)) * splat2.g;
	if (splat2.b > 0.0) groundColor += texture2D(splattex, vec2(splatcoord.x + 0.0412, splatcoord.y + 0.7080)) * splat2.b;
	
	//groundColor = texture2D(splattex, gl_TexCoord[0].xy);
	
	vec3 finalColor = groundColor.rgb * lightcolor.rgb;
	gl_FragColor = vec4(finalColor, 1.0);
}
