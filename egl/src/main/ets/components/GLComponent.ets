import { hilog } from '@kit.PerformanceAnalysisKit';
import egl from "libegl.so"
import { Params } from './Contants';
import resourceManager from '@ohos.resourceManager';

interface XComponentAttrs {
  id: string;
  type: number;
  libraryname: string;
}

@Component
export struct GLComponent {
  @State context: Context = this.getUIContext().getHostContext() as Context;
  private resMgr: resourceManager.ResourceManager = this.context.resourceManager;
  xComponentContext: object | undefined = undefined;
  xComponentAttrs: XComponentAttrs = {
    id: 'xcomponentId', // 与XComponent组件为一一对应关系，不建议重复。可以在native侧通过OH_NativeXComponent_GetXComponentId接口来获取对应的id从而绑定对应的XComponent。
    type: XComponentType.SURFACE,
    libraryname: 'egl' // 加载模块的名称，必须与在native侧Napi模块注册时模块名字一致。
  }

  build() {
    Row() {
      Column() {
        Button('点击显示菜单')
          .bindMenu([
            {
              value: '绘制三角形',
              action: () => {
                egl.setParams(Params.TYPE_TRIANGLE)
              }
            },
            {
              value: '使用VBO绘制三角形',
              action: () => {
                egl.setParams(Params.TYPE_VBO_TRIANGLE)
              }
            },
            {
              value: '使用EBO绘制矩形',
              action: () => {
                egl.setParams(Params.TYPE_EBO_TRIANGLE)
              }
            },
            {
              value: '使用VAO绘制三角形',
              action: () => {
                egl.setParams(Params.TYPE_VAO_TRIANGLE)
              }
            },
            {
              value: '原生读取PNG',
              action: () => {
                try {
                  egl.setResourceManager(this.resMgr);
                  egl.setParams(Params.TYPE_IMAGE_FROM_NATIVE)
                } catch (e) {
                  hilog.error(0x0000, 'GL', `原生读取PNG失败: ${JSON.stringify(e)}`)
                }
              }
            }
          ])
          .height(48)
          .width('90%')
          .margin({top: 10, bottom: 10})
        XComponent(this.xComponentAttrs)
          .onLoad((xComponentContext) => {
            this.xComponentContext = xComponentContext
          })
      }
      .width('100%')
    }
    .height('100%')
  }

}