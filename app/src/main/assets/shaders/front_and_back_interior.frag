#version 310 es

precision highp float;
precision highp sampler3D;
layout(binding = 0) uniform sampler3D data;
uniform int depth;
uniform int max_depth;
out vec4 outColor;
void main() {
   //int dir = depth == 0 ? 1 : -1;   // todo remove if statement
   ivec2 tcoord = ivec2(gl_FragCoord.xy);
   vec4 value = vec4(0.0f);
   int count = 0;
   for (int x = -1; x <= 1; x++){
      for (int y = -1; y <= 1; y++){
         for (int z = -1; z <= 1; z++){
            if (depth + z < 0 || depth + z > max_depth){
               value += texelFetch(data, ivec3(tcoord.x + x, tcoord.y + y, depth + z), 0);
               count++;
            }
         }
      }
   }
   value /= float(count);
   outColor = value;
}