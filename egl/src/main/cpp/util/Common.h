#pragma once

#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include "EnumReflect.h"

DECLARE_ENUM(EPlayType, FULLSCREEN = 0, PARTIAL)
DECLARE_ENUM(EPictureType, PNG = 0, JPG, WEBP, PNG8, PKM, KTX2)
DECLARE_ENUM(EPlayMode, DEPTH = 0, CHANNEL)
DECLARE_ENUM(EBlendingMode, NORMAL = 0, MULTIPLY, LIGHTEN, LINEAR_DODGE, LIGHTER_COLOR, NONE, COUNT)
DECLARE_ENUM(ELayerType, SEQUENCE_FRAME, SINGLE_PICTURE, BILLBOARD, SLIDE_WINDOW)

namespace hiveVG::TAG_KEYWORD
{
    const char *const MAIN_TAG = "Main";
    const char *const RENDERER_TAG = "CRenderer";
    const char *const SEQFRAME_RENDERER_TAG = "CSequenceFrameRenderer";
    const char *const TEXTURE2D_TAG = "CTexture2D";
    const char *const SHADER_PROGRAM_TAG = "CShaderProgram";
    const char *const SINGLE_PALYER_TAG = "CSequenceFramePlayer";
    const char *const SEQFRAME_PALYER_TAG = "CSequenceFramePlayer";
    const char *const ASYNC_SEQFRAME_PALYER_TAG = "CAsyncSequenceFramePlayer";
    const char *const TEXTURE_BLENDER_TAG       = "CTextureBlender";
    const char *const BLENDER_MANAGER_TAG       = "CBlendManager";
    const char *const SYSTEMTEST_TAG            = "SystemTest";
    const char *const SNOW_STYLIZE_TAG          = "SnowStylize";
    const char *const FILE_UTILS_TAG            = "FileUtils";
    const char *const APP_CONTEXT_TAG           = "AppContext";
    const char *const UNIT_TEST_TAG             = "UnitTest";
    const char *const WEATHER_TAG               = "Weather";
    const char *const EFFECT_TEST_TAG           = "EffectTest";
    const char *const SPLASH_MANAGER_TAG        = "CSplashManager";
}

namespace hiveVG::CONFIG_KEYWORD
{
    const char *const TexturePath = "texture_path";
}

namespace hiveVG
{
    enum class ERenderType : std::uint8_t
    {
        NONE = 0,
        SNOW,
        RAIN,
        CLOUD,
        SCROLL_RAIN,
        SNOW_SCENE,
        SMALL_SNOW_FORE,
        SMALL_SNOW_BACK,
        BIG_SNOW_FORE,
        BIG_SNOW_BACK,
        SMALL_RAIN_FORE,
        SMALL_RAIN_BACK,
        BIG_RAIN_FORE,
        BIG_RAIN_BACK,
        RAIN_QBC,
        RAIN_QBD,
        RAIN_Q32,
        SNOW_QBC,
        SNOW_QBD,
        SNOW_Q32
    };

    enum class ERenderChannel : std::uint8_t
    {
        R = 0,
        G,
        B,
        A
    };

    const static std::string P60SaveToPhotoPath = "/storage/emulated/0/Pictures/Screenshots/";

    // Shaders config path
    const static std::string SeqTexPlayVertPNG     = "shaders/sequenceTexturePlayer.vert";
    const static std::string SeqTexPlayFragPNG     = "shaders/sequenceTexturePlayerPNG.frag";
    const static std::string SeqTexPlayVertKTX     = "shaders/sequenceTexturePlayerKTX.vert";
    const static std::string SeqTexPlayFragKTX     = "shaders/sequenceTexturePlayerKTX.frag";
    const static std::string SingleTexPlayVert     = "shaders/singleTexturePlayer.vert";
    const static std::string SingleTexPlayFragPNG  = "shaders/singleTexturePlayerPNG.frag";
    const static std::string SingleTexPlayFragJPG  = "shaders/singleTexturePlayerJPG.frag";
    const static std::string SeqTexPlayFragJPG     = "shaders/sequenceTexturePlayerJPG.frag";
    const static std::string SeqTexPlayLerp = "shaders/sequenceTextureIerpPlayer.frag";
    const static std::string SeqTexPlayLerpQuan = "shaders/sequenceTextureIerpQuanPlayer.frag";
    const static std::string SlideWindowVert       = "shaders/slideWindow.vert";
    const static std::string SlideWindowHFrag      = "shaders/slideWindowH.frag";
    const static std::string SlideWindowHCFrag     = "shaders/slideWindowHCompressed.frag";
    const static std::string SlideWindowVFrag      = "shaders/slideWindowV.frag";
    const static std::string SlideWindowVCFrag     = "shaders/slideWindowVCompressed.frag";

    const static std::string BlitTex2ScreenVert = "shaders/blitTex2Screen.vert";
    const static std::string BlitTex2ScreenFrag = "shaders/blitTex2Screen.frag";
    const static std::string BlendAlphaFrag     = "shaders/blendAlpha.frag";
    const static std::string BlendMultiplyFrag  = "shaders/blendMultiply.frag";
    const static std::string BlendLightenFrag   = "shaders/blendLighten.frag";
    const static std::string BlendLinearDodgeFrag  = "shaders/blendLinearDodge.frag";
    const static std::string BlendLighterColorFrag = "shaders/blendLighterColor.frag";
    const static std::string BlendOverlayFrag   = "shaders/blendOverlay.frag";

    struct SSequenceState
    {
        bool _IsAlive;
        float _PlannedLivingTime;
        float _PlannedDeadTime;
        float _AlreadyLivingTime;
        float _AlreadyDeadTime;
        float _MovingDirection;
        float _MovingSpeed;
        glm::vec2 _UVOffset;
        float _UVScale;
    };
}