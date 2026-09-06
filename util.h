#pragma once

#include "eqclientmod.h"

char *StringFormat(const char *format, ...);
void Log(const char *format, ...);

void Patch(void *dst, const void *src, size_t len);
bool Detour(void *address, void *dst);
void *MakeTrampoline(void *target, int len);
void *DetourWithTrampoline(void *address, void *dst, int len);
void *FindEntryPoint(HMODULE module);
void *FindIATPointer(HMODULE module, const char *functionName);

unsigned int InstructionLength(BYTE *pc);
