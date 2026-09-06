// eqclientmod
// hacks/mods for everquest client in a planted dll
// solar@takp.info

#include "eqclientmod.h"
#include "Personalities/personality.h"
#include "common.h"
#include "util.h"
#include "eq_trilogy.h"

//extern "C" __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//extern "C" __declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;
#ifdef BUILD_VERSION
extern "C" __declspec(dllexport) const char *EQCLIENTMOD_BUILD_VERSION = BUILD_VERSION;
#endif

void *Entry_Trampoline;
int tramplen = 0;
DWORD entryPoint;

void Payload2()
{
	//Log("Payload2");

	// restore original entry point code
	Patch((void *)entryPoint, Entry_Trampoline, tramplen);

	LoadCommon();

#ifdef TIMER_HACK
	extern void LoadTimerHack();
	LoadTimerHack();
#endif
#ifdef GAMMA_HACK
	extern void LoadGammaHack();
	LoadGammaHack();
#endif

	// continue from normal program entry point, this never returns
	((void (*)())entryPoint)();
}

bool Payload()
{
	// make sure we're loading into eqgame.exe
	HMODULE callerModule = GetModuleHandleA(0);
	char lpFilename[260];
	GetModuleFileNameA(callerModule, lpFilename, 260);
	if (!(lpFilename && strlen(lpFilename) >= 10 && !strncmp(lpFilename + strlen(lpFilename) - 10, "eqgame.exe", 10)))
	{
		Log("eqclientmod loaded but process file name is not eqgame.exe, skipping eqgame hacks.");
		return FALSE;
	}
	if (strncmp((const char *)Offset_WindowNameString, "EverQuest", 9))
	{
		Log("This doesn't look like the process we expect, skipping eqgame hacks.");
		return FALSE;
	}
	hEQGameEXE = callerModule;

	// This dll expects to be wrapping a dll that the parent module imports from and we're being loaded by the windows dynamic linker before the program starts executing.
	// By detouring the entry point, we can run anything we want at the start of the program once it's actually ready to run.  We can do that here too but we're inside DllMain().
	DWORD entryPointRVA = (DWORD)FindEntryPoint(callerModule);
	entryPoint = (DWORD)callerModule + entryPointRVA;
	for (; tramplen < 5; tramplen += InstructionLength((BYTE *)entryPoint + tramplen));
	Log("eqclientmod: Detouring entry point of %s at %08X + %08X = %08X trampoline bytes = %d", lpFilename, (DWORD)callerModule, entryPointRVA, (DWORD)callerModule + entryPointRVA, tramplen);
	Entry_Trampoline = DetourWithTrampoline((void *)entryPoint, Payload2, tramplen);

	return TRUE;
}

bool DllPersonalityInit(); // the dll whose personality we take on can be changed

BOOL WINAPI DllMain(HMODULE hinstDLL, DWORD r, LPVOID)
{
	if (r == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hinstDLL);
		if (!DllPersonalityInit()) return FALSE;

		if (Payload())
		{
			// prevent self from being unloaded
			HMODULE hThisModule;
			hThisModule = LoadLibrary(PERSONALITY_DLL);
			//GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_PIN, PERSONALITY_DLL, &hThisModule);
		}
	}

	return TRUE;
}
