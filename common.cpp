// the command handler here is wedged in before the normal command handler.
// it doesn't do partial matches like the usual commands

#include "eqclientmod.h"
#include "common.h"
#include "util.h"
#include "settings.h"
#include "eq_trilogy.h"
#include <string.h>
#include <ctype.h>

// globals
HMODULE hEQGameEXE;
HMODULE hEQGfxDll;
#ifdef COMMAND_HANDLER
std::map<const char *, _slashCommandHandler> ChatCommandMap;
#endif

#ifdef COMMAND_HANDLER
#include <map>

void command_eqclientmod(void *LocalPlayer, char *text, char *cmd, char *&sep)
{
	char buf[200];
	sprintf(buf, "eqclientmod %s solar@takp.info", BUILD_VERSION);
	EverQuestObject->dsp_chat(buf, 269);
}

//#include <WerApi.h>
void command_crash(void *LocalPlayer, char *text, char *cmd, char *&sep)
{
	//WerSetFlags(WER_FAULT_REPORTING_ALWAYS_SHOW_UI);
	//char *crash = 0;
	//*crash = 0x90;
	//RaiseException(0x0000DEAD, 0, 0, 0);
	ZeroMemory(NULL, 100);
	//int a = 0;
	//int b = 100 / a;
}

typedef void(__thiscall *_CEverQuest__InterpretCmd)(void *this_ptr, void *localplayer, char *text);
_CEverQuest__InterpretCmd CEverQuest__InterpretCmd = (_CEverQuest__InterpretCmd)Offset_CEverQuest__InterpretCmd;
class CEverQuest__InterpretCmd_Detour_type
{
public:
	void CEverQuest__InterpretCmd_Detour(void *LocalPlayer, char *text)
	{
		bool handled = false;
		char *commandText = text;
		while (commandText && isspace((unsigned char)*commandText))
		{
			++commandText;
		}

		if (LocalPlayer && commandText && *commandText == '/')
		{
			char buf2[201];
			strncpy_s(buf2, sizeof(buf2), commandText, _TRUNCATE);

			char *sep = buf2;
			char *cmd = strtok_s(sep, " \t\r\n\v\f", &sep);

			//for (auto item : ChatCommandMap)
			for (std::map<const char *, _slashCommandHandler>::iterator item = ChatCommandMap.begin(); item != ChatCommandMap.end(); ++item)
			{
				if (item->first && !strcmp(cmd, item->first))
				{
					handled = true;
					item->second(LocalPlayer, text, cmd, sep);
				}
			}
		}

		if (handled)
		{
			((CEverQuest *)this)->clr_chat_input();
			return;
		}

		CEverQuest__InterpretCmd(this, LocalPlayer, text);
	}
};
#endif

void LoadCommon()
{
	Log("LoadCommon()");

	hEQGfxDll = LoadLibrary("eqgfx_dx7.dll");
	if (!hEQGfxDll)
	{
		Log("LoadCommon(): NULL result from LoadLibrary(\"eqgfx_dx7.dll\")");
		return;
	}

	//SetProcessAffinityMask(GetCurrentProcess(), 1);
	//SetThreadAffinityMask(GetCurrentThread(), 1);

#ifdef COMMAND_HANDLER
	bool enableCommandHandler = true;

#ifdef INI_FILE
	char buf[2048];
	const char *desc = "This mod adds the extra command handling that some of the other hacks use but it's not strictly necessary to enable this to use the other hacks.  Basic commands included: /eqclientmod /crash";
	WritePrivateProfileStringA("CommandHandler", "Description", desc, INI_FILE);
	GetINIString("CommandHandler", "Enabled", "TRUE", buf, sizeof(buf), true);
	enableCommandHandler = ParseINIBool(buf);
#endif

	Log("LoadCommon(): CommandHandler hack is %s thread %d", enableCommandHandler ? "ENABLED" : "DISABLED", GetCurrentThreadId());

	if (enableCommandHandler)
	{
		//MethodAddressToVariable(NP_CEverQuest__InterpretCmd_Detour, CEverQuest__InterpretCmd_Detour_type::CEverQuest__InterpretCmd_Detour);
		intptr_t NP_CEverQuest__InterpretCmd_Detour; { void(__thiscall CEverQuest__InterpretCmd_Detour_type::* fp)(void *, char *) = &CEverQuest__InterpretCmd_Detour_type::CEverQuest__InterpretCmd_Detour; memcpy(&NP_CEverQuest__InterpretCmd_Detour, &fp, 4); }

		int32_t displacement = (int32_t)(NP_CEverQuest__InterpretCmd_Detour - Offset_InterpretCmd_TypedCall - 5);
		Patch((void *)(Offset_InterpretCmd_TypedCall + 1), &displacement, sizeof(displacement));

		displacement = (int32_t)(NP_CEverQuest__InterpretCmd_Detour - Offset_InterpretCmd_SocialCall - 5);
		Patch((void *)(Offset_InterpretCmd_SocialCall + 1), &displacement, sizeof(displacement));

		ChatCommandMap["/eqclientmod"] = command_eqclientmod;
		ChatCommandMap["/crash"] = command_crash;
	}
#endif
}
