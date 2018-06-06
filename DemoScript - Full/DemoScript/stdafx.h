// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include "ESL/ESL.h"
#include "io.h"
#include "vector"
#include "CRInterp.h"
#include "HxCode.h"
#include "Math.h"
#include <d3d9.h>
#include <d3dx9.h>

#ifdef _DEBUG
#pragma comment(lib, "../debug/esl.lib")
#else
#pragma comment(lib, "../release/esl.lib")
#endif

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")


#include "BindFuncs.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// INTERNAL PROTOTYPES
///////////////////////////////////////////////////////////////////////////////////////////////////////
double _CalcLength(VEC& v);
double _CalcDistance(VEC& a, VEC& b);
void _Normalize(VEC& v);
void DebugOutput(const char* fmt, ...);
void ResetPacket();
void AddPacket(void* buf, int n);
void InitializeSharedVariables();
BOOL InitializeScript();
void DoSimulation();
