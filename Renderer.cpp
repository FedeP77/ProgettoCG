#include "Renderer.h"

void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}


//Per renderizzare servono tre elementi: il VertexArray, un IndexBuffer e le Shader
void Renderer::draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const{
    //Si collegano tutti i buffer e lo shader ad ogni frame
    shader.bind();
    va.bind();
    ib.bind();

    //Dice ad OpenGL di disegnare gli elementi scritti nel buffer, interpretandoli come un triangolo
    glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr);
}
