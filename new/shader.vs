#version 330 core

in vec3 Position;

uniform mat4 gWorld;

out vec4 Color;

const vec4 colors[3] = vec4[3]( vec4(1,0,0,1),
								vec4(0,1,0,1),
								vec4(0,0,1,1));

void main()
{
	gl_Position = gWorld * vec4(Position, 1.0);
	Color = colors[gl_VertexID];
}
