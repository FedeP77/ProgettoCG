#pragma once
#include "vertexBuffer.h"
#include "Renderer.h"

class VertexBuffer {
private:
	unsigned int m_rendererID;	//ID univoco che identifica lo specifico buffer o oggetto che creo

public:
    VertexBuffer(const void* data, unsigned int size) {
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

    ~VertexBuffer() {
        glDeleteBuffers(1, &m_rendererID);
    }

    void Bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
    }

    void Unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};
