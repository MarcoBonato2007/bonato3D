#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

struct Shader {
    GLuint ID;

    Shader (const char* vertex_shader_path, const char* fragment_shader_path) {
        std::string vertex_shader_string;
        std::string fragment_shader_string;
        std::ifstream vertex_shader_file;
        std::ifstream fragment_shader_file;

        try {
            vertex_shader_file.open(vertex_shader_path);
            fragment_shader_file.open(fragment_shader_path);
            std::stringstream vertex_shader_stream, fragment_shader_stream;
            // read file's buffer contents into streams
            vertex_shader_stream << vertex_shader_file.rdbuf();
            fragment_shader_stream << fragment_shader_file.rdbuf();		
            // close file handlers
            vertex_shader_file.close();
            fragment_shader_file.close();
            // convert stream into string
            vertex_shader_string = vertex_shader_stream.str();
            fragment_shader_string = fragment_shader_stream.str();
        }
        catch(const std::ifstream::failure &e) {
            std::cout << "Shader file not read succesfully.";
        }

        const char* vertex_shader_code = vertex_shader_string.c_str();
        const char* fragment_shader_code = fragment_shader_string.c_str();

        // 2. compile shaders
        GLuint vertex_shader, fragment_shader;
        int success;
        char infoLog[512];
        
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
        glCompileShader(vertex_shader);
        // print compile errors if any
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
            std::cout << "Vertex shader compilation failed.\n" << infoLog << std::endl;
        };

        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
        glCompileShader(fragment_shader);
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
            std::cout << "Fragment shader compilation failed.\n" << infoLog << std::endl;
        };
        
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex_shader);
        glAttachShader(ID, fragment_shader);
        glLinkProgram(ID);
        // print linking errors if any
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "Shader linking failed.\n" << infoLog << std::endl;
        }
        
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }

    void execute() {
        glUseProgram(ID);
    }
};

#endif
