#ifndef _TEXTURE_GLSL_
#define _TEXTURE_GLSL_

#define getTexelSize(src) (1.0 / textureSize(src, 0))
#define calculateMipLevels(src) floor(log2(float(textureSize(src, 0).x))) + 1.0

#endif
