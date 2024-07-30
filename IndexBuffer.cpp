#include "IndexBuffer.h"
#include "Renderer.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) : m_Count(count) {
    glGenBuffers(1, &m_rendererID);
    //Collega il mio buffer a quello di OpenGL (di un tipo specifico)
    //In pratica, dici ad OpenGL di usare il mio buffer che ho creato 
    //ATTENZIONE: è possibile collegare al massimo un buffer alla volta
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
    //Passo i dati al buffer
    //Il secondo parametro deve indicari quanti vertici avrà la figura finale
    //(Indipendetemente dal numero degli elementi dell'array positions)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(unsigned int), data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer(){
    glDeleteBuffers(1, &m_rendererID);
}

void IndexBuffer::bind() const{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
}

void IndexBuffer::unBind() const{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

