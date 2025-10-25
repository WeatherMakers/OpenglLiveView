
export class Params {
  static readonly SINGLE_RENDER_TYPE: number = 1
  static readonly FULL_RAIN_SCENE_RENDER_TYPE: number = Params.SINGLE_RENDER_TYPE + 1
  static readonly FULL_SNOW_SCENE_RENDER_TYPE: number = Params.FULL_RAIN_SCENE_RENDER_TYPE + 1
  static readonly FULL_ALL_SCENE_RENDER_TYPE: number = Params.FULL_SNOW_SCENE_RENDER_TYPE + 1
}
