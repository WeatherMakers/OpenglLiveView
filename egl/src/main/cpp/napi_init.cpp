#include "Common.h"
#include "render/NativeRenderer.h"
#include "napi/native_api.h"
#include "log.h"

using namespace hiveVG;

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
    LOGI(TAG_KEYWORD::MAIN_TAG, "执行 Init ");
    return CNativeRenderer::Init(env, exports);
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "egl",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterEglModule(void)
{
    napi_module_register(&demoModule);
}
