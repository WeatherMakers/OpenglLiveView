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
  aboutToAppear() {
    console.log("aboutToAppear.");
    // let resMgr = this.context.resourceManager;
    // egl.setResourceManager(resMgr);
    // console.log("ResourceManager has been set.");
  }
  build() {
    Row() {
      Column() {
        Button('点击显示菜单')
          .bindMenu([
            {
              value: '原生读取PNG',
              action: () => {
                try {
                  egl.setRenderType(Params.TYPE_IMAGE_FROM_NATIVE)
                } catch (e) {
                  hilog.error(0x0000, 'GL', `原生读取PNG失败: ${JSON.stringify(e)}`)
                }
              }
            },
            {
              value: '原生SinglePlayer显示',
              action: () => {
                try {
                  egl.setRenderType(Params.TYPE_SINGLE_PLAYER)
                } catch (e) {
                  hilog.error(0x0000, 'GL', `原生SinglePlayer显示失败: ${JSON.stringify(e)}`)
                }
              }
            },
            {
              value: '原生SeqPlayer显示',
              action: () => {
                try {
                  egl.setRenderType(Params.TYPE_SEQ_PLAYER)
                } catch (e) {
                  hilog.error(0x0000, 'GL', `原生SeqPlayer显示失败: ${JSON.stringify(e)}`)
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
            egl.setResourceManager(this.resMgr);
          })
      }
      .width('100%')
    }
    .height('100%')
  }

}