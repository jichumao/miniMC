#version 150

uniform mat4 u_Model;       // The matrix that defines the transformation of the
                            // object we're rendering. In this assignment,
                            // this will be the result of traversing your scene graph.

uniform mat4 u_ModelInvTr;  // The inverse transpose of the model matrix.
                            // This allows us to transform the object's normals properly
                            // if the object has been non-uniformly scaled.

uniform mat4 u_View;

uniform mat4 u_ViewProj;    // The matrix that defines the camera's transformation.
                            // We've written a static matrix for you to use for HW2,
                            // but in HW3 you'll have to generate one yourself

uniform vec4 u_Color;       // When drawing the cube instance, we'll set our uniform color to represent different block types.

uniform int u_Time;

in vec4 vs_Pos;             // The array of vertex positions passed to the shader

in vec4 vs_Nor;             // The array of vertex normals passed to the shader

in vec4 vs_Col;             // The array of vertex colors passed to the shader.

in vec4 vs_UV;

out vec4 fs_Pos;
out vec4 fs_Nor;            // The array of normals that has been transformed by u_ModelInvTr. This is implicitly passed to the fragment shader.
out vec4 fs_Col;            // The color of each vertex. This is implicitly passed to the fragment shader.
out vec4 fs_UV;
out vec4 fs_UnhomShadowMapCoord;
out vec4 fs_ViewSpacePos;

const vec4 lightDir = normalize(vec4(1, -1, 1, 0));  // The direction of our virtual light, which is used to compute the shading of
                                        // the geometry in the fragment shader.

uniform mat4 u_ModelLightProj;
uniform mat4 u_UnhomScreenToTexture;

vec4 distort_surface_pos(vec4 o) {
    vec4 res = o;
    res.y += sin(o.x * 2.f + u_Time % 50 / 50.f * 2.f * 3.14159265) * 0.07;
    return res;
}

vec4 distort_surface_norm(vec4 o) {
    vec3 grad_x = vec3(1, 2.f * cos(o.x * 2.f + u_Time % 50 / 50.f * 2.f * 3.14159265) * 0.07, 0);
    vec3 grad_z = vec3(0, 0, 1);
    vec3 res = cross(grad_x, grad_z);
    return vec4(-normalize(res), 0);
}

void main() {
    fs_Pos = vs_Pos;
    fs_Col = vs_Col;
    fs_UV = vs_UV;

    // Pass the vertex normals to the fragment shader for interpolation.
    // Transform the geometry's normals by the inverse transpose of the
    // model matrix. This is necessary to ensure the normals remain
    // perpendicular to the surface after the surface is transformed by
    // the model matrix.
    mat3 invTranspose = mat3(u_ModelInvTr);

    vec4 distort_pos;
    if (fs_UV.z != 0) {
            distort_pos = distort_surface_pos(u_Model * vs_Pos);
    }
        else {
            distort_pos = u_Model * vs_Pos;
    }

    if (fs_UV.z != 0) {
        fs_Nor = distort_surface_norm(distort_pos);
    } else {
        fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);
    }

    //vec4 modelposition = u_Model * vs_Pos;
    vec4 modelposition = distort_pos;

    fs_UnhomShadowMapCoord = u_UnhomScreenToTexture * u_ModelLightProj * modelposition;

    fs_ViewSpacePos = u_View * modelposition;

    gl_Position = u_ViewProj * modelposition;
}
