#include "Precompiled.h"
//=============================================================================
// EXTERNAL DECLARATIONS
//=============================================================================
#include "Core/Game/IGameEngine.h"


//-----------------------------------------------------------------------------
// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	IEngine::Instance()->init();

	while (IEngine::Instance()->run())
	{}

	IEngine::Instance()->cleanup();
	return 0;
}

//-----------------------------------------------------------------------------
//
int _tmain(int argc, _TCHAR* argv[])
{
	return WinMain(GetModuleHandle(NULL), 0, nullptr, true);
}

