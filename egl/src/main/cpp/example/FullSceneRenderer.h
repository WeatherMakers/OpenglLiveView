#pragma once

#include "BaseRenderer.h"
#include "Common.h"
#include <glm/glm.hpp>
#include "JsonReader.h"

namespace hiveVG
{
    class CScreenQuad;
    class CSequenceFramePlayer;
    class CThickCloudSequencePlayer;
    class CRainWithBackgroundSeqPlayer;
    class CSingleTexturePlayer;

    class CFullSceneRenderer : public CBaseRenderer
    {
    public:
        CFullSceneRenderer();
        ~CFullSceneRenderer() override;
        bool init() override;
        void draw() override;

        void setWindowSize(glm::vec2 vWindowSize) { m_WindowSize = vWindowSize; }
        
        // 雨景相关函数
        void setRainChannel(ERenderChannel vChannel);
        void toggleCloud();
        
        // 雪景相关函数
        void setSnowChannel(ERenderChannel vChannel);
        void toggleSnowBackground();
        void toggleSnowForeground();
    
        // 背景自适应相关函数，在其中设置哪些需要变换颜色，其余的直接使用默认值1.0
        void setColor(float vValue);
        void setBackgroundColor(float vR, float vG, float vB) { m_BackgroundColor[0] = vR; m_BackgroundColor[1] = vG; m_BackgroundColor[2] = vB; }
        void getBackgroundColor(float& oR, float& oG, float& oB) { oR = m_BackgroundColor[0]; oG = m_BackgroundColor[1]; oB = m_BackgroundColor[2]; }
        void updateBackgroundLumin();
        float adjustRainColor();
        float getBackgroundLumin() { return m_BackgroundLumin; }
        // 设置云的厚度
        void setCloudThickness(float vValue);
    
        void setFPS(int fps);
        int getFPS();

    private:
        template<typename T>
        void __deleteSafely(T*& vPointer);

        // 雨景初始化函数
        void __initRainSeqPlayer();
        void __initCloudPlayer();
        void __initThickCloudPlayer();
        void __initBackgroundImageProperties(const std::string& vImagePath);
        
        // 雪景初始化函数
        void __initBackgroundPlayer();
        void __initSnowBackgroundPlayer();
        void __initSnowForegroundPlayer();

        std::string                m_ConfigFile    = "configs/FullSceneConfig.json";
        CJsonReader*               m_pConfigReader = nullptr;
    
        // 背景颜色
        float                      m_BackgroundColor[3] = {0.345f, 0.345f, 0.345f};
        float                      m_BackgroundImageLumin; // 背景图片亮度(透明部分按0计算)
        float                      m_BackgroundLuminMax;
        float                      m_BackgroundImageOpaquePercentage; // 完全不透明像素占总像素数的百分比
        float                      m_BackgroundLumin; // 背景亮度(透明部分按背景色计算)
    
        // 雨景相关
        ERenderChannel             m_RainRenderChannel = ERenderChannel::R;
        double                     m_RainLastFrameTime = 0.0f;
        double                     m_RainCurrentTime = 0.0f;
        
        // 雪景相关
        ERenderChannel             m_SnowRenderChannel = ERenderChannel::R;
        double                     m_SnowLastFrameTime = 0.0f;
        double                     m_SnowCurrentTime = 0.0f;
        
        CScreenQuad*               m_pScreenQuad = nullptr;
        glm::vec2                  m_WindowSize = glm::vec2(0,0);

        // 雨景播放器
        CRainWithBackgroundSeqPlayer* m_pRainSeqPlayer = nullptr;
        CSequenceFramePlayer*      m_pCloudPlayer = nullptr;
        CThickCloudSequencePlayer*  m_pThickCloudPlayer = nullptr;

        // 雪景播放器
        CSingleTexturePlayer*      m_pBackgroundPlayer = nullptr;
        CSequenceFramePlayer*      m_pSnowBackgroundPlayer = nullptr;
        CSequenceFramePlayer*      m_pSnowForegroundPlayer = nullptr;

        // 雨景初始化状态
        bool                       m_RainSeqInitialized = false;
        bool                       m_RainBackgroundInitialized = false;
        bool                       m_CloudInitialized = false;
        bool                       m_ThickCloudInitialized = false;

        // 雪景初始化状态
        bool                       m_BackgroundInitialized = false;
        bool                       m_SnowBackgroundInitialized = false;
        bool                       m_SnowForegroundInitialized = false;

        // 雨景可见性状态
        bool                       m_CloudVisible = false;
        bool                       m_ThickCloudVisible = false;

        // 雪景可见性状态
        bool                       m_SnowBackgroundVisible = false;
        bool                       m_SnowForegroundVisible = false;
        
        // 互斥渲染状态
        bool                       m_RainActive = true;   // 雨景是否激活
        bool                       m_SnowActive = false;   // 雪景是否激活
    
        // 雪景 FPS
        int                        m_snowFpsLight = 13;
        int                        m_snowFpsModerate = 18;
        int                        m_snowFpsHeavy = 23;
        int                        m_snowFpsStorm = 28;
    
        // 雨景 FPS
        int                        m_rainFPS = 80;
    };

    template<typename T>
    void CFullSceneRenderer::__deleteSafely(T*& vPointer)
    {
        if (vPointer != nullptr)
        {
            delete vPointer;
            vPointer = nullptr;
        }
    }
}
