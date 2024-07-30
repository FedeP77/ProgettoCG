#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;//Uniform che contiene le coordinate della texture

out vec2 v_texCoord;//Manda in output le coordinate della texture al fragment shader

uniform mat4 u_MVP;	//Model View Projection matrix
void main()
{
   gl_Position = u_MVP * position;
   v_texCoord = texCoord;
}
	
#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_texCoord;//Prende in input le coordinate della texture dal vertex shader

uniform vec4 u_color;
uniform sampler2D u_texture;

void main()
{
	vec4 texColor = texture(u_texture, v_texCoord);//Ottiene il colore di pixel dalla texture e dalle coordinate
	 color = texColor;
}