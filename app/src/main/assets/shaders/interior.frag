#version 310 es

precision highp float;
precision highp sampler3D;
layout(binding = 0) uniform sampler3D data;
uniform int depth; 
out vec4 outColor;
void main() {
   ivec2 tcoord = ivec2(gl_FragCoord.xy);
   vec4 value = vec4(0.0f);
   int count = 0;
   for(int x = -1; x <= 1; x++){
      for (int y = -1; y <= 1; y++){
         for (int z = -1; z <= 1; z++){
            value += texelFetch(data, ivec3(tcoord.x + x, tcoord.y + y, depth + z), 0);
            count++;
         }
      }
   }

   //value += texelFetch(data ,ivec3( tcoord.x + 1, tcoord.y, depth),0).x;
   //value += texelFetch(data ,ivec3( tcoord.x, tcoord.y - 1, depth),0).x;
   //value += texelFetch(data ,ivec3( tcoord.x, tcoord.y + 1, depth),0).x;
   //value += texelFetch(data ,ivec3( tcoord.x, tcoord.y, depth +1 ),0).x;
   //value += texelFetch(data ,ivec3( tcoord.x, tcoord.y, depth -1 ),0).x;
   value.rgb /= 0.975*float(count);
   value.a /= 0.995*float(count);
   outColor = value;
}