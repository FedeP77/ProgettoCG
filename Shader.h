#pragma once

#include <string>
#include <unordered_map>
#include "Librerie/GLM/glm.hpp"

using namespace std;

struct ShaderProgramSource {
	string vertexSource;
	string fragmentSource;
};

class Shader {
private:
	string m_filePath;
	unsigned int m_rendererID;
	unordered_map<string, int> m_uniformLocationCache;
public:
	Shader(const string& filepath);
	~Shader();

	void bind() const;
	void unBind() const;

	//SetUniforms
	void setUniform4f(const string& name, float v0, float v1, float v2, float v3);
	void setUniform1i(const string& name, int value);
	void setUniformMat4f(const string& name, const glm::mat4& matrix);

private:
	ShaderProgramSource ParseShader(const string& filepath);
	int CompileShader(unsigned int type, const string& source);
	unsigned int CreateShader(const string& vertexShader, const string& fragmentShader);
	int getUniformLocation(const string& name);

};
