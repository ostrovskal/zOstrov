precision mediump float;
uniform sampler2D tex;
uniform mat4 cflt;
uniform vec4 tcolor;
varying vec2 v_Texture;
const float threshold = .01;

void main(void) {
	vec3 c = texture2D(tex, gl_FragCoord.xy / v_Texture *.3).xxx;
	vec3 res = smoothstep(.5 - threshold, .5 + threshold, c);
	gl_FragColor = vec4(res, 1.0) * tcolor * cflt;
}
