#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Librerie/GLM/glm.hpp"
#include "Librerie/GLM/gtc/matrix_transform.hpp"
#include "Librerie/ImGui/imgui.h"
#include "Librerie/ImGui/imgui_impl_glfw_gl3.h"

#define N_PUNTI 101
#define PPT 10

#define MAX_HEIGHT 300.f

using namespace std;

float* genGrid(float grid[N_PUNTI*N_PUNTI*5],  float dim_lato, unsigned int indici[], string filename) {
    //La griglia è NxN punti
    //Quindi (N-1)x(N-1) quadrati

    string line;
    string cell;

    ifstream mappa_di_altezze(filename);

    if (!mappa_di_altezze.is_open())
    {
        perror("COGLIONE DI MERDA");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < N_PUNTI; i++) {

        getline(mappa_di_altezze, line);
        stringstream lineStream(line);

        for (int j = 0; j < N_PUNTI; j++) {
            
            //Genera i 4 vertici di ogni quadrato
            /*
            for (int m = 0; m < 4; m++) {
                grid[k] = sqr[m * 5] + incrX;        // x
                grid[k + 1] = sqr[m * 5 + 1];        // y
                grid[k + 2] = sqr[m * 5 + 2] + incrZ; // z
                grid[k + 3] = sqr[m * 5 + 3];        // t1
                grid[k + 4] = sqr[m * 5 + 4];        // t2
                k += 5;
            }
            */

            getline(lineStream, cell, ',');

            grid[(i * N_PUNTI + j)*5 + 0] = i * dim_lato - dim_lato * (N_PUNTI - 1) / 2;
            grid[(i * N_PUNTI + j)*5 + 1] = stof(cell) * MAX_HEIGHT;
            grid[(i * N_PUNTI + j)*5 + 2] = j * dim_lato - dim_lato * (N_PUNTI - 1) / 2;
            grid[(i * N_PUNTI + j) * 5 + 3] = float(i) / PPT;
            grid[(i * N_PUNTI + j) * 5 + 4] = float(j) / PPT;
        }
    }

    //Definisce l'indexBuffer


    

    for (int i = 0; i < N_PUNTI - 1; i++)
    {
        for (int j = 0; j < N_PUNTI-1; j++)
        {
            indici[6 * (N_PUNTI - 1) *i + 6 * j + 0] = i * N_PUNTI + j + 0;
            indici[6 * (N_PUNTI - 1) *i + 6 * j + 1] = i * N_PUNTI + j + 1;
            indici[6 * (N_PUNTI - 1) * i + 6 * j + 2] = i * N_PUNTI + j + N_PUNTI;

            indici[6 * (N_PUNTI - 1) * i + 6 * j + 3] = i * N_PUNTI + j + 1;
            indici[6 * (N_PUNTI - 1) * i + 6 * j + 4] = i * N_PUNTI + j + N_PUNTI + 1;
            indici[6 * (N_PUNTI - 1) * i + 6 * j + 5] = i * N_PUNTI + j + N_PUNTI;
        }
    }

    return grid;
}

int main(void)
{
    GLFWwindow* window;

    //Inizializza la libreria
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Crea una finestra con dimensioni fissate e un nome specifico
    window = glfwCreateWindow(2000, 1600, "Computer Grafica", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glClearColor(0.0, 0.0, 0.0, 1.0);

    //Sposta il contesto (il focus di Windows) sulla finestra creata
    //Il codice successivo effettuerà tutte le operazioni solo su questa finestra
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);    //Sincronizza il frame-rate dell'animazione con quello dello schermo

    if (glewInit() != GLEW_OK) {
        cout << "ERRORE glewInit" << endl;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    //Array che contiene le coordinate (e altri dati) dei punti
    //PER IL VERTEX BUFFER
    float sqr[] = {
        -50.0f, 0.0f, -50.0f, 0.0f, 0.0f,
         50.0f, 0.0f, -50.0f, 1.0f, 0.0f,
         50.0f,  0.0f, 50.0f, 1.0f, 1.0f,
        -50.0f,  0.0f, 50.0f, 0.0f, 1.0f
    };

    

    //Il terzo e il quarto parametro sono le coordinate della texture: 
    //il pixel in basso a sinistra è (0,0)
    //il pixel in basso a destra è (1,0)
    //il pixel in alto a destra è (1,1)
    //il pixel in alto a sinistra è (0.1)

    //Per evitare di duplicare i punti nell'array positions, si crea un indexBuffer, in cui si indicano le posizioni
    //dei punti da usare per creare ciascun elemento
    //PER L'INDEX BUFFER
    unsigned int indici[(N_PUNTI-1) * (N_PUNTI - 1)*6];

    float lato = 10.f;

    float positions[N_PUNTI * N_PUNTI * 5];
    genGrid(positions, lato, indici, "res/mappa_altezze.csv");


    //Funzione di blend per far funzionare anche le texture semi-trasparenti
    //glBlendFunc(GL_SRC0_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_BLEND);

    //Si crea il vertex array object. Questo è un oggetto alla quale poi si collega il buffer e l'indexBuffer
    //Il VertexArrayObject è una specie di array che contiene tutti i buffer che vengono generati e li collega insieme
    VertexArray va;

    //Crea il VertexBuffer
    VertexBuffer vb(positions, N_PUNTI * N_PUNTI * 5 * sizeof(float));

    //Crea l'oggetto che definisce come è strutturato il vertexBuffer
    //Specifica quanti attributi hanno i vertici (in questo caso 2, ovvero le coordinate x e y)
    VertexBufferLayout layout;
    layout.push(GL_FLOAT, 3);
    //Aggiungo altri 2 attributi ad ogni vertice (le coordinate delle texture)
    layout.push(GL_FLOAT, 2);

    //Nel VertexArray, collega il vertexBuffer ai layout che ho definito
    va.addBuffer(vb, layout);


    //Crea l'IndexBuffer
    //Specifica, tramite un array di indici, quali vertici compongono ciascun oggetto da renderizzare
    IndexBuffer ib(indici, (N_PUNTI - 1) * (N_PUNTI - 1) * 6);

    //Crea una matrice di proiezione 4x4
    //rispetto ai lati della finestra: (sinistra, destra, sotto, sopra, vicino, lontano)
    //Tutti i vertici verranno poi moltiplicati per questa matrice
    //Tutti i vertici che si trovano fuori da questi limiti non saranno visualizzati
    //La finestra che ho creato ha un rapporto 4:3 e non è quadrata, quindi devo convertire anche i miei vertici in 4:3
    //in modo che le distanze tra loro non vengano alterate
    //glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f); //Matrice con formato 4:3

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.0f, 0.1f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 500.0f, -1500.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

    //La view matrix riguarda la posizione della telecamera
    //In realtà si simula lo spostamento della telecamera spostando gli oggetti nella scena
    //Si crea una matrice di traslazione
    //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0, 0, 0));    //Num negativo --> Telecamera verso destra
                                                                                //Num positivo --> Telecamera verso

    Shader shader("Shader/vertexfragment.shader");
    shader.bind();

    //shader.setUniform4f("u_color", 0.8f, 0.3f, 0.8f, 1.0f);

    //Carica la texture dal file e la collega
    Texture texture("res/grass_tile.png");
    texture.bind();

    //Uniform da passare allo shader per renderizzare la texture
    //Lo uniform conterrà la texture che abbiamo inserito nello slot specificato (con il texture.bind())
    shader.setUniform1i("u_texture", 0);

    //float r = -1.0f;
    //float increment = 0.5f;

    //Si toglie tutto perché è meglio collegarli frame per frame
    //Immagino che torna comodo quando c'è da fare cambiamenti tra frame
    shader.unBind();
    va.unBind();
    vb.Unbind();
    ib.unBind();

    Renderer renderer;

    ImGui::CreateContext();                     //ImGui è un'API per modificare la scena in tempo reale
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();

    glm::vec3 translation(0.0, 0, 0);
    float rot = 0.0f;
    float speed = 0.0f;

    //Il ciclo continua finché l'utente non chiude la finestra
    while (!glfwWindowShouldClose(window))
    {
        //Pulisci il buffer dei colori (Preparazione dell'ambiente di rendering)
        renderer.clear();
        
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplGlfwGL3_NewFrame();

        //La model matrix sposta gli oggetti nella scena 
        glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
        rot = rot + speed;
        model = glm::rotate(model, rot, glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 mvp = proj * view * model;    //L'ordine dei prodotti è importante
        shader.setUniformMat4f("u_MVP", mvp);


        //shader.setUniform4f("u_color", r, 0.3f, 0.8f, 1.0f);

        //Dice ad OpenGL di disegnare gli elementi scritti nel buffer, interpretandoli come un triangolo
        renderer.draw(va, ib, shader);


        /*if (r > 1.0f)
            increment = -0.5f;
        else if (r < 0.0f)
            increment = 0.05f;

        r += increment;*/

        {   //Finestra di ImGui
            static float f = 0.0f;
            ImGui::SliderFloat3("Translation", &translation.x, -1000.0f, 1000.0f);
            ImGui::SliderFloat("Rotation", &speed, -0.01f, 0.01f);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }
            
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        //Scambia il buffer corrente con quello che è stato appena creato
        glfwSwapBuffers(window);

        //Processa gli eventi (Renderizza la scena)
        glfwPollEvents();
    }

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}