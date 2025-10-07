#pragma once

#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include "EnumReflect.h"

DECLARE_ENUM(EPictureType, PNG = 0, JPG, ASTC)

namespace hiveVG::TAG_KEYWORD
{
    const char *const MAIN_TAG                  = "Main";
    const char *const TEXTURE2D_TAG             = "CTexture2D";
    const char *const SHADER_PROGRAM_TAG        = "CShaderProgram";
    const char *const SEQFRAME_PALYER_TAG       = "CSequenceFramePlayer";
    const char *const SYSTEMTEST_TAG            = "SystemTest";
    const char *const APP_CONTEXT_TAG           = "AppContext";
    
    // Renderer class tags
    const char *const SINGLE_PLAYER_RENDERER_TAG = "CSinglePlayerRenderer";
    const char *const RAIN_SCENE_RENDERER_TAG    = "CRainSceneRenderer";
    const char *const SNOW_SCENE_RENDERER_TAG    = "CSnowSceneRenderer";
    const char *const CLOUD_SCENE_RENDERER_TAG   = "CCloudSceneRenderer";
    const char *const FULL_SCENE_RENDERER_TAG    = "CFullSceneRenderer";
    const char *const NATIVE_RENDERER_TAG        = "CNativeRenderer";
    const char *const FILE_UTILS_TAG             = "FileUtils";
    const char *const SCREEN_QUAD_TAG            = "CScreenQuad";
    const char *const SINGLE_TEXTURE_PLAYER_TAG       = "CSingleTexturePlayer";
    const char *const LIGHTNING_SEQUENCE_PLAYER_TAG   = "CLightningSequencePlayer";
    const char *const NIGHT_SCENE_SEQUENCE_PLAYER_TAG = "CNightSceneSequencePlayer";
}

namespace hiveVG
{
    enum class ERenderChannel : std::uint8_t
    {
        R = 0,
        G,
        B,
        A
    };

    const static std::string SeqTexPlayVertPNG     = "shaders/sequenceTexturePlayer.vert";
    const static std::string SeqTexPlayFragPNG     = "shaders/sequenceTexturePlayerPNG.frag";
    const static std::string SeqTexPlayVertASTC    = "shaders/sequenceTexturePlayerASTC.vert";
    const static std::string SeqTexPlayFragASTC    = "shaders/sequenceTexturePlayerASTC.frag";
    const static std::string SeqTexPlayFragRainCloudASTC = "shaders/sequenceTexturePlayerRainCloudASTC.frag";
    const static std::string SingleTexPlayVert     = "shaders/singleTexturePlayer.vert";
    const static std::string SingleTexPlayFragPNG  = "shaders/singleTexturePlayerPNG.frag";
    const static std::string SingleTexPlayFragJPG  = "shaders/singleTexturePlayerJPG.frag";
    const static std::string SingleTexPlayFragASTC = "shaders/singleTexturePlayerASTC.frag";
    const static std::string SeqTexPlayFragJPG     = "shaders/sequenceTexturePlayerJPG.frag";
}