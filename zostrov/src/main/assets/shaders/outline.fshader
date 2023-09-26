precision mediump float;
uniform sampler2D tex;
uniform mat4 cflt;
uniform vec4 tcolor;
varying vec2 v_Texture;
const float contrast = 20.;

void main(void) {
	vec3 c = texture2D(tex, gl_FragCoord.xy / v_Texture *.35).xxx;
	vec3 c1 = (c - .45) * contrast;
	vec3 c2 = 1.-(c - .5) * contrast;
	vec3 res = mix(c1, c2, (c - .5) * contrast);
	gl_FragColor = vec4(res, 1.0) * tcolor * cflt;
}