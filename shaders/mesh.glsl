#version 460
#extension GL_EXT_mesh_shader : enable

const int count = 128;

layout(local_size_x=1, local_size_y=1) in;
layout(max_vertices=2*count, max_primitives=count) out;
layout(lines) out;

layout(binding=0) uniform Buffer{
    uint index;
} Frame;

layout(location=0) out vec3 color[];

void main() {
    float time_in_s = Frame.index * 0.001;
    float theta = time_in_s * 3.14/4;
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
    mat4 transform = persp * move * rotate_y * rotate_z; 

    const int t = int(gl_WorkGroupID.x);
    const int t_sign = (t%2 == 1) ? 1 : -1;
    const int t_ = t_sign * ((t+1)/2);
    const float t_width = 10.0;

    const float t_start = t_width * t_;
    const float t_end   = t_width * t_ + t_width;
    vec3 values[count+1];
    for (int i = 0; i < count+1; i++) {
        float t = t_start + i*(t_end-t_start)/count;
        values[i].x = cos(t);
        values[i].y = sin(t);
        values[i].z = t;
    }
    for (int i = 0; i < count; i++) {
        gl_MeshVerticesEXT[2*i].gl_Position = transform * vec4(values[i].x,values[i].y,values[i].z,1);
        color[2*i] = vec3(1,1,1);
        gl_MeshVerticesEXT[2*i+1].gl_Position = transform * vec4(values[i+1].x,values[i+1].y,values[i+1].z,1);
        color[2*i+1] = vec3(1,1,1.0);

        gl_PrimitiveLineIndicesEXT[i] = uvec2(2*i, 2*i+1);
    }
    SetMeshOutputsEXT(2*count, count);
}
