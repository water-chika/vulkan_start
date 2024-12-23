#version 460
#extension GL_EXT_mesh_shader : enable

layout(local_size_x=1, local_size_y=1) in;
layout(max_primitives=1, max_vertices=3) out;
layout(triangles) out;

layout(binding=0) uniform Buffer{
    uint index;
} Frame;

layout(location=0) out vec3 color[];

void main() {
    gl_MeshVerticesEXT[0].gl_Position = vec4(0,0,0,1);
    color[0] = vec3(1,0,0);
    gl_MeshVerticesEXT[1].gl_Position = vec4(1,1,0,1);
    color[1] = vec3(0,0,1.0);
    gl_MeshVerticesEXT[2].gl_Position = vec4(1,0,0,1);
    color[2] = vec3(0,1,0);

    gl_PrimitiveTriangleIndicesEXT[0] = uvec3(0, 1, 2);
    SetMeshOutputsEXT(3, 1);
}
