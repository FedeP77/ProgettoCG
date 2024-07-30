#include "Texture.h"
#include "stb_image.h"

Texture::Texture(const string& path): m_rendererID(0), m_filepath(path), m_localBuffer(nullptr), m_width(0), m_height(0), m_BPP(0){
	//Inverte la texture specchiandola sull'asse y, perché OpenGL inizia a caricare i pixel dall'angolo in basso a sinistra
	stbi_set_flip_vertically_on_load(1);
	//Carica la texture nel buffer locale
	m_localBuffer = stbi_load(path.c_str(), &m_width, &m_height, &m_BPP, 4);	//4 perché si usa il formato RGBA

	//Crea una o più texture e le assegna a rendererID (numero che identifica la texture)
	glGenTextures(1, &m_rendererID);
	//Collega le texture dentro rendererID nella struttura di OpenGL
	glBindTexture(GL_TEXTURE_2D, m_rendererID);

	//MIN e MAG FILTER sono parametri e servono per definire il comportamento della texture quando viene ingrandita o rimpicciolita
	//LINEAR significa che i pixel vengono calcolati in modo lineare
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//Con CLAMP vogliamo che l'area della texture non venga estesa (S, T) = (x, y)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Carica i dati della texture nella GPU
	//Formato -> (target, livelli, formato che deve usare opengl, width, height, dim bordo, formato di cio che gli si passa, tipo di dati passati, buffer che contiene i pixel)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_localBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if(m_localBuffer)
		stbi_image_free(m_localBuffer);	//Questo buffer non serve più quindi si libera
}

Texture::~Texture(){
	glDeleteTextures(1, &m_rendererID);
}

void Texture::bind(unsigned int slot) const{
	//"slot" è un parametro OPZIONALE in cui si specifica a quale slot si vuole legare la texture
	//Questo perché è possibile collegare più di una texture ad un oggetto (in Windows fino a 32)
	
	//Indica quale slot stiamo usando (vale per sempre, finche non viene cambiato)
	glActiveTexture(GL_TEXTURE0 + slot);

	glBindTexture(GL_TEXTURE_2D, m_rendererID);
}

void Texture::unBind() const{
	glBindTexture(GL_TEXTURE_2D, 0);
}
