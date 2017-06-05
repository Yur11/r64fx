"#version 130\n"
"\n"
"in vec2 frag_tex_coord;\n"
"\n"
"uniform int        mode          = 0;\n"
"uniform vec4       colors[4]     = vec4[4](\n"
"    vec4(0.0, 0.0, 0.0, 0.0),\n"
"    vec4(0.0, 0.0, 0.0, 0.0),\n"
"    vec4(0.0, 0.0, 0.0, 0.0),\n"
"    vec4(0.0, 0.0, 0.0, 0.0)\n"
");\n"
"uniform sampler1D  sampler1d;\n"
"uniform sampler2D  sampler2d;\n"
"uniform vec2       rect_size     = vec2(0, 0);\n"
"uniform float      stroke_width  = 0;\n"
"\n"
"void main()\n"
"{\n"
"    switch(mode)\n"
"    {\n"
"        //putImage()\n"
"        case 1:\n"
"        {\n"
"            vec4 texel = texelFetch(sampler2d, ivec2(frag_tex_coord), 0);\n"
"            gl_FragColor = vec4(texel[0], texel[0], texel[0], 0.0);\n"
"            break;\n"
"        }\n"
"\n"
"        case 2:\n"
"        {\n"
"            vec4 texel = texelFetch(sampler2d, ivec2(frag_tex_coord), 0);\n"
"            gl_FragColor = vec4(texel[0], texel[0], texel[0], texel[1]);\n"
"            break;\n"
"        }\n"
"\n"
"        case 3:\n"
"        {\n"
"            vec4 texel = texelFetch(sampler2d, ivec2(frag_tex_coord), 0);\n"
"            gl_FragColor = vec4(texel[0], texel[1], texel[2], 0.0);\n"
"            break;\n"
"        }\n"
"\n"
"        case 4:\n"
"        {\n"
"            vec4 texel = texelFetch(sampler2d, ivec2(frag_tex_coord), 0);\n"
"            gl_FragColor = texel;\n"
"            break;\n"
"        }\n"
"\n"
"        //blendColors()\n"
"        case 5:\n"
"        case 6:\n"
"        case 7:\n"
"        case 8:\n"
"        {\n"
"            vec4 frag_color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"            vec4 texel = texelFetch(sampler2d, ivec2(frag_tex_coord), 0);\n"
"            for(int i=0; i<(mode-4); i++)\n"
"            {\n"
"                 frag_color += vec4(colors[i][0], colors[i][1], colors[i][2], 1.0 - texel[i]);\n"
"            }\n"
"            gl_FragColor = frag_color;\n"
"            break;\n"
"        }\n"
"\n"
"        //fillRect()\n"
"        case 9:\n"
"        {\n"
"            gl_FragColor = colors[0];\n"
"            break;\n"
"        }\n"
"\n"
"        //drawWaveform()\n"
"        case 10:\n"
"        {\n"
"            vec4 texel = texelFetch(sampler1d, int(frag_tex_coord[0]), 0);\n"
"            float min_value = texel[0];\n"
"            float max_value = texel[1];\n"
"            float value = float(frag_tex_coord[1]);\n"
"            if(value > min_value && value < max_value)\n"
"            {\n"
"                gl_FragColor = colors[0];\n"
"            }\n"
"            else\n"
"            {\n"
"                discard;\n"
"            }\n"
"            break;\n"
"        }\n"
"\n"
"        case 11:\n"
"        {\n"
"            ivec2 coord = ivec2(floor(frag_tex_coord[0]), floor(frag_tex_coord[1]));\n"
"            bool border = (\n"
"                (coord.x < stroke_width) ||\n"
"                (coord.y < stroke_width) ||\n"
"                (coord.x >= rect_size.x - stroke_width) ||\n"
"                (coord.y >= rect_size.y - stroke_width)\n"
"            );\n"
"            gl_FragColor = colors[border ? 0 : 1];\n"
"            break;\n"
"        }\n"
"\n"
"        default:\n"
"            discard;\n"
"    }\n"
"}\n"
"\n"
