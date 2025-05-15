#include "renderer2d.h"
#include <GL/glew.h>

#include <stdio.h>
#include <stdlib.h>

typedef struct {
  unsigned int program;
  unsigned int vao;
  unsigned int vbo;
  unsigned int ibo;
  unsigned int *textures;
  unsigned int boundTexture;
  int texturesCount;
  int indicesCount;
  int maxTextures;
  mat4 view;
  mat4 projection;
} renderer2d_t;

static renderer2d_t renderer;


static void setupBuffers() {
  float verticies[] = {
     0.0f,  0.0f,  0.0f, 1.0f,  // top left
     1.0f,  0.0f,  1.0f, 1.0f,  // top right
     0.0f, -1.0f,  0.0f, 0.0f,  // bottom left
     1.0f, -1.0f,  1.0f, 0.0f   // bottom right
  };
  unsigned short indices[] = {
    2, 1, 0,
    2, 3, 1
  };
  renderer.indicesCount = 6;

  glGenVertexArrays(1, &renderer.vao);
  glBindVertexArray(renderer.vao);

  glGenBuffers(1, &renderer.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

  glGenBuffers(1, &renderer.ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

// caller in charge of free
static char *loadFile(char *filename) {
  FILE *fp;
  long sizeBytes = 0;
  char *buffer;

  fp = fopen(filename, "r");

  if (!fp) {
    printf("Failed to open file: %s\n", filename);
    return 0;
  }

  fseek(fp, 0, SEEK_END);
  sizeBytes = ftell(fp);
  rewind(fp);

  buffer = (char*)malloc(sizeBytes + 1);

  if (!buffer) {
    printf("Failed to create file buffer\n");
    return 0;
  }

  fread(buffer, sizeof(char), sizeBytes, fp);
  fclose(fp);

  // NULL TERMINATE
  buffer[sizeBytes] = '\0';

  return buffer;
}

static int compileShader(unsigned int shader, const char *source) {
  glShaderSource(shader, 1, &source, 0);
  glCompileShader(shader);

  int status = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    int length = 0;
    char message[1024];
    glGetShaderInfoLog(shader, 1024, &length, message);
    printf("Shader error: %s\n", message);
    return 1;
  }

  return 0;
}

static int installShaders(unsigned int *program) {
  char *vertexSource;
  char *fragmentSource;
  unsigned int vshader, fshader;

  vertexSource = loadFile("assets/shader.vert");
  fragmentSource = loadFile("assets/shader.frag");
  if (!vertexSource || !fragmentSource) {
    free(vertexSource);
    free(fragmentSource);
    return 1;
  }

  vshader = glCreateShader(GL_VERTEX_SHADER);
  fshader = glCreateShader(GL_FRAGMENT_SHADER);

  if (compileShader(vshader, vertexSource)) {
    free(vertexSource);
    free(fragmentSource);
    return 1;
  }
  if (compileShader(fshader, fragmentSource)) {
    free(vertexSource);
    free(fragmentSource);
    return 1;
  }

  free(vertexSource);
  free(fragmentSource);

  *program = glCreateProgram();

  glAttachShader(*program, vshader);
  glAttachShader(*program, fshader);
  glLinkProgram(*program);

  int status = 0;
  glGetProgramiv(*program, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    int length = 0;
    char message[1024];
    glGetProgramInfoLog(*program, 1024, &length, message);
    printf("Link error: %s\n", message);
    return 1;
  }

  glUseProgram(*program);

  return 0;
}

int r2dInit() {
  renderer = (renderer2d_t){
    .program = 0,
    .vao = 0,
    .vbo = 0,
    .ibo = 0,
    .indicesCount = 0,
    .texturesCount = 0,
    .textures = 0,
    .maxTextures = 16, // TODO query opengl
    .view = mat4Init(1.0f),
    .projection = mat4Init(1.0f)
  };

  renderer.textures = (unsigned int*)malloc(renderer.maxTextures * sizeof(unsigned int));
  if (!renderer.textures) {
    printf("Failed to allocate memory for textures\n");
    return 1;
  }

  unsigned int program;
  if (installShaders(&program)) {
    printf("Failed to install shaders\n");
    return 1;
  }
  renderer.program = program;

  setupBuffers();

  // Enable alpha blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  return 0;
}

void r2dTerminate() {
  glDeleteTextures(renderer.texturesCount, renderer.textures);
  free(renderer.textures);
}

void r2dClear() {
  glClear(GL_COLOR_BUFFER_BIT);
}

int r2dCreateTexture(int width, int height, unsigned char *data, unsigned int *textureID) {
  if (renderer.texturesCount >= renderer.maxTextures) {
    return 1;
  }

  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, textureID);
  glBindTexture(GL_TEXTURE_2D, *textureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  renderer.textures[renderer.texturesCount] = *textureID;
  renderer.texturesCount++;

  return 0;
}

void r2dDrawSprite(camera_t *camera, sprite_t sprite) {
  if (renderer.boundTexture != sprite.texture) {
    glBindTexture(GL_TEXTURE_2D, sprite.texture);
    renderer.boundTexture = sprite.texture;
  }

  // populate uniforms
  int modelLoc = glGetUniformLocation(renderer.program, "model");
  mat4 model = mat4Init(1.0f);
  model = rotate(model, radians(sprite.rotation), (vec3){0, 0, 1.0f});
  model = scale(model, (vec3){sprite.width, sprite.height, 0});
  model = translate(model, (vec3){sprite.x, sprite.y, 0});
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, matValue(&model));

  int colorLoc = glGetUniformLocation(renderer.program, "color");
  glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);

  int viewLoc = glGetUniformLocation(renderer.program, "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, matValue(&camera->view));

  int projectionLoc = glGetUniformLocation(renderer.program, "projection");
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, matValue(&camera->projection));


  glDrawElements(GL_TRIANGLES, renderer.indicesCount, GL_UNSIGNED_SHORT, 0);
}

void r2dSetView(mat4 a) {
  renderer.view = a;
}

void r2dSetProjection(mat4 a) {
  renderer.projection = a;
}

sprite_t createSprite(float x, float y, float width, float height, float rotation, unsigned int textureID) {
  sprite_t sprite = {
    .x = x,
    .y = y,
    .width = width,
    .height = height,
    .rotation = rotation,
    .texture = textureID
  };
  return sprite;
}

void r2dSetClearColorRGBA(float r, float g, float b, float a) {
  glClearColor(r / 256, g / 256, b / 256, a);
}
