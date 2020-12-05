varying vec2 v_uv;
uniform sampler2D u_texture;


void main(){
	vec4 color = texture2D(u_texture,v_uv);
	color.b = (color.b - 0.5) * 0.8 + 0.5;
	color *= 1.5 - length(v_uv - vec2(0.5));
	gl_FragColor = color;

}