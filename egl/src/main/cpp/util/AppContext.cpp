#include "AppContext.h"
#include "Common.h"
#include "log.h"
#include "native_common.h"

using namespace hiveVG;

NativeResourceManager* CAppContext::m_pNativeResManager = nullptr;

napi_value CAppContext::setResourceManager(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    m_pNativeResManager = OH_ResourceManager_InitNativeResourceManager(env, args[0]);
    LOGI(TAG_KEYWORD::APP_CONTEXT_TAG, "Resource manager sets down!");
    return 0;
}

NativeResourceManager* CAppContext::getResourceManager()
{
    if (m_pNativeResManager == nullptr)
        LOGE(TAG_KEYWORD::APP_CONTEXT_TAG, "Resource manager does not exist.");
    return m_pNativeResManager;
}
