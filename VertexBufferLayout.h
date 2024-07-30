#pragma once
#include <vector>
#include <GL/glew.h>
#include "Renderer.h"

using namespace std;

struct VertexBufferElement {
	unsigned int count;
	unsigned int type;
	unsigned char normalized;

	static unsigned int getSizeOfType(unsigned int type) {
		switch (type) {
			case GL_FLOAT: 
				return 4;
			case GL_UNSIGNED_INT:
				return 4;
			case GL_UNSIGNED_BYTE:
				return 1;
			default:
				return 0;
		}
	}
};

class VertexBufferLayout {
private:
	vector<VertexBufferElement> m_elements;
	unsigned int m_stride;

public:
	VertexBufferLayout() : m_stride(0) {}

	void push(unsigned int type, unsigned int count) {
		//Aggiunge un elemento a m_elements, ovvero uno struct che definisce un layout per un insieme di vertici
		m_elements.push_back({ count, type, GL_FALSE });
		m_stride += count * VertexBufferElement::getSizeOfType(type);
	}

	inline const vector<VertexBufferElement> getElements() const {
		return m_elements;
	}

	inline unsigned int getStride() const {
		return m_stride;
	}
};
