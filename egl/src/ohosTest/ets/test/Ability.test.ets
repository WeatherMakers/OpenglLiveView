import { hilog } from '@kit.PerformanceAnalysisKit';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from '@ohos/hypium';

interface SharedObj {
  name: string;
  value: number;
  /** 声明一个可选属性，用于示范 assertUndefined */
  extra?: undefined;
}

export default function abilityTest(): void {
  describe('ActsAbilityTest', () => {
    let sharedArray: number[] = [];
    let sharedObj: SharedObj | null = null;

    beforeAll(() => {
      // 整个测试套开始前执行一次
      hilog.info(0xFFFF, 'testTag', '%{public}s', 'beforeAll: init global resources');
      sharedArray = [1, 2, 3];
    });

    beforeEach(() => {
      // 每个用例开始前执行
      hilog.info(0xFFFF, 'testTag', '%{public}s', 'beforeEach: prepare data');
      sharedObj = { name: 'test', value: 42 }; // 类型已约束为 SharedObj
    });

    afterEach(() => {
      // 每个用例结束后执行
      hilog.info(0x0000, 'testTag', '%{public}s', 'afterEach: cleanup');
      sharedObj = null;
      expect(sharedObj).assertNull(); // 确保清理成功
    });

    afterAll(() => {
      // 所有用例执行结束后执行一次
      hilog.info(0xFFFF, 'testTag', '%{public}s', 'afterAll: release global resources');
      sharedArray = [];
      expect(sharedArray.length).assertEqual(0);
    });

    it('testStringContain', 0, () => {
      hilog.info(0xFFFF, 'testTag', '%{public}s', 'it testStringContain begin');
      let a = 'abc';
      expect(a).assertContain('b');       // 包含子串
      expect(a).assertEqual('abc');       // 内容相等
    });

    it('testBooleanAssertions', 0, () => {
      hilog.info(0xFFFF, 'testTag', '%{public}s', 'it testBooleanAssertions begin');
      let flag = (2 + 2 === 4);
      expect(flag).assertTrue();          // 断言为 true
      expect(!flag).assertFalse();        // 断言为 false
    });

    it('testNumberArray', 0, () => {
      hilog.info(0x0000, 'testTag', '%{public}s', 'it testNumberArray begin');
      expect(sharedArray.length).assertEqual(3);
      expect(sharedArray[0]).assertEqual(1);
      expect(sharedArray).assertContain(2);
    });

    it('testObjectProperty', 0, () => {
      hilog.info(0xFFFF, 'testTag', '%{public}s', 'it testObjectProperty begin');
      // sharedObj 在 beforeEach 已经被赋值为 SharedObj
      if (sharedObj === null) {
        // 为了类型安全（理论上不会走到这里），仍然做一次保护
        throw new Error('sharedObj should not be null here');
      }
      expect(sharedObj.name).assertEqual('test');
      expect(sharedObj.value).assertLarger(10);   // 数字比较
      // 测试一个不存在的可选属性，类型已声明为 undefined | absent
      expect(sharedObj.extra).assertUndefined();
    });
  });
}
