#version 330 core

in vec4 test_color[];
flat in float out_a[];
out vec2 fg_tex;
flat out float fg_a;

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

layout (std140) uniform uniforms_matrices {
    mat4 view;                      // camera - viewmatrix
    mat4 projection;                // camera - projection matrix
    mat4 viewProjection;            // camera - viewprojection matrix
};

layout (std140) uniform uniforms_global {
    vec2 resolution;                // resolution of gl-canvas
    vec2 mousePosition;             // window-based mouse position
    vec2 glMousePosition;           // openGL-based mouse position
    vec2 random;                    // some random vec2 (both values between -1.0 and 1.0)
    vec2 mouseClickPosition;        // last mouseClick Position
    vec2 pixelSize;                 // pixelSize in OpenGLCoords
    vec2 bottomleft;
    vec2 topright;
    vec2 resolution_plot;
    float mouseTime;                // time since last mouseClick
    float timer;                    // time since program started (in seconds) resets every 3600 seconds
};

layout (std140) uniform uniforms_gui {
    vec4 color_borders;             // color of borders
    vec4 color_background;          // background color
    vec2 position;                  // (px) bottomLeft of boundingRectangle
    vec2 size;                      // (px) size of boundingRectangle
    vec2 cornersize;                // size of corners
    float borderwidth;              // px-borderwidth, can be negative, but shouldn't have any effect
    int type;                       // element-type
};

uniform float pixelx = 4.0f;
uniform float pixely = 4.0f;

void emitRectangle(vec4 pos) {
    float w = pixelSize.x * pixelx;
    float h = pixelSize.y * pixely;

    fg_a = out_a[0];

    fg_tex = vec2(0.0, 1.0);
    gl_Position = pos + vec4(-w, +h, 0.0, 0.0);
    EmitVertex();

    fg_tex = vec2(0.0, 0.0);
    gl_Position = pos + vec4(-w, -h, 0.0, 0.0);
    EmitVertex();

    fg_tex = vec2(1.0, 1.0);
    gl_Position = pos + vec4(+w, +h, 0.0, 0.0);
    EmitVertex();

    fg_tex = vec2(1.0, 0.0);
    gl_Position = pos + vec4(+w, -h, 0.0, 0.0);
    EmitVertex();
    EndPrimitive();
}

void main() {
    emitRectangle(gl_in[0].gl_Position);
}
