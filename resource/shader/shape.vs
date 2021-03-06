#version 110

attribute vec3 _Position;
attribute vec2 _TexCoord;

uniform mat4 _mat1;
uniform mat4 ui_PerspectiveMatrix4x4;
uniform mat4 ui_ModelViewMatrix4x4;

void main(){
	gl_Position = ui_PerspectiveMatrix4x4 * ui_ModelViewMatrix4x4 * _mat1*vec4(_Position, 1.0);
}
