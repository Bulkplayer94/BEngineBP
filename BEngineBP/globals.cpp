#include "pch.h"
#include "globals.h"

namespace Globals {
    namespace CUserCmd {
        bool userCmd[CUserCmd_Count] = {};
    }

    namespace Status {
        bool windowStatus[WindowStatus_COUNT] = {};
        extern int loadingStatus = LoadingStatus_NONE;
    }
}