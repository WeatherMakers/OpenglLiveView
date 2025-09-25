#pragma once

#include "SequenceFramePlayer.h"

namespace hiveVG
{
    class CTexture2D;

    class CNightSceneSequencePlayer : public CSequenceFramePlayer
    {
    public:
        CNightSceneSequencePlayer(const std::string& vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType::EPictureType vPictureType = EPictureType::PNG);
        ~CNightSceneSequencePlayer();

        void initBackground(const std::string& vTexturePath, EPictureType::EPictureType vPictureType);
        void draw(CScreenQuad *vQuad);
        void setCurrentChannel(int vChannelIndex) {m_CurrentChannel = vChannelIndex;}

    private:
        CTexture2D* m_pBackground = nullptr;
    };
}