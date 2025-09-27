import { hilog } from '@kit.PerformanceAnalysisKit';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, MockKit, when } from '@ohos/hypium';

interface SharedObj {
  name: string;
  value: number;
  /** 声明一个可选属性，用于示范 assertUndefined */
  extra?: undefined;
}

// 定义一个示例类
class Calculator {
  add(a: number, b: number): number {
    return a + b;
  }
  multiply(a: number, b: number): number {
    return a * b;
  }
}

export default function mockTest() {
  describe('MockAbilityTest', () => {
    let mocker: MockKit;
    let calc: Calculator;

    beforeAll(() => {
      hilog.info(0x0000, 'testTag', '%{public}s', 'Before all tests');
    });

    beforeEach(() => {
      hilog.info(0x0000, 'testTag', '%{public}s', 'Before each test');
      mocker = new MockKit();
      calc = new Calculator();
    });

    afterEach(() => {
      hilog.info(0x0000, 'testTag', '%{public}s', 'After each test');
      mocker.clear(mocker); // 还原 mock
    });

    afterAll(() => {
      hilog.info(0x0000, 'testTag', '%{public}s', 'After all tests');
    });

    it('test without mock', 0, () => {
      let result = calc.add(2, 3);
      expect(result).assertEqual(5);
    });

    it('test with mock return custom value', 0, () => {
      let mockFunc = mocker.mockFunc(calc, calc.add);
      when(mockFunc)(2, 3).afterReturn(999); // mock add(2,3) 返回 999

      let result = calc.add(2, 3);
      expect(result).assertEqual(999);
    });

    it('test with mock return undefined', 0, () => {
      let mockFunc = mocker.mockFunc(calc, calc.multiply);
      when(mockFunc)(5, 6).afterReturnNothing(); // mock multiply(5,6) 返回 undefined

      let result = calc.multiply(5, 6);
      expect(result).assertUndefined();
    });

    it('test with interface object', 0, () => {
      const obj: SharedObj = { name: "foo", value: 42 };
      expect(obj.name).assertEqual("foo");
      expect(obj.extra).assertUndefined();
    });
  });
}
