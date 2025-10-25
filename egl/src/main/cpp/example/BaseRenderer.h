#pragma once

#define SINGLE_RENDER_TYPE 1
#define FULL_RAIN_SCENE_RENDER_TYPE SINGLE_RENDER_TYPE + 1
#define FULL_SNOW_SCENE_RENDER_TYPE FULL_RAIN_SCENE_RENDER_TYPE + 1
#define FULL_ALL_SCENE_RENDER_TYPE FULL_SNOW_SCENE_RENDER_TYPE + 1

namespace hiveVG
{
    class CBaseRenderer
    {
    public:
        CBaseRenderer();
        virtual ~CBaseRenderer();
        virtual bool init() = 0;
        virtual void draw() = 0;
    };
}

