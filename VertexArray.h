#pragma once
#include <GL/glew.h>
#include "Renderer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"


class VertexArray {
private:
	unsigned int m_rendererID;
public:
	VertexArray() {
		glGenVertexArrays(1, &m_rendererID);
	}

	~VertexArray() {
		glDeleteVertexArrays(1, &m_rendererID);
	}

	void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
	{
		bind();	//Bind del VertexArray
		vb.Bind();	//Bind del buffer che viene passato, viene collegato al VertexArray

		const auto& elements = layout.getElements();
		unsigned int offset = 0;

		//Questo serve nel caso in cui il vertex buffer contiene figure diverse, in cui mi serve
		//quindi specificare più di un layout
		for (unsigned int i = 0; i < elements.size(); i++) {
			const auto& element = elements[i];
			glEnableVertexAttribArray(i*4);	//Qui dovrebbe servire la i e non 0
			glVertexAttribPointer(i*4, element.count, element.type, element.normalized, layout.getStride(), (const void*)offset);
			offset += element.count * VertexBufferElement::getSizeOfType(element.type);
		}

	}

	void bind() const
	{
		glBindVertexArray(m_rendererID);
	}

	void unBind() const
	{
		glBindVertexArray(0);
	}
};
