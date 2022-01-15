#version 330 

in vec3 Position;
in vec2 TexCoord;

uniform mat4 gWorld;

out vec2 TexCoord0;

void main()
{
	gl_Position = gWorld * vec4(Position, 1.0);
	TexCoord0 = TexCoord;
}
