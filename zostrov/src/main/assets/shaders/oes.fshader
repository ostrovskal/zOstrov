#extension GL_OES_EGL_image_external : require
precision mediump float;
varying vec2 v_Texture;
uniform mat4 cflt;
uniform vec4 tcolor;
uniform samplerExternalOES tex;

void main() {
	vec4 tmp = texture2D(tex, v_Texture);
	gl_FragColor = (tmp * tcolor) * cflt;
}

