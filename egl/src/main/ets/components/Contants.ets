
export class Params {
  static readonly TYPE_TRIANGLE: number = 1
  static readonly TYPE_VBO_TRIANGLE: number = Params.TYPE_TRIANGLE + 1
  static readonly TYPE_EBO_TRIANGLE: number = Params.TYPE_VBO_TRIANGLE + 1
  static readonly TYPE_VAO_TRIANGLE: number = Params.TYPE_EBO_TRIANGLE + 1
  static readonly TYPE_IMAGE: number = Params.TYPE_VAO_TRIANGLE + 1
  static readonly TYPE_IMAGE_FROM_NATIVE: number = Params.TYPE_IMAGE + 1
  static readonly TYPE_SINGLE_PLAYER: number = Params.TYPE_IMAGE_FROM_NATIVE + 1
  static readonly TYPE_SEQ_PLAYER: number = Params.TYPE_SINGLE_PLAYER + 1
}
