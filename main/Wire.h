#ifndef R64FX_MAIN_WIRE_H
#define R64FX_MAIN_WIRE_H

#include "Socket.h"

namespace r64fx{
    
class Wire{
    static ShadingProgram shading_program;
    
    /** 
        vec4
        
            x, y : vertex coordinates.
            z, w : texture coordinates. 
     */
    static GLint vertex_data_attribute;

    static GLint color_uniform;
    static GLint sampler_uniform;
    
    static GLuint sampler;
    static GLuint texture;
    
    static int node_count;
    
    static void init_shader();
    
    static void init_textures();

    enum{
        Body = 0,
        Cap = 1,
        Cap1 = 1,
        Cap2 = 2
    };
    
    GLuint vao[max_rendering_context_count][3];
    GLuint vbo[3];
    
    void calculate_perpendicular_points(Point<float> in_a, Point<float> in_b, Point<float> in_c, Point<float> &out_a, Point<float> &out_b);

    void calculate_cap_coordinates(Point<float> a, Point<float> b, Point<float> &c, Point<float> &d);
    
    void init_cap_vertices(int item, Point<float> v1, Point<float> v2);

    std::vector<Point<float>> control_points;
    std::vector<Point<float>> wire_points;
    
    Socket* _source_socket = nullptr;
    Socket* _sink_socket = nullptr;
    
public:
    static void init();
    
    Wire(Socket* source_socket, Socket* sink_socket);
    
    void setupForContext(RenderingContextId_t context_id);
    
    void cleanupForContext(RenderingContextId_t context_id);
    
    Color color;
    
    int wire_width = 10;
    
    void update();
    
    void render(RenderingContextId_t context_id);
};
    
}//namespace r64fx

#endif//R64FX_MAIN_WIRE_H