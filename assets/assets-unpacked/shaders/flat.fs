#version 100

precision lowp float;

varying vec2 UV;
uniform sampler2D texture0;

void main()
{
	gl_FragColor = texture2D(texture0, UV);
}
