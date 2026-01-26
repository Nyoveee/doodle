#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

#include "../AndroidUtils/AndroidOut.h"
#include "Model.h"

#include "../Game/GameObject/GameObject.h"

namespace {
    std::string readAssetText(AAssetManager* assetManager, const char* filename) {
        if (assetManager == nullptr) {
            // Handle error: Asset manager is null
            return "";
        }

        AAsset* asset = AAssetManager_open(assetManager, filename, AASSET_MODE_BUFFER);
        if (asset == nullptr) {
            // Handle error: Could not open asset
            return "";
        }

        // Get the file size
        off_t fileSize = AAsset_getLength(asset);

        // Read the file content into a buffer
        std::vector<char> buffer(fileSize + 1); // +1 for null-terminator
        AAsset_read(asset, buffer.data(), fileSize);

        // Null-terminate the string (important for text files)
        buffer[fileSize] = '\0';

        // Close the asset
        AAsset_close(asset);

        // Convert the buffer to a std::string and return
        return std::string(buffer.data());
    }
}

Shader* Shader::loadShader(
        const std::string &vertexSource,
        const std::string &fragmentSource) {
    Shader *shader = nullptr;

    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader) {
        return nullptr;
    }

    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader) {
        glDeleteShader(vertexShader);
        return nullptr;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);

        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        if (linkStatus != GL_TRUE) {
            GLint logLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

            // If we fail to link the shader program, log the result for debugging
            if (logLength) {
                GLchar *log = new GLchar[logLength];
                glGetProgramInfoLog(program, logLength, nullptr, log);
                aout << "Failed to link program with:\n" << log << std::endl;
                delete[] log;
            }
            glDeleteProgram(program);
        } else {
            shader = new Shader(program);
        }
    }

    // The shaders are no longer needed once the program is linked. Release their memory.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shader;
}

Shader* Shader::loadShader(
        const char* vertexPath,
        const char* fragmentPath,
        AAssetManager *assetManager) {
    // using the asset manager, load the respective vertex and fragment shader..
    return Shader::loadShader(
            readAssetText(assetManager, vertexPath),
            readAssetText(assetManager, fragmentPath)
            );
}

GLuint Shader::loadShader(GLenum shaderType, const std::string &shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        auto *shaderRawString = (GLchar *) shaderSource.c_str();
        GLint shaderLength = shaderSource.length();
        glShaderSource(shader, 1, &shaderRawString, &shaderLength);
        glCompileShader(shader);

        GLint shaderCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);

        // If the shader doesn't compile, log the result to the terminal for debugging
        if (!shaderCompiled) {
            GLint infoLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);

            if (infoLength) {
                auto *infoLog = new GLchar[infoLength];
                glGetShaderInfoLog(shader, infoLength, nullptr, infoLog);
                aout << "Failed to compile with:\n" << infoLog << std::endl;
                delete[] infoLog;
            }

            glDeleteShader(shader);
            shader = 0;
        }
    }
    return shader;
}

void Shader::activate() const {
    glUseProgram(program_);
}

void Shader::setBool(const std::string& name, bool const value) const {
    glUniform1i(glGetUniformLocation(program_, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string& name, int const value) const {
    glUniform1i(glGetUniformLocation(program_, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float const value) const {
    glUniform1f(glGetUniformLocation(program_, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, glm::vec2 const& list) const {
    glUniform2f(glGetUniformLocation(program_, name.c_str()), list[0], list[1]);
}

void Shader::setMatrix(const std::string& name, const glm::mat4x4& matrix, bool transpose) const {
    glUniformMatrix4fv(glGetUniformLocation(program_, name.c_str()), 1, transpose, glm::value_ptr(matrix));
}

void Shader::setImageUniform(const std::string& name, int uniform) const {
    glUniform1i(glGetUniformLocation(program_, name.c_str()), uniform);
}

void Shader::setVec3(const std::string& name, glm::vec3 const& list) const {
    glUniform3f(glGetUniformLocation(program_, name.c_str()), list[0], list[1], list[2]);
}

void Shader::setVec2(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(program_, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(program_, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, glm::vec4 const& list) const {
    glUniform4f(glGetUniformLocation(program_, name.c_str()), list[0], list[1], list[2], list[3]);
}