precision mediump float;
uniform mat4 cflt;
uniform vec4 tcolor;
uniform sampler2D tex;
varying vec2 v_Texture;

void main() {
	vec4 tmp = texture2D(tex, v_Texture);
	gl_FragColor = (tmp * tcolor) * cflt;
}
