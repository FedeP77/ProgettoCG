#shader vertex
#version 430 core 
layout (location = 0) in vec3 aPosition; 

uniform mat4 uLightMatrix;


void main(void) 
{ 
    gl_Position = uLightMatrix*vec4(aPosition, 1.0); 
}

#shader fragment
#version 430 core  
out vec4 color; 

uniform float uPlaneApprox;

float  PlaneApprox() {   
	// Compute partial derivatives of depth.    
	float dx = dFdx(gl_FragCoord.z);   
	float dy = dFdy(gl_FragCoord.z);

	// Compute second moment over the pixel extents.   
	return  gl_FragCoord.z*gl_FragCoord.z + uPlaneApprox*0.5*(dx*dx + dy*dy);   
} 

void main(void) 
{ 
//	color = vec4(vec3((gl_FragCoord.z)),1.0);
//	color = vec4(1.0,0.0,0.0,1.0);
 	color = vec4(gl_FragCoord.z,0.0,0.0,1.0);
} 