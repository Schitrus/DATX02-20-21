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
   vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
   color.a = 0.0f;
   float h = 1.0/256.0;         // todo fix
   vec3 tr = hit;
   vec3 rayStep = direction * h;
   vec3 baseColor = vec3(1.0,0.5,0.0);
   //float opacityThreshold = 0.95f;
   for(float t = 0.0; t<=D; t+=h){
           ivec3 iv = ivec3(tr);
           //vec4 samp = vec4(baseColor, texture(volume, tr).x);
           //vec4 samp = texelFetch(volume, iv, 0);
           float samp = min(texture(volume, tr).x, 1.0);
           //calculate Alpha
           //accumulating collor and alpha using under operator
           float alpha = pow(samp,1.0);
           float over = color.a + alpha * (1.0 - color.a);
           if(over > 0.0)
              color.rgb = ( color.rgb * color.a + baseColor * alpha * (1.0 - color.a))/over;
           color.a = over;
           // checking early ray termination
           //if(1.0f - color.w > opacityThreshold) break;
           //increment ray step
           tr += rayStep;
   }
        color.rgb = pow( color.rgb, vec3(0.4545));
        outColor = color;
        //outColor = vec4(1.0f,1.0f,1.0f,1.0f); // todo
}