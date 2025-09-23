#pragma once

#include <queue>
#include <string>
#include <thread>
#include <vector>
#include <glm/glm.hpp>
#include "Common.h"

namespace hiveVG
{
    // pending task 的容器（线程间传递）
    struct PendingAstc {
        int index;
        std::string path;
        std::vector<unsigned char> data; // full astc file bytes
        int width = 0;
        int height = 0;
    };
    class CTexture2D;
    class CShaderProgram;
    class CScreenQuad;
    struct SSequenceState;

    class CAsycSequenceFramePlayer
    {
    public:
        CAsycSequenceFramePlayer(const std::string &vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType::EPictureType vPictureType = EPictureType::PNG);
        CAsycSequenceFramePlayer(const std::string &vTextureRootPath, int vTextureCount, int vOneTextureFrames, float vFrameSeconds, EPictureType::EPictureType vPictureType);
        //[[nodiscard]] CAsycSequenceFramePlayer *clone() const { return new CAsycSequenceFramePlayer(*this); }
        ~CAsycSequenceFramePlayer();

        void setLoopPlayback(bool vLoopTag) { m_IsLoop = vLoopTag; }
        void setScreenUVScale(float vScreenScale) { m_ScreenUVScale = glm::vec2(vScreenScale, vScreenScale); }
        void setScreenUVScale(const glm::vec2 &vScreenScale) { m_ScreenUVScale = vScreenScale; }
        void setScreenUVOffset(const glm::vec2 &vScreenOffset) { m_ScreenUVOffset = vScreenOffset; }
        void setFrameRate(int vFrameRate) { m_FramePerSecond = static_cast<float>(vFrameRate); }
        void setFrameRate(float vFrameRate) { m_FramePerSecond = vFrameRate; }
        void setValidFrames(int vValidFrames) { m_ValidFrames = vValidFrames; }
        void setRotationAngle(float vAngle) { m_RotationAngle = vAngle; }
        void setIsMoving(bool vIsMoving) { m_IsMoving = vIsMoving; }
        void setLifeCycle(bool vUseLifeCycle)
        {
            m_UseLifeCycle = vUseLifeCycle;
            __initSequenceParams();
        }
        void setScreenUVMovingSpeed(const glm::vec2 &vMovingSpeed) { m_MovingSpeed = vMovingSpeed; }
        void setWindowSize(glm::vec2 vWindowSize) { m_WindowSize = vWindowSize; }

        [[nodiscard]] int getSingleTextureWidth() const { return m_SeqSingleTexWidth; }
        [[nodiscard]] int getSingleTextureHeight() const { return m_SeqSingleTexHeight; }
        [[nodiscard]] bool getLoopState() const { return m_IsLoop; }
        [[nodiscard]] bool getFinishState() const { return m_IsFinished; }
        [[nodiscard]] EPictureType::EPictureType getPicType() const { return m_TextureType; }
        [[nodiscard]] std::vector<CTexture2D *> getTextures() const { return m_SeqTextures; }
        
        bool initTextureAndShaderProgram();

        bool initTextureAndShaderProgram(const std::string &vVertexShaderPath, const std::string &vFragShaderShaderPath);

        void updateFrameAndUV(double vDeltaTime);
        void updateSeqKTXFrame(double vDeltaTime);

        void updateQuantizationFrame(double vDeltaTime);
        void updateMultiChannelFrame(double vDeltaTime, ERenderChannel vRenderChannel);
        void updateInterpolationFrame(double vDeltaTime);
        void updateLerpQuantFrame(double vDeltaTime);

        void draw(CScreenQuad *vQuad);
        void drawQuantization(CScreenQuad *vQuad);
        void drawMultiChannelKTX(CScreenQuad *vQuad);
        void drawSeqKTX(CScreenQuad *vQuad);
        void drawInterpolation(CScreenQuad *vQuad);
        void drawInterpolationWithDisplacement(CScreenQuad *vQuad);
        void drawInterpolationWithFiltering(CScreenQuad *vQuad);
        void setRatioUniform();


        void loaderThreadFunc();
        void updateAstcTOGPU();
        void shutdownLoader();
    protected:
        void __initSequenceParams();

        int    m_SequenceRows       = 1;
        int    m_SequenceCols       = 1;
        int    m_OneTextureFrames   = 1;
        int    m_CurrentChannel     = 0;
        int    m_SequenceWidth      = 0;
        int    m_SequenceHeight     = 0;
        int    m_SeqSingleTexWidth  = 0;
        int    m_SeqSingleTexHeight = 0;
        int    m_ValidFrames;
        float  m_FramePerSecond   = 24.0f;
        bool   m_IsLoop           = true;
        bool   m_EnableChannel    = false;
        bool   m_IsFinished       = false;
        float  m_RotationAngle    = 0.0f;
        bool   m_IsMoving         = false;
        bool   m_UseLifeCycle     = false;
        bool   m_UseCompressedPNG = false;
        int    m_CurrentFrame     = 0;
        int    m_NextFrame        = 0;
        float  m_InterpolationFactor = 0.0f;
        double m_AccumFrameTime      = 0.0f;
        std::string m_TextureRootPath;
        int         m_CurrentTexture  = 0;
        int         m_NextTexture     = 0;
        int         m_TextureCount;
        SSequenceState m_SequenceState;
        glm::vec2 m_ScreenUVScale  = glm::vec2(0.6f, 0.8f);
        glm::vec2 m_ScreenUVOffset = glm::vec2(0.0f, 0.0f);
        glm::vec2 m_WindowSize     = glm::vec2(0.0f, 0.0f);
        glm::vec2 m_MovingSpeed    = glm::vec2(0.1f, 0.0f);
        EPictureType::EPictureType m_TextureType = EPictureType::PNG;
        std::vector<CTexture2D *>  m_SeqTextures;
        CShaderProgram*            m_pSequenceShaderProgram = nullptr;

        std::thread m_LoadThread;
        std::mutex m_PendingMutex;
        std::condition_variable m_PendingCV;
        std::queue<PendingAstc> m_PendingQueue; // 已读完，需要在主线程上传的数据
        std::atomic<bool> m_StopLoader{false};

        // 用于管理任务启动时的路径与数量（无需 future 了）
        std::vector<std::string> m_QueuedPaths; // 由 init 填充，loader 线程逐条读取
        std::atomic<int> m_NextLoadIndex{0};
    };
}
