#pragma once

//vertex shader
static const char* vShader =
R"(

#version 330
layout (location = 0) in vec3 pos;
uniform float xMove;
uniform float yMove;

void main(){
    gl_Position = vec4(0.5*pos.x + xMove, 0.5*pos.y+yMove, 0.5*pos.z, 1.0);	
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