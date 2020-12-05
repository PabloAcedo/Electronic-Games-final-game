
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;
varying vec3 v_tolight;

uniform vec4 u_color;

uniform sampler2D u_texture;
uniform sampler2D u_texture2;
uniform sampler2D u_texture3;
uniform sampler2D u_texture_map;

uniform float u_time;
uniform float u_texture_tiling;
uniform vec3 u_camera_eye;

//uniform vec3 u_light_ambient = vec3(1.0f,1.0f,1.0f);
uniform vec3 u_light_color_diffuse;
uniform vec3 u_light_color_specular;
uniform vec3 u_sky_color;
uniform float u_far_plane;

void main()
{
	vec3 u_light_ambient = vec3(0.5f,0.5f,0.5f);
	vec3 N = normalize(v_normal);//normals
	vec3 L = normalize(v_tolight);//light
	vec3 V = normalize(u_camera_eye - v_world_position);
	vec3 R = normalize(reflect(-L,N));
	
	float dist2 = length(v_tolight);
	float light_attenuation = 1.0f + (0.00002f*dist2) + (0.000005f*dist2*dist2);
	//light_attenuation = 1.0f;

	//light_attenuation = 1.0f; //de momento no activo el efecto de atenuacion (cuando haya multiples luces lo hare)
	//ambient light
	vec3 Ia = u_light_ambient;
	
	//diffuse light
	float Dot_N_L = dot(N, L);
	float brightness = max(Dot_N_L,0.0);
	vec3 Id = (brightness * u_light_color_diffuse)/light_attenuation; //diffuse component
	
	//specular light
	float shininess = 50.0;
	float Dot_R_V = max(dot(R,V),0.0);
	vec3 Is = (u_light_color_specular*(pow(Dot_R_V,shininess)))/light_attenuation;
	
	
	vec2 uv = v_uv;

	vec4 map_color = texture2D(u_texture_map,uv);

	float texture_1_amount = 1 - (map_color.x + map_color.y + map_color.z);

	vec2 tiling_coords = uv*u_texture_tiling;

	
	vec4 color_1 = texture2D( u_texture, tiling_coords)*texture_1_amount;//texture1
	vec4 color_2 = texture2D(u_texture2, tiling_coords)*map_color.x;//texture2
	vec4 color_3 = texture2D(u_texture3, tiling_coords)*map_color.y;//texture3
	vec4 color_t = color_1 + color_2 + color_3;//total texture color
	
	
	vec3 final_color = (Ia + Id + Is*color_t.a)*color_t.xyz; //final color

	//fog effect
	float dist = length(v_world_position - u_camera_eye);
	dist = clamp(dist/u_far_plane, 0.0, 1.0);

	final_color = mix(final_color,u_sky_color,dist);

	gl_FragColor = vec4(final_color,1.0);

}
