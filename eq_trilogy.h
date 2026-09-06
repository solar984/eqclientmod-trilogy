#pragma once

// eqgame.exe - offsets include 0x00400000 base
#define Offset_WindowNameString 0x0056248C
#define Offset_rdtsc_elapsed 0x004E96B9
#define Offset_CEverQuest__clr_chat_input 0x004D0540
#define Offset_CEverQuest__dsp_chat 0x004D055E
#define Offset_CEverQuest__dsp_chat_simple 0x004D05B7
#define Offset_EverQuestObject 0x006EFE2C
#define Offset_CEverQuest__InterpretCmd 0x004DB9B5
#define Offset_InterpretCmd_TypedCall 0x004D9ABE
#define Offset_InterpretCmd_SocialCall 0x00427942

#define EQ_FUNCTION_AT_ADDRESS(function, offset) __declspec(naked) function { __asm{mov eax, offset}; __asm{jmp eax}; }

class CEverQuest
{
public:
	void CEverQuest::clr_chat_input();
	void CEverQuest::dsp_chat(const char *text, short color);
	void CEverQuest::dsp_chat(const char *text);
};

#define EverQuestObject (*(CEverQuest **)Offset_EverQuestObject)
