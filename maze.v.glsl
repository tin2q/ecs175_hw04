attribute vec3 coord3d;
attribute vec3 normals;
attribute vec2 texcoord;

varying vec2 f_texcoord;
varying vec4 f_color;

uniform vec3 v_color;
uniform int tex_enabler;

// Projection matrix
uniform mat4 m_proj;

void main(void) {        
     
//     f_color = vec4(v_color, 1.0);
//     vec4 norm = vec4(normals, 1.0);

	if(tex_enabler == 0){
		f_color = vec4(1,1,1,1.0);
	}
	else {
		f_texcoord = texcoord;
    }
	 // multiply matrix times position vector
     gl_Position  = m_proj *  vec4(coord3d,1.0);

}

