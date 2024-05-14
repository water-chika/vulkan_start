#version 460

layout(location=0) in vec3 vertex;

layout(binding=0) uniform Buffer{
    uint index;
} Frame;
void main() {
    vec2 pos = vertex.xy;
    float theta = Frame.index*0.1;
    mat4 rotate = mat4(
        cos(theta), -sin(theta), 0, 0,
        sin(theta), cos(theta), 0, 0,
        0, 0, 1, 0,
        0,0,0,1
    );
    mat4 move = mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, +1, 1
    );
    mat4 persp = mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 1,
        0, 0, -1, 1
    );
    gl_Position = persp * move * rotate * vec4(pos, 0, 1);
}