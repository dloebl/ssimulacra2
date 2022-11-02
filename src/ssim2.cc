// Copyright (c) Jon Sneyers, Cloudinary. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <stdio.h>

#include "lib/extras/codec.h"
#include "lib/jxl/color_management.h"
#include "lib/jxl/enc_color_management.h"
#include "ssimulacra2.h"

#ifdef __cplusplus
extern "C" {
#endif

double GetSSIM2Score(const uint8_t* org, size_t org_len, const uint8_t* dist, size_t dist_len) {
  jxl::CodecInOut io1;
  jxl::CodecInOut io2;

  jxl::Span<const uint8_t> a = jxl::Span<const uint8_t>(org, org_len);
  jxl::Span<const uint8_t> b = jxl::Span<const uint8_t>(dist, dist_len);
  JXL_CHECK(SetFromBytes(a, jxl::extras::ColorHints(), &io1));

  if (io1.xsize() < 8 || io1.ysize() < 8) {
    fprintf(stderr, "Minimum image size is 8x8 pixels\n");
    return 1;
  }

  JXL_CHECK(SetFromBytes(b, jxl::extras::ColorHints(), &io2));
  if (io1.xsize() != io2.xsize() || io1.ysize() != io2.ysize()) {
    fprintf(stderr, "Image size mismatch\n");
    return 1;
  }

  if (!io1.Main().HasAlpha()) {
    Msssim msssim = ComputeSSIMULACRA2(io1.Main(), io2.Main());
    return msssim.Score();
  } else {
    // in case of alpha transparency: blend against dark and bright backgrounds
    // and return the worst of both scores
    Msssim msssim0 = ComputeSSIMULACRA2(io1.Main(), io2.Main(), 0.1f);
    Msssim msssim1 = ComputeSSIMULACRA2(io1.Main(), io2.Main(), 0.9f);
    return std::min(msssim0.Score(), msssim1.Score());
  }
  return 0;
}

#ifdef __cplusplus
}
#endif
