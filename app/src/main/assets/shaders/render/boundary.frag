#version 310 es
precision highp float;
precision highp sampler3D;
layout(binding = 0) uniform sampler3D data;
uniform int width;
uniform int height; 
uniform int depth; 
uniform float scale; 
out vec4 outColor;
void main() {
    ivec2 tcoord = ivec2(gl_FragCoord.xy);
    vec4 value = vec4(0.0f);
    int count = 0;
    for(int x = -1; x <= 1; x++){
        for (int y = -1; y <= 1; y++){
            for (int z = -1; z <= 1; z++){
                if (tcoord.x + x < 0 || tcoord.x + x > width || tcoord.y + y < 0 || tcoord.y + y > height){
                    value += texelFetch(data, ivec3(tcoord.x + x, tcoord.y + y, depth + z), 0);
                    count++;
                }
            }
        }
    }
    //if(tcoord.x == 0){ value += scale * texelFetch(data , ivec3( tcoord.x + 1, tcoord.y, depth),0);}
    //if(tcoord.x == width -1){ value += scale * texelFetch(data , ivec3( tcoord.x - 1, tcoord.y, depth),0).x;}
    //if(tcoord.y == 0){ value += scale * texelFetch(data , ivec3( tcoord.x, tcoord.y + 1, depth ),0).x;}
    //if(tcoord.y == height -1){ value += scale * texelFetch(data , ivec3( tcoord.x, tcoord.y - 1, depth),0).x;}
    //bool corner = ((tcoord.x == 0 || tcoord.x == width -1) && (tcoord.y == 0 || tcoord.y == height -1 ));
    //if(corner){ value *= 0.5f; }
    value /= float(count);
    outColor = value;
}