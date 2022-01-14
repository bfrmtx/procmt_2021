#version 330 core

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

layout(location = $loc_position_x$) in float in_x;
layout(location = $loc_position_y$) in float in_y;
layout(location = $loc_add_1$) in float in_a;
flat out float fv_a;

void main() {
    gl_PointSize = 3.0;

    if(type == 1) {
        fv_a = in_a;
        gl_Position.x = 1.0 - ((topright.x - in_x) / resolution_plot.x) * 2.0;
        gl_Position.y = 1.0 - ((topright.y - in_y) / resolution_plot.y) * 2.0;
        gl_Position.z = in_a;
        gl_Position.w = 1.0;
    } else if(type == 2) {
        fv_a = in_a;
        gl_Position.x = -1.0 + ((topright.x - in_x) / resolution_plot.x) * 2.0;
        gl_Position.y =  1.0 - ((topright.y - in_y) / resolution_plot.y) * 2.0;
        gl_Position.z = in_a;
        gl_Position.w = 1.0;
    }
}
