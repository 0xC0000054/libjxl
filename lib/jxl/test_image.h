// Copyright (c) the JPEG XL Project Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef LIB_JXL_TEST_IMAGE_H_
#define LIB_JXL_TEST_IMAGE_H_

#include <stdint.h>

#include <vector>

#include "lib/jxl/base/random.h"

namespace jxl {
namespace test {

// Returns a test image with some autogenerated pixel content, using 16 bits per
// channel, big endian order, 1 to 4 channels
// The seed parameter allows to create images with different pixel content.
std::vector<uint8_t> GetSomeTestImage(size_t xsize, size_t ysize,
                                      size_t num_channels, uint16_t seed) {
  // Cause more significant image difference for successive seeds.
  Rng generator(seed);

  // Returns random integer in interval [0, max_value)
  auto rng = [&generator](size_t max_value) -> size_t {
    return generator.UniformU(0, max_value);
  };

  // Dark background gradient color
  uint16_t r0 = rng(32768);
  uint16_t g0 = rng(32768);
  uint16_t b0 = rng(32768);
  uint16_t a0 = rng(32768);
  uint16_t r1 = rng(32768);
  uint16_t g1 = rng(32768);
  uint16_t b1 = rng(32768);
  uint16_t a1 = rng(32768);

  // Circle with different color
  size_t circle_x = rng(xsize);
  size_t circle_y = rng(ysize);
  size_t circle_r = rng(std::min(xsize, ysize));

  // Rectangle with random noise
  size_t rect_x0 = rng(xsize);
  size_t rect_y0 = rng(ysize);
  size_t rect_x1 = rng(xsize);
  size_t rect_y1 = rng(ysize);
  if (rect_x1 < rect_x0) std::swap(rect_x0, rect_y1);
  if (rect_y1 < rect_y0) std::swap(rect_y0, rect_y1);

  size_t num_pixels = xsize * ysize;
  // 16 bits per channel, big endian, 4 channels
  std::vector<uint8_t> pixels(num_pixels * num_channels * 2);
  // Create pixel content to test, actual content does not matter as long as it
  // can be compared after roundtrip.
  for (size_t y = 0; y < ysize; y++) {
    for (size_t x = 0; x < xsize; x++) {
      uint16_t r = r0 * (ysize - y - 1) / ysize + r1 * y / ysize;
      uint16_t g = g0 * (ysize - y - 1) / ysize + g1 * y / ysize;
      uint16_t b = b0 * (ysize - y - 1) / ysize + b1 * y / ysize;
      uint16_t a = a0 * (ysize - y - 1) / ysize + a1 * y / ysize;
      // put some shape in there for visual debugging
      if ((x - circle_x) * (x - circle_x) + (y - circle_y) * (y - circle_y) <
          circle_r * circle_r) {
        r = (65535 - x * y) ^ seed;
        g = (x << 8) + y + seed;
        b = (y << 8) + x * seed;
        a = 32768 + x * 256 - y;
      } else if (x > rect_x0 && x < rect_x1 && y > rect_y0 && y < rect_y1) {
        r = rng(65536);
        g = rng(65536);
        b = rng(65536);
        a = rng(65536);
      }
      size_t i = (y * xsize + x) * 2 * num_channels;
      pixels[i + 0] = (r >> 8);
      pixels[i + 1] = (r & 255);
      if (num_channels >= 2) {
        // This may store what is called 'g' in the alpha channel of a 2-channel
        // image, but that's ok since the content is arbitrary
        pixels[i + 2] = (g >> 8);
        pixels[i + 3] = (g & 255);
      }
      if (num_channels >= 3) {
        pixels[i + 4] = (b >> 8);
        pixels[i + 5] = (b & 255);
      }
      if (num_channels >= 4) {
        pixels[i + 6] = (a >> 8);
        pixels[i + 7] = (a & 255);
      }
    }
  }
  return pixels;
}

}  // namespace test
}  // namespace jxl

#endif  // LIB_JXL_TEST_IMAGE_H_