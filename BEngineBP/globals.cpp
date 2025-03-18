#include "pch.h"
#include "globals.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize2.h>

namespace Globals {
    namespace CUserCmd {
        bool userCmd[CUserCmd_Count] = {};
    }

    namespace Status {
        bool windowStatus[WindowStatus_COUNT] = {};
        extern int loadingStatus = LoadingStatus_NONE;
    }
}