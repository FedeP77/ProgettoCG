#shader vertex
#version 460 core 
layout (location = 0) in vec3 aPosition; 
layout (location = 1) in vec3 aColor; 
layout (location = 2) in vec3 aNormal; 
 
out vec3 vPos;
out vec3 vNorm;


uniform mat4 uProj;
uniform mat4 uView;
uniform mat4 uModel;
uniform mat4 uRot;

void main(void) 
{ 
	
	vPos = (uView*uRot*uModel*vec4(aPosition, 1.0)).xyz; 
	vNorm =  normalize((uView*uRot*inverse(transpose(uModel))*vec4(aNormal, 0.0)).xyz);
    gl_Position = uProj*uView*uRot*uModel*vec4(aPosition, 1.0);
	 
}

#shader fragment
#version 460 core  
out vec4 color; 
in vec3 vColor;
in vec3 vPos;
in vec3 vNorm;


uniform vec3 uColor;

void main(void) 
{
	vec3 L0 = normalize(vec3(2,2,0)-vPos);
	vec3 L1 = normalize(vec3(-2,1,0)-vPos);
   float contrib = (max(0.f,dot(vNorm,L0))+max(0.f,dot(vNorm,L1)))*0.5;

   color = vec4(uColor*contrib, 1.0); 
} 