import {LiveViewComponent} from './LiveViewComponent'
@Entry
@Component
struct DeliveryApp {
    @State _IsPlaying:     boolean = false;
    @State _RestartSignal: boolean = false;

    build() {
        Column() {
            LiveViewComponent({
                m_IsPlaying: $_IsPlaying,
                m_ShouldRestart: $_RestartSignal
            })
                .margin(20)

            Row() {
                Button('开始动画')
                    .width(120)
                    .height(40)
                    .fontColor('#FFFFFF')
                    .backgroundColor('#6E7A8A')
                    .borderRadius(8)
                    .onClick(() => {
                        this._RestartSignal = true;
                    })
                Button(this._IsPlaying ? '暂停动画' : '继续动画')
                    .width(120)
                    .height(40)
                    .margin({ left: 20 })
                    .fontColor('#000000')
                    .backgroundColor(this._IsPlaying ? '#D5DDE5' : '#63A2D8')
                    .borderRadius(8)
                    .onClick(() => {
                        this._IsPlaying = !this._IsPlaying;
                    })
            }
            .margin({ top: 20 })
        }
        .width('100%')
        .height('100%')
        .backgroundColor('#F5F5F5')
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)
    }
}