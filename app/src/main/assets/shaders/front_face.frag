#version 310 es
precision highp float;
precision highp sampler3D;
in vec3 hit;
layout(binding = 0) uniform sampler2D lastHit;
layout(binding = 3) uniform sampler3D volume;
out vec4 outColor;
void main() {
   ivec2 tcoord = ivec2(gl_FragCoord.xy);
   vec3 last = texelFetch(lastHit , tcoord,0).xyz;
   vec3 direction = last.xyz - hit.xyz;
   float D = length(direction);
   direction = normalize(direction);
   vec4 color = vec4(0.0f);
   color.a = 1.0f;
   float h = 0.015f;         // todo fix
   vec3 tr = hit;
   vec3 rayStep = direction * h;
   float alpha;
   vec3 sampColor;
   vec3 baseColor = vec3(1.0,0.0,0.0);
   vec3 otherColor = vec3(1.0,1.0,0.0);
   baseColor = mix(baseColor, otherColor, length(gl_FragCoord.xy));
   //float opacityThreshold = 0.95f;
   for(float t = 0.0f; t<=D; t += h){
           float samp = texture(volume, tr).x;
           //calculate Alpha
           alpha = 1.0f - exp(-0.5f * samp);
           //accumulating collor and alpha using under operator
           sampColor = baseColor * alpha;
           color.rgb += sampColor * color.a;
           color.a *= 1.0f - samp;
           // checking early ray termination
           //if(1.0f - color.w > opacityThreshold) break;
           //increment ray step
           tr += rayStep;
   }

color.w = 1.0f - color.w;
color.rgb = pow( color.rgb, vec3(0.4545)); 
outColor = color;
//  outColor = vec4(hit,1.0f); // todo
}