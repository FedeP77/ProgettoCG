#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
//#include "stb_image.h"
#include "Librerie/gltf_loader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Custom/Texture.h"
#include "Librerie/GLM/glm.hpp"
#include "Librerie/GLM/gtc/matrix_transform.hpp"
#include "Librerie/ImGui/imgui.h"
#include "Librerie/ImGui/imgui_impl_glfw_gl3.h"
#include "matrix_stack.h"
//#include "Librerie/renderable.h"   

#define N_PUNTI 101
#define PPT 10
#define P_CERCH 500

#define MAX_HEIGHT 300.f
#define NUM_LIGHTS 8
#define TREE_COUNT 10

using namespace std;

float* genGrid(float grid[N_PUNTI*N_PUNTI*5],  float dim_lato, unsigned int indici[], string filename) {
    //La griglia è NxN punti
    //Quindi (N-1)x(N-1) quadrati

    

    string line;
    string cell;

    ifstream mappa_di_altezze(filename);

    if (!mappa_di_altezze.is_open())
    {
        perror("Impossibile aprire la mappa di altezze");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < N_PUNTI; i++) {

        getline(mappa_di_altezze, line);
        stringstream lineStream(line);

        for (int j = 0; j < N_PUNTI; j++) {

            getline(lineStream, cell, ',');

            grid[(i * N_PUNTI + j)*5 + 0] = i * dim_lato - dim_lato * (N_PUNTI - 1) / 2;
            grid[(i * N_PUNTI + j)*5 + 1] = stof(cell) * MAX_HEIGHT;
            grid[(i * N_PUNTI + j)*5 + 2] = j * dim_lato - dim_lato * (N_PUNTI - 1) / 2;
            grid[(i * N_PUNTI + j) * 5 + 3] = float(i) / PPT;
            grid[(i * N_PUNTI + j) * 5 + 4] = float(j) / PPT;
        }
    }

    for (int i = 0; i < N_PUNTI - 1; i++)
    {
        for (int j = 0; j < N_PUNTI - 1; j++)
        {
            indici[6 * (N_PUNTI - 1) * i + 6 * j + 0] = i * N_PUNTI + j + 0;
            indici[6 * (N_PUNTI - 1) *i + 6 * j + 1] = i * N_PUNTI + j + 1;
            indici[6 * (N_PUNTI - 1) * i + 6 * j + 2] = i * N_PUNTI + j + N_PUNTI;

            indici[6 * (N_PUNTI - 1) * i + 6 * j + 3] = i * N_PUNTI + j + 1;
            indici[6 * (N_PUNTI - 1) * i + 6 * j + 4] = i * N_PUNTI + j + N_PUNTI + 1;
            indici[6 * (N_PUNTI - 1) * i + 6 * j + 5] = i * N_PUNTI + j + N_PUNTI;
        }
    }

    return grid;
}

glm::vec2 genCyrcle(float t) {
    float r = 470.0f;

    float x = r * cos(t);
    float y = r * sin(t);   //Z

    return glm::vec2(x, y);
}

void genRoad(float road[], unsigned int ind_road[], float width, int numPunti) {
    //numPunti è il numero di punti di cui è composto il cerchio base usato per creare la strada
    //In tutto, per la strada saranno generati (numPunti)*2 punti

    int dim = (numPunti * 2) * 5;

    float passo = 2 * M_PI / numPunti;
    float t = 0.0f;

    int k = 0;

    for (int i = 0; i < numPunti; i++) {
        t = i * passo;

        glm::vec2 centro = genCyrcle(t);                    //Calcola il punto centrale della strada
        glm::vec2 nextCentro = genCyrcle(t + passo);        //Serve a calcolare la normale

        glm::vec2 tangente = glm::normalize(nextCentro - centro);
        glm::vec2 normale = glm::vec2(-tangente.y, tangente.x);

        glm::vec2 sx = centro + normale * (width / 2.0f);   //Vertice a "sx" del punto centrale
        glm::vec2 dx = centro - normale * (width / 2.0f);   //Vertice a "dx" del punto centrale

        road[k + 0] = sx.x;
        road[k + 1] = 0.01f;
        road[k + 2] = sx.y;
        road[k + 3] = 0.0f; 
        road[k + 4] = t; 
        
        road[k + 5] = dx.x;
        road[k + 6] = 0.01f;
        road[k + 7] = dx.y;
        road[k + 8] = 1.0f; 
        road[k + 9] = t; 

        k += 10;
    }

    //Calcolo dei punti per l'IndexBuffer
    for (int i = 0; i < numPunti - 1; i++) {
        ind_road[(i * 6) + 0] = (i * 2) + 1;
        ind_road[(i * 6) + 1] = (i * 2) + 0;
        ind_road[(i * 6) + 2] = (i * 2) + 2;

        ind_road[(i * 6) + 3] = (i * 2) + 1;
        ind_road[(i * 6) + 4] = (i * 2) + 2;
        ind_road[(i * 6) + 5] = (i * 2) + 3;
    }

    ind_road[((numPunti - 1) * 6) + 0] = ((numPunti - 1) * 2) + 1;
    ind_road[((numPunti - 1) * 6) + 1] = ((numPunti - 1) * 2) + 0;
    ind_road[((numPunti - 1) * 6) + 2] = 0;

    ind_road[((numPunti - 1) * 6) + 3] = ((numPunti - 1) * 2) + 1;
    ind_road[((numPunti - 1) * 6) + 4] = 0;
    ind_road[((numPunti - 1) * 6) + 5] = 1;
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

    glEnable(GL_MULTISAMPLE);
    glActiveTexture(GL_TEXTURE0);

    //Per evitare di duplicare i punti nell'array positions, si crea un indexBuffer, in cui si indicano le posizioni
    //dei punti da usare per creare ciascun elemento
    //PER L'INDEX BUFFER
    unsigned int indici[(N_PUNTI - 1) * (N_PUNTI - 1) * 6];

    float lato = 10.f;

	float move_speed = 2.f;

    float positions[N_PUNTI * N_PUNTI * 5];
    genGrid(positions, lato, indici, "res/maps/mappa_altezze_modificato.csv");


    //Funzione di blend per far funzionare anche le texture semi-trasparenti
    //glBlendFunc(GL_SRC0_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_BLEND);

    //Si crea il vertex array object. Questo è un oggetto alla quale poi si collega il buffer e l'indexBuffer
    //Il VertexArrayObject è una specie di array che contiene tutti i buffer che vengono generati e li collega insieme
    VertexArray va_terrain;

    //Crea il VertexBuffer
    VertexBuffer vb_terrain(positions, N_PUNTI * N_PUNTI * 5 * sizeof(float));

    //Crea l'oggetto che definisce come è strutturato il vertexBuffer
    //Specifica quanti attributi hanno i vertici (in questo caso 2, ovvero le coordinate x e y)
    VertexBufferLayout layout;
    layout.push(GL_FLOAT, 3);
    //Aggiungo altri 2 attributi ad ogni vertice (le coordinate delle texture)
    layout.push(GL_FLOAT, 2);

    //Nel VertexArray, collega il vertexBuffer ai layout che ho definito
    va_terrain.addBuffer(vb_terrain, layout);


    //Crea l'IndexBuffer
    //Specifica, tramite un array di indici, quali vertici compongono ciascun oggetto da renderizzare
    IndexBuffer ib_terrain(indici, (N_PUNTI - 1) * (N_PUNTI - 1) * 6);

    //Crea una matrice di proiezione 4x4
    //rispetto ai lati della finestra: (sinistra, destra, sotto, sopra, vicino, lontano)
    //Tutti i vertici verranno poi moltiplicati per questa matrice
    //Tutti i vertici che si trovano fuori da questi limiti non saranno visualizzati
    //La finestra che ho creato ha un rapporto 4:3 e non è quadrata, quindi devo convertire anche i miei vertici in 4:3
    //in modo che le distanze tra loro non vengano alterate
    //glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f); //Matrice con formato 4:3

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1.0f, 100.0f, 4000.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 500.0f, -1500.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

    //La view matrix riguarda la posizione della telecamera
    //In realtà si simula lo spostamento della telecamera spostando gli oggetti nella scena
    //Si crea una matrice di traslazione
    //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0, 0, 0));    //Num negativo --> Telecamera verso destra
                                                                                //Num positivo --> Telecamera verso

    Shader shader("Shader/vertexfragment.shader");
    shader.bind();

    float a_color[3] = { 0.15f,0.15f,0.15f };   //Ambient
    float d_color[3] = { 0.1f,0.1f,0.1f };      //Diffuse
    float s_color[3] = { 0.1f,0.1f,0.1f };      //Specular
    //float e_color[3] = { 0.5f,0.1f,0.2f };      //Emissive
    float sun_color[3] = { 0.2f,0.2f,0.2f };      //Sun Color
    float shininess = .0f;
    float lamp_brightness = 5.f;
    glm::vec3 Ldir(0.0f, 1.0f, 0.0f);
    shader.setUniform3f("uAmbientColor", a_color);
    shader.setUniform3f("uDiffuseColor", d_color);
    shader.setUniform3f("uSpecularColor", s_color);
    //shader.setUniform3f("uEmissiveColor", e_color);
    shader.setUniform3f("uSunColor", sun_color);
    shader.setUniform1f("uShininess", shininess);
    shader.setUniform3f("uLDir", Ldir.x, Ldir.y, Ldir.z);
    shader.setUniform1f("lamp_brightness", lamp_brightness);

    int num_lights = 8;
    glm::vec3 light_pos[NUM_LIGHTS]{
        glm::vec3(437.f, 75.f, 0.f),
        glm::vec3(-437.f, 75.f, 0.f),
        glm::vec3(0.f, 75.f, 437.f),
        glm::vec3(0.f, 75.f, -437.f),

        glm::vec3(309.f, 75.f, 309.f),
        glm::vec3(-309.f, 75.f, 309.f),
        glm::vec3(309.f, 75.f, -309.f),
        glm::vec3(-309.f, 75.f, -309.f)
    };

    glm::vec3 light_color[NUM_LIGHTS]{
        glm::vec3(0.9f, 0.9f, 0.9f),
        glm::vec3(0.9f, 0.9f, 0.9f),
        glm::vec3(0.9f, 0.9f, 0.9f),
        glm::vec3(0.9f, 0.9f, 0.9f),
        glm::vec3(0.9f, 0.9f, 0.9f),
        glm::vec3(0.9f, 0.9f, 0.9f),
        glm::vec3(0.9f, 0.9f, 0.9f),
        glm::vec3(0.9f, 0.9f, 0.9f)
    };

    shader.setUniform3fv("uLightPos", num_lights, light_pos);
    shader.setUniform3fv("uLightColor", num_lights, light_color);
    shader.setUniform1i("numLights", num_lights);

    //Carica la texture dal file e la collega
    Texture texture("res/textures/grass_tile.png");
    texture.bind(0);

    //Uniform da passare allo shader per renderizzare la texture
    //Lo uniform conterrà la texture che abbiamo inserito nello slot specificato (con il texture.bind())
    shader.setUniform1i("u_texture", 0);

    //Si toglie tutto perché è meglio collegarli frame per frame
    //Immagino che torna comodo quando c'è da fare cambiamenti tra frame
    shader.unBind();
    va_terrain.unBind();
    vb_terrain.Unbind();
    ib_terrain.unBind();

    Renderer renderer;

    ImGui::CreateContext();                     //ImGui è un'API per modificare la scena in tempo reale
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();

    glm::vec3 translation(0.0, 0, 0);
    float rot = 0.0f;
    float speed = 0.0f;
    glActiveTexture(GL_TEXTURE2);

    //--------------------------------------
    //CARICAMENTO GLTF (MACCHINA)
    //--------------------------------------
    gltf_loader gltfL_car; 
    
    box3 bbox_car;
    vector <renderable> car;

    gltfL_car.load_to_renderable("res/glbModels/simple_sport_car.glb", car, bbox_car);

    //------------------------------------------
    //CARICAMENTO GLTF (LAMPIONE)
    //------------------------------------------
    gltf_loader gltfL_lamp;

    box3 bbox_lamp;
    vector <renderable> lamp;
    gltfL_lamp.load_to_renderable("res/glbModels/eng_street_lamp.glb", lamp, bbox_lamp);

    //------------------------------------------
    //CARICAMENTO GLTF (ALBERO)
    //------------------------------------------
    gltf_loader gltfL_tree;

    box3 bbox_tree;
    vector <renderable> tree;
    gltfL_tree.load_to_renderable("res/glbModels/pine_tree.glb", tree, bbox_tree);

    //------------------------------------------


    //Creazione della strada
    float road[(P_CERCH * 2) * 5];
    unsigned int ind_road[P_CERCH * 2 * 3];
    genRoad(road, ind_road, 50.0f, P_CERCH);

    VertexArray va_road;
    VertexBuffer vb_road(road, ((P_CERCH * 2) * 5) * sizeof(float));

    va_road.addBuffer(vb_road, layout);

    IndexBuffer ib_road(ind_road, P_CERCH * 2 * 3);

    Texture tex_road("res/textures/street_tile.png");
    tex_road.bind(1);

    //Fattore di scalatura per la macchina 
    float scale_factor = 0.25f;

    glm::vec3 objTranslation(1875.0f, 50.f, 0.0f);
    //objTranslation = glm::vec3(0.0f, 300.0f, 0.0f);
    float objRot = 1.0f;

    bool camera_lock = false;
    glm::vec3 old_pos(0.f, 0.f, 0.f);

    glm::vec3 tree_pos[TREE_COUNT] = {
        glm::vec3(-1.f, 8.5f, 1.f),
        glm::vec3(3.f, 0.f, -17.f),
        glm::vec3(-10.f, 4.f, -10.f),
        glm::vec3(10.f, 3.75f, 3.f),
        glm::vec3(-18.f, 0.f, 5.f),
        glm::vec3(19.f, 0.f, 5.f),
        glm::vec3(19.f, 0.f, -5.f),
        glm::vec3(0.f, 5.f, -7.f),
        glm::vec3(7.f, 3.75f, 8.f),
        glm::vec3(-6.f, 4.f, 10.f)
    };

    while (!glfwWindowShouldClose(window))
    {
        //Pulisci il buffer dei colori (Preparazione dell'ambiente di rendering)
        renderer.clear();
        
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplGlfwGL3_NewFrame();

        //La model matrix sposta gli oggetti nella scena 
        glm::mat4 model = glm::mat4(1.0f);
        rot = rot + speed;
        view = glm::translate(view, translation);
        translation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 view_trasl(view[3][0], view[3][1], view[3][2]);
        view = glm::translate(glm::rotate(glm::translate(view, view_trasl), rot, glm::vec3(.0f, 1.f, .0f)), -view_trasl);

        rot = 0.0f;
        glm::mat4 mvp = proj * view * model;
        shader.setUniformMat4f("u_MVP", mvp);
        shader.setUniformMat4f("uModel", model);

        //Rendering del terreno
        shader.setUniform1i("u_texture", 0);
        renderer.draw(va_terrain, ib_terrain, shader);

        //Rendering della strada
        shader.setUniform1i("u_texture", 1);
        renderer.draw(va_road, ib_road, shader);

        //Rendering di una macchina
        objRot -= 0.5f;
        glm::mat4 model_car;
        for (unsigned int i = 0; i < car.size(); ++i) {
            //Utilizza i buffer dell'oggetto
            car[i].bind();

            // Binding delle texture
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, car[i].mater.base_color_texture);
            shader.setUniform1i("u_texture", 2);

            //Scala l'oggetto
            model_car = glm::scale(glm::mat4(1.0f), glm::vec3(scale_factor, scale_factor, scale_factor));
            //Ruota l'oggetto progresstivamente
            model_car = glm::rotate(model_car, glm::radians(objRot), glm::vec3(0.0f, 1.0f, 0.0f));
            //Trasla l'oggetto nella posizione desiderata
            model_car = glm::translate(model_car, objTranslation);
            //Applica le trasformazioni specifiche dell'oggetto
            model_car = model_car * car[i].transform;

            //Crea la matrice ModelViewProjection della macchina
            mvp = proj * view * model_car;
            shader.setUniformMat4f("u_MVP", mvp);
            shader.setUniformMat4f("uModel", model_car);
            //shader.setUniformMat4f("uProj", proj);
            

            glDrawElements(car[i]().mode, car[i]().count, car[i]().itype, 0);
        }

        glm::mat4 model_lamp;
        for (int j = 0; j < 8; j++) {
            for (unsigned int i = 1; i < lamp.size(); ++i) {
                lamp[i].bind();

                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, lamp[i].mater.base_color_texture);
                shader.setUniform1i("u_texture", 3);

                model_lamp = glm::scale(glm::mat4(1.0f), glm::vec3(10, 10, 10));
                model_lamp = glm::rotate(model_lamp, glm::radians((float)((360 / 8) * j)), glm::vec3(0.f, 1.f, 0.f));
                model_lamp = glm::translate(model_lamp, glm::vec3(44.f, 0.f, 0.f));
                model_lamp = model_lamp * lamp[i].transform;

                mvp = proj * view * model_lamp;
                shader.setUniformMat4f("u_MVP", mvp);
                shader.setUniformMat4f("uModel", model_lamp);

                glDrawElements(lamp[i]().mode, lamp[i]().count, lamp[i]().itype, 0);
            }
        }

        glm::mat4 model_tree;
        for (int j = 0; j < TREE_COUNT; j++) {
            for (unsigned int i = 0; i < tree.size(); ++i) {
                tree[i].bind();

                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, tree[i].mater.base_color_texture);
                shader.setUniform1i("u_texture", 4);

                model_tree = glm::scale(glm::mat4(1.0f), glm::vec3(20, 20, 20));
                model_tree = glm::translate(model_tree, tree_pos[j]);
                model_tree = model_tree * lamp[i].transform;

                mvp = proj * view * model_tree;
                shader.setUniformMat4f("u_MVP", mvp);
                shader.setUniformMat4f("uModel", model_tree);

                glDrawElements(tree[i]().mode, tree[i]().count, tree[i]().itype, 0);
            }
        }

        {   //Finestra di ImGui
            //Gestione della posizione della scena con i tasti della tastiera
            if (!camera_lock) {
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                    //translation.z -= 2.0f;

					translation.x = view[0][2] * move_speed;
					translation.z = view[2][2] * move_speed;
                }
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
					translation.x = -view[0][2] * move_speed;
					translation.z = -view[2][2] * move_speed;
                }
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                    translation.x = view[0][0] * move_speed;
					translation.z = view[2][0] * move_speed;
                }
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
					translation.x = -view[0][0] * move_speed;
					translation.z = -view[2][0] * move_speed;
                }
                if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                    translation.y += 1.0f * move_speed;
                }
                if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                    translation.y -= 1.0f * move_speed;
                }

                //Gestione della rotazione della scena con i tasti della tastiera
                if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
                    speed = -0.005f;
                }
                else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
                    speed = 0.005f;
                }
                else {
                    speed = 0.0f;
                }
            }

            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
                camera_lock = false;
                view = glm::lookAt(glm::vec3(0.0f, 500.0f, -1500.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
            }
            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
                camera_lock = false;
                view = glm::lookAt(glm::vec3(-360.0f, 55.0f, -500.0f), glm::vec3(-350.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
            }
            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
                speed = 0;
                camera_lock = true;
            }

            glm::vec3 car_pos(model_car[3][0], model_car[3][1], model_car[3][2]);
            if (camera_lock) {
                
                view = glm::lookAt((car_pos - ((car_pos - old_pos) * 40.f)) + glm::vec3(0.f, 75.f, 0.f), car_pos, glm::vec3(0.f, 1.f, 0.f));
                
            }
            old_pos = car_pos;

            //Testi visualizzati da ImGui per l'utente
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("----------------------------------------------------");
            ImGui::Text("Sposta la telecamera con WASD");
            ImGui::Text("Ruota la telecamera con ZX");
            ImGui::Text("Alza/abbassa la telecamera con UP/DOWN");
            ImGui::Text("----------------------------------------------------");
            ImGui::Text("GESTIONE INQUADRATURE (Premi il numero corrispondente:");
            ImGui::Text("PREDEFINITO    --> 1");
            ImGui::Text("STRADA (FIXED) --> 2");
            ImGui::Text("MACCHINA       --> 3");
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