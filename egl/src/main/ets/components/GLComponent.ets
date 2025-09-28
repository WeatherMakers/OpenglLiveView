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
  @State currentSceneType: string = 'none' // 当前场景类型：'rain'、'snow' 或 'none'
  xComponentAttrs: XComponentAttrs = {
    id: 'xcomponentId', // 与XComponent组件为一一对应关系，不建议重复。可以在native侧通过OH_NativeXComponent_GetXComponentId接口来获取对应的id从而绑定对应的XComponent。
    type: XComponentType.SURFACE,
    libraryname: 'egl' // 加载模块的名称，必须与在native侧Napi模块注册时模块名字一致。
  }
  // 空函数：闪电、云朵、小雨、中雨、大雨、暴雨
  private onLightning(): void {
    try { egl.triggerLightning() } catch (e) { hilog.error(0x0000, 'GL', `triggerLightning 失败: ${JSON.stringify(e)}`) }
  }
  private onCloud(): void {
    try { egl.triggerCloud() } catch (e) { hilog.error(0x0000, 'GL', `triggerCloud 失败: ${JSON.stringify(e)}`) }
  }
  private onLightRain(): void {
    try { egl.triggerLightRain() } catch (e) { hilog.error(0x0000, 'GL', `triggerLightRain 失败: ${JSON.stringify(e)}`) }
  }
  private onModerateRain(): void {
    try { egl.triggerModerateRain() } catch (e) { hilog.error(0x0000, 'GL', `triggerModerateRain 失败: ${JSON.stringify(e)}`) }
  }
  private onHeavyRain(): void {
    try { egl.triggerHeavyRain() } catch (e) { hilog.error(0x0000, 'GL', `triggerHeavyRain 失败: ${JSON.stringify(e)}`) }
  }
  private onStormRain(): void {
    try { egl.triggerStormRain() } catch (e) { hilog.error(0x0000, 'GL', `triggerStormRain 失败: ${JSON.stringify(e)}`) }
  }
  
  // 雪景相关函数
  private onSnowBackground(): void {
    try { egl.triggerSnowBackground() } catch (e) { hilog.error(0x0000, 'GL', `triggerSnowBackground 失败: ${JSON.stringify(e)}`) }
  }
  private onSnowForeground(): void {
    try { egl.triggerSnowForeground() } catch (e) { hilog.error(0x0000, 'GL', `triggerSnowForeground 失败: ${JSON.stringify(e)}`) }
  }
  private onLightSnow(): void {
    try { egl.triggerLightSnow() } catch (e) { hilog.error(0x0000, 'GL', `triggerLightSnow 失败: ${JSON.stringify(e)}`) }
  }
  private onModerateSnow(): void {
    try { egl.triggerModerateSnow() } catch (e) { hilog.error(0x0000, 'GL', `triggerModerateSnow 失败: ${JSON.stringify(e)}`) }
  }
  private onHeavySnow(): void {
    try { egl.triggerHeavySnow() } catch (e) { hilog.error(0x0000, 'GL', `triggerHeavySnow 失败: ${JSON.stringify(e)}`) }
  }
  private onStormSnow(): void {
    try { egl.triggerStormSnow() } catch (e) { hilog.error(0x0000, 'GL', `triggerStormSnow 失败: ${JSON.stringify(e)}`) }
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
                   this.currentSceneType = 'none'
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
                   this.currentSceneType = 'none'
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
                   this.currentSceneType = 'none'
                 } catch (e) {
                   hilog.error(0x0000, 'GL', `雨景，显示失败: ${JSON.stringify(e)}`)
                 }
               }
             },
             {
               value: '云景',
               action: () => {
                 try {
                   egl.setRenderType(Params.CLOUD_RENDER_TYPE)
                   this.currentSceneType = 'none'
                 } catch (e) {
                   hilog.error(0x0000, 'GL', `云景，显示失败: ${JSON.stringify(e)}`)
                 }
               }
             },
             {
               value: '雪景',
               action: () => {
                 try {
                   egl.setRenderType(Params.SNOW_RENDER_TYPE)
                   this.currentSceneType = 'none'
                 } catch (e) {
                   hilog.error(0x0000, 'GL', `雪景，显示失败: ${JSON.stringify(e)}`)
                 }
               }
             },
            {
              value: '雨景全场景',
              action: () => {
                try {
                  egl.setRenderType(Params.FULL_SCENE_RENDER_TYPE)
                  this.currentSceneType = 'rain'
                } catch (e) {
                  hilog.error(0x0000, 'GL', `雨景全场景，显示失败: ${JSON.stringify(e)}`)
                }
              }
            },
            {
              value: '雪景全场景',
              action: () => {
                try {
                  egl.setRenderType(Params.FULL_SNOW_SCENE_RENDER_TYPE)
                  this.currentSceneType = 'snow'
                } catch (e) {
                  hilog.error(0x0000, 'GL', `雪景全场景，显示失败: ${JSON.stringify(e)}`)
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
           .width('100%')
           .height('55%')
         
         // 根据场景类型显示不同的按钮组
         if (this.currentSceneType === 'rain') {
           Grid() {
             GridItem() {
               Button('闪电')
                 .borderRadius(16)
                 .width('100%')
                 .height(40)
                 .onClick(() => this.onLightning())
             }
             GridItem() {
               Button('云朵')
                 .borderRadius(16)
                 .width('100%')
                 .height(40)
                 .onClick(() => this.onCloud())
             }
             GridItem() {
               Button('小雨')
                 .borderRadius(16)
                 .width('100%')
                 .height(40)
                 .onClick(() => this.onLightRain())
             }
             GridItem() {
               Button('中雨')
                 .borderRadius(16)
                 .width('100%')
                 .height(40)
                 .onClick(() => this.onModerateRain())
             }
             GridItem() {
               Button('大雨')
                 .borderRadius(16)
                 .width('100%')
                 .height(40)
                 .onClick(() => this.onHeavyRain())
             }
             GridItem() {
               Button('暴雨')
                 .borderRadius(16)
                 .width('100%')
                 .height(40)
                 .onClick(() => this.onStormRain())
             }
           }
           .columnsTemplate('1fr 1fr')
           .rowsTemplate('auto auto auto')
           .columnsGap(12)
           .rowsGap(12)
           .width('90%')
           .height('30%')
           .margin({ top: 10 })
         } else if (this.currentSceneType === 'snow') {
           Grid() {
             GridItem() {
               Button('前景雪')
                 .borderRadius(16)
                 .width('100%')
                 .height(40)
                 .onClick(() => this.onSnowForeground())
             }
             GridItem() {
               Button('背景雪')
                 .borderRadius(16)
                 .width('100%')
                 .height(40)
                 .onClick(() => this.onSnowBackground())
             }
             GridItem() {
               Button('小雪')
                 .borderRadius(16)
                 .width('100%')
                 .height(40)
                 .onClick(() => this.onLightSnow())
             }
             GridItem() {
               Button('中雪')
                 .borderRadius(16)
                 .width('100%')
                 .height(40)
                 .onClick(() => this.onModerateSnow())
             }
             GridItem() {
               Button('大雪')
                 .borderRadius(16)
                 .width('100%')
                 .height(40)
                 .onClick(() => this.onHeavySnow())
             }
             GridItem() {
               Button('暴雪')
                 .borderRadius(16)
                 .width('100%')
                 .height(40)
                 .onClick(() => this.onStormSnow())
             }
           }
           .columnsTemplate('1fr 1fr')
           .rowsTemplate('auto auto auto')
           .columnsGap(12)
           .rowsGap(12)
           .width('90%')
           .height('30%')
           .margin({ top: 10 })
         } else {
           Blank()
             .width('90%')
             .height('30%')
             .margin({ top: 10 })
         }
      }
      .width('100%')
    }
    .height('100%')
  }
}