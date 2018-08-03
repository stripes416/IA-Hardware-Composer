/*
// Copyright (c) 2016 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/

/**
 * \file
 *
 * drmbuffer class declarations and basic getter implementations for members
 */
#ifndef WSI_DRMBUFFER_H_
#define WSI_DRMBUFFER_H_

#include <platformdefines.h>

#include "framebuffermanager.h"
#include "overlaybuffer.h"

#define METADATA(x) image_.handle_->meta_data_.x

namespace hwcomposer {

class NativeBufferHandler;

/**
 * Maintains and works with data connected to DrmBuffer
 */
class DrmBuffer : public OverlayBuffer {
 public:
  DrmBuffer(DrmBuffer&& rhs) = default;
  DrmBuffer& operator=(DrmBuffer&& other) = default;

  DrmBuffer() = default;

  ~DrmBuffer() override;

  /**
   * Initialize DrmBuffer object with data from native handle
   *
   * @param handle internal state reference of struct gbm_handle
   * @param buffer_manager
   * @param frame_buffer_manager
   */
  void InitializeFromNativeHandle(
      HWCNativeHandle handle, ResourceManager* buffer_manager,
      FrameBufferManager* frame_buffer_manager) override;

  uint32_t GetWidth() const override {
    return METADATA(width_);
  }

  uint32_t GetHeight() const override {
    return METADATA(height_);
  }

  uint32_t GetFormat() const override {
    return format_;
  }

  HWCLayerType GetUsage() const override {
    return METADATA(usage_);
  }

  uint32_t GetFb() const override {
    return image_.drm_fd_;
  }

  uint32_t GetPrimeFD() const override {
    return METADATA(prime_fds_[0]);
  }

  const uint32_t* GetPitches() const override {
    return METADATA(pitches_);
  }

  const uint32_t* GetOffsets() const override {
    return METADATA(offsets_);
  }

  uint32_t GetTilingMode() const override {
    return METADATA(tiling_mode_);
  }

  /**
   * Dependent on use of Vulkan or GL, create image for DrmBuffer
   *
   * @param egl_display GpuDisplay handle
   * @param external_import if image is imported
   * @return image_
   */
  const ResourceHandle& GetGpuResource(GpuDisplay egl_display,
                                       bool external_import) override;

  /**
   * With no parameters, return image_
   *
   * @return image_
   */
  const ResourceHandle& GetGpuResource() override;

  /**
   * Returns media_image_ dependent on media backend
   *
   * If libva is enabled:
   *  --  width & height are both the same as the prior
   *      media_image_'s previous_width and previous_height, return prior
   *      media_image_.
   *  --  width & height are not the same as the prior
   *      media_image's set members and return newly created media_image_.
   * If using media backend other than libva, GetMediaResource is not
   * implemented for it -- will return old media_image_.
   *
   * @param display VADisplay handle
   * @param width
   * @param height
   * @return media_image_
   */
  const MediaResourceHandle& GetMediaResource(MediaDisplay display,
                                              uint32_t width,
                                              uint32_t height) override;

  /**
   * Creates frame buffer, or verifies a frame buffer already exists.
   *
   * @return true if frame buffer already exists
   * @return true otherwise after frame buffer is created
   */
  bool CreateFrameBuffer() override;

  /**
   * Creates frame buffer with a modifier, or verifies a frame buffer already
   * exists.
   *
   * @param modifier drm format modifier
   * @return true if frame buffer already exists
   * @return true otherwise after frame buffer is created with modifier
   */
  bool CreateFrameBufferWithModifier(uint64_t modifier) override;

  HWCNativeHandle GetOriginalHandle() const override {
    return original_handle_;
  }

  /**
   * Sets original_handle equal to HWCNative handle
   *
   * @param handle HWCNativeHandle handle
   */
  void SetOriginalHandle(HWCNativeHandle handle) override;

  /**
   * DRMBuffer status output via DUMPTRACE
   */
  void Dump() override;

 private:
  /**
   * Initailize DRMBuffer object with data from the HWC buffer object
   *
   * @param bo HWC buffer object
   */
  void Initialize(const HwcBuffer& bo);
  uint32_t format_ = 0;
  uint32_t frame_buffer_format_ = 0;
  uint32_t previous_width_ = 0;   // For Media usage.
  uint32_t previous_height_ = 0;  // For Media usage.
  ResourceManager* resource_manager_ = 0;
  ResourceHandle image_;
  MediaResourceHandle media_image_;
  HWCNativeHandle original_handle_;
  FrameBufferManager* fb_manager_ = NULL;
};

}  // namespace hwcomposer
#endif  // WSI_DRMBUFFER_H_
