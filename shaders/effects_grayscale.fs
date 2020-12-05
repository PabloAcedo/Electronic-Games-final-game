varying vec2 v_uv;
uniform sampler2D u_texture;


void main(){
	vec4 color = texture2D(u_texture,v_uv);
	color *= 1.2 - length(v_uv - vec2(0.5));
	float gray_scale = dot(color.rgb, vec3(0.3, 0.5, 0.1));
	gl_FragColor = vec4(vec3(gray_scale), 1.0);

}