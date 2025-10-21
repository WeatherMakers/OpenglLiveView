export const setResourceManager: (ctx: any) => void;
export const setRenderType: (params: number) => void;
export const triggerCloud: () => void;
export const triggerLightRain: () => void;
export const triggerModerateRain: () => void;
export const triggerHeavyRain: () => void;
export const triggerStormRain: () => void;
export const triggerSnowBackground: () => void;
export const triggerSnowForeground: () => void;
export const triggerLightSnow: () => void;
export const triggerModerateSnow: () => void;
export const triggerHeavySnow: () => void;
export const triggerStormSnow: () => void;

// 全场景相关函数
export const triggerFullSceneRainLight: () => void;
export const triggerFullSceneRainModerate: () => void;
export const triggerFullSceneRainHeavy: () => void;
export const triggerFullSceneRainStorm: () => void;
export const triggerFullSceneRainCloud: () => void;
export const triggerFullSceneSnowLight: () => void;
export const triggerFullSceneSnowModerate: () => void;
export const triggerFullSceneSnowHeavy: () => void;
export const triggerFullSceneSnowStorm: () => void;
export const triggerFullSceneSnowBackground: () => void;
export const triggerFullSceneSnowForeground: () => void;
export const changeFPS: (params: number) => void;
export const getFPS: () => number;