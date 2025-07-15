// -*- mode: C; -*-

varying vec3 viewerdir;
varying vec3 lightdir;
varying vec3 normal;
//varying vec2 foamtexcoord; // ATI strikes again!!
varying vec3 aoftexcoord; // xy = tex coord, z = aof around land
varying vec4 reflectiontexcoord;	// x,y,z,w
varying vec4 noise_texc; // xy = 1st coord pair, za = 2nd coord pair

uniform sampler2D tex_normal;		// normal map, RGB
uniform sampler2D tex_reflection;	// reflection, RGB
uniform sampler2D tex_foamamount;	// r = aof open sea, g = aof round land, b = actual foam tex
uniform vec3 landscape_size;
#ifdef USE_SHADOWS
uniform sampler2DShadow tex_shadow;	// shadow map DEPTH_COMPONENT
#endif
uniform float transparency;

const float water_shininess = 120.0;

void main()
{
	// add exp fog
	float fog = exp2(-gl_Fog.density * gl_FogFragCoord * 3.0 * 1.442695);
	float fog_factor = clamp(fog, 0.0, 1.0);

	// compute shaodw amount
	float s0 = 1.0;
#ifdef USE_SHADOWS
	s0 = shadow2DProj(tex_shadow, gl_TexCoord[2]).r;
#endif

	// compute normal vector
	vec3 N0 = vec3(texture2D(tex_normal, noise_texc.xy) * 2.0 - 1.0) * fog_factor;
	vec3 N1 = vec3(texture2D(tex_normal, noise_texc.zw) * 2.0 - 1.0) * fog_factor;
	vec3 N = normalize(normal+N0+N1);

	// compute direction to viewer
	vec3 E = normalize(viewerdir);

	// compute direction to light source
	vec3 L = normalize(lightdir);

	// compute reflected light vector (N must be normalized)
	vec3 R = reflect(-L, N);

	// compute fresnel term. we need to clamp because the product could be negative
	// when the angle between face normal and viewer is nearly perpendicular.
	// using abs gives ugly ring-like effects, but direct clamp leads to large areas with maximum
	// reflectivity. We need to tweak E in the vertex shader, so that this term can't be negative,
	// as work-around... fixme.
	// IDEA: if we would use a texture map as lookup for the fresnel term, we can avoid this:
	// - texture lookup could be faster!
	// - use a texture with clamping, but allow coordinate < 0 (by scaling, then clamp)
	// - fresnel term is 1/((x+1)^8), so fresnel(0)=1, BUT: fresnel(x) for x < 0,
	//   is erratic/noisy (smoothed) between 0 and 1, simulating rough water.
	//   That way the rings disappear,
	//   and are replaced by additional surface detail.
	// new result: without the abs() around dot(), it doesn't look worse. so keep out the abs()
	// this also means the case that E*N < 0 is not so problematic. So we don't need the lookup
	// texture.
	float fresnel = clamp(dot(E, N), 0.0, 1.0) + 1.0;
	// approximation for fresnel term is 1/((x+1)^8)
#if 1
	// using pow() seems a little bit faster (Geforce5700)
	fresnel = pow(fresnel, -8.0) * 0.8;
#else
	fresnel = fresnel * fresnel;	// ^2
	fresnel = fresnel * fresnel;	// ^4
	fresnel = fresnel * fresnel;	// ^8
	fresnel = (1.0/fresnel) * 0.8;	// never use full reflectivity, at most 0.8
	// that multiplication greatly increases the realism of the appearance!
#endif

	// possible optimization: make a 2d lookup texture, one dimension with fresnel term,
	// second dimension with specular term. Spares two pow() instructions. But we already used
	// all four texture units. Doubtful that performance would be higher...

	// compute specular light brightness (phong shading)
	vec3 specular_color = vec3(gl_LightSource[0].diffuse)
		* pow(clamp(dot(R, E), 0.0, 1.0), water_shininess);

	// compute refraction color
	float dl = max(dot(L, N), 0.0);
	vec3 refractioncol = vec3(gl_Color) * dl * min(1.0, s0 + 0.9);

	// mix reflection and refraction (upwelling) color, and add specular color
	vec3 water_color = mix(refractioncol, vec3(texture2DProj(tex_reflection, reflectiontexcoord)),
			       fresnel* min(1.0, s0 + 0.8)) + specular_color * s0;
	
	// foam due to breakers in the sea
	float aof = 
		texture2D(tex_foamamount, aoftexcoord.xy / 256.0).x * fog_factor;
		
	// foam due to breakers at the shore
	float landfoam = 0.0;
	float trans = transparency;//1.0;
	float aoflandxper = aoftexcoord.x / landscape_size.x;
	float aoflandyper = aoftexcoord.y / landscape_size.y;
	float aoflandxclamp = clamp(aoflandxper, 0.0, 1.0);
	float aoflandyclamp = clamp(aoflandyper, 0.0, 1.0);
	if (aoflandxper == aoflandxclamp &&
		aoflandyper == aoflandyclamp &&
		aoftexcoord.z > 0.0)
	{
		float aofland = texture2D(tex_foamamount, vec2(aoflandxper, aoflandyper)).y;
		trans = min(1.0, 0.4 + aofland * 1.8) * transparency;
		//if (trans < 0.25) 
		//{
		//	landfoam = (0.25 - trans) * 20;
		//	trans = min(1.0, (0.25 - trans) * 20 + 0.2);
		//}
	}
	
	// calc total foam from both of these	
	float foam_amount = max(min(aof + landfoam, 1.0) - ((1.0 - s0) * 0.5), 0.0) * 
		texture2D(tex_foamamount, vec2(aoflandxper, aoflandyper) * 25.0).z;

	// Find color taking water and foam
	vec3 final_color = mix(water_color, vec3(gl_LightSource[0].diffuse), foam_amount);

	// output color is a mix between fog and final color
	gl_FragColor = vec4(mix(vec3(gl_Fog.color), final_color, fog_factor), trans);
}
