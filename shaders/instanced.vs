attribute vec3 a_vertex;
attribute vec3 a_normal;
attribute vec2 a_uv;


attribute mat4 u_model;

uniform vec3 u_camera_pos;

uniform mat4 u_viewprojection;

uniform vec3 u_light_pos;

//this will store the color for the pixel shader
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec3 v_tolight;

void main()
{	
	//calcule the normal in camera space (the NormalMatrix is like ViewMatrix but without traslation)
	v_normal = (u_model * vec4( a_normal, 0.0) ).xyz;
	
	//calcule the vertex in object space
	v_position = a_vertex;
	v_world_position = (u_model * vec4( a_vertex, 1.0) ).xyz;
	
	//store the texture coordinates
	v_uv = a_uv;

	//computing light vector
	v_tolight = u_light_pos - v_world_position;

	//calcule the position of the vertex using the matrices
	gl_Position = u_viewprojection * vec4( v_world_position, 1.0 );
}