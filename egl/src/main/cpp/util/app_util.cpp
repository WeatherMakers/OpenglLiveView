/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "app_util.h"
#include "render/EglRender.h"
#include "log.h"
#include <GLES3/gl3.h>
#include <string>

#include "stb_image.h"
#include <multimedia/image_framework/image/image_source_native.h>

bool LoadPngFromAssetManager(const std::string &path)
{
    auto resMgr = hiveVG::EglRender::m_pNativeResManager;
    auto file = OH_ResourceManager_OpenRawFile(resMgr, path.c_str());
    if (file == nullptr) {
        LOGE("Failed to run OH_ResourceManager_OpenRawFile(%{public}s).", path.c_str());
        return false;
    }
    int fileSize = OH_ResourceManager_GetRawFileSize(file);
    int realReadBytes = 0;
    unsigned char *buffer = nullptr;

    do {
        buffer = new (std::nothrow) unsigned char[fileSize];
        if (buffer == nullptr) {
            LOGE("Failed to alloc mem for read rawfile(%{public}s), file size: %d.", path.c_str(), fileSize);
            break;
        }

        realReadBytes = OH_ResourceManager_ReadRawFile(file, buffer, fileSize);
        if (fileSize != realReadBytes) {
            LOGE("Failed to run OH_ResourceManager_ReadRawFile(%{public}s), file size: %{public}d, "
                 "real read: %{public}d.", path.c_str(), fileSize, realReadBytes);
            break;
        }

        int w;
        int h;
        int channels;
        int align;
        unsigned char *img = stbi_load_from_memory(buffer, fileSize, &w, &h, &channels, 4);
        LOGI("LoadPngFromAssetManager wh = %{public}dx%{public}d.", w, h);
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &align);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);
        if (img) {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, (const void *)img);
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT, align);
        stbi_image_free(img);
    } while (false);
    delete[] buffer;
    OH_ResourceManager_CloseRawFile(file);
    return true;
}