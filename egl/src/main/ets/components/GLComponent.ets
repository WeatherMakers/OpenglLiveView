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
  allSceneButtonHeight :number = 30;
  @State rainGrayValue : number = 100;


  @State currentSceneType: string = 'none' // 当前场景类型：'rain'、'snow'、'all' 或 'none'
  xComponentAttrs: XComponentAttrs = {
    id: 'xcomponentId', // 与XComponent组件为一一对应关系，不建议重复。可以在native侧通过OH_NativeXComponent_GetXComponentId接口来获取对应的id从而绑定对应的XComponent。
    type: XComponentType.SURFACE,
    libraryname: 'egl' // 加载模块的名称，必须与在native侧Napi模块注册时模块名字一致。
  }

  @State fpsValue: number = 30;

  // 空函数：云朵、小雨、中雨、大雨、暴雨
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
  
  // 全场景相关函数
  private onFullSceneRainLight(): void {
    try { egl.triggerFullSceneRainLight() } catch (e) { hilog.error(0x0000, 'GL', `triggerFullSceneRainLight 失败: ${JSON.stringify(e)}`) }
  }
  private onFullSceneRainModerate(): void {
    try { egl.triggerFullSceneRainModerate() } catch (e) { hilog.error(0x0000, 'GL', `triggerFullSceneRainModerate 失败: ${JSON.stringify(e)}`) }
  }
  private onFullSceneRainHeavy(): void {
    try { egl.triggerFullSceneRainHeavy() } catch (e) { hilog.error(0x0000, 'GL', `triggerFullSceneRainHeavy 失败: ${JSON.stringify(e)}`) }
  }
  private onFullSceneRainStorm(): void {
    try { egl.triggerFullSceneRainStorm() } catch (e) { hilog.error(0x0000, 'GL', `triggerFullSceneRainStorm 失败: ${JSON.stringify(e)}`) }
  }
  private onFullSceneRainCloud(): void {
    try { egl.triggerFullSceneRainCloud() } catch (e) { hilog.error(0x0000, 'GL', `triggerFullSceneRainCloud 失败: ${JSON.stringify(e)}`) }
  }
  private onFullSceneSnowLight(): void {
    try { egl.triggerFullSceneSnowLight() } catch (e) { hilog.error(0x0000, 'GL', `triggerFullSceneSnowLight 失败: ${JSON.stringify(e)}`) }
  }
  private onFullSceneSnowModerate(): void {
    try { egl.triggerFullSceneSnowModerate() } catch (e) { hilog.error(0x0000, 'GL', `triggerFullSceneSnowModerate 失败: ${JSON.stringify(e)}`) }
  }
  private onFullSceneSnowHeavy(): void {
    try { egl.triggerFullSceneSnowHeavy() } catch (e) { hilog.error(0x0000, 'GL', `triggerFullSceneSnowHeavy 失败: ${JSON.stringify(e)}`) }
  }
  private onFullSceneSnowStorm(): void {
    try { egl.triggerFullSceneSnowStorm() } catch (e) { hilog.error(0x0000, 'GL', `triggerFullSceneSnowStorm 失败: ${JSON.stringify(e)}`) }
  }
  private onFullSceneSnowBackground(): void {
    try { egl.triggerFullSceneSnowBackground() } catch (e) { hilog.error(0x0000, 'GL', `triggerFullSceneSnowBackground 失败: ${JSON.stringify(e)}`) }
  }
  private onFullSceneSnowForeground(): void {
    try { egl.triggerFullSceneSnowForeground() } catch (e) { hilog.error(0x0000, 'GL', `triggerFullSceneSnowForeground 失败: ${JSON.stringify(e)}`) }
  }

  // 背景自适应相关函数
  private onFullSceneColorSetting(vValue : number) : void {
    try { egl.triggerColorSetting(vValue) } catch(e) { hilog.error(0x0000, 'GL', `triggerFullSceneColorSetting 失败: ${JSON.stringify(e)}`) }
  }

  private onFullSceneCloudThicknessSetting(vValue : number) : void {
    try { egl.triggerCloudThicknessSetting(vValue) } catch(e) { hilog.error(0x0000, 'GL', `triggerCloudThicknessSetting 失败: ${JSON.stringify(e)}`) }
  }


  private onRainColorSelfAdjust() : void {
    try {
      let result = egl.triggerColorSelfAdjustment()
      if (result) {
        this.rainGrayValue = 0;
        this.rainGrayValue = result * 100;
      }
    } catch(e) {
      hilog.error(0x0000, 'GL', `triggerColorSelfAdjustment 失败: ${JSON.stringify(e)}`)
    }
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
             },
             {
               value: '全场景',
               action: () => {
                 try {
                   egl.setRenderType(Params.FULL_ALL_SCENE_RENDER_TYPE)
                   this.currentSceneType = 'all'
                 } catch (e) {
                   hilog.error(0x0000, 'GL', `全场景，显示失败: ${JSON.stringify(e)}`)
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
             this.rainGrayValue = egl.onStartupColorSelfAdjustment()
             this.rainGrayValue = this.rainGrayValue * 100
           })
           .width('100%')
           .height('55%')
         
         // 根据场景类型显示不同的按钮组
         if (this.currentSceneType === 'rain') {
           Grid() {
             GridItem() {
               Button('云朵')
                 .borderRadius(16)
                 .width('100%')
                 .height(40)
                 .onClick(() => this.onCloud())
             }.columnStart(0).columnEnd(1)
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
         } else if (this.currentSceneType === 'all') {
           Row() {
             // 左边：雨景相关（2列3行）
             Column() {
               Text('雨景')
                 .fontSize(14)
                 .fontColor('#666666')
                 .margin({ bottom: 8 })
               
               Grid() {
                 // 第一行
                 GridItem() {
                   Button('小雨')
                     .borderRadius(16)
                     .width('100%')
                     .height(40)
                     .onClick(() => {
                       this.onFullSceneRainLight()
                       this.fpsValue = egl.getFPS()
                     })
                 }
                 GridItem() {
                   Button('中雨')
                     .borderRadius(16)
                     .width('100%')
                     .height(40)
                     .onClick(() => {
                       this.onFullSceneRainModerate()
                       this.fpsValue = egl.getFPS()
                     })
                 }
                 // 第二行
                 GridItem() {
                   Button('大雨')
                     .borderRadius(16)
                     .width('100%')
                     .height(40)
                     .onClick(() => {
                       this.onFullSceneRainHeavy()
                       this.fpsValue = egl.getFPS()
                     })
                 }
                 GridItem() {
                   Button('暴雨')
                     .borderRadius(16)
                     .width('100%')
                     .height(40)
                     .onClick(() => {
                       this.onFullSceneRainStorm()
                       this.fpsValue = egl.getFPS()
                     })
                 }
                 // 第三行
                 GridItem() {
                   Button('云朵')
                     .borderRadius(16)
                     .width('100%')
                     .height(40)
                     .onClick(() => {
                       this.onFullSceneRainCloud()
                       this.fpsValue = egl.getFPS()
                     })
                 }.columnStart(0).columnEnd(1)
               }
               .columnsTemplate('1fr 1fr')
               .rowsTemplate('auto auto auto')
               .columnsGap(12)
               .rowsGap(12)
               .width('100%')
             }
             .width('50%')
             .padding({ right: 8 })

             // 右边：雪景相关（2列3行）
             Column() {
               Text('雪景')
                 .fontSize(14)
                 .fontColor('#666666')
                 .margin({ bottom: 8 })

               Grid() {
                 // 第一行
                 GridItem() {
                   Button('小雪')
                     .borderRadius(16)
                     .width('100%')
                     .height(40)
                     .onClick(() => {
                       this.onFullSceneSnowLight()
                       this.fpsValue = egl.getFPS()
                     })
                 }
                 GridItem() {
                   Button('中雪')
                     .borderRadius(16)
                     .width('100%')
                     .height(40)
                     .onClick(() => {
                       this.onFullSceneSnowModerate()
                       this.fpsValue = egl.getFPS()
                     })
                 }
                 // 第二行
                 GridItem() {
                   Button('大雪')
                     .borderRadius(16)
                     .width('100%')
                     .height(40)
                     .onClick(() => {
                       this.onFullSceneSnowHeavy()
                       this.fpsValue = egl.getFPS()
                     })
                 }
                 GridItem() {
                   Button('暴雪')
                     .borderRadius(16)
                     .width('100%')
                     .height(40)
                     .onClick(() => {
                       this.onFullSceneSnowStorm()
                       this.fpsValue = egl.getFPS()
                     })
                 }
                 // 第三行
                 GridItem() {
                   Button('前景雪')
                     .borderRadius(16)
                     .width('100%')
                     .height(40)
                     .onClick(() => {
                       this.onFullSceneSnowForeground()
                       this.fpsValue = egl.getFPS()
                     })
                 }
                 GridItem() {
                   Button('背景雪')
                     .borderRadius(16)
                     .width('100%')
                     .height(40)
                     .onClick(() => {
                       this.onFullSceneSnowBackground()
                       this.fpsValue = egl.getFPS()
                     })
                 }
               }
               .columnsTemplate('1fr 1fr')
               .rowsTemplate('auto auto auto')
               .columnsGap(12)
               .rowsGap(12)
               .width('100%')
             }
             .width('50%')
             .padding({ left: 8 })
           }
           .width('90%')
           .height('30%')
           .margin({ top: 15 })

           // rain or snow's fps
           Row() {
             Text('fps:')
               .fontSize(14)
               .fontColor('#444')
               .width(50)

             TextInput({ text: this.fpsValue.toString() })
               .type(InputType.Number)
               .width('60%')
               .height(36)
               .border({ width: 1, color: '#CCCCCC' })
               .borderRadius(8)
               .onChange((value: string) => {
                 const num = parseInt(value)
                 if (!isNaN(num) && Number.isInteger(num)) {
                   this.fpsValue = num
                   egl.changeFPS(this.fpsValue)
                 }
               })
           }
           .margin({ top: 16 })
           .alignItems(VerticalAlign.Center)

           Column() {
             Text('雨雪灰度颜色调节')
               .fontSize(14)
               .fontColor('#666666')
               .margin({ top: 30, bottom: 0 })

             Slider({
               value: this.rainGrayValue, // 默认值为100
               min: 0,
               max: 100,
               step: 1,
               style: SliderStyle.OutSet
             })
               .width('80%')
               .onChange((value: number) => {
                 this.onFullSceneColorSetting(value)
               }).height('20%')

             Text('云朵厚度调节')
               .fontSize(14)
               .fontColor('#666666')
               .margin({ top: 0, bottom: 0 })

             Slider({
               value: 100, // 默认值为100
               min: 0,
               max: 100,
               step: 1,
               style: SliderStyle.OutSet
             })
               .width('80%')
               .onChange((value: number) => {
                 this.onFullSceneCloudThicknessSetting(value)
               })
           }.height('20%')

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