import List from '@ohos.util.List'

interface IComponentRect {
  position: Position;
  width: number;
  height: number;
}

export class InteractiveUtils {
  private m_Components: List<string> = new List();

  static drawBackground<T extends CustomComponent>(component: T): void{
    console.log('框架内部ID:', component['id_']);
  }

}

@Component
export struct InteractiveComponent {
  @BuilderParam itemBuilder: () => void;

  interactWithBackground(flag: boolean): InteractiveComponent {
    InteractiveUtils.drawBackground(this);
    return this;
  }

  aboutToAppear(): void {
    InteractiveUtils.drawBackground(this);
    this.interactWithBackground(true);
  }

  build() {
    this.itemBuilder();
  }
}
