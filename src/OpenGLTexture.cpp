#include "OpenGLTexture.h"

#include "TextureLink.h"
#include "../../personal/graphviewer/ui/GL.h"

#include <cassert>

using namespace std;
using namespace canvas;

size_t OpenGLTexture::total_textures = 0;
vector<unsigned int> OpenGLTexture::freed_textures;

static GLenum getOpenGLFilterType(FilterMode mode) {
  switch (mode) {
  case NEAREST: return GL_NEAREST;
  case LINEAR: return GL_LINEAR;
  case LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
  }
  return 0;
}

void
OpenGLTexture::updateData(unsigned char * buffer) {
  assert(buffer);

  // glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getOpenGLFilterType(getMinFilter()) );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getOpenGLFilterType(getMagFilter()) );
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, getMinFilter() == LINEAR_MIPMAP_LINEAR ? GL_TRUE : GL_FALSE);

  // glGenerateMipmap(GL_TEXTURE_2D);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getWidth(), getHeight(),
	       0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buffer);
}

void
OpenGLTexture::releaseTextures() {
  // cerr << "DELETING TEXTURES: " << OpenGLTexture::getFreedTextures().size() << "/" << OpenGLTexture::getNumTextures() << endl;
  
  for (vector<unsigned int>::const_iterator it = freed_textures.begin(); it != freed_textures.end(); it++) {
    GLuint texid = *it;
    glDeleteTextures(1, &texid);
  }
  freed_textures.clear();
}

TextureLink
OpenGLTexture::createTexture(unsigned int width, unsigned int height) {
  if (freed_textures.empty()) {
    unsigned int id;
    glGenTextures(1, &id);
    return TextureLink(width, height, new OpenGLTexture(width, height, id));
  } else {
    unsigned int id = freed_textures.back();
    freed_textures.pop_back();
    return TextureLink(width, height, new OpenGLTexture(width, height, id));
  }
}