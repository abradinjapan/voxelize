#version 330 core

// inputs
layout (location = 0) in uint GLOBAL_compact_coords;
layout (location = 1) in uint GLOBAL_compact_styling;

// texture uniforms
uniform sampler2DArray GLOBAL_current_texture_unit;

// offset uniforms
uniform mat4 GLOBAL_transform;

// outputs
out float GLOBAL_texture_x;
out float GLOBAL_texture_y;
out float GLOBAL_texture_z;

// take compact coords uint and extract x, y, z, texture_x, texture_y
void VERTEX_decompress_coords(in uint compact_coords, out float x, out float y, out float z, out float texture_x, out float texture_y) {
    const float scale = 1.0f;

    // decompress
    x = float(uint(compact_coords & uint(255))) * scale;
    y = float(uint(compact_coords >> uint(8)) & uint(255)) * scale;
    z = float(uint(compact_coords >> uint(16)) & uint(255)) * scale;
    texture_x = float(uint(compact_coords >> uint(24)) & uint(15)) / 15.0f;
    texture_y = 15.0f - (float(uint(compact_coords >> uint(28)) & uint(15)) / 15.0f);

    return;
}

// take compact styling uint and extract texture_z
void VERTEX_decompress_styling(in uint compact_styling, out float texture_z) {
    // decompress
    texture_z = float(uint(compact_styling & uint(65535)));

    return;
}

// shader entry point
void main() {
    // opengl vertex positions
    float x;
    float y;
    float z;

    // decompress the vbo input
    VERTEX_decompress_coords(GLOBAL_compact_coords, x, y, z, GLOBAL_texture_x, GLOBAL_texture_y);
    VERTEX_decompress_styling(GLOBAL_compact_styling, GLOBAL_texture_z);

    // apply transformation
    gl_Position = GLOBAL_transform * vec4(x, y, z, 1.0f);
    
    return;
}
