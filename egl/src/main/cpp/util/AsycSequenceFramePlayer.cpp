#include "AsycSequenceFramePlayer.h"
#include <random>
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "log.h"
#include "TimeUtils.h"
#include <chrono>
#define M_PI 3.14159265358979323846

using namespace hiveVG;

CAsycSequenceFramePlayer::CAsycSequenceFramePlayer(const std::string& vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType::EPictureType vPictureType)
        : m_SequenceRows(vSequenceRows), m_SequenceCols(vSequenceCols), m_TextureRootPath(vTextureRootPath), m_TextureCount(vTextureCount), m_TextureType(vPictureType)
{
    m_ValidFrames = m_SequenceRows * m_SequenceCols;
}

CAsycSequenceFramePlayer::CAsycSequenceFramePlayer(const std::string& vTextureRootPath, int vTextureCount, int vOneTextureFrames, float vFrameSeconds, EPictureType::EPictureType vPictureType)
        : m_TextureRootPath(vTextureRootPath), m_TextureCount(vTextureCount), m_OneTextureFrames(vOneTextureFrames), m_FramePerSecond(vFrameSeconds), m_TextureType(vPictureType)
{
}

CAsycSequenceFramePlayer::~CAsycSequenceFramePlayer()
{
    shutdownLoader();

    // 释放 GPU 纹理
    for (auto p : m_SeqTextures) {
        if (p) {
            delete p; // 确保 CTexture2D 的析构会释放 GL 资源（或提供专门的 release）
        }
    }
    if (m_pSequenceShaderProgram)
    {
        delete m_pSequenceShaderProgram;
        m_pSequenceShaderProgram = nullptr;
    }
   
}

bool CAsycSequenceFramePlayer::initTextureAndShaderProgram()
{
    if (m_TextureType == EPictureType::PNG)
        m_pSequenceShaderProgram = CShaderProgram::createProgram(SeqTexPlayVertPNG, SeqTexPlayFragPNG);
    else if (m_TextureType == EPictureType::JPG)
        m_pSequenceShaderProgram = CShaderProgram::createProgram(SeqTexPlayVertPNG, SeqTexPlayFragJPG);
    else if (m_TextureType == EPictureType::ASTC)
        m_pSequenceShaderProgram = CShaderProgram::createProgram(SeqTexPlayVertKTX, SeqTexPlayFragKTX);

    if (!m_pSequenceShaderProgram)
    {
        LOGE( "[%{public}s] ShaderProgram init Failed.", m_TextureRootPath.c_str());
        return false;
    }
    assert(m_pSequenceShaderProgram != nullptr);
    
    if (!m_TextureRootPath.empty() && m_TextureRootPath.back() != '/')
        m_TextureRootPath += '/';
    std::string PictureSuffix;
    if (m_TextureType == EPictureType::PNG)        PictureSuffix = ".png";
    else if (m_TextureType == EPictureType::JPG)   PictureSuffix = ".jpg";
    else if (m_TextureType == EPictureType::ASTC)  PictureSuffix = ".astc";

      // 预分配输出纹理数组（nullptr 表示尚未上传到 GPU）
    m_SeqTextures.assign(m_TextureCount, nullptr);

    // 填充路径列表
    m_QueuedPaths.clear();
    m_QueuedPaths.reserve(m_TextureCount);
    for (int i = 0; i < m_TextureCount; ++i)
    {
        std::string TexturePath = m_TextureRootPath +
            "frame_" + std::string(3 - std::to_string(i + 1).length(), '0') +
            std::to_string(i + 1) + PictureSuffix;
        m_QueuedPaths.push_back(TexturePath);
    }
     // 若 ASTC：启动 loader 线程；PNG/JPG 仍然可以走同步加载（按需）
    if (m_TextureType == EPictureType::ASTC)
    {
        m_StopLoader = false;
        m_NextLoadIndex.store(0);
        m_LoadThread = std::thread(&CAsycSequenceFramePlayer::loaderThreadFunc, this);
        LOGI("%s ASTC loader thread started.", m_TextureRootPath.c_str());
    }
    else
    {
        // 可保留原同步加载逻辑（如果你想仍然支持 png/jpg）
        for (int i = 0; i < m_TextureCount; ++i) {
            std::string TexturePath = m_QueuedPaths[i];
            CTexture2D* pTex = CTexture2D::loadTexture(TexturePath, m_SequenceWidth, m_SequenceHeight, m_TextureType);
            if (!pTex) {
                LOGE("Error loading texture from path [%s].", TexturePath.c_str());
                return false;
            }
            m_SeqTextures[i] = pTex;
        }
    }
    m_SeqSingleTexWidth  = m_SequenceWidth / m_SequenceCols;
    m_SeqSingleTexHeight = m_SequenceHeight / m_SequenceRows;
    
    LOGI("%{public}s frames load Succeed. Program Created Succeed.", m_TextureRootPath.c_str());
    return true;
}

bool CAsycSequenceFramePlayer::initTextureAndShaderProgram(const std::string& vVertexShaderPath, const std::string& vFragShaderShaderPath)
{
    if (!m_TextureRootPath.empty() && m_TextureRootPath.back() != '/')
        m_TextureRootPath += '/';

    std::string PictureSuffix;
    if (m_TextureType == EPictureType::PNG)       PictureSuffix = ".png";
    else if (m_TextureType == EPictureType::JPG)  PictureSuffix = ".jpg";
    else if (m_TextureType == EPictureType::ASTC) PictureSuffix = ".astc";

    for (int i = 0; i < m_TextureCount; i++)
    {
        std::string TexturePath = m_TextureRootPath + "frame_" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1) + PictureSuffix;
        CTexture2D* pSequenceTexture = CTexture2D::loadTexture(TexturePath, m_SequenceWidth, m_SequenceHeight, m_TextureType);
        if (!pSequenceTexture)
        {
            LOGE( "Error loading texture from path [%{public}s].", TexturePath.c_str());
            return false;
        }
        m_SeqTextures.push_back(pSequenceTexture);
    }
    m_SeqSingleTexWidth  = m_SequenceWidth / m_SequenceCols;
    m_SeqSingleTexHeight = m_SequenceHeight / m_SequenceRows;

    m_pSequenceShaderProgram = CShaderProgram::createProgram(vVertexShaderPath, vFragShaderShaderPath);

    if (!m_pSequenceShaderProgram)
    {
        LOGE( "[%{public}s] ShaderProgram init Failed.", m_TextureRootPath.c_str());
        return false;
    }
    assert(m_pSequenceShaderProgram != nullptr);
    LOGI("%{public}s frames load Succeed. Program Created Succeed.", m_TextureRootPath.c_str());
    return true;
}

void CAsycSequenceFramePlayer::updateQuantizationFrame(double vDeltaTime)
{
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;
    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime = 0.0;
        m_CurrentChannel = (m_CurrentChannel + 1) % m_OneTextureFrames;
        if (m_CurrentChannel == 0)
        {
            m_CurrentTexture++;
            if (m_SeqTextures.size() == m_CurrentTexture)
                m_CurrentTexture = 0;
        }
//        LOGI("SeqTexture: %{public}d, Current Channel: %{public}d" , m_CurrentTexture, m_CurrentChannel);
    }
}

void CAsycSequenceFramePlayer::updateMultiChannelFrame(double vDeltaTime, ERenderChannel vRenderChannel)
{
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;
    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime -= FrameTime;
        m_CurrentTexture = (m_CurrentTexture + 1) % static_cast<int>(m_SeqTextures.size());
        m_CurrentChannel = static_cast<std::uint8_t>(vRenderChannel);
//        LOGI("RainSeqTexture: %{public}d, RainCurrentChannel: %{public}d" , m_CurrentTexture, m_CurrentChannel);
    }
}

void CAsycSequenceFramePlayer::updateInterpolationFrame(double vDeltaTime)
{
    // TODO: 目前仅针对单行单列素材做插值，合并大图插值待添加
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;
    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime = 0.0f;
        m_CurrentTexture = m_NextTexture;
        m_CurrentFrame = m_NextFrame;
        if (m_NextFrame == m_ValidFrames - 1)
        {
            if (m_NextTexture == m_TextureCount - 1) m_IsFinished = true;
            m_NextTexture = (m_NextTexture + 1) % m_TextureCount;
        }
        m_NextFrame = (m_NextFrame + 1) % m_ValidFrames;
    }
    m_InterpolationFactor = m_AccumFrameTime / FrameTime;
}

void CAsycSequenceFramePlayer::updateLerpQuantFrame(double vDeltaTime)
{
    // TODO: 目前仅针对单行单列素材做插值，合并大图插值待添加
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;
    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime = 0.0f;

        m_CurrentChannel++;
        if (m_CurrentChannel == m_OneTextureFrames - 1)
        {
            m_NextTexture++;
            if (m_NextTexture == m_TextureCount)
            {
                m_NextTexture = 0;
                m_IsFinished = true;
            }
        }
        else if(m_CurrentChannel == m_OneTextureFrames)
        {
            m_CurrentTexture = m_NextTexture;
            m_CurrentChannel = 0;
        }
//        LOGI("Current Channel: %{public}d, CurrentSeqTexture: %{public}d, NextSeqTexture: %{public}d" , m_CurrentChannel, m_CurrentTexture,  m_NextTexture);
    }
    m_InterpolationFactor = m_AccumFrameTime / FrameTime;
}

void CAsycSequenceFramePlayer::updateSeqKTXFrame(double vDeltaTime)
{
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;
    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime -= FrameTime;
        m_CurrentTexture = (m_CurrentTexture + 1) % static_cast<int>(m_SeqTextures.size());
    }
}

void CAsycSequenceFramePlayer::updateFrameAndUV(double vDeltaTime)
{
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;
//    LOGI("DeltaTime: %lf", vDeltaTime);
    if (m_AccumFrameTime >= FrameTime)
    {
//        LOGI("update Frame");
        m_AccumFrameTime = 0.0f;
        if (m_CurrentFrame == m_ValidFrames - 1)
        {
            if (m_CurrentTexture == m_TextureCount - 1) m_IsFinished = true;
            m_CurrentTexture = (m_CurrentTexture + 1) % m_TextureCount;
        }
        m_CurrentFrame = (m_CurrentFrame + 1) % m_ValidFrames;
    }

    if (m_IsMoving)
    {
        if (m_UseLifeCycle)
        {
            // TODO : update logic
            if (!m_SequenceState._IsAlive)
            {
                m_SequenceState._AlreadyDeadTime += float(vDeltaTime);
                if (m_SequenceState._AlreadyDeadTime > m_SequenceState._PlannedDeadTime)
                {
                    __initSequenceParams();
                    m_SequenceState._IsAlive = true;
                }
            }
            else
            {
                m_SequenceState._AlreadyLivingTime += float(vDeltaTime);
                if (m_SequenceState._AlreadyLivingTime > m_SequenceState._PlannedLivingTime)
                    m_SequenceState._IsAlive = false;
            }
        }
        m_ScreenUVOffset += m_MovingSpeed * float(vDeltaTime);
        float ScreenMaxUV = 1.0f;
        if (m_ScreenUVOffset.x > ScreenMaxUV + m_ScreenUVScale.x || m_ScreenUVOffset.x < -ScreenMaxUV - m_ScreenUVScale.x)
            m_ScreenUVOffset.x = -ScreenMaxUV - m_ScreenUVScale.x;
        if (m_ScreenUVOffset.y > ScreenMaxUV + m_ScreenUVScale.y || m_ScreenUVOffset.y < -ScreenMaxUV - m_ScreenUVScale.y)
            m_ScreenUVOffset.y = -ScreenMaxUV - m_ScreenUVScale.y;
    }
}

void CAsycSequenceFramePlayer::draw(CScreenQuad *vQuad)
{
    if (m_UseLifeCycle && !m_SequenceState._IsAlive)
        return ;

    if (!m_IsLoop && m_IsFinished)
    {
        m_CurrentFrame   = m_ValidFrames - 1;
        m_CurrentTexture = m_TextureCount - 1;
    }
    float RotationAngle   = m_RotationAngle * M_PI / 180.0f;
    int   CurrentFrameRow = m_CurrentFrame / m_SequenceCols;
    int   CurrentFrameCol = m_CurrentFrame % m_SequenceCols;
    float CurrentFrameU0 = static_cast<float>(CurrentFrameCol) / static_cast<float>(m_SequenceCols);
    float CurrentFrameV0 = static_cast<float>(CurrentFrameRow) / static_cast<float>(m_SequenceRows);
    float CurrentFrameU1 = static_cast<float>(CurrentFrameCol + 1) / static_cast<float>(m_SequenceCols);
    float CurrentFrameV1 = static_cast<float>(CurrentFrameRow + 1) / static_cast<float>(m_SequenceRows);
    glm::vec2 TextureUVOffset = glm::vec2(CurrentFrameU0, CurrentFrameV0);
    glm::vec2 TextureUVScale  = glm::vec2(CurrentFrameU1 - CurrentFrameU0, CurrentFrameV1 - CurrentFrameV0);

    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("rotationAngle", RotationAngle);
    m_pSequenceShaderProgram->setUniform("screenUVOffset", m_ScreenUVOffset);
    m_pSequenceShaderProgram->setUniform("screenUVScale", m_ScreenUVScale);
    m_pSequenceShaderProgram->setUniform("texUVOffset", TextureUVOffset);
    m_pSequenceShaderProgram->setUniform("texUVScale", TextureUVScale);
    m_pSequenceShaderProgram->setUniform("sequenceTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    vQuad->bindAndDraw();
}

void CAsycSequenceFramePlayer::drawQuantization(CScreenQuad *vQuad)
{
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("indexTexture", 2);
    glActiveTexture(GL_TEXTURE2);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    vQuad->bindAndDraw();
}

void CAsycSequenceFramePlayer::drawSeqKTX(CScreenQuad *vQuad)
{
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("indexTexture", 1);
    glActiveTexture(GL_TEXTURE1);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    vQuad->bindAndDraw();
}

void CAsycSequenceFramePlayer::drawMultiChannelKTX(CScreenQuad *vQuad)
{
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("indexTexture", 0);
    m_pSequenceShaderProgram->setUniform("channelIndex", m_CurrentChannel);
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    vQuad->bindAndDraw();
}

void CAsycSequenceFramePlayer::drawInterpolation(CScreenQuad *vQuad)
{
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("CurrentTexture", 0);
    m_pSequenceShaderProgram->setUniform("NextTexture", 1);
    m_pSequenceShaderProgram->setUniform("Factor", m_InterpolationFactor);
    m_pSequenceShaderProgram->setUniform("Displacement", 0.0f);
    m_pSequenceShaderProgram->setUniform("CurrentChannel", m_CurrentChannel);
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    glActiveTexture(GL_TEXTURE1);
    m_SeqTextures[m_NextTexture]->bindTexture();
    vQuad->bindAndDraw();
}

void CAsycSequenceFramePlayer::drawInterpolationWithDisplacement(CScreenQuad *vQuad)
{
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("CurrentTexture", 0);
    m_pSequenceShaderProgram->setUniform("NextTexture", 1);
    m_pSequenceShaderProgram->setUniform("Factor", m_InterpolationFactor);
    m_pSequenceShaderProgram->setUniform("Displacement", 0.01f);
    m_pSequenceShaderProgram->setUniform("CurrentChannel", m_CurrentChannel);
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    glActiveTexture(GL_TEXTURE1);
    m_SeqTextures[m_NextTexture]->bindTexture();
    vQuad->bindAndDraw();
}

void CAsycSequenceFramePlayer::drawInterpolationWithFiltering(CScreenQuad *vQuad)
{
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("CurrentTexture", 0);
    m_pSequenceShaderProgram->setUniform("NextTexture", 1);
    m_pSequenceShaderProgram->setUniform("Factor", m_InterpolationFactor);
    m_pSequenceShaderProgram->setUniform("Displacement", 0.01f);
    m_pSequenceShaderProgram->setUniform("CurrentChannel", m_CurrentChannel);
    m_pSequenceShaderProgram->setUniform("TexelSize", glm::vec2( 1.0f / m_SeqSingleTexWidth, 1.0f / m_SeqSingleTexHeight));
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    glActiveTexture(GL_TEXTURE1);
    m_SeqTextures[m_NextTexture]->bindTexture();
    vQuad->bindAndDraw();
}
void CAsycSequenceFramePlayer::setRatioUniform()
{
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("cloudUVOffset", glm::vec2(0.0,1.2));
    m_pSequenceShaderProgram->setUniform("cloudUVScale", glm::vec2(1,0.3));
}
void CAsycSequenceFramePlayer::__initSequenceParams()
{
    std::random_device Rd;
    std::mt19937 Gen(Rd());
    std::uniform_int_distribution<> IntDistribution(0, 1);
    std::uniform_real_distribution<float> FloatDistribution(0.0, 1.0);

    m_SequenceState._IsAlive = true;

    FloatDistribution.param(std::uniform_real_distribution<float>::param_type(30.0f, 40.0f));
    m_SequenceState._PlannedLivingTime = FloatDistribution(Gen);

    FloatDistribution.param(std::uniform_real_distribution<float>::param_type(1.0f, 2.0f));
    m_SequenceState._PlannedDeadTime   = FloatDistribution(Gen);
    m_SequenceState._AlreadyDeadTime   = 0;
    m_SequenceState._AlreadyLivingTime = 0;

    FloatDistribution.param(std::uniform_real_distribution<float>::param_type(0.2f, 1.0f));
    float ScreenRandomUV = FloatDistribution(Gen);
    m_ScreenUVScale = glm::vec2(ScreenRandomUV, ScreenRandomUV);

    FloatDistribution.param(std::uniform_real_distribution<float>::param_type(-0.5f, 0.5f));
    float ScreenRandomOffset = FloatDistribution(Gen);
    // TODO : move from up to down is to be completed later
    float ScreenMaxUV = 1.0f;
    m_ScreenUVOffset = m_MovingSpeed.x > 0 ? glm::vec2(-ScreenMaxUV - ScreenRandomUV, ScreenRandomOffset) : glm::vec2(ScreenMaxUV + ScreenRandomUV, ScreenRandomOffset);

    float MovingDistance = 2.0f + 2 * m_SequenceState._UVScale; // 2.0f is from -1.0 ~ 1.0; * 2 is from left to right
    float Speed = MovingDistance / m_SequenceState._PlannedLivingTime;
    m_MovingSpeed.x = m_MovingSpeed.x > 0 ? Speed : -Speed;
}
// loader 线程函数（成员函数或 lambda 均可）
void CAsycSequenceFramePlayer::loaderThreadFunc()
{
    // 按索引顺序读取文件，不做 GL 操作
    int total = static_cast<int>(m_QueuedPaths.size());
    while (!m_StopLoader && m_NextLoadIndex < total)
    {
        int idx = m_NextLoadIndex.fetch_add(1);
        if (idx >= total) break;

        std::string path = m_QueuedPaths[idx];
        PendingAstc task;
        task.index = idx;
        task.path  = path;

        double t0 = CTimeUtils::getCurrentTime();
        bool ok = CTexture2D::loadAstcToMemory(path, task.data, task.width, task.height);
        double t1 = CTimeUtils::getCurrentTime();

        if (!ok) {
            LOGE("loaderThreadFunc - failed to read ASTC: %s", path.c_str());
            // 继续下一个
            continue;
        }

        LOGI("ASTC read to memory: %{public}s (w=%{public}d h=%{public}d) cost: %{public}f", path.c_str(), task.width, task.height, t1 - t0);

        // push 到主线程待上传队列
        {
            std::lock_guard<std::mutex> lk(m_PendingMutex);
            m_PendingQueue.push(std::move(task));
        }
        m_PendingCV.notify_one();

        // 可选：短暂 sleep 防止 IO 压力过大（根据需要把注释去掉）
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
// 每帧在渲染线程调用：把等待上传的 ASTC 上传到 GPU
void CAsycSequenceFramePlayer::updateAstcTOGPU()
{
    // 1) 处理所有 pending 队列项（如果有），这些数据由 loader 线程读完放入队列
    std::queue<PendingAstc> localQueue;
    {
        std::lock_guard<std::mutex> lk(m_PendingMutex);
        // move all items out（一次性上传，或者按需逐个处理）
        std::swap(localQueue, m_PendingQueue);
    }

    while (!localQueue.empty())
    {
        PendingAstc &task = localQueue.front();

        if (!task.data.empty())
        {
            // 在渲染线程（有 GL context）调用上传函数
            double t0 = CTimeUtils::getCurrentTime();
            CTexture2D* pTex = CTexture2D::createFromAstcMemory(task.data);
            double t1 = CTimeUtils::getCurrentTime();

            if (pTex) {
                // 将生成的 GPU 贴图放到对应 index
                if (task.index >= 0 && task.index < (int)m_SeqTextures.size()) {
                    // 若已有旧纹理，则释放（你需要实现或调用释放逻辑）
                    if (m_SeqTextures[task.index]) {
                        delete m_SeqTextures[task.index];
                        m_SeqTextures[task.index] = nullptr;
                    }
                    m_SeqTextures[task.index] = pTex;
                    LOGI("ASTC uploaded to GPU: %{public}s (index=%{public}d) upload time: %{public}f", task.path.c_str(), task.index, t1 - t0);
                } else {
                    LOGW("ASTC uploaded but index out of range: %d", task.index);
                    delete pTex;
                }
            } else {
                LOGE("Failed to create GL texture from ASTC memory for %s", task.path.c_str());
            }
        } else {
            LOGE("PendingAstc had empty data for %s", task.path.c_str());
        }

        localQueue.pop();
    }

}
// 停止并清理 loader 线程（在销毁时调用）
void CAsycSequenceFramePlayer::shutdownLoader()
{
    m_StopLoader = true;
    if (m_LoadThread.joinable()) {
        // 唤醒线程以便它能检查 m_StopLoader
        m_PendingCV.notify_all();
        m_LoadThread.join();
    }

    // 清空 pending 队列的内存
    {
        std::lock_guard<std::mutex> lk(m_PendingMutex);
        while (!m_PendingQueue.empty()) m_PendingQueue.pop();
    }
}
