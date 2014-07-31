#include "Surface.h"

#include "Color.h"

#include <cstring>
#include <vector>
#include <cmath>
#include <cassert>

using namespace std;
using namespace canvas;

static vector<int> make_kernel(float radius) {
  int r = (int)ceil(radius);
  int rows = 2 * r + 1;
  float sigma = radius / 3;
  float sigma22 = 2 * sigma * sigma;
  float sigmaPi2 = 2 * M_PI * sigma;
  float sqrtSigmaPi2 = sqrt(sigmaPi2);
  // float radius2 = radius*radius;
  vector<int> kernel;
  kernel.reserve(rows);

  int row = -r;
  float first_value = exp(-row*row/sigma22) / sqrtSigmaPi2;
  kernel.push_back(1);
  
  for (row++; row <= r; row++) {
    kernel.push_back(int(exp(-row * row / sigma22) / sqrtSigmaPi2 / first_value));
  }
  return kernel;
}
  
void
Surface::gaussianBlur(float hradius, float vradius) {
  unsigned char * buffer = getBuffer();
  assert(buffer);

  unsigned int width = getWidth(), height = getHeight();
  unsigned char * tmp = new unsigned char[width * height * 4];
  memset(tmp, 0, width * height * 4);
  vector<int> hkernel = make_kernel(hradius), vkernel = make_kernel(vradius);
  unsigned short hsize = hkernel.size(), vsize = vkernel.size();
  int htotal = 0, vtotal = 0;
  for (vector<int>::iterator it = hkernel.begin(); it != hkernel.end(); it++) {
    htotal += *it;
  }
  for (vector<int>::iterator it = vkernel.begin(); it != vkernel.end(); it++) {
    vtotal += *it;
  }
  for (unsigned int row = 0; row < height; row++) {
    for (unsigned int col = 0; col + hsize < width; col++) {
      int c0 = 0, c1 = 0, c2 = 0, c3 = 0;
      for (unsigned int i = 0; i < hsize; i++) {
	unsigned char * ptr = buffer + (row * width + col + i) * 4;
	c0 += *ptr++ * hkernel[i];
	c1 += *ptr++ * hkernel[i];
	c2 += *ptr++ * hkernel[i];
	c3 += *ptr++ * hkernel[i];
      }
      unsigned char * ptr = tmp + (row * width + col + hsize / 2) * 4;
      *ptr++ = (unsigned char)(c0 / htotal);
      *ptr++ = (unsigned char)(c1 / htotal);
      *ptr++ = (unsigned char)(c2 / htotal);
      *ptr++ = (unsigned char)(c3 / htotal);
    }
  }
  memset(buffer, 0, width * height * 4);
  for (unsigned int col = 0; col < height; col++) {
    for (unsigned int row = 0; row + vsize < height; row++) {
      int c0 = 0, c1 = 0, c2 = 0, c3 = 0;
      for (unsigned int i = 0; i < vsize; i++) {
	unsigned char * ptr = tmp + ((row + i) * width + col) * 4;
	c0 += *ptr++ * vkernel[i];
	c1 += *ptr++ * vkernel[i];
	c2 += *ptr++ * vkernel[i];
	c3 += *ptr++ * vkernel[i];
      }
      unsigned char * ptr = buffer + ((row + vsize / 2) * width + col) * 4;
      *ptr++ = (unsigned char)(c0 / vtotal);
      *ptr++ = (unsigned char)(c1 / vtotal);
      *ptr++ = (unsigned char)(c2 / vtotal);
      *ptr++ = (unsigned char)(c3 / vtotal);
    }
  }
  delete[] tmp;
}

void
Surface::colorize(const Color & color) {
  
}