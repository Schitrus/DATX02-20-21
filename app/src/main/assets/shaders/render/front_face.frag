#version 310 es

precision highp float;
precision highp sampler3D;

in vec3 hit;

layout(binding = 0) uniform sampler2D lastHit;
layout(binding = 2) uniform sampler3D pressure;
layout(binding = 3) uniform sampler3D temperature;

out vec4 outColor;
void main() {
   ivec2 tcoord = ivec2(gl_FragCoord.xy);
   vec3 last = texelFetch(lastHit , tcoord,0).xyz;
   vec3 direction = last.xyz - hit.xyz;
   float D = length(direction);
   direction = normalize(direction);
   vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
   color.a = 0.0f;
   float h = 1.0/32.0;         // todo fix
   vec3 tr = hit;
   vec3 rayStep = direction * h;
   vec3 blackbody = vec3(1.0,0.3,0.0);
   //float opacityThreshold = 0.95f;
   for(float t = 0.0; t<=D; t+=h){
           ivec3 iv = ivec3(tr);
           //vec4 samp = vec4(baseColor, texture(volume, tr).x);
           //vec4 samp = texelFetch(volume, iv, 0);
           float samp = clamp(texture(pressure, tr).x, 0.0, 1.0);
           float lum = clamp((texture(temperature, tr).x-20.0)/50.0, 0.0, 1.0);
           float rad = clamp((texture(temperature, tr).x-70.0)/130.0, 0.0, 1.0);
           float hot = clamp((texture(temperature, tr).x-200.0)/100.0, 0.0, 1.0);
           //calculate Alpha
           //accumulating collor and alpha using under operator
           vec3 baseColor = mix(mix(mix(vec3(0.0,0.0,0.0),
                                        vec3(1.0,0.0,0.0), lum),
                                        vec3(1.0,1.0,0.5), rad),
                                        vec3(0.25,0.5,1.0), hot);
           float alpha = pow(samp,2.0);
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