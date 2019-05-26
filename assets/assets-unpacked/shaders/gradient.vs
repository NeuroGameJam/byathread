#version 100

precision lowp float;

attribute vec3 vertex_location;

varying vec2 UV;

void main()
{
	gl_Position =  vec4(vertex_location, 1);
	UV = vec2(vertex_location.x, vertex_location.y);
}
