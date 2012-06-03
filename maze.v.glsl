attribute vec3 coord3d;
attribute vec3 normals;
attribute vec2 texcoord;

varying vec2 f_texcoord;
varying vec4 f_color;
varying vec4 f_normals;

//uniform vec3 v_color;
uniform int tex_enabler;

// Projection matrix
uniform mat4 m_proj;
uniform mat4 normal_mat;

void main(void) {        

	if(tex_enabler == 0){
		f_normals = normalize(normal_mat * vec4(normals, 1.0));
	}
	else {
		f_texcoord = texcoord;
		f_normals = normalize(normal_mat * vec4(normals, 1.0));
    }
	 // multiply matrix times position vector
     gl_Position  = m_proj *  vec4(coord3d,1.0);

}

