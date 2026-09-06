#include "../eqclientmod.h"
// CPU high clock speed overflow fix

#ifdef TIMER_HACK
#include <stdlib.h>
#include "../common.h"
#include "../util.h"
#include "../settings.h"
#include "../eq_trilogy.h"

LARGE_INTEGER g_ProcessorSpeed;
LARGE_INTEGER g_ProcessorTicks;

unsigned __int64 GetCpuTicks_Detour()
{
	LARGE_INTEGER qpcResult;
	QueryPerformanceCounter(&qpcResult);
	return (qpcResult.QuadPart - g_ProcessorTicks.QuadPart);
}

unsigned __int64 GetCpuSpeed2_Detour()
{
	LARGE_INTEGER Frequency;

	//OutputDebugString("GetCpuSpeed2_Detour");
	if (!QueryPerformanceFrequency(&Frequency))
	{
		MessageBoxW(0, L"This OS is not supported.", L"Error", 0);
		exit(-1);
	}

	g_ProcessorSpeed.QuadPart = Frequency.QuadPart / 1000;
	QueryPerformanceCounter(&g_ProcessorTicks);
	return g_ProcessorSpeed.QuadPart;
}

void LoadTimerHack()
{
	bool enable = true;

#ifdef INI_FILE
	char buf[2048];
	const char *desc = "CPU high clock speed overflow fix.  If you have a CPU that's more than 4.2 Ghz you probably need this to make the game run at the right speed.";
	WritePrivateProfileStringA("CPUHighSpeedFix", "Description", desc, INI_FILE);
	GetINIString("CPUHighSpeedFix", "Enabled", "TRUE", buf, sizeof(buf), true);
	enable = ParseINIBool(buf);
#endif

	Log("LoadTimerHack(): hack is %s", enable ? "ENABLED" : "DISABLED");

	if (!enable)
	{
		return;
	}

	if (!hEQGfxDll)
	{
		Log("LoadTimerHack(): graphics DLL not loaded");
		return;
	}

	FARPROC cpuSpeed2 = GetProcAddress(hEQGfxDll, "GetCpuSpeed2");
	FARPROC cpuSpeed3 = GetProcAddress(hEQGfxDll, "GetCpuSpeed3");

	if (!cpuSpeed2 || !cpuSpeed3)
	{
		Log("LoadTimerHack(): CPU timing exports not found");
		return;
	}

	Detour((void *)Offset_rdtsc_elapsed, (void *)GetCpuTicks_Detour);
	Detour((void *)cpuSpeed2, (void *)GetCpuSpeed2_Detour);
	Detour((void *)cpuSpeed3, (void *)GetCpuSpeed2_Detour);
}

#endif
