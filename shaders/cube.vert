#version 460

layout(location=0) in vec3 vertex;
layout(location=0) out vec3 color;

layout(binding=0) uniform Buffer{
    uint index;
} Frame;
void main() {
    float time_in_s = Frame.index * 0.001;
    float theta = time_in_s*3.14/4;
    mat4 rotate_z = mat4(
        cos(theta), -sin(theta), 0, 0,
        sin(theta), cos(theta), 0, 0,
        0, 0, 1, 0,
        0,0,0,1
    );
    mat4 rotate_y = mat4(
        cos(theta), 0, -sin(theta), 0,
        0, 1, 0, 0,
        sin(theta), 0, cos(theta), 0,
        0, 0, 0, 1
    );
    mat4 move = mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, +4, 1
    );
    mat4 persp = mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 1,
        0, 0, 0, 1
    );
    gl_Position = persp * move * rotate_y * rotate_z * vec4(vertex, 1);
    color = (vertex+1)/2;
}
