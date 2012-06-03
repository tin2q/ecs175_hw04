
varying vec2 f_texcoord;
varying vec4 f_normals;

uniform sampler2D u_texture;

uniform int tex_enabler;
uniform vec3 v_color;

varying vec4 f_color;

void main(void) {    
	vec4 color; 
	vec4 light = vec4(1.5,2.0,1.0,1.0);
		light = normalize(light);
		float brightness = min(dot(f_normals,light),1);

	if(tex_enabler == 0){
		color = vec4(brightness * v_color, 1.0);
    }
	else {
		color = brightness * texture2D(u_texture, f_texcoord);
	}

	gl_FragColor = color;

}

