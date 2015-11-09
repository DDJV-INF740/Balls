#include "Precompiled.h"
//=============================================================================
// EXTERNAL DECLARATIONS
//=============================================================================
#include "Core/Game/IGameEngine.h"

using namespace engine;

//-----------------------------------------------------------------------------
// the entry point for any Windows program
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	IGameEngine::Instance()->init();

	while (IGameEngine::Instance()->run())
	{
	}

	IGameEngine::Instance()->cleanup();

	return 0;
}

//-----------------------------------------------------------------------------
//
int _tmain(int argc, _TCHAR* argv[])
{
	return WinMain(GetModuleHandle(NULL), 0, "", true);
}

