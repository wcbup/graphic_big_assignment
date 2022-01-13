#version 330 core

in vec3 Position;

uniform mat4 gTranslation;

void main()
{
	gl_Position = gTranslation * vec4(Position, 1.0);
}
