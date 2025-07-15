// -*- mode: C; -*-

varying vec3 viewerdir;
varying vec3 lightdir;
varying vec3 normal;
varying vec4 reflectiontexcoord;	// x,y,z,w

uniform sampler2D tex_reflection;	// reflection, RGB
uniform sampler2DShadow tex_shadow;	// shadow map DEPTH_COMPONENT
uniform float use_shadow;

const float water_shininess = 120.0;

void main()
{
	// add exp fog
	float fog = exp2(-gl_Fog.density * gl_FogFragCoord * 3.0 * 1.442695);
	float fog_factor = clamp(fog, 0.0, 1.0);

	// compute shaodw amount
	float s0 = 1.0;
	if (use_shadow == 1.0) s0 = shadow2DProj(tex_shadow, gl_TexCoord[2]).r;
	if (s0 < 1.0) s0 = 0.0;

	vec3 N = normalize(normal);

	// compute direction to viewer
	vec3 E = normalize(viewerdir);

	// compute direction to light source
	vec3 L = normalize(lightdir);

	// compute reflected light vector (N must be normalized)
	vec3 R = reflect(-L, N);
	float fresnel = 0.5;

	// compute specular light brightness (phong shading)
	vec3 specular_color = vec3(gl_LightSource[0].diffuse)
		* pow(clamp(dot(R, E), 0.0, 1.0), water_shininess);

	// compute refraction color
	float dl = max(dot(L, N), 0.0);
	vec3 refractioncol = vec3(gl_Color) * dl * min(1.0, s0 + 0.9);

	// mix reflection and refraction (upwelling) color, and add specular color
	vec3 water_color = mix(refractioncol, vec3(texture2DProj(tex_reflection, reflectiontexcoord)),
			       fresnel* min(1.0, s0 + 0.8)) + specular_color * s0;
	
	// output color is a mix between fog and final color
	gl_FragColor = vec4(mix(vec3(gl_Fog.color), water_color, fog_factor), max(1.0 - fog_factor, 0.85));
}
