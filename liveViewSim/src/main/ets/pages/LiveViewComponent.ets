import {CRiderStatus} from './LiveViewModel';

@Component
export struct LiveViewComponent {
  @State m_RiderStatus:   CRiderStatus = new CRiderStatus();
  @State m_IconPosition:  number = 0;
  @Link  m_IsPlaying:     boolean;
  @Link @Watch('resetAnimation')  m_ShouldRestart: boolean;

  private readonly m_LogoIcon:        Resource = $r('app.media.logo');
  private readonly m_CheckIcon:       Resource = $r('app.media.check_icon');
  private readonly m_DeliveryManIcon: Resource = $r('app.media.delivery_man');

  private m_Timer: number | null = null;
  private m_ProgressValue: number = 0;

  aboutToAppear(): void
  {
    if(this.m_IsPlaying)
    {
      this.__startAnimation();
    }
  }

  resetAnimation(): void
  {
    if(this.m_ShouldRestart)
    {
      this.m_IsPlaying = true;
      this.__startAnimation();
      this.m_ShouldRestart = false;
    }
  }

  aboutToDisappear(): void
  {
    this.__clearTimer();
  }

  private __startAnimation(): void
  {
    if(this.m_Timer != null) this.__clearTimer();

    this.m_Timer = setInterval(() =>
    {
      if (this.m_IsPlaying)
      {
        this.m_ProgressValue = (this.m_ProgressValue + 1);
        this.__updateRiderInfo();
        this.__updateIconPosition();
        console.log("定时器ID:", this.m_Timer)

        if (this.m_ProgressValue >= 100)
        {
          this.m_IsPlaying = false;
        }
      }
    }, 50);
    console.log("赋值后:", this.m_Timer)
  }

  private __clearTimer(): void
  {
    if (this.m_Timer != null)
    {
      clearInterval(this.m_Timer);
      this.m_Timer = null;
    }
    this.m_ProgressValue = 0;
  }

  private __updateRiderInfo(): void
  {
    const Distance = 300 - Math.floor(this.m_ProgressValue * 3);
    const Time = 3 - Math.floor(this.m_ProgressValue / 33);

    this.m_RiderStatus = new CRiderStatus(
      "骑手已接单",
      Math.max(Distance, 0),
      Math.max(Time, 0)
    );
  }

  private __updateIconPosition(): void {
    // 进度条总宽度约280px (320-16 * 2 padding)
    this.m_IconPosition = this.m_ProgressValue * 2.5;
  }

  build() {
    Stack() {
      Image($r('app.media.background1200_670'))
        .width('100%')
        .height('100%')
        .objectFit(ImageFit.Fill)
        .position({ y: 10 })
        .opacity(0.4)

      Column() {
        this.buildHeader()
        Blank().height(40)
        this.buildProgressBar()
      }
      .width('100%')
      .height('100%')
    }
    .width(320)
    .height(160)
    .padding(16)
    .backgroundColor('#FFFFFF')
    .borderRadius(20)
    .shadow({radius: 10, color: '#10000000', offsetX: 0, offsetY: 4})
  }
  @Builder
  buildHeader() {
    Row() {
      Image(this.m_LogoIcon)
        .width(32)
        .height(32)
        .margin({right: 12})

      Column() {
        Text(this.m_RiderStatus.m_Status)
          .fontSize(16)
          .fontColor('#000000')
          .fontWeight(FontWeight.Bold)

        Text(`距商家 ${this.m_RiderStatus.m_Distance}米 | ${this.m_RiderStatus.m_Time}分钟到店`)
          .fontSize(12)
          .fontColor('#666666')
          .margin({ top: 4 })
      }
      .alignItems(HorizontalAlign.Start)

      Blank()

      Image(this.m_CheckIcon)
        .width(24)
        .height(24)
    }
    .width('100%')
    .margin({ bottom: 20 })
  }

  @Builder
  buildProgressBar() {
    Stack() {
      Row()
        .width('100%')
        .height(6)
        .backgroundColor('#646464')
        .borderRadius(3)
        .position({ y: 22 })

      // 可移动的SVG图标
      this.buildMovingIcon()
    }
    .width('100%')
    .height(30)
  }

  @Builder
  buildMovingIcon() {
    Column() {
      Image(this.m_DeliveryManIcon)
        .width(40)
        .height(40)
        .objectFit(ImageFit.Contain)
    }
    .width(40)
    .height(40)
    .position({ x: this.m_IconPosition, y: -10 })
  }
}