varying vec2 f_texcoord;
uniform sampler2D u_texture;

void main(void) {        
  gl_FragColor = texture2D(u_texture, f_texcoord);;
}

