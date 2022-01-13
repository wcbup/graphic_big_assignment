#version 330 core

in vec3 Position;

uniform mat4 gWorld;

void main()
{
	gl_Position = gWorld * vec4(Position, 1.0);
}
