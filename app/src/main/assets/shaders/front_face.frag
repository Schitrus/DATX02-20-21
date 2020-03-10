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
   color.a = 0.0f;
   float h = 1.0/256.0;         // todo fix
   vec3 tr = hit;
   vec3 rayStep = direction * h;
   float alpha;
   vec3 sampColor;
   vec3 baseColor = vec3(1.0,0.0,0.0);
   //float opacityThreshold = 0.95f;
   for(float t = 0.0; t<=D; t+=h){
           ivec3 iv = ivec3(tr);
           float samp = texture(volume, tr).x*0.05;
           //calculate Alpha
           alpha = samp;
           //accumulating collor and alpha using under operator
           color.a = color.a + alpha * (1.0 - color.a);
           // checking early ray termination
           //if(1.0f - color.w > opacityThreshold) break;
           //increment ray step
           tr += rayStep;
   }
        color.rgb = pow( baseColor.rgb, vec3(0.4545)); 
        outColor = color;
        //  outColor = vec4(hit,1.0f); // todo
}