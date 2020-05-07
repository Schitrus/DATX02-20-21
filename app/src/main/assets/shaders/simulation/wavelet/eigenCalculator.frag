#version 310 es

precision highp float;
precision highp sampler3D;

layout(binding = 0) uniform sampler3D m1;
layout(binding = 1) uniform sampler3D m2;
layout(binding = 1) uniform sampler3D m3;

uniform int depth;
uniform vec3 gridSize;
uniform int maxIterations;

// Result data from the advection
out vec3 outTextureCoord;

//This shader utilize the QR algorithm to calculate the eigenvalues.
//This does however give a minor numerival error, but through testing it got fairly similar as the eigen library
void main() {
    int i;
    ivec3 position = ivec3(gl_FragCoord.xy, depth);

    //create the jacobian
    vec3 col1 = texture(m1, (vec3(position) + vec3(0.5))/gridSize).xyz;
    vec3 col2 = texture(m2, (vec3(position) + vec3(0.5))/gridSize).xyz;
    vec3 col3 = texture(m3, (vec3(position) + vec3(0.5))/gridSize).xyz;
    mat3 A = mat3(col1, col2, col3);

    //in case of the jacobian being singular, deafult values are set as eigenvalues
    vec3 eigenValues = vec3(0.1f, 10.0f, 0.1f);

    // if jacobian is not singular, perform the QR algorithm
    if(determinant(A) != 0.0f){
        mat3 Q;
        mat3 R;
        for(i = 0; i < maxIterations; i++){
            vec3 u1 = A[0];
            vec3 u2 = A[1] - (dot(u1,A[1])/ dot(u1,u1) * u1);                                       // a[1] - proj_u1(a[1])
            vec3 u3 = A[2] - (dot(u1,A[2])/ dot(u1,u1) * u1) - (dot(u2,A[2])/ dot(u2,u2) * u2);     // a[2] - proj_u1(a[2]) - proj_u2(a[2])
            Q = mat3(u1/length(u1), u2/length(u2), u3/length(u3));
            R = transpose(Q) * A;
            A = R*Q;
        }
        eigenValues = vec3(A[0][0],A[1][1],A[2][2]);
    }
    outTextureCoord = eigenValues;
}
