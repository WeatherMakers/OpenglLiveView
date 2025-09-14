#include "render/EglRender.h"
#include "napi/native_api.h"
#include "AppContext.h"

using namespace hiveVG;

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        { "renderScene", nullptr, EglRender::getInstance()->renderScene, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "setResourceManager", nullptr, CAppContext::setResourceManager, nullptr, nullptr, nullptr, napi_default, nullptr},
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    EglRender::getInstance()->Export(env, exports);
    return exports;
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
