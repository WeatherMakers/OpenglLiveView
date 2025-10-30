#include "FullSceneRenderer.h"
#include "AstcUtils.h"
#include "Common.h"
#include "ImageUtils.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "TimeUtils.h"
#include "SequenceFramePlayer.h"
#include "RainWithBackgroundSeqPlayer.h"
#include "ThickCloudSequencePlayer.h"
#include "SingleTexturePlayer.h"
#include "log.h"

using namespace hiveVG;

CFullSceneRenderer::CFullSceneRenderer()
{
}

CFullSceneRenderer::~CFullSceneRenderer()
{
    // 雨景播放器
    __deleteSafely(m_pRainSeqPlayer);
    __deleteSafely(m_pCloudPlayer);
    __deleteSafely(m_pThickCloudPlayer);
    
    // 雪景播放器
    __deleteSafely(m_pBackgroundPlayer);
    __deleteSafely(m_pSnowBackgroundPlayer);
    __deleteSafely(m_pSnowForegroundPlayer);
    
    __deleteSafely(m_pConfigReader);
}

bool CFullSceneRenderer::init()
{
    if (m_pConfigReader == nullptr)
        m_pConfigReader = new CJsonReader(m_ConfigFile);

    // 初始化雨景主序列播放器
    __initRainSeqPlayer();

    // 初始化雪景背景播放器
    __initBackgroundPlayer();

    m_pScreenQuad = &CScreenQuad::getInstance();
    if (!m_pScreenQuad->init())
    {
        LOGE(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Failed to initialize CScreenQuad");
        return false;
    }

    m_RainRenderChannel = ERenderChannel::R;
    m_SnowRenderChannel = ERenderChannel::R;
    m_RainLastFrameTime = CTimeUtils::getCurrentTime();
    m_SnowLastFrameTime = CTimeUtils::getCurrentTime();
    setCloudThickness(15/100.f);
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "FullSceneRenderer initialized successfully (lazy loading mode)");
    
    toggleCloud();
    toggleSnowBackground();
    toggleSnowForeground();
    
    return true;
}

void CFullSceneRenderer::setFPS(int fps)
{
    if (m_SnowActive)
    {
        switch (m_SnowRenderChannel)
        {
            case ERenderChannel::R: m_snowFpsLight = fps; break;
            case ERenderChannel::G: m_snowFpsModerate = fps; break;
            case ERenderChannel::B: m_snowFpsHeavy = fps; break;
            case ERenderChannel::A: m_snowFpsStorm = fps; break;
        }
    }
    if (m_RainActive)
    {
        switch (m_RainRenderChannel)
        {
            case ERenderChannel::R: m_rainFpsLight = fps; break;
            case ERenderChannel::G: m_rainFpsModerate = fps; break;
            case ERenderChannel::B: m_rainFpsHeavy = fps; break;
            case ERenderChannel::A: m_rainFpsStorm = fps; break;
        }
    }
}

int CFullSceneRenderer::getFPS()
{
    int fps = 0;
    if (m_SnowActive)
    {
        switch (m_SnowRenderChannel)
        {
            case ERenderChannel::R: fps = m_snowFpsLight; break;
            case ERenderChannel::G: fps = m_snowFpsModerate; break;
            case ERenderChannel::B: fps = m_snowFpsHeavy; break;
            case ERenderChannel::A: fps = m_snowFpsStorm; break;
        }
    }
    if (m_RainActive)
    {
        switch (m_RainRenderChannel)
        {
            case ERenderChannel::R: fps = m_rainFpsLight; break;
            case ERenderChannel::G: fps = m_rainFpsModerate; break;
            case ERenderChannel::B: fps = m_rainFpsHeavy; break;
            case ERenderChannel::A: fps = m_rainFpsStorm; break;
        }
    }
    return fps;
}

void CFullSceneRenderer::draw()
{
    m_RainCurrentTime = CTimeUtils::getCurrentTime();
    m_SnowCurrentTime = CTimeUtils::getCurrentTime();
    double RainDeltaTime = m_RainCurrentTime - m_RainLastFrameTime;
    double SnowDeltaTime = m_SnowCurrentTime - m_SnowLastFrameTime;
    m_RainLastFrameTime = m_RainCurrentTime;
    m_SnowLastFrameTime = m_SnowCurrentTime;

    glClearColor(m_BackgroundColor[0], m_BackgroundColor[1], m_BackgroundColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // === 雪景渲染 ===
    // 雪景背景播放器 - 始终渲染（仅在雪景激活时）
    if (m_SnowActive && m_pBackgroundPlayer)
    {
       // m_pBackgroundPlayer->updateFrame();
       // m_pScreenQuad->bindAndDraw();
    }

    // 根据雪景通道设置不同的fps
    int SnowCurrentFps = m_snowFpsLight; // 默认R通道
    switch (m_SnowRenderChannel)
    {
        case ERenderChannel::R: SnowCurrentFps = m_snowFpsLight; break;
        case ERenderChannel::G: SnowCurrentFps = m_snowFpsModerate; break;
        case ERenderChannel::B: SnowCurrentFps = m_snowFpsHeavy; break;
        case ERenderChannel::A: SnowCurrentFps = m_snowFpsStorm; break;
    }

    // 雪景背景 - 受可见性控制，按通道同步（仅在雪景激活时）
    if (m_SnowActive && m_SnowBackgroundInitialized && m_SnowBackgroundVisible && m_pSnowBackgroundPlayer)
    {
        m_pSnowBackgroundPlayer->setFrameRate(SnowCurrentFps);
        m_pSnowBackgroundPlayer->updateMultiChannelFrame(SnowDeltaTime, m_SnowRenderChannel);
        m_pSnowBackgroundPlayer->drawMultiChannelFrame(m_pScreenQuad);
    }

    // 雪景前景 - 受可见性控制，按通道同步（仅在雪景激活时）
    if (m_SnowActive && m_SnowForegroundInitialized && m_SnowForegroundVisible && m_pSnowForegroundPlayer)
    {
        m_pSnowForegroundPlayer->setFrameRate(SnowCurrentFps);
        m_pSnowForegroundPlayer->updateMultiChannelFrame(SnowDeltaTime, m_SnowRenderChannel);
        m_pSnowForegroundPlayer->drawMultiChannelFrame(m_pScreenQuad);
    }

    // === 雨景渲染 ===
    // 雨景主序列播放器 - 始终渲染（仅在雨景激活时）
    // 根据雪景通道设置不同的fps
    int RainCurrentFps = m_rainFpsLight; // 默认R通道
    switch (m_RainRenderChannel)
    {
        case ERenderChannel::R: SnowCurrentFps = m_rainFpsLight; break;
        case ERenderChannel::G: SnowCurrentFps = m_rainFpsModerate; break;
        case ERenderChannel::B: SnowCurrentFps = m_rainFpsHeavy; break;
        case ERenderChannel::A: SnowCurrentFps = m_rainFpsStorm; break;
    }
    if (m_RainActive && m_RainSeqInitialized && m_pRainSeqPlayer)
    {
        //m_pRainSeqPlayer->setFrameRate(RainCurrentFps);
        //m_pRainSeqPlayer->updateMultiChannelFrame(RainDeltaTime, m_RainRenderChannel);
        //m_pRainSeqPlayer->draw(m_pScreenQuad);
        m_pRainSeqPlayer->setFrameRate(RainCurrentFps);
        m_pRainSeqPlayer->updateMultiChannelFrame(RainDeltaTime, m_RainRenderChannel);
        m_pRainSeqPlayer->drawMultiChannelFrame(m_pScreenQuad);
    }
    

    // 云朵效果 - 在R和G通道显示（可见性开关，仅在雨景激活时）
    if (m_RainActive && m_CloudInitialized && m_CloudVisible && m_pCloudPlayer)
    {
        m_pCloudPlayer->updateCloudLerpMultiChannelFrame(RainDeltaTime);
        if (m_RainRenderChannel == ERenderChannel::R || m_RainRenderChannel == ERenderChannel::G)
        {
            m_pCloudPlayer->drawCloudLerpMultiChannelFrame(m_pScreenQuad);
        }
    }

    // 厚云朵效果 - 在B和A通道显示（可见性开关，仅在雨景激活时）
    if (m_RainActive && m_ThickCloudInitialized && m_ThickCloudVisible && m_pThickCloudPlayer)
    {
        m_pThickCloudPlayer->updateQuantizationFrame(RainDeltaTime);
        if (m_RainRenderChannel == ERenderChannel::B || m_RainRenderChannel == ERenderChannel::A)
        {
            m_pThickCloudPlayer->draw(m_pScreenQuad);
        }
    }
}

void CFullSceneRenderer::setRainChannel(ERenderChannel vChannel)
{
    m_RainRenderChannel = vChannel;
    m_RainActive = true;
    m_SnowActive = false;
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Setting rain render channel to %d, Rain active, Snow disabled", static_cast<int>(vChannel));
}

void CFullSceneRenderer::toggleCloud()
{
    if (!m_CloudInitialized)
    {
        LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Initializing Cloud player (toggle)...");
        __initCloudPlayer();
        __initThickCloudPlayer();
        m_CloudVisible = true;
        m_ThickCloudVisible = true;
    }
    else
    {
        m_CloudVisible = !m_CloudVisible;
        m_ThickCloudVisible = !m_ThickCloudVisible;
    }
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Cloud visibility toggled to %d. Thick cloud visibility toggled to %d", m_CloudVisible,m_ThickCloudVisible);
}

void CFullSceneRenderer::setSnowChannel(ERenderChannel vChannel)
{
    m_SnowRenderChannel = vChannel;
    m_SnowActive = true;
    m_RainActive = false;
    if (!m_BackgroundInitialized) { __initBackgroundPlayer(); }
    if (!m_SnowBackgroundInitialized) { __initSnowBackgroundPlayer(); }
    if (!m_SnowForegroundInitialized) { __initSnowForegroundPlayer(); }
    
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Setting snow render channel to %{public}d, Snow active, Rain disabled", static_cast<int>(vChannel));
}

void CFullSceneRenderer::toggleSnowBackground()
{
    if (!m_SnowBackgroundInitialized)
    {
        LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Initializing Snow Background player (toggle)...");
        __initSnowBackgroundPlayer();
        m_SnowBackgroundVisible = true;
    }
    else
    {
        m_SnowBackgroundVisible = !m_SnowBackgroundVisible;
    }
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Snow Background visibility toggled to %d", m_SnowBackgroundVisible);
}

void CFullSceneRenderer::toggleSnowForeground()
{
    if (!m_SnowForegroundInitialized)
    {
        LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Initializing Snow Foreground player (toggle)...");
        __initSnowForegroundPlayer();
        m_SnowForegroundVisible = true;
    }
    else
    {
        m_SnowForegroundVisible = !m_SnowForegroundVisible;
    }
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Snow Foreground visibility toggled to %d", m_SnowForegroundVisible);
}

void CFullSceneRenderer::setColor(float vValue)
{
    if(!m_pRainSeqPlayer) __initRainSeqPlayer();
    if(!m_SnowForegroundInitialized) 
    {
        __initSnowForegroundPlayer();
        m_SnowForegroundInitialized = true;
    }
    if(!m_SnowBackgroundInitialized) 
    {
        __initSnowBackgroundPlayer();
        m_SnowBackgroundInitialized = true;
    }
    
    m_pRainSeqPlayer->setColor(vValue);
    m_pSnowForegroundPlayer->setColor(vValue);
    m_pSnowBackgroundPlayer->setColor(vValue);
}



void CFullSceneRenderer::updateBackgroundLumin(){
    float BackgroundColorLumin = CImageUtils::CalculateRGBLumin(m_BackgroundColor[0],m_BackgroundColor[1],m_BackgroundColor[2]);
    m_BackgroundLumin = m_BackgroundImageLumin + BackgroundColorLumin * (1.0f - m_BackgroundImageOpaquePercentage);
}

static inline float Remap(float vValue, float vMin, float vMax,float vMinOut, float vMaxOut){
    return (vValue - vMin) / (vMax - vMin) * (vMaxOut - vMinOut) + vMinOut;
}



float CFullSceneRenderer::adjustRainColor(){
    float RainColorValue = Remap(m_BackgroundLumin, m_BackgroundImageLumin, m_BackgroundLuminMax, -1.0, 1.0);
    RainColorValue =  -RainColorValue;
    RainColorValue = 1.f / (1 + exp(-6*RainColorValue));
    if(RainColorValue < 0.35){
        RainColorValue = 0.35;
    }else if(RainColorValue > 0.95){
        RainColorValue = 0.95;
    }
    setColor(RainColorValue);
    return RainColorValue;
}

void CFullSceneRenderer::__initBackgroundImageProperties(const std::string& vImagePath){
    int Width, Height;
    std::vector<unsigned char> AstcData;
    // 假设类型为ASTC
    if(CAstcUtils::ReadASTC(vImagePath,AstcData,Width,Height)){
        int OpaquePixelCount;
        std::vector<unsigned char> RGBAData;
        CAstcUtils::DecodeToRGBA32(AstcData, Width, Height, RGBAData);
        m_BackgroundImageLumin = CImageUtils::CalculateImageAverageLumin(RGBAData,{0,0,0},OpaquePixelCount);
        m_BackgroundImageOpaquePercentage = static_cast<float>(OpaquePixelCount) / (Width * Height);
        m_BackgroundLuminMax = m_BackgroundImageLumin + 1.f - m_BackgroundImageOpaquePercentage;
    }
}

void CFullSceneRenderer::setCloudThickness(float vValue){
    if(!m_ThickCloudInitialized) __initThickCloudPlayer();
    if(!m_CloudInitialized) __initCloudPlayer();
    m_pThickCloudPlayer->setCloudThickness(vValue);
    m_pCloudPlayer->setCloudThickness(vValue);
}

void CFullSceneRenderer::__initRainSeqPlayer()
{
    if (m_pRainSeqPlayer) return;
    Json::Value RainConfig = m_pConfigReader->getObject("Rain");
    Json::Value BackGroundConfig = m_pConfigReader->getObject("RainBackground");
    std::string RainPath = RainConfig["frames_path"].asString();
    std::string RainFrameType = RainConfig["frames_type"].asString();
    int RainTextureCount = RainConfig["frames_count"].asInt();
    int RainOneTextureFrames = RainConfig["one_texture_frames"].asInt();
    float RainFramePerSecond = RainConfig["fps"].asFloat();
    std::string RainVertexShader = RainConfig["vertex_shader"].asString();
    std::string RainFragShader = RainConfig["fragment_shader"].asString();
    EPictureType::EPictureType RainPictureType = EPictureType::FromString(RainFrameType);
    std::string BackImgPath = BackGroundConfig["frames_path"].asString();
    std::string BackFrameType = BackGroundConfig["frames_type"].asString();
    EPictureType::EPictureType BackPicType = EPictureType::FromString(BackFrameType);
    
//    std::string DayBackImgPath = BackGroundConfig["day_frames_path"].asString();
//     __initBackgroundImageProperties(DayBackImgPath);
    
//    m_pRainSeqPlayer = new CRainWithBackgroundSeqPlayer(RainPath, RainTextureCount, RainOneTextureFrames, RainFramePerSecond, RainPictureType);
//    m_pRainSeqPlayer->initTextureAndShaderProgram(RainVertexShader, RainFragShader);
//    m_pRainSeqPlayer->initBackground(BackImgPath, BackPicType);
    
    m_pRainSeqPlayer = new CSequenceFramePlayer(RainPath, RainTextureCount, RainOneTextureFrames, RainFramePerSecond, RainPictureType);
    m_pRainSeqPlayer->initTextureAndShaderProgram(RainVertexShader, RainFragShader);
    m_pRainSeqPlayer->setWindowSize(m_WindowSize);
    
    m_RainSeqInitialized = true;
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Rain Player initialized.");
}

void CFullSceneRenderer::__initCloudPlayer()
{
    if (m_pCloudPlayer) return;
    Json::Value CloudConfig = m_pConfigReader->getObject("Cloud");
    std::string CloudPath = CloudConfig["frames_path"].asString();
    std::string CloudType = CloudConfig["frames_type"].asString();
    int CloudFrameCount = CloudConfig["frames_count"].asInt();
    int CloudOneTextureFrames = CloudConfig["one_texture_frames"].asInt();
    float CloudPlayFPS = CloudConfig["fps"].asFloat();
    std::string CloudVertexShader = CloudConfig["vertex_shader"].asString();
    std::string CloudFragShader = CloudConfig["fragment_shader"].asString();
    EPictureType::EPictureType CloudPicType = EPictureType::FromString(CloudType);
    m_pCloudPlayer = new CSequenceFramePlayer(CloudPath, CloudFrameCount, CloudOneTextureFrames, CloudPlayFPS, CloudPicType);
    m_pCloudPlayer->initTextureAndShaderProgram(CloudVertexShader, CloudFragShader);
    m_pCloudPlayer->setWindowSize(m_WindowSize);
    m_pCloudPlayer->setRatioUniform();
    m_CloudInitialized = true;
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Cloud Player initialized.");
}

void CFullSceneRenderer::__initThickCloudPlayer()
{
    if (m_pThickCloudPlayer) return;
    if (m_pConfigReader == nullptr) m_pConfigReader = new CJsonReader(m_ConfigFile);
    Json::Value ThickCloudConfig = m_pConfigReader->getObject("ThickCloud");
    std::string ThickCloudFramePath = ThickCloudConfig["frames_path"].asString();
    std::string ThickCloudFrameType = ThickCloudConfig["frames_type"].asString();
    int ThickCloudFrameCount = ThickCloudConfig["frames_count"].asInt();
    int ThickCloudOneTextureFrames = ThickCloudConfig["one_texture_frames"].asInt();
    float ThickCloudPlayFPS = ThickCloudConfig["fps"].asFloat();
    std::string ThickCloudVertexShader = ThickCloudConfig["vertex_shader"].asString();
    std::string ThickCloudFragShader = ThickCloudConfig["fragment_shader"].asString();
    EPictureType::EPictureType ThickCloudPicType = EPictureType::FromString(ThickCloudFrameType);
    m_pThickCloudPlayer = new CThickCloudSequencePlayer(ThickCloudFramePath, ThickCloudFrameCount, ThickCloudOneTextureFrames, ThickCloudPlayFPS, ThickCloudPicType);
    
    Json::Value LightningConfig = m_pConfigReader->getObject("LightningWithMask");
    std::string LightningFramePath = LightningConfig["frames_path"].asString();
    std::string LightningFrameType = LightningConfig["frames_type"].asString();
    EPictureType::EPictureType LightningPicType = EPictureType::FromString(LightningFrameType);
    int LightningFrameCount = LightningConfig["frames_count"].asInt();
    int LightningOneTextureFrames = LightningConfig["one_texture_frames"].asInt();
    float LightningPlayFPS = LightningConfig["fps"].asFloat();
    m_pThickCloudPlayer->setLightningAnimationParams(LightningFramePath,LightningFrameCount,LightningOneTextureFrames,LightningPlayFPS,LightningPicType);
    
    if (m_pThickCloudPlayer->initTextureAndShaderProgram(ThickCloudVertexShader, ThickCloudFragShader))
    {
        m_pThickCloudPlayer->setWindowSize(m_WindowSize);
        m_ThickCloudInitialized = true;
    }
    else
    {
        LOGE(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "ThickCloudPlayer initialization failed.");
    }
}

void CFullSceneRenderer::__initBackgroundPlayer()
{
    if (m_pBackgroundPlayer) return;
    std::string TexturePath    = "textures/background.astc";
    EPictureType::EPictureType TextureType    = EPictureType::ASTC;
    m_pBackgroundPlayer = new CSingleTexturePlayer(TexturePath, TextureType);
    if (!m_pBackgroundPlayer->initTextureAndShaderProgram())
    {
        LOGE(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Failed to init Background player");
    }
    m_BackgroundInitialized = true;
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Background Player initialized.");
}

void CFullSceneRenderer::__initSnowBackgroundPlayer()
{
    if (m_pSnowBackgroundPlayer) return;
    Json::Value Config = m_pConfigReader->getObject("SnowBackground");
    std::string FramesPath = Config["frames_path"].asString();
    std::string FramesType = Config["frames_type"].asString();
    int FramesCount = Config["frames_count"].asInt();
    int OneTextureFrames = Config["one_texture_frames"].asInt();
    float Fps = Config["fps"].asFloat();
    std::string VertexShader = Config["vertex_shader"].asString();
    std::string FragShader = Config["fragment_shader"].asString();
    EPictureType::EPictureType PicType = EPictureType::FromString(FramesType);
    m_pSnowBackgroundPlayer = new CSequenceFramePlayer(FramesPath, FramesCount, OneTextureFrames, Fps, PicType);
    m_pSnowBackgroundPlayer->initTextureAndShaderProgram(VertexShader, FragShader);
    m_pSnowBackgroundPlayer->setWindowSize(m_WindowSize);
    m_pSnowBackgroundPlayer->setRatioUniform();
    m_SnowBackgroundInitialized = true;
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Snow Background Player initialized.");
}

void CFullSceneRenderer::__initSnowForegroundPlayer()
{
    if (m_pSnowForegroundPlayer) return;
    Json::Value Config = m_pConfigReader->getObject("SnowForeground");
    std::string FramesPath = Config["frames_path"].asString();
    std::string FramesType = Config["frames_type"].asString();
    int FramesCount = Config["frames_count"].asInt();
    int OneTextureFrames = Config["one_texture_frames"].asInt();
    float Fps = Config["fps"].asFloat();
    std::string VertexShader = Config["vertex_shader"].asString();
    std::string FragShader = Config["fragment_shader"].asString();
    EPictureType::EPictureType PicType = EPictureType::FromString(FramesType);
    m_pSnowForegroundPlayer = new CSequenceFramePlayer(FramesPath, FramesCount, OneTextureFrames, Fps, PicType);
    m_pSnowForegroundPlayer->initTextureAndShaderProgram(VertexShader, FragShader);
    m_pSnowForegroundPlayer->setWindowSize(m_WindowSize);
    m_pSnowForegroundPlayer->setRatioUniform();
    m_SnowForegroundInitialized = true;
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Snow Foreground Player initialized.");
}
