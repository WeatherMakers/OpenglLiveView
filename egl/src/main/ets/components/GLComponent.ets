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
              value: '背景',
              action: () => {
                try {
                  egl.setRenderType(Params.SINGLE_RENDER_TYPE)
                } catch (e) {
                  hilog.error(0x0000, 'GL', `背景，显示失败: ${JSON.stringify(e)}`)
                }
              }
            },
            {
              value: '雨滴飞溅',
              action: () => {
                try {
                  egl.setRenderType(Params.SEQ_RENDER_TYPE)
                } catch (e) {
                  hilog.error(0x0000, 'GL', `雨滴飞溅，显示失败: ${JSON.stringify(e)}`)
                }
              }
            },
            {
              value: '雨景',
              action: () => {
                try {
                  egl.setRenderType(Params.RAIN_RENDER_TYPE)
                } catch (e) {
                  hilog.error(0x0000, 'GL', `雨景，显示失败: ${JSON.stringify(e)}`)
                }
              }
            },
            {
              value: '雪景',
              action: () => {
                try {
                  egl.setRenderType(Params.SNOW_RENDER_TYPE)
                } catch (e) {
                  hilog.error(0x0000, 'GL', `雪景，显示失败: ${JSON.stringify(e)}`)
                }
              }
            },
            {
              value: '云景',
              action: () => {
                try {
                  egl.setRenderType(Params.CLOUD_RENDER_TYPE)
                } catch (e) {
                  hilog.error(0x0000, 'GL', `云景，显示失败: ${JSON.stringify(e)}`)
                }
              }
            }
          ])
          .height(48)
          .width('90%')
          .margin({top: 10, bottom: 10})
        XComponent(this.xComponentAttrs)
          .onLoad((xComponentContext) => {
            console.info('XComponent on Load start');
            this.xComponentContext = xComponentContext
            egl.setResourceManager(this.resMgr)
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}