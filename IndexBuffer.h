#pragma once

class IndexBuffer {
private:
	unsigned int m_rendererID;	//ID univoco che identifica lo specifico buffer o oggetto che creo
	unsigned int m_Count;

public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	void bind() const;
	void unBind() const;

	inline unsigned int getCount() const {
		return m_Count;
	}
};
