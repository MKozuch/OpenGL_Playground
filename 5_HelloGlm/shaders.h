#pragma once

//vertex shader
static const char* vShader =
R"(

#version 330
layout (location = 0) in vec3 pos;
uniform mat4 model;

void main(){
    gl_Position = model * vec4(pos, 1.0);	
}

)";

//fragment shader
static const char* fShader =
R"(

#version 330
out vec4 color;

void main(){
   color = vec4(1.0, 0.0, 0.0, 0.5);
}

)";