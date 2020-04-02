#version 310 es
precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D data_field;

uniform vec3 gridSize;
uniform int depth;
uniform float band;
uniform float min_band;

out float outData;

void main() {

    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    vec3 data = pow(2.0, band) * texture(data_field, vec3(position)).xyz;

    vec3 internal_position = fract(data);
    vec3 external_position = floor(data);

    vec3 corners[8] = {{0,0,0}, {1,0,0}, {0,1,0}, {1,1,0},
    {0,0,1}, {1,0,1}, {0,1,1}, {1,1,1}};

    double dots[8] = {0};
    for (int c = 0; c < 8; c++){
        vec3 external_corner = external_position + corners[c] + seed;
        double yaw   = (angle[int(external_corner.x*external_corner.x+external_corner.y)%256]
        +  angle[int(external_corner.y*external_corner.y+external_corner.x)%256]) / 180.0 * PI;
        double pitch = (angle[int(external_corner.x*external_corner.x+external_corner.z)%256]
        +  angle[int(external_corner.z*external_corner.z+external_corner.x)%256]) / 180.0 * PI;
        dots[c] = dot(vec3(cos(yaw)*cos(pitch), sin(yaw)*cos(pitch), cos(pitch)), internal_position - corners[c]);
    }
    //perlin
    float p = mix(mix(mix(dots[0], dots[1], internal_position.x),
                      mix(dots[2], dots[3], internal_position.x),
                                            internal_position.y),
                  mix(mix(dots[4], dots[5], internal_position.x),
                      mix(dots[6], dots[7], internal_position.x),
                                            internal_position.y),
                                            internal_position.z);
    // turbulance
    float w = abs(p);
    outData = w*pow(2.0,-(5.0/6.0)*(band - min_band));
}