
export class Params {
  static readonly SINGLE_RENDER_TYPE: number = 1
  static readonly SEQ_RENDER_TYPE: number = Params.SINGLE_RENDER_TYPE + 1
  static readonly RAIN_RENDER_TYPE: number = Params.SEQ_RENDER_TYPE + 1
  static readonly SNOW_RENDER_TYPE: number = Params.RAIN_RENDER_TYPE + 1
  static readonly CLOUD_RENDER_TYPE: number = Params.SNOW_RENDER_TYPE + 1
  static readonly FULL_SCENE_RENDER_TYPE: number = Params.CLOUD_RENDER_TYPE + 1
}
