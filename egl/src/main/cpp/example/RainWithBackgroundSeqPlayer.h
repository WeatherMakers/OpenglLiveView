#pragma once

#include "SequenceFramePlayer.h"

namespace hiveVG
{
    class CTexture2D;

    class CRainWithBackgroundSeqPlayer : public CSequenceFramePlayer
    {
    public:
        CRainWithBackgroundSeqPlayer(const std::string& vTextureRootPath, int vTextureCount, int vOneTextureFrames, float vFramePerSecond, EPictureType::EPictureType vPictureType = EPictureType::PNG);
        ~CRainWithBackgroundSeqPlayer();

        void initBackground(const std::string& vTexturePath, EPictureType::EPictureType vPictureType);
        void draw(CScreenQuad *vQuad);
        void setCurrentChannel(int vChannelIndex) {m_CurrentChannel = vChannelIndex;}

    private:
        CTexture2D* m_pBackground = nullptr;
    };
}
