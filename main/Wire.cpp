#include "Wire.h"
#include "gui/bezier.h"
#include "gui/TexturedRect.h"

#ifdef DEBUG
#include <iostream>
#include <assert.h>
using namespace std;
#endif//DEBUG

namespace r64fx{
    
ShadingProgram Wire::shading_program;
    
GLint Wire::vertex_data_attribute;

GLint Wire::color_uniform;
GLint Wire::sampler_uniform;

GLuint Wire::sampler;
GLuint Wire::texture;
        
int Wire::node_count = 32;

const int cap_vertex_count = 17;

void Wire::init()
{
    init_shader();
    init_textures();
}


void Wire::init_shader()
{
    VertexShader vertex_shader(
        #include "Wire.vert.h"
    );
    
#ifdef DEBUG
    if(!vertex_shader.isOk())
    {
        cerr << "Problem in vertex shader!\n";
        cerr << vertex_shader.infoLog() << "\n";
        abort();
    }
#endif//DEBUG
    
    FragmentShader fragment_shader(
        #include "Wire.frag.h"
    );
    
#ifdef DEBUG
    if(!fragment_shader.isOk())
    {
        cerr << "Problem in fragment shader!\n";
        cerr << fragment_shader.infoLog() << "\n";
        abort();
    }
#endif//DEBUG
    
    shading_program = ShadingProgram(vertex_shader, fragment_shader);
#ifdef DEBUG
    if(!shading_program.isOk())
    {
        cerr << "Problem in shading program!\n";
        cerr << shading_program.infoLog() << "\n";
        abort();
    }
#endif//DEBUG

    vertex_data_attribute = glGetAttribLocation(shading_program.id(), "vertex_data");
#ifdef DEBUG
    assert(vertex_data_attribute != -1);
#endif//DEBUG

    sampler_uniform = glGetUniformLocation(shading_program.id(), "sampler");
    color_uniform = glGetUniformLocation(shading_program.id(), "color");
#ifdef DEBUG
    assert(sampler_uniform != -1);
    assert(color_uniform != -1);
#endif//DEBUG
}



void Wire::init_textures()
{
    glGenTextures(1, &texture);
    
    const int tex_size = 32; //Size of a single side.
    const int c = tex_size / 2;
    
    unsigned char tex_bytes[tex_size*tex_size];

    for(int i=0; i<tex_size; i++)
    {
        tex_bytes[i] = 255 * sin((float(i) / float(tex_size)) * M_PI);
    }
    
    glBindTexture(GL_TEXTURE_1D, texture);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_1D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RED, tex_size, 0, GL_RED, GL_UNSIGNED_BYTE, tex_bytes);  
    
    glGenSamplers(1, &sampler);
    glBindSampler(texture, sampler);
}


Wire::Wire(Socket* source_socket, Socket* sink_socket)
: control_points(3, {0.0, 0.0})
, wire_points(node_count, {0.0, 0.0})
, _source_socket(source_socket)
, _sink_socket(sink_socket)
{
    for(int i=0; i<max_rendering_context_count; i++)
        for(int j=0; j<3; j++)
            vao[i][j] = 0;
    
    glGenBuffers(3, vbo);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[Body]);
    glBufferData(GL_ARRAY_BUFFER, node_count * 2 * 4 * sizeof(float), nullptr, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[Cap1]);
    glBufferData(GL_ARRAY_BUFFER, cap_vertex_count * 4 * sizeof(float), nullptr, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[Cap2]);
    glBufferData(GL_ARRAY_BUFFER, cap_vertex_count * 4 * sizeof(float), nullptr, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);    
}

Wire::~Wire()
{
}


void Wire::setupForContext(RenderingContextId_t context_id)
{    
    cout << "Wire::setupForContext " << context_id << "\n";
    
    if(vao[context_id][Body] != 0 || vao[context_id][Cap1] != 0 || vao[context_id][Cap2] != 0)
    {
#ifdef DEBUG
        cerr << "Wire: Double setup for context: " << context_id << "\n";
#endif//DEBUG
        return;
    }
    
    GLuint handle[3];
    glGenVertexArrays(3, handle);
    
    for(int i=0; i<3; i++)
        vao[context_id][i] = handle[i];
    
    glBindVertexArray(vao[context_id][Body]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[Body]);
    glEnableVertexAttribArray(vertex_data_attribute);
    glVertexAttribPointer(vertex_data_attribute, 4, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindVertexArray(vao[context_id][Cap1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[Cap1]);
    glEnableVertexAttribArray(vertex_data_attribute);
    glVertexAttribPointer(vertex_data_attribute, 4, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindVertexArray(vao[context_id][Cap2]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[Cap2]);
    glEnableVertexAttribArray(vertex_data_attribute);
    glVertexAttribPointer(vertex_data_attribute, 4, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindVertexArray(0);
}


void Wire::cleanupForContext(RenderingContextId_t context_id)
{
    if(vao[context_id][Body] == 0 || vao[context_id][Cap1] == 0 || vao[context_id][Cap2] == 0)
    {
#ifdef DEBUG
        cerr << "Wire: Double cleanup for context: " << context_id << "\n";
#endif//DEBUG
        return;
    }
    
    GLuint handle[3];
    
    for(int i=0; i<3; i++)
        handle[i] = vao[context_id][i];
    
    glDeleteVertexArrays(3, handle);
}


/* Normal angle for a line given by two points. */
float normal_angle(Point<float> a, Point<float> b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    
    float angle;
    
    if(dx == 0)
        angle = 0.0;
    else
        angle = atan2(dy, dx);
    
    angle += M_PI * 0.5;
    
    if(angle > M_PI * 2)
        angle -= M_PI * 2;
    
    return angle;
}


void Wire::calculate_perpendicular_points(Point<float> in_a, Point<float> in_b, Point<float> in_c, Point<float> &out_a, Point<float> &out_b)
{
    float angle = normal_angle(in_a, in_b);
    
    out_a = Point<float>(
        in_b.x + cos(angle) * wire_width,
        in_b.y + sin(angle) * wire_width       
    );
    
    out_b = Point<float>(
        in_b.x - cos(angle) * wire_width,
        in_b.y - sin(angle) * wire_width       
    );
}


void Wire::calculate_cap_coordinates(Point<float> a, Point<float> b, Point<float> &c, Point<float> &d)
{
    float angle = normal_angle(a, b);
    
    float dx = cos(angle) * wire_width * 0.5;
    float dy = sin(angle) * wire_width * 0.5;
    
    c = a;
    c.x += dx;
    c.y += dy;
    
    d = b;
    d.x += dx;
    d.y += dy;
}


void Wire::init_cap_vertices(int item, Point<float> v1, Point<float> v2)
{
    float buffer_data[cap_vertex_count*4];
    
    auto center_point = v1;
    
    buffer_data[0] = center_point.x;
    buffer_data[1] = center_point.y;
    buffer_data[2] = 0.6;
    buffer_data[3] = 0.0;
    
    float angle_delta = M_PI / (cap_vertex_count-2);
    float angle = normal_angle(v1, v2);
    float radius = wire_width;
    for(int i=1; i<cap_vertex_count; i++)
    {
        buffer_data[i*4 + 0] = center_point.x + cos(angle) * radius;
        buffer_data[i*4 + 1] = center_point.y + sin(angle) * radius;
        buffer_data[i*4 + 2] = 0.9;
        buffer_data[i*4 + 3] = 0.0;
        
        angle += angle_delta;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[item]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, (cap_vertex_count) * 4 * sizeof(float), buffer_data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void Wire::update()
{
#ifdef DEBUG
    assert(_source_socket != nullptr);
    assert(_sink_socket != nullptr);
#endif//DEBUG
    
    control_points[0] = _source_socket->toSceneCoords(_source_socket->center());
    control_points[2] = _sink_socket->toSceneCoords(_sink_socket->center());
    
    if(control_points[0].x > control_points[2].x)
        swap(control_points[0], control_points[2]);
    
    auto topmost = control_points[0].y > control_points[2].y ? control_points[0] : control_points[2];
    auto bottommost = control_points[0].y <= control_points[2].y ? control_points[0] : control_points[2];
    
    auto rightmost = control_points[0].x > control_points[2].x ? control_points[0] : control_points[2];
    auto leftmost = control_points[0].x <= control_points[2].x ? control_points[0] : control_points[2];
    
    
    
    control_points[1] = {
        leftmost.x + (rightmost.x - leftmost.x) * 0.5,
        bottommost.y - 500
    };
    
    bezier_points(control_points, wire_points);
    
    const int buffer_data_size = node_count * 4 * 2;
    float buffer_data[buffer_data_size];
    
    for(int i=1; i<(int)wire_points.size(); i++)
    {
        Point<float> perp_a, perp_b;
        
        calculate_perpendicular_points(wire_points[i-1], wire_points[i], wire_points[i], perp_a, perp_b);
        
        buffer_data[i*8 + 0] = perp_a.x;
        buffer_data[i*8 + 1] = perp_a.y;
        buffer_data[i*8 + 2] = 0.1;
        buffer_data[i*8 + 3] = 0.0;
        
        buffer_data[i*8 + 4] = perp_b.x;
        buffer_data[i*8 + 5] = perp_b.y;
        buffer_data[i*8 + 6] = 0.9;
        buffer_data[i*8 + 7] = 0.0;
    }
    
    /*Fix the first point. */
    auto d = wire_points[1] - wire_points[0];
    buffer_data[0] = buffer_data[8] - d.x;
    buffer_data[1] = buffer_data[9] - d.y;
    buffer_data[2] = 0.1;
    buffer_data[3] = 0.0;
    
    buffer_data[4] = buffer_data[12] - d.x;
    buffer_data[5] = buffer_data[13] - d.y;
    buffer_data[6] = 0.9;
    buffer_data[7] = 0.0;
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[Body]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, buffer_data_size * sizeof(float), buffer_data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
   
    init_cap_vertices(Cap1, wire_points[0], wire_points[1]);
    init_cap_vertices(Cap2, wire_points[wire_points.size()-1], wire_points[wire_points.size()-2]);
}

void Wire::render(RenderingContextId_t context_id)
{   
    float color[4] = {1.0, 0.7, 0.3, 0.0};
    
    shading_program.use();
    
    glBindTexture(GL_TEXTURE_1D, texture);
    glUniform1f(sampler_uniform, sampler);
    glUniform4fv(color_uniform, 1, color);
    
    glBindVertexArray(vao[context_id][Body]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, node_count * 2);
    
    glBindVertexArray(vao[context_id][Cap1]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, cap_vertex_count);
    
    glBindVertexArray(vao[context_id][Cap2]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, cap_vertex_count);
    
    glBindVertexArray(0);
    
    glUseProgram(0);
}

}//namespace r64fx
