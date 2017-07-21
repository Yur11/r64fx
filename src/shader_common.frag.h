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
"uniform int        zero_index    = 0;\n"
"uniform int        zero_count    = 0;\n"
"uniform int        pole_index    = 0;\n"
"uniform int        pole_count    = 0;\n"
"\n"
"vec2 complex_add(in vec2 a, in vec2 b)\n"
"{\n"
"    return vec2(a.x + b.x, a.y + b.y);\n"
"}\n"
"\n"
"vec2 complex_sub(in vec2 a, in vec2 b)\n"
"{\n"
"    return vec2(a.x - b.x, a.y - b.y);\n"
"}\n"
"\n"
"vec2 complex_mul(in vec2 a, in vec2 b)\n"
"{\n"
"    return vec2(a.x*b.x - a.y*b.y, a.y*b.x + a.x*b.y);\n"
"}\n"
"\n"
"vec2 complex_div(in vec2 a, in vec2 b)\n"
"{\n"
"    return vec2(\n"
"        (a.x * b.x + a.y * b.y) / (b.x * b.x + b.y * b.y),\n"
"        (a.y * b.x - a.x * b.y) / (b.x * b.x + b.y * b.y)\n"
"    );\n"
"}\n"
"\n"
"float complex_maginitude(in vec2 c)\n"
"{\n"
"    return sqrt(c[0] * c[0] + c[1] * c[1]);\n"
"}\n"
"\n"
"float complex_phase(in vec2 c)\n"
"{\n"
"    return atan(c[1], c[0]);\n"
"}\n"
"\n"
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
"        case 12:\n"
"        {\n"
"            float radius = min(rect_size.x, rect_size.y) * 0.45;\n"
"\n"
"            vec2 z = vec2(\n"
"                (floor(frag_tex_coord.x) - (rect_size.x * 0.5)) / radius,\n"
"                (floor(frag_tex_coord.y) - (rect_size.y * 0.5)) / radius\n"
"            );\n"
"\n"
"            vec2 numerator = vec2(1.0, 0.0);\n"
"            for(int i=0; i<zero_count; i++)\n"
"            {\n"
"                vec2 zero = texelFetch(sampler1d, i + zero_index, 0).xy;\n"
"                numerator = complex_mul(numerator, complex_sub(z, zero));\n"
"            }\n"
"\n"
"            vec2 denominator = vec2(1.0, 0.0);\n"
"            for(int i=0; i<pole_count; i++)\n"
"            {\n"
"                vec2 pole = texelFetch(sampler1d, i + pole_index, 0).xy;\n"
"                denominator = complex_mul(denominator, complex_sub(z, pole));\n"
"            }\n"
"\n"
"            float mag = complex_maginitude(denominator);\n"
"            if(abs(mag) > 0.0)\n"
"            {\n"
"                vec2 res = complex_div(numerator, denominator);\n"
"                mag = complex_maginitude(res);\n"
"\n"
"                if(mag < 500.0)\n"
"                {\n"
"                    mag /= 500.0;\n"
"                    mag = sqrt(mag);\n"
"                    gl_FragColor = vec4(mag, mag, mag, 0.0);\n"
"                    break;\n"
"                }\n"
"            }\n"
"\n"
"            gl_FragColor = vec4(1.0, 1.0, 1.0, 0.0);\n"
"            break;\n"
"        }\n"
"\n"
"        default:\n"
"            discard;\n"
"    }\n"
"}\n"
"\n"
