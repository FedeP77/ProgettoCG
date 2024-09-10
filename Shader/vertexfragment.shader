#shader vertex
#version 410 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 aNormal;
layout(location = 4) in vec2 texCoord;

out vec2 v_texCoord;
out vec3 vPosVS;
out vec3 vNormalVS;

uniform mat4 u_MVP;
uniform mat4 uModel;

void main() {
    gl_Position = u_MVP * vec4(position, 1.0);
    v_texCoord = texCoord;
    vPosVS = (uModel * vec4(position, 1.0)).xyz;  // Trasformo la posizione del vertice nello spazio della vista
    vNormalVS = (uModel * vec4(aNormal, 0.0)).xyz;  // Normale nello spazio della vista
}
	
#shader fragment
#version 410 core

layout(location = 0) out vec4 color;

in vec2 v_texCoord;
in vec3 vPosVS;
in vec3 vNormalVS;

uniform sampler2D u_texture;

uniform vec3 uLightPos[8];  // Array per le posizioni delle luci
uniform vec3 uLightColor[8];  // Array per i colori delle luci
uniform int numLights;  // Numero di luci attive

uniform vec3 uLDir; //Direzione della luce del sole
uniform vec3 uSunColor; //Colore della luce del sole

uniform vec3 uDiffuseColor;
uniform vec3 uAmbientColor;
uniform vec3 uSpecularColor;
uniform float uShininess;
uniform float lamp_brightness;

vec3 phong(vec3 L, vec3 V, vec3 N, vec3 lightColor) {
    float LN = max(0.0, dot(L, N));
    vec3 R = -L+2*dot(L,N)*N;

	float spec = ((LN>0.f)?1.f:0.f) * max(0.0,pow(dot(V,R),uShininess));
    return (uAmbientColor + LN * uDiffuseColor + spec * uSpecularColor) * lightColor;
}

void main() {
    vec3 finalColor = vec3(0.0);
    vec3 V = normalize(-vPosVS);  // Direzione verso la camera
    vec3 N = normalize(vNormalVS);  // Normale del frammento

    // Itero su ogni luce e sommo il contributo
    for (int i = 0; i < numLights; i++) {
        vec3 L = normalize(uLightPos[i] - vPosVS);  // Direzione della luce
        float distance = length(uLightPos[i] - vPosVS);  // Distanza dal punto luce
        float attenuation = lamp_brightness / (1.0 + 0.0009 * distance + 0.00032 * distance * distance);  // Attenuazione
        //attenuation = 1.0;

        // Calcolo il contributo Phong per ogni luce e lo attenuo
        vec3 lightContribution = phong(L, V, N, uLightColor[i]) * attenuation;
        finalColor += lightContribution;  // Sommo il contributo di ogni luce
    }

    vec4 sun_contribution = vec4(phong(uLDir, V, N, uSunColor),1.0);

    vec4 t_color = texture(u_texture, v_texCoord.xy);  // Recupero il colore dalla texture
    finalColor *= t_color.rgb;  // Applico il colore della texture

    color = vec4((finalColor * lamp_brightness) + sun_contribution.xyz, 1.0);  // Applico la luminosità e imposto il colore finale
}
