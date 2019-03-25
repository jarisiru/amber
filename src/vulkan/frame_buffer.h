// Copyright 2018 The Amber Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SRC_VULKAN_FRAME_BUFFER_H_
#define SRC_VULKAN_FRAME_BUFFER_H_

#include <memory>
#include <vector>

#include "src/pipeline.h"
#include "src/vulkan/transfer_image.h"

namespace amber {
namespace vulkan {

enum class FrameImageState : uint8_t {
  kInit = 0,
  kClearOrDraw,
  kProbe,
};

class CommandBuffer;
class Device;

class FrameBuffer {
 public:
  FrameBuffer(
      Device* device,
      const std::vector<const amber::Pipeline::BufferInfo*>& color_attachments,
      uint32_t width,
      uint32_t height);
  ~FrameBuffer();

  Result Initialize(VkRenderPass render_pass,
                    VkFormat depth_format,
                    const VkPhysicalDeviceMemoryProperties& properties);

  Result ChangeFrameImageLayout(CommandBuffer* command, FrameImageState layout);

  VkFramebuffer GetVkFrameBuffer() const { return frame_; }
  const void* GetColorBufferPtr(size_t idx) const {
    return color_images_[idx]->HostAccessibleMemoryPtr();
  }

  // Only record the command for copying the image that backs this
  // framebuffer to the host accessible buffer. The actual submission
  // of the command must be done later.
  void CopyColorImagesToHost(CommandBuffer* command);

  void CopyImagesToBuffers();

  uint32_t GetWidth() const { return width_; }
  uint32_t GetHeight() const { return height_; }

 private:
  Device* device_ = nullptr;
  std::vector<const amber::Pipeline::BufferInfo*> color_attachments_;
  VkFramebuffer frame_ = VK_NULL_HANDLE;
  std::vector<std::unique_ptr<TransferImage>> color_images_;
  std::unique_ptr<TransferImage> depth_image_;
  uint32_t width_ = 0;
  uint32_t height_ = 0;
  uint32_t depth_ = 1;
  FrameImageState frame_image_layout_ = FrameImageState::kInit;
};

}  // namespace vulkan
}  // namespace amber

#endif  // SRC_VULKAN_FRAME_BUFFER_H_
