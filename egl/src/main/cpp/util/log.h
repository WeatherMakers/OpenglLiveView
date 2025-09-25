/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef LOG_H
#define LOG_H

#include <hilog/log.h>

#ifndef LOG_TAG
#define LOG_TAG "[OpenGL]"
#endif

// 支持Android风格的日志宏：
// LOGI(TAG, "message %d", value) - 使用自定义标签
// 也支持向后兼容：LOGI("message") - 使用默认标签

#ifndef LOGI
#define LOGI(tag, ...) ((void)OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, tag, __VA_ARGS__))
#endif

#ifndef LOGD
#define LOGD(tag, ...) ((void)OH_LOG_Print(LOG_APP, LOG_DEBUG, LOG_DOMAIN, tag, __VA_ARGS__))
#endif

#ifndef LOGW
#define LOGW(tag, ...) ((void)OH_LOG_Print(LOG_APP, LOG_WARN, LOG_DOMAIN, tag, __VA_ARGS__))
#endif

#ifndef LOGE
#define LOGE(tag, ...) ((void)OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, tag, __VA_ARGS__))
#endif
#endif 