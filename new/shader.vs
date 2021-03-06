#version 330 

in vec3 Position;
in vec2 TexCoord;
in vec3 Normal;

uniform mat4 gWVP;

out vec2 TexCoord0;
out vec3 Normal0;

void main()
{
	gl_Position = gWVP * vec4(Position, 1.0);
	TexCoord0 = TexCoord;
	Normal0 = Normal;
}
