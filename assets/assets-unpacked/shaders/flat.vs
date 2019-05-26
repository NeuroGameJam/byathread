#version 100

precision lowp float;

attribute vec3 vertex_location;
attribute vec2 texture_location;

// Output data 
varying vec2 UV;

uniform mat4 mvpMatrix;

void main()
{
	gl_Position =  mvpMatrix * vec4(vertex_location, 1);
	UV = texture_location;
}
