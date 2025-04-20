#include "render2d.h"
#include "math.h"

#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static char* loadFile(char *filename);
static int compileShader(unsigned int shader, const char *source);
static int installShaders(unsigned int *program);
static void setupBuffers(Render2d *render);


Render2d* r2dInit() {
  Render2d *r = (Render2d*)malloc(sizeof(Render2d));
  if (!r) {
    printf("Failed to allocate memory for render\n");
    return 0;
  }
  r->program = 0;
  r->vao = 0;
  r->vbo = 0;
  r->ibo = 0;
  r->indicesCount = 0;
  r->texturesCount = 0;
  r->maxTextures = 8; // TODO query opengl
  r->view = mat4Init(1.0f);
  r->projection = mat4Init(1.0f);

  r->textures = (unsigned int*)malloc(r->maxTextures * sizeof(unsigned int));
  if (!r->textures) {
    printf("Failed to allocate memory for textures\n");
    return 0;
  }

  unsigned int program;
  if (installShaders(&program)) {
    printf("Failed to install shaders\n");
    return 0;
  }
  r->program = program;

  setupBuffers(r);

  return r;
}

void r2dTerminate(Render2d *render) {
  free(render->textures);
  free(render);
}

void r2dClear() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

int r2dCreateTexture(Render2d *render, const char *filename, unsigned int *textureID) {
  if (render->texturesCount >= render->maxTextures) {
    return 1;
  }

  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, textureID);
  glBindTexture(GL_TEXTURE_2D, *textureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_set_flip_vertically_on_load(1);
  int width, height, nrChannels;
  unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 4);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    return 1;
  }
  stbi_image_free(data);

  render->textures[render->texturesCount] = *textureID;
  render->texturesCount++;

  return 0;
}

void r2dDrawSprite(Render2d *render, Sprite sprite) {
  glBindTexture(GL_TEXTURE_2D, sprite.texture);

  // populate uniforms
  int modelLoc = glGetUniformLocation(render->program, "model");
  mat4 model = mat4Init(1.0f);
  model = rotate(model, radians(sprite.rotation), (vec3){0, 0, 1.0f});
  model = scale(model, (vec3){sprite.width, sprite.height, 0});
  model = translate(model, (vec3){sprite.x, sprite.y, 0});
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, matValue(&model));

  int colorLoc = glGetUniformLocation(render->program, "color");
  glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);

  int viewLoc = glGetUniformLocation(render->program, "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, matValue(&render->view));

  int projectionLoc = glGetUniformLocation(render->program, "projection");
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, matValue(&render->projection));


  glDrawElements(GL_TRIANGLES, render->indicesCount, GL_UNSIGNED_SHORT, 0);
}


Sprite createSprite(float x, float y, float width, float height, float rotation, unsigned int textureID) {
  Sprite sprite = {
    .x = x,
    .y = y,
    .width = width,
    .height = height,
    .rotation = rotation,
    .texture = textureID
  };
  return sprite;
}


// =======================================================
// Static
// =======================================================


static void setupBuffers(Render2d *render) {
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
  render->indicesCount = 6;

  glGenVertexArrays(1, &render->vao);
  glBindVertexArray(render->vao);

  glGenBuffers(1, &render->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, render->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

  glGenBuffers(1, &render->ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render->ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}



static char* loadFile(char *filename) {
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
    return 1;
  }
  if (compileShader(fshader, fragmentSource)) {
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
