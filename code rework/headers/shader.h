#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

struct Shader {
    GLuint ID;
    GLuint look_at_location;
    GLuint projection_location;

    Shader() {}
    Shader(const char* vertex_shader_path, const char* fragment_shader_path) {
        // 1. read shader files
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
        int success;
        char infoLog[512];
        
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
        glCompileShader(vertex_shader);
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success); // print any compile errors
        if(!success)
        {
            glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
            std::cout << "Vertex shader compilation failed.\n" << infoLog << std::endl;
        };

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
        glCompileShader(fragment_shader);
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
            std::cout << "Fragment shader compilation failed.\n" << infoLog << std::endl;
        };
        
        // 3. create shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex_shader);
        glAttachShader(ID, fragment_shader);
        glLinkProgram(ID);

        glGetProgramiv(ID, GL_LINK_STATUS, &success); // print any linking errors
        if(!success)
        {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "Shader linking failed.\n" << infoLog << std::endl;
        }

        look_at_location = glGetUniformLocation(ID, "look_at");
        projection_location = glGetUniformLocation(ID, "projection");

        glDeleteShader(vertex_shader); // can be deleted since they're linked to our program now
        glDeleteShader(fragment_shader);
    }

    void execute() {
        glUseProgram(ID);
    }
};

#endif
