#include <GL/glew.h>
#include "Renderer.h"
#include "VertexArray.h"

VertexArray::VertexArray(){
	glGenVertexArrays(1, &m_rendererID);
}

VertexArray::~VertexArray(){
	glDeleteVertexArrays(1, &m_rendererID);
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	bind();	//Bind del VertexArray
	vb.Bind();	//Bind del buffer che viene passato, viene collegato al VertexArray

	const auto& elements = layout.getElements();
	unsigned int offset = 0;

	//Questo serve nel caso in cui il vertex buffer contiene figure diverse, in cui mi serve
	//quindi specificare più di un layout
	for (unsigned int i = 0; i < elements.size(); i++) {
		const auto& element = elements[i];
		glEnableVertexAttribArray(i);	//Qui dovrebbe servire la i e non 0
		glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.getStride(), (const void*)offset);
		offset += element.count * VertexBufferElement::getSizeOfType(element.type);
	}

}

void VertexArray::bind() const
{
	glBindVertexArray(m_rendererID);
}

void VertexArray::unBind() const
{
	glBindVertexArray(0);
}
