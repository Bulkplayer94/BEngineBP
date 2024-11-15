#pragma once

#include <assert.h>

namespace Globals {
	namespace CUserCmd {
		enum CUserCmd_ {
			CUserCmd_None,
			CUserCmd_MoveCamFwd,
			CUserCmd_MoveCamBack,
			CUserCmd_MoveCamLeft,
			CUserCmd_MoveCamRight,
			CUserCmd_TurnCamLeft,
			CUserCmd_TurnCamRight,
			CUserCmd_LookUp,
			CUserCmd_LookDown,
			CUserCmd_RaiseCam,
			CUserCmd_LowerCam,
			CUserCmd_Fast,
			CUserCmd_Count,
		};

		extern bool userCmd[CUserCmd_Count];
	}

	namespace Status {
		enum WindowStatus_ {
			WindowStatus_NONE,
			WindowStatus_LOADING,
			WindowStatus_RESIZE,
			WindowStatus_PAUSED,
			WindowStatus_OK,
			WindowStatus_COUNT,
		};

		extern bool windowStatus[WindowStatus_COUNT];

		enum LoadingStatus_ {
			LoadingStatus_NONE,
			LoadingStatus_D3D11,
			LoadingStatus_PhysX,
			LoadingStatus_COUNT,
		};

		extern int loadingStatus;
	}
}