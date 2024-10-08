#shader vertex
#version 410 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 aNormal;
layout(location = 4) in vec2 texCoord;

out vec2 v_texCoord;
out vec3 vPosVS;
out vec3 vNormalVS;
out vec4 projTexCoords;
out vec4 sun_shadowCoords;

out vec4 lamp_shadowCoords[8];

uniform mat4 u_MVP;
uniform mat4 uModel;

uniform mat4 proj_fanale;
uniform mat4 view_fanale;

uniform mat4 proj_sun;
uniform mat4 view_sun;

uniform mat4 proj_lamp;     
uniform mat4 view_lamp[8];  

void main() 
{
    gl_Position = u_MVP * vec4(position, 1.0);
    v_texCoord = texCoord;
    vPosVS = (uModel * vec4(position, 1.0)).xyz;  // Trasformo la posizione del vertice nello spazio della vista
    vNormalVS = (uModel * vec4(aNormal, 0.0)).xyz;  // Normale nello spazio della vista

    //Texturing proiettivo
    projTexCoords = proj_fanale * view_fanale * uModel * vec4(position, 1.0);
    sun_shadowCoords = proj_sun * view_sun * uModel * vec4(position, 1.0);

    for(int i = 0; i < 8; i++)
    {
        lamp_shadowCoords[i] = proj_lamp * view_lamp[i] * uModel * vec4(position, 1.0);
    }
}
	
#shader fragment
#version 410 core

layout(location = 0) out vec4 color;

in vec2 v_texCoord;
in vec3 vPosVS;
in vec3 vNormalVS;
in vec4 projTexCoords;
in vec4 sun_shadowCoords;
in vec4 lamp_shadowCoords[8]; 

uniform sampler2D u_texture;
uniform sampler2D normalMap_texture;
uniform sampler2D shadowMap_texture;
uniform sampler2D shadowMap_texture_sun;

uniform sampler2D shadowMap_texture_lamp[8];    //Gli va passato

uniform int isStreet;

uniform vec3 uLightPos[8];  // Array per le posizioni delle luci
uniform vec3 uLightColor[8];  // Array per i colori delle luci
uniform mat4 view_lamp[8];  //Arrei di viu mat dei lampioni
uniform int numLights;  // Numero di luci attive

uniform vec3 uLDir; //Direzione della luce del sole
uniform vec3 uSunColor; //Colore della luce del sole

uniform vec3 uDiffuseColor;
uniform vec3 uAmbientColor;
uniform vec3 uSpecularColor;
uniform float uShininess;
uniform float lamp_brightness;

uniform vec3 lampDir;
uniform float cutoff;
uniform float innerCutoff;

uniform vec3 fanale_color;
uniform mat4 view_fanale;

uniform mat4 view_sun;

uniform float uBias;



vec3 phong(vec3 L, vec3 V, vec3 N, vec3 lightColor) 
{
    float LN = max(0.0, dot(L, N));
    vec3 R = -L+2*dot(L,N)*N;

	float spec = ((LN>0.f)?1.f:0.f) * max(0.0,pow(dot(V,R),uShininess));
    return (uAmbientColor + LN * uDiffuseColor + spec * uSpecularColor) * lightColor;
}

float headlight_fading(vec3 coord)
{
    float begin_fading_x = 0.1;
    
    float end_fading_x = 0.5;
    
    float aux = abs(coord.x - 0.5);
    float fading = 1.0;

    

    if(aux < begin_fading_x)
        fading *= 1.0;
    else
        fading *= (end_fading_x - aux) / (end_fading_x - begin_fading_x);

    

    return fading;
}

void main() {
    vec3 finalColor = vec3(0.0);
    vec3 V = normalize(-vPosVS);  // Direzione verso la camera
    vec3 N = normalize(vNormalVS);  // Normale

    //Calcolo della normal map della strada
    if(isStreet == 1){
        N = texture(normalMap_texture,vec2(v_texCoord.x, v_texCoord.y)).xyz ;
        N = normalize(N*2.0-1.0);
    }

    // Itero su ogni luce dei lampioni e sommo il contributo
    for (int i = 0; i < numLights; i++) {
        vec3 L = normalize(uLightPos[i] - vPosVS);
        float theta = dot(L, -lampDir);

        float epsilon = cos(radians(innerCutoff)) - cos(radians(cutoff));
        float intensity = clamp((theta - (cos(radians(cutoff)))) / epsilon, 0.0, 1.0);
        
        vec4 lampCoords = (lamp_shadowCoords[i]/lamp_shadowCoords[i].w)*0.5 + 0.5;
        vec3 L2 = normalize(vec3(view_lamp[i][3]) - vPosVS);
        float bias = clamp(uBias*tan(acos(dot(N,L2))),uBias,0.05);
	    float depth = texture(shadowMap_texture_lamp[i],lampCoords.xy).x;

        if(theta > cos(radians(cutoff)) && !(depth + bias < lampCoords.z)){
            finalColor += phong(L, V, N, uLightColor[i]) * intensity;
        }
    }

    //Calcolo della luce del sole con le ombre (shadow map)
    vec4 sun_contribution = vec4(0.0, 0.0, 0.0, 1.0);

    vec4 sunCoords = (sun_shadowCoords/sun_shadowCoords.w)*0.5+0.5;
    vec3 L = normalize(vec3(view_sun[3]) - vPosVS);
    float bias = clamp(uBias*tan(acos(dot(N,L))),uBias,0.05);
	float depth = texture(shadowMap_texture_sun,sunCoords.xy).x;
        
	if(!(depth + bias < sunCoords.z)){
        sun_contribution = vec4(phong(uLDir, V, N, uSunColor),1.0);
    }

    //TEXTURING PROIETTIVO  
    vec4 texCoords = (projTexCoords/projTexCoords.w)*0.5+0.5;
    vec3 fanale_contribution = vec3(0.0, 0.0, 0.0);

    if (!(texCoords.x < 0.0 || texCoords.x > 1.0 || texCoords.y < 0.0 || texCoords.y > 1.0 || texCoords.z < 0.0 || texCoords.z > 1.0)) {
        //Calcolo delle ombre dei fanali con Shadow Map
        vec3 L = normalize(vec3(view_fanale[3]) - vPosVS);
        float bias = clamp(uBias*tan(acos(dot(N,L))),uBias,0.05);
		float depth = texture(shadowMap_texture,texCoords.xy).x;
        
		if(!(depth + bias < texCoords.z)){
            fanale_contribution = fanale_color * headlight_fading(texCoords.xyz);   
        }
    }

    vec4 t_color = texture(u_texture, v_texCoord.xy);  // Recupero il colore dalla texture

    color = vec4(((finalColor * lamp_brightness) + (sun_contribution.xyz*10) + (fanale_contribution)) * t_color.rgb, 1.0);
}
