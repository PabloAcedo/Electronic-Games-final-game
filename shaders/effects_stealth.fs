varying vec2 v_uv;
uniform sampler2D u_texture;


void main(){
	vec4 color = texture2D(u_texture,v_uv);
	color.r = (color.r - 0.5) * 0.8 + 0.5;
	color *= 1.3 - length(v_uv - vec2(0.5,0.8));
	//color.g = 1.0 - color.g;
	gl_FragColor = color;

}