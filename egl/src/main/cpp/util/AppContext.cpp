#include "AppContext.h"
#include "log.h"
#include "native_common.h"

using namespace hiveVG;

std::mutex             CAppContext::m_AppMutex;
NativeResourceManager* CAppContext::m_pNativeResManager = nullptr;
std::string            CAppContext::m_StoragePath = "";

napi_value CAppContext::setResourceManager(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    m_pNativeResManager = OH_ResourceManager_InitNativeResourceManager(env, args[0]);
    LOGI("Resource manager sets down!");
    return 0;
}

NativeResourceManager* CAppContext::getResourceManager()
{
    if (m_pNativeResManager == nullptr)
        LOGE("Resource manager does not exist.");
    return m_pNativeResManager;
}

void CAppContext::setStoragePath(const std::string &vPath)
{
    std::string StoragePath = vPath;
    if (StoragePath.back() != '/') {
        StoragePath += '/';
    }
    m_StoragePath = StoragePath;
}

std::string CAppContext::getStoragePath()
{
    if(m_StoragePath.empty())
        LOGE("Storage path does not set.");
    return m_StoragePath;
}
