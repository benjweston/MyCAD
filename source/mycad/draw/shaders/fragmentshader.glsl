varying vec3 ex_Color;
void main() {
	gl_FragColor = vec4(ex_Color.x, ex_Color.y, ex_Color.z, 1.0);
}