/* platform.h
 * They haven't given us much that could credibly pass for terrain.
 * We do have two useful decals: edge and platform.
 * Height of a platform is always 9 pixels.
 * Width must be odd and greater than 12.
 */

#ifndef PLATFORM_H
#define PLATFORM_H

struct platform {
  int x,y,w; // Position and size in world pixels. (y) is my top edge, the business edge.
  int style; // NS_platform_
};

void platform_render(struct platform *platform);

#endif
