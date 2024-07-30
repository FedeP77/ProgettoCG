#pragma once

class VertexBuffer {
private:
	unsigned int m_rendererID;	//ID univoco che identifica lo specifico buffer o oggetto che creo

public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};
