#include "Shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Renderer.h"

Shader::Shader(const string& filepath): m_filePath(filepath), m_rendererID(0){

    ShaderProgramSource source = ParseShader(filepath);
    //Crea lo shader, realizzando un program che contiene sia il vertexShader che il fragmentShader
    m_rendererID = CreateShader(source.vertexSource, source.fragmentSource);


}

Shader::~Shader(){
    glDeleteProgram(m_rendererID);
}

ShaderProgramSource Shader::ParseShader(const string& filepath) {
    //Legge il file che contiene lo shader linea per linea ed estrapola i due shader
    ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    string line;
    stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
        if (line.find("#shader") != string::npos) {

            if (line.find("vertex") != string::npos) {
                //Questo è il vertex shader
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != string::npos) {
                //Questo è il fragment shader
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

int Shader::CompileShader(unsigned int type, const string& source) {
    //Crea lo shader del tipo specificato
    unsigned int id = glCreateShader(type);
    //Puntatore all'inizio della stringa
    const char* src = source.c_str();

    //Indica qual è il codice che lo shader deve eseguire
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    //Controllo errori
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)malloc(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        cout << "ERRORE: Compilazione dello shader fallita!" << endl << message << endl;
        glDeleteShader(id);
        free(message);
        return 0;
    }

    return id;
}

unsigned int Shader::CreateShader(const string& vertexShader, const string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void Shader::bind() const{
    glUseProgram(m_rendererID);
}

void Shader::unBind() const {
    glUseProgram(0);
}

void Shader::setUniform4f(const string& name, float v0, float v1, float v2, float v3) {
    glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

void Shader::setUniform1i(const string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniformMat4f(const string& name, const glm::mat4& matrix) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

int Shader::getUniformLocation(const string& name){

    if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end()) {
        return m_uniformLocationCache[name];
    }

    int location = glGetUniformLocation(m_rendererID, name.c_str());

    if (location == -1) {
        cout << "ATTENZIONE: l'uniform " << name << " non esiste!" << endl;
    }
    m_uniformLocationCache[name] = location;

    return location;
}
