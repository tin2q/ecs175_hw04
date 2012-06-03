varying vec2 f_texcoord;
uniform sampler2D u_texture;

uniform int tex_enabler;

varying vec4 f_color;

void main(void) {     

	if(tex_enabler == 0){
		gl_FragColor = f_color;
    }
	else {
		gl_FragColor = texture2D(u_texture, f_texcoord);
	}
}

