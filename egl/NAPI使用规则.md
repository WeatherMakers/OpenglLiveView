# NAPI使用规则

基于EGL项目的C++代码和TypeScript接口定义，总结的NAPI开发使用规则。

## 1. 项目结构规范

### 1.1 目录结构
```
src/main/cpp/
├── include/           # 头文件目录
│   ├── EglCore.h     # 核心功能类头文件
│   ├── EglRender.h   # 渲染管理类头文件
│   ├── log.h         # 日志头文件
│   └── native_common.h # NAPI通用宏定义
├── render/           # 实现文件目录
│   ├── EglCore.cpp   # 核心功能实现
│   └── EglRender.cpp # 渲染管理实现
├── example/          # 示例代码目录
├── util/             # 工具类目录
├── types/            # TypeScript类型定义
│   └── libegl/
│       └── index.d.ts # 接口定义文件
└── napi_init.cpp     # NAPI模块初始化文件
```

### 1.2 文件命名规范
- 头文件：`ClassName.h`
- 实现文件：`ClassName.cpp`
- TypeScript接口：`index.d.ts`
- 模块初始化：`napi_init.cpp`

## 2. NAPI模块注册规范

### 2.1 模块初始化文件 (napi_init.cpp)

```cpp
#include "EglRender.h"
#include "log.h"
#include "napi/native_api.h"

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
    // 定义导出的函数属性
    napi_property_descriptor desc[] = {
        { "setParams", nullptr, EglRender::getInstance()->setParams, nullptr, nullptr, nullptr, napi_default, nullptr }
    };
    
    // 注册属性到exports对象
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    
    // 调用类的导出方法
    EglRender::getInstance()->Export(env, exports);
    
    return exports;
}
EXTERN_C_END

// 定义模块结构
static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "egl",  // 模块名称，必须与前端导入名称一致
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

// 模块注册函数
extern "C" __attribute__((constructor)) void RegisterEglModule(void)
{
    napi_module_register(&demoModule);
}
```

### 2.2 关键要点
- 使用 `EXTERN_C_START` 和 `EXTERN_C_END` 包装C++代码
- 模块名称 `nm_modname` 必须与前端导入的库名称一致
- 使用 `__attribute__((constructor))` 确保模块自动注册

## 3. 类设计规范

### 3.1 单例模式实现

```cpp
// 头文件 (EglRender.h)
class EglRender {
private:
    static EglRender* instance;
    
public:
    static EglRender* getInstance();
    void Export(napi_env env, napi_value exports);
    static napi_value setParams(napi_env env, napi_callback_info info);
    // ... 其他成员
};

// 实现文件 (EglRender.cpp)
EglRender *EglRender::instance;

EglRender *EglRender::getInstance()
{
    if (instance == nullptr)
    {
        instance = new EglRender();
    }
    return instance;
}
```

### 3.2 核心功能类设计

```cpp
// 头文件 (EglCore.h)
class EglCore {
private:
    // EGL相关成员变量
    EGLDisplay eglDisplay;
    EGLContext eglContext;
    EGLConfig eglConfig;
    EGLSurface eglSurface;
    
public:
    ~EglCore();
    bool EglContextInit(void *window, int width, int height);
    void setParams(int params);
    // ... 其他方法
};
```

## 4. NAPI函数实现规范

### 4.1 函数签名规范

```cpp
// 静态函数，用于NAPI回调
static napi_value functionName(napi_env env, napi_callback_info info)
{
    // 函数实现
}
```

### 4.2 参数处理规范

```cpp
napi_value EglRender::setParams(napi_env env, napi_callback_info info)
{
    // 1. 声明参数个数
    size_t argc = 1;
    
    // 2. 声明参数数组
    napi_value args[1] = {nullptr};
    
    // 3. 获取参数
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    
    // 4. 类型转换
    int params;
    napi_get_value_int32(env, args[0], &params);
    
    // 5. 业务逻辑处理
    EglCore *eglCore = EglRender::getInstance()->eglCore;
    eglCore->setParams(params);
    
    // 6. 返回值（void函数返回nullptr）
    return nullptr;
}
```

### 4.3 常用类型转换

```cpp
// 获取int32类型参数
int32_t value;
napi_get_value_int32(env, args[0], &value);

// 获取string类型参数
size_t strLen;
napi_get_value_string_utf8(env, args[0], nullptr, 0, &strLen);
char* str = new char[strLen + 1];
napi_get_value_string_utf8(env, args[0], str, strLen + 1, &strLen);

// 获取boolean类型参数
bool boolValue;
napi_get_value_bool(env, args[0], &boolValue);

// 获取object类型参数
napi_value obj = args[0];
napi_value property;
napi_get_named_property(env, obj, "propertyName", &property);
```

## 5. XComponent集成规范

### 5.1 回调函数注册

```cpp
void EglRender::Export(napi_env env, napi_value exports)
{
    // 1. 获取XComponent对象
    napi_value exportInstance = nullptr;
    napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance);
    
    // 2. 解包获取原生对象
    OH_NativeXComponent *nativeXComponent = nullptr;
    napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent));
    
    // 3. 获取组件ID
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t size = OH_XCOMPONENT_ID_LEN_MAX + 1;
    OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &size);
    
    // 4. 注册回调函数
    OH_NativeXComponent_RegisterCallback(nativeXComponent, &callback);
}
```

### 5.2 回调函数实现

```cpp
// 表面创建回调
void OnSurfaceCreated(OH_NativeXComponent *component, void *window)
{
    // 参数验证
    if (nullptr == component || nullptr == window)
    {
        LOGE("component or window is null");
        return;
    }
    
    // 获取尺寸
    uint64_t width, height;
    OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
    
    // 初始化EGL上下文
    EglRender::getInstance()->eglCore->EglContextInit(window, width, height);
}

// 表面销毁回调
void OnSurfaceDestroyed(OH_NativeXComponent *component, void *window)
{
    // 清理资源
    EglRender *instance = EglRender::getInstance();
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
}
```

## 6. TypeScript接口定义规范

### 6.1 接口文件 (index.d.ts)

```typescript
// 导出函数接口
export const setParams: (params: number) => void;
```

### 6.2 常量定义 (Contants.ets)

```typescript
export class Params {
  static readonly TYPE_TRIANGLE: number = 1
  static readonly TYPE_VBO_TRIANGLE: number = Params.TYPE_TRIANGLE + 1
  static readonly TYPE_EBO_TRIANGLE: number = Params.TYPE_VBO_TRIANGLE + 1
  static readonly TYPE_VAO_TRIANGLE: number = Params.TYPE_EBO_TRIANGLE + 1
}
```

## 7. 前端使用规范

### 7.1 模块导入

```typescript
import egl from "libegl.so"  // 导入编译后的.so库
import { Params } from './Contants';  // 导入常量定义
```

### 7.2 XComponent配置

```typescript
interface XComponentAttrs {
  id: string;           // 组件ID，用于与native侧绑定
  type: number;         // 组件类型
  libraryname: string;  // 库名称，必须与native侧模块名一致
}

xComponentAttrs: XComponentAttrs = {
  id: 'xcomponentId',           // 唯一ID
  type: XComponentType.SURFACE, // 表面类型
  libraryname: 'egl'            // 与napi_init.cpp中的nm_modname一致
}
```

### 7.3 函数调用

```typescript
// 调用native函数
egl.setParams(Params.TYPE_TRIANGLE)
```

## 8. 错误处理规范

### 8.1 使用通用宏定义

```cpp
// 使用native_common.h中定义的宏
NAPI_ASSERT(env, assertion, message);
NAPI_CALL(env, napi_function_call);
GET_AND_THROW_LAST_ERROR(env);
```

### 8.2 日志记录

```cpp
#include "log.h"

// 使用日志宏
LOGD("调试信息");
LOGE("错误信息");
```

## 9. 资源管理规范

### 9.1 内存管理

```cpp
// 构造函数中分配资源
EglRender::EglRender()
{
    eglCore = new EglCore();
    // 设置回调
    callback.OnSurfaceCreated = OnSurfaceCreated;
    callback.OnSurfaceChanged = OnSurfaceChanged;
    callback.OnSurfaceDestroyed = OnSurfaceDestroyed;
}

// 析构函数中释放资源
EglRender::~EglRender()
{
    if (eglCore)
    {
        delete eglCore;
        eglCore = nullptr;
    }
}
```

### 9.2 EGL资源管理

```cpp
void EglCore::release()
{
    // 按顺序释放EGL资源
    if (!eglDestroySurface(eglDisplay, eglSurface))
    {
        LOGE("销毁eglSurface失败");
    }
    if (!eglDestroyContext(eglDisplay, eglContext))
    {
        LOGE("销毁eglContext失败");
    }
    if (!eglTerminate(eglDisplay))
    {
        LOGE("销毁eglDisplay失败");
    }
}
```

## 10. 编译配置规范

### 10.1 CMakeLists.txt配置

```cmake
# 添加NAPI头文件路径
target_include_directories(${CMAKE_CURRENT_SOURCE_DIR} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
)

# 链接必要的库
target_link_libraries(${CMAKE_CURRENT_SOURCE_DIR} PUBLIC
    libace_napi.z.so
    libace_xcomponent.z.so
    libEGL.so
    libGLESv3.so
)
```

### 10.2 模块配置文件

```json5
// oh-package.json5
{
  "name": "libegl",
  "version": "1.0.0",
  "description": "EGL OpenGL ES NAPI Module",
  "main": "index.d.ts",
  "author": "",
  "license": "",
  "dependencies": {},
  "devDependencies": {}
}
```

## 11. 最佳实践

### 11.1 代码组织
- 将NAPI相关代码与业务逻辑分离
- 使用单例模式管理全局状态
- 合理使用RAII进行资源管理

### 11.2 性能优化
- 避免频繁的JS-Native类型转换
- 使用对象池管理临时对象
- 合理使用缓存减少重复计算

### 11.3 调试技巧
- 使用日志宏记录关键信息
- 在关键路径添加错误检查
- 使用断言验证前置条件

### 11.4 兼容性考虑
- 注意不同API版本的差异
- 处理不同设备的兼容性问题
- 提供降级方案

## 12. 常见问题与解决方案

### 12.1 模块加载失败
- 检查模块名称是否一致
- 确认.so文件路径正确
- 验证依赖库是否完整

### 12.2 函数调用失败
- 检查参数类型和数量
- 验证函数签名匹配
- 确认对象生命周期正确

### 12.3 内存泄漏
- 确保所有分配的资源都被释放
- 使用智能指针管理内存
- 定期检查内存使用情况

这些规则基于EGL项目的实际代码总结，可以作为NAPI开发的参考指南。
