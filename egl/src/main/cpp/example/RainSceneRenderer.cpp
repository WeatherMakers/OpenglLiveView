#include "RainSceneRenderer.h"
#include "SequenceFramePlayer.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "TimeUtils.h"
#include "log.h"

using namespace hiveVG;

CRainSceneRenderer::CRainSceneRenderer() {}

CRainSceneRenderer::~CRainSceneRenderer()
{
    if (m_pTexturePlayer != nullptr)
    {
        delete m_pTexturePlayer;
        m_pTexturePlayer = nullptr;
    }
}

bool CRainSceneRenderer::init()
{
    m_pBackGroundPlayer = new CSingleTexturePlayer(m_BackgroundTexPath,m_PictureType);
    if(!m_pBackGroundPlayer->initTextureAndShaderProgram())
    {
        LOGE(TAG_KEYWORD::RAIN_SCENE_RENDERER_TAG, "Failed to initialize single texture and shader program");
        return false;
    }
    
    m_pTexturePlayer = new CSequenceFramePlayer(m_TextureRootPath, m_TextureCount, m_OneTextureFrames, m_FrameSeconds, m_PictureType);
    if (!m_pTexturePlayer->initTextureAndShaderProgram(SeqTexPlayVertASTC, SeqTexPlayFragRainCloudASTC))
    {
        LOGE(TAG_KEYWORD::RAIN_SCENE_RENDERER_TAG, "Failed to initialize sequence texture and shader program");
        return false;
    }

    m_pScreenQuad = &CScreenQuad::getInstance();
    if (!m_pScreenQuad->init())
    {
        LOGE(TAG_KEYWORD::RAIN_SCENE_RENDERER_TAG, "Failed to initialize CScreenQuad");
        return false;
    }
    LOGI(TAG_KEYWORD::RAIN_SCENE_RENDERER_TAG, "RainSceneRenderer initialized successfully");
    return true;
}

void CRainSceneRenderer::draw()
{
    glClearColor(0.345f, 0.345f, 0.345f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    
    m_CurrentTime = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime = m_CurrentTime;
    m_pTexturePlayer->updateMultiChannelFrame(DeltaTime, ERenderChannel::R);
    m_pTexturePlayer->drawMultiChannelFrame(m_pScreenQuad);
    m_pBackGroundPlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();
}
