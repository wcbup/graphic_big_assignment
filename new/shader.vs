#version 330 core

in vec3 Position;
in vec3 inColor;

uniform mat4 gWorld;

out vec4 Color;

void main()
{
	gl_Position = gWorld * vec4(Position, 1.0);
	Color = vec4(inColor, 1);
}
