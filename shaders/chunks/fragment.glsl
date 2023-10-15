#version 330 core

// inputs
in float GLOBAL_texture_x;
in float GLOBAL_texture_y;
in float GLOBAL_texture_z;

// texture uniforms
uniform sampler2DArray GLOBAL_current_texture_unit;

// outputs
out vec4 GLOBAL_fragment_color;

// shader entry point
void main() {
    // setup fragment color
    GLOBAL_fragment_color = texture(GLOBAL_current_texture_unit, vec3(GLOBAL_texture_x, GLOBAL_texture_y, GLOBAL_texture_z));

    // transparency test
    if (GLOBAL_fragment_color.a < 1.0f) {
        discard;
    }

    return;
}
