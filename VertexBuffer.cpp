#include "vertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size){
    glGenBuffers(1, &m_rendererID);
    //Collega il mio buffer a quello di OpenGL (di un tipo specifico)
    //In pratica, dici ad OpenGL di usare il mio buffer che ho creato 
    //ATTENZIONE: è possibile collegare al massimo un buffer alla volta
    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
    //Passo i dati al buffer
    //Il secondo parametro deve indicari quanti vertici avrà la figura finale
    //(Indipendetemente dal numero degli elementi dell'array positions)
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer(){
    glDeleteBuffers(1, &m_rendererID);
}

void VertexBuffer::Bind() const{
    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
}

void VertexBuffer::Unbind() const{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

