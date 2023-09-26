attribute vec4 pos;
attribute vec2 tex;
varying vec2 v_Texture;
uniform mat4 wmtx;
uniform mat4 pmtx;

void main() {
	v_Texture = tex;
	gl_Position = (pmtx * wmtx) * pos;
}
