#version 330 core

in vec3 Position;

uniform mat4 gRotation;

void main()
{
	gl_Position = gRotation * vec4(Position, 1.0);
}
