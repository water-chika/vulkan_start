#version 460

layout(location=0) in vec2 vertex;

void main() {
    vec2 pos = vertex.xy;
    gl_Position = vec4(pos, 0, 0.5);
}