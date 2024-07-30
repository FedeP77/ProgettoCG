#pragma once

#include "Renderer.h"

using namespace std;

class Texture {
private:
	unsigned int m_rendererID;
	string m_filepath;
	unsigned char* m_localBuffer;	//Buffer locale che contiene i dati della texture una volta caricata
	int m_width, m_height, m_BPP;	//BPP -> Bits Per Pixel
public:
	Texture(const string& path);
	~Texture();

	void bind(unsigned int slot = 0) const;
	void unBind() const;

	inline int getWidth() const {
		return m_width;
	}

	inline int getHeight() const {
		return m_height;
	}
};
