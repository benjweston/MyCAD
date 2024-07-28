attribute vec4 in_Position;
attribute vec3 in_Color;
varying vec3 ex_Color;
void main(void) {
	gl_Position = gl_ModelViewProjectionMatrix * in_Position;
	ex_Color = in_Color;
}