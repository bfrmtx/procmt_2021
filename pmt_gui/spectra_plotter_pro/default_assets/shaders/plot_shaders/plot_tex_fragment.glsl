#version 330 core

in  vec2 fg_tex;
flat in float fg_a;
out vec4 out_color;

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

uniform sampler2D sampler;
uniform float layer = 0.0f;
uniform vec4 color_normal = vec4(0.0f, 0.0f, 1.0f, 1.0f);
uniform vec4 color_select = vec4(1.0f, 0.0f, 0.0f, 1.0f);
uniform vec4 color_hidden = vec4(0.7f, 0.7f, 0.7f, 1.0f);

void main () {
    vec4 col = fg_a * color_select + (1.0f - fg_a) * color_normal;

    out_color.rgb = (1.0 - texture(sampler, fg_tex).rgb) * col.rgb;
    out_color.a = texture(sampler, fg_tex).a;
}
