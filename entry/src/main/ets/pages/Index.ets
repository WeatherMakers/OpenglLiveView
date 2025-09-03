import { GLComponent } from 'egl'

@Entry
@Component
struct Index {
    @State message: string = 'Hello World';

    build() {
        RelativeContainer() {
            RelativeContainer() {
                GLComponent()
                    .height('100%')
                    .width('100%')
            }
            .height('100%')
            .width('100%')
        }
    }
}