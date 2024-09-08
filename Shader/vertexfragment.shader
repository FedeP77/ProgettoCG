#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 aNormal;
layout(location = 4) in vec2 texCoord;//Uniform che contiene le coordinate della texture

out vec2 v_texCoord;//Manda in output le coordinate della texture al fragment shader
out vec3 vLDirVS;
out vec3 vPosVS;
out vec3 vNormalVS;
out vec3 vColor;

uniform mat4 u_MVP;	//Model View Projection matrix
//uniform mat4 uProj;
uniform mat4 uView;
uniform mat4 uModel;
uniform vec3 uLDir;
//out vec4 v_position;

uniform vec3 uDiffuseColor;
uniform vec3 uAmbientColor;
uniform vec3 uSpecularColor;
//uniform vec3 uEmissiveColor;
uniform vec3 uLightColor;
uniform float uShininess;

vec3 phong ( vec3 L, vec3 V, vec3 N){
	float LN = max(0.0,dot(L,N));

	vec3 R = -L+2*dot(L,N)*N;

	float spec = ((LN>0.f)?1.f:0.f) * max(0.0,pow(dot(V,R),uShininess));

	return (uAmbientColor+LN*uDiffuseColor + spec * uSpecularColor)*uLightColor;
}

void main()
{
   gl_Position = u_MVP * vec4(position, 1.f);
   v_texCoord = texCoord;
   //v_position = position;

   vLDirVS   =  (uView*vec4(uLDir,0.f)).xyz; 
   vNormalVS =  (uView*uModel*vec4(aNormal, 0.0)).xyz; 

   vPosVS = (uView*uModel*normalize(vec4(position, 1.f))).xyz; 

   vColor    = phong(vLDirVS,normalize(-vPosVS),normalize(vNormalVS));
}
	
#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_texCoord;//Prende in input le coordinate della texture dal vertex shader
in vec3 vColor;
in vec3 vPosVS;
in vec3 vNormalVS;
in vec3 vLDirVS;
//in vec4 v_position;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform vec3 uLDir;

uniform vec3 uDiffuseColor;
uniform vec3 uAmbientColor;
uniform vec3 uSpecularColor;
//uniform vec3 uEmissiveColor;
uniform vec3 uLightColor;
uniform float uShininess;
uniform float light_brightness;

vec3 phong ( vec3 L, vec3 V, vec3 N){
	float LN = max(0.0,dot(L,N));

	vec3 R = -L+2*dot(L,N)*N;

	float spec = ((LN>0.f)?1.f:0.f) * max(0.0,pow(dot(V,R),uShininess));

	return (uAmbientColor + LN * uDiffuseColor + spec * uSpecularColor)*uLightColor;
}

void main()
{
	//vec4 texColor = texture(u_texture, v_texCoord);//Ottiene il colore di pixel dalla texture e dalle coordinate
	//color = texColor;
	//color = u_color * v_position;

	color = vec4(phong(vLDirVS,normalize(-vPosVS),normalize(vNormalVS)),1.0);
	//color = texture2D(u_texture,v_texCoord.xy)*(color.x+color.y+color.z)/3.f;
	vec4 t_color = texture(u_texture, v_texCoord.xy);

	color.x = color.x * t_color.x;
	color.y = color.y * t_color.y;
	color.z = color.z * t_color.z;

	color = normalize(color);
	color = vec4(color.xyz * light_brightness, 1.f);
}