#version 330 core

layout(location = 0) in vec4 vertex; // pos, tex

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 color;

out vec2 TexCoord;
out vec4 Color;

void main() {
  gl_Position = projection * view * model * vec4(vertex.xy, 0.0, 1.0);

  TexCoord = vertex.zw;
  Color = color;
}
