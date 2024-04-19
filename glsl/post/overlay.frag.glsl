#version 150

uniform sampler2D u_Texture;
in vec4 fs_UV;
out vec4 out_Col;

void main()
{
    // background texture
    vec4 texture_color  = texture(u_Texture, fs_UV.xy);
    out_Col = texture_color;
}
