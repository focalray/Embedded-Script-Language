// DemoScript.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BindFuncs.h"

using namespace Hx;


///////////////////////////////////////////////////////////////////////////////////////////////////////
// INTERNAL GLOBALS
///////////////////////////////////////////////////////////////////////////////////////////////////////
CCitrus					esl;

int						SimTime = 10;
int						SimFreq = 60;
int						SimTick = 0;
int						SimTickSec = 0;
unsigned __int64		SimTimeStamp = 0;
double					SimEventProgress = 0.0;
double					SimEventProgressReturn = 0.0;
double					SimEventProgressSmooth = 0.0;
double					SimEventProgressSmoothReturn = 0.0;
char					SimCameraFollowTarget[256] = {0,};
double					SimCameraFollowDist[3] = {0,};
double					SimCameraFollowAngleOffset[3] = {0,};
char					SimCameraFollowAngleLookAt[256] = {0,};
double					SimCameraLookatOffset[3] = {0,};


#define	PACKETMAX		8192
static unsigned char	Packet[PACKETMAX] = {0,};
static int				PacketSize = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////////
// SCRIPT SHARED VARIABLES
///////////////////////////////////////////////////////////////////////////////////////////////////////
HXCONTROL_MOVEENTITY						svHXCONTROL_MOVEENTITY;
HXCONTROL_ACTIVECAMERA						svHXCONTROL_ACTIVECAMERA;
HXCONTROL_KILLENTITY						svHXCONTROL_KILLENTITY;
HXCONTROL_ATMOSPHERE						svHXCONTROL_ATMOSPHERE;
HXCONTROL_SCENECOMPOSITION					svHXCONTROL_SCENECOMPOSITION;
HXCONTROL_EMITPARTICLE						svHXCONTROL_EMITPARTICLE;
HXCONTROL_ONEWAVE							svHXCONTROL_ONEWAVE;
HXCONTROL_TORPEDOHIT						svHXCONTROL_TORPEDOHIT;
HXCONTROL_EXPLOSION							svHXCONTROL_EXPLOSION;
HXCONTROL_ROCKETTRAIL						svHXCONTROL_ROCKETTRAIL;
HXCONTROL_CAVITATIONBUBBLE					svHXCONTROL_CAVITATIONBUBBLE;
HXCONTROL_CAMERA							svHXCONTROL_CAMERA;
HXCONTROL_CAMERAOFFSET						svHXCONTROL_CAMERAOFFSET;
HXCONTROL_OBJECTWATERCOUPLING_REGION		svHXCONTROL_OBJECTWATERCOUPLING_REGION;
HXCONTROL_OBJECTWATERCOUPLING_ADDOBJECT		svHXCONTROL_OBJECTWATERCOUPLING_ADDOBJECT;
HXCONTROL_GUIDELINE							svHXCONTROL_GUIDELINE;
HXCONTROL_WEATHER							svHXCONTROL_WEATHER;
HXCONTROL_UNDERWATEREXPLOSION				svHXCONTROL_UNDERWATEREXPLOSION;
HXCONTROL_UNDERWATER_IMPURITY				svHXCONTROL_UNDERWATER_IMPURITY;
HXCONTROL_SWELL								svHXCONTROL_SWELL;


ENTITY_SCRIPT								svEntityInfoSnapshot;
DYNAMIC_STATE_MISSILE						g_DSHarpoon;
DYNAMIC_STATE_MISSILE						g_DSTorpedo[NUMTOR];
DYNAMIC_STATE_MISSILE						g_DSDecoy[NUMDECOY];
EXPLOSION_STATE								g_Explosions[1+NUMTOR];

std::vector<EVENT>							g_event_list;
std::vector<EVENT>							g_frameevent_list;
std::vector<EVENT>							g_booking_event_list;
std::vector<EVENT>							g_booking_frameevent_list;
std::vector<PATHNODE>						g_path_list[100];
std::vector<HaanUtils::SIMDATA>				g_path_build_list[100];
std::vector<ENTITY>							g_entity_list;
VEC											g_CDTesterPos;
BOOL										g_OnEventStage = FALSE;


///////////////////////////////////////////////////////////////////////////////////////////////////////
// CALL-GATE
///////////////////////////////////////////////////////////////////////////////////////////////////////
// 이벤트가 Host-function으로 제공되지 않아 직접 script에서 처리할 수 있도록 
// 해주는 핸들러.  Host-function이 제공되는 항목은 지원할 필요 없음.
void _stdcall vmCallGate( int funcid, int param )
{
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// SCRIPT DRIVER
///////////////////////////////////////////////////////////////////////////////////////////////////////
void BuildEntity(ENTITY& ent, HaanUtils::SIMDATA& sim)
{
	if(0 == ent.live)
	{
		svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
		svHXCONTROL_MOVEENTITY.Length = sizeof(Hx::HXCONTROL_MOVEENTITY);
		svHXCONTROL_MOVEENTITY.iEntity = atoi(ent.name);
		svHXCONTROL_MOVEENTITY.Flags = 0x20;
		svHXCONTROL_MOVEENTITY.Velo[0] = 0.0;
		svHXCONTROL_MOVEENTITY.Velo[1] = 0.0;
		svHXCONTROL_MOVEENTITY.Velo[2] = 0.0;
		AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));
		return;
	}

	svHXCONTROL_MOVEENTITY.Code = HXCODE_MOVEENTITY;
	svHXCONTROL_MOVEENTITY.Length = sizeof(Hx::HXCONTROL_MOVEENTITY);
	svHXCONTROL_MOVEENTITY.iEntity = atoi(ent.name);
	svHXCONTROL_MOVEENTITY.Flags = 0x78;
	svHXCONTROL_MOVEENTITY.TimeStamp = SimTimeStamp;
	svHXCONTROL_MOVEENTITY.Pos[0] = sim.Pos[0];
	svHXCONTROL_MOVEENTITY.Pos[1] = sim.Pos[1];
	svHXCONTROL_MOVEENTITY.Pos[2] = sim.Pos[2];
	svHXCONTROL_MOVEENTITY.Velo[0] = sim.Vel[0];
	svHXCONTROL_MOVEENTITY.Velo[1] = sim.Vel[1];
	svHXCONTROL_MOVEENTITY.Velo[2] = sim.Vel[2];

	float hpr[3] = {Radian(sim.Ori[0]), Radian(sim.Ori[1]), Radian(sim.Ori[2])};
	EulerToRotationMatrix(hpr, svHXCONTROL_MOVEENTITY.matRot);

	//DebugOutput("[HXCONTROL_MOVEENTITY] = (%f, %f, %f), (%.1f, %.1f, %.1f)\n", 
	//	varHXCONTROL_MOVEENTITY.Pos[0], varHXCONTROL_MOVEENTITY.Pos[1], varHXCONTROL_MOVEENTITY.Pos[2], 
	//	Degree(hpr[0]), Degree(hpr[1]), Degree(hpr[2]));

	AddPacket(&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
	// 스크립터 초기화
	InitializeScript();

	// 공유 변수 초기화
	InitializeSharedVariables();

	// 스크립트 최초 진입
	esl.call(NULL);

	// 스크립트기반 시뮬레이션 시작
	DoSimulation();

////////////////////////////////////// TEST CODE BELOW ////////////////////////////////////////

	return 0;  

	// 다음은 레코드 데이터 읽기 위한 테스트 코드이다.
	
	// DEBUG RECORD FILE ---
	DataStream dsRead;	
	dsRead.Init("G:\\DEV_SHIP\\J3\\HxDemo\\HX\\Run\\Rec.out");
	//dsRead.Init("Rec.out");
	//r = dsRead.OpenToReadStream(); // can cause periodic hdd access
	BOOL r = dsRead.OpenToRead(); // on memory access (if memory is enough)
	assert(r);


	char* codemap[] = 
	{
		"HXCODE_BEGINPACKET",
		"HXCODE_NEWFRAME",        
		"HXCODE_MOVEENTITY",
		"HXCODE_ACTIVECAMERA",
		"HXCODE_KILLENTITY",
		"HXCODE_ATMOSPHERE",
		"HXCODE_SCENECOMPOSITION",
		"HXCODE_CAMERA",
		"HXCODE_CAMERAOFFSET",
		"HXCODE_ONEWAVE",
		"HXCODE_EMITPARTICLE",
		"HXCODE_TORPEDOHIT",
		"HXCODE_EXPLOSION",
		"HXCODE_ROCKETTRAIL",
		"HXCODE_CAVITATIONBUBBLE"
	};


	int iFrameCount = 0;
	while(!dsRead.IsEnd())
	{
		iFrameCount++;
		DebugOutput("-------------------------------FRAME %d-----------------------------------\n", iFrameCount);

		dsRead.GetData<int>(&PacketSize);
		dsRead.GetData<unsigned char>(Packet, PacketSize);

		unsigned char* pByte = (unsigned char*)Packet;

		// PACKET EXECUTE---
		while(1)  
		{
			HXCONTROLHEADER* pHdr = (HXCONTROLHEADER*)pByte;
			if(pHdr->Code < 0 || pHdr->Code > 255)
				continue;

			DWORD diff = (unsigned int)(pByte-Packet);
			DebugOutput("%s: Offset= %d, Len= %d\n", 
				codemap[pHdr->Code], diff, pHdr->Length);

			switch(pHdr->Code)
			{

			case HXCODE_CAMERA:
				{
					HXCONTROL_CAMERA* op = (HXCONTROL_CAMERA*)pByte;
					DebugOutput("Fov = %f\n", Degree(op->vFov));
				}
				break;
			case HXCODE_MOVEENTITY:
				{
					HXCONTROL_MOVEENTITY* op = (HXCONTROL_MOVEENTITY*)pByte;
					float rot[3] = {0,};
					RotationMatrixToEuler(op->matRot, rot);
					//DebugOutput("[HXCONTROL_MOVEENTITY] = (%f, %f, %f), (%.1f, %.1f, %.1f)\n", 
					//	op->Pos[0], op->Pos[1], op->Pos[2], 
					//	Degree(rot[0]), Degree(rot[1]), Degree(rot[2]));
				}
				break;
			}

			pByte += pHdr->Length;

			if((int)(pByte-Packet) >= PacketSize)
				break;
		}
	}
	
	return 0;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
// SHARED VARIABLES INITIALIZER
///////////////////////////////////////////////////////////////////////////////////////////////////////
void InitializeSharedVariables()
{
	svHXCONTROL_ATMOSPHERE.Code = HXCODE_ATMOSPHERE;
	svHXCONTROL_ATMOSPHERE.Length = sizeof(Hx::HXCONTROL_ATMOSPHERE);
	svHXCONTROL_ATMOSPHERE.AirAttenuation = -0.00005f;
	svHXCONTROL_ATMOSPHERE.WaterAttenuation = -0.001f;
    svHXCONTROL_ATMOSPHERE.FogHigh = 800.0;
    svHXCONTROL_ATMOSPHERE.FogColor[0] = 14.0;
    svHXCONTROL_ATMOSPHERE.FogColor[1] = 14.0;
    svHXCONTROL_ATMOSPHERE.FogColor[2] = 14.0;
    svHXCONTROL_ATMOSPHERE.WaterColor[0] = 0;
    svHXCONTROL_ATMOSPHERE.WaterColor[1] = 1;
    svHXCONTROL_ATMOSPHERE.WaterColor[2] = 2;
    svHXCONTROL_ATMOSPHERE.SkyBright[0] = 8.0;
    svHXCONTROL_ATMOSPHERE.SkyBright[1] = 8.0;
    svHXCONTROL_ATMOSPHERE.SkyBright[2] = 8.0;
    svHXCONTROL_ATMOSPHERE.SunLight[0] = 12.0;
    svHXCONTROL_ATMOSPHERE.SunLight[1] = 12.0;
    svHXCONTROL_ATMOSPHERE.SunLight[2] = 12.0;
    svHXCONTROL_ATMOSPHERE.SunDir[0] = 0.0;
    svHXCONTROL_ATMOSPHERE.SunDir[1] = 1.0;
    svHXCONTROL_ATMOSPHERE.SunDir[2] = 0.0;

	svHXCONTROL_WEATHER.Code = HXCODE_WEATHER;
	svHXCONTROL_WEATHER.Length = sizeof(svHXCONTROL_WEATHER);
	svHXCONTROL_WEATHER.nCountSnow = 0;
	svHXCONTROL_WEATHER.bRain = FALSE;
	svHXCONTROL_WEATHER.fSnowSize = 0.1f;
	svHXCONTROL_WEATHER.fRainWidth = 0.01f;
	svHXCONTROL_WEATHER.fRainHeight = 0.3f;
	svHXCONTROL_WEATHER.nCloudCount = 0;
	svHXCONTROL_WEATHER.bCloudVisible = FALSE;
	svHXCONTROL_WEATHER.nCloudAltitude = 1000;
	svHXCONTROL_WEATHER.nCloudWidth = 1000;
	svHXCONTROL_WEATHER.nCloudHeight = 1000;

	svHXCONTROL_ACTIVECAMERA.iCamera = 1; // main camera

	svHXCONTROL_SCENECOMPOSITION.Exposure = 0.07f;
	svHXCONTROL_SCENECOMPOSITION.WaterReflection = 0.5f;
	svHXCONTROL_SCENECOMPOSITION.WaterMixture = 0.75f;

	svHXCONTROL_ONEWAVE.WhiteCapStrength = 0.001f;

	memset(&g_DSHarpoon, 0, sizeof(g_DSHarpoon));
	memset(g_DSTorpedo, 0, sizeof(g_DSTorpedo));
	memset(g_Explosions, 0, sizeof(g_Explosions));
}
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL InitializeScript()
{
	//DebugOutput("HXCONTROL_MOVEENTITY = %d\n", sizeof Hx::HXCONTROL_MOVEENTITY);
	//DebugOutput("HXCONTROL_ACTIVECAMERA = %d\n", sizeof Hx::HXCONTROL_ACTIVECAMERA);
	//DebugOutput("HXCONTROL_KILLENTITY = %d\n", sizeof Hx::HXCONTROL_KILLENTITY);
	//DebugOutput("HXCONTROL_ATMOSPHERE = %d\n", sizeof Hx::HXCONTROL_ATMOSPHERE);
	//DebugOutput("HXCONTROL_SCENECOMPOSITION = %d\n", sizeof Hx::HXCONTROL_SCENECOMPOSITION);
	//DebugOutput("HXCONTROL_EMITPARTICLE = %d\n", sizeof Hx::HXCONTROL_EMITPARTICLE);
	//DebugOutput("HXCONTROL_ONEWAVE = %d\n", sizeof Hx::HXCONTROL_ONEWAVE);
	//DebugOutput("HXCONTROL_TORPEDOHIT = %d\n", sizeof Hx::HXCONTROL_TORPEDOHIT);
	//DebugOutput("HXCONTROL_EXPLOSION = %d\n", sizeof Hx::HXCONTROL_EXPLOSION);
	//DebugOutput("HXCONTROL_ROCKETTRAIL = %d\n", sizeof Hx::HXCONTROL_ROCKETTRAIL);
	//DebugOutput("HXCONTROL_CAVITATIONBUBBLE = %d\n", sizeof Hx::HXCONTROL_CAVITATIONBUBBLE);
	//DebugOutput("HXCONTROL_CAMERA = %d\n", sizeof Hx::HXCONTROL_CAMERA);
	//DebugOutput("HXCONTROL_CAMERAOFFSET = %d\n", sizeof Hx::HXCONTROL_CAMERAOFFSET);
	//DebugOutput("HXCONTROL_OBJECTWATERCOUPLING_REGION = %d\n", sizeof Hx::HXCONTROL_OBJECTWATERCOUPLING_REGION);
	//DebugOutput("HXCONTROL_OBJECTWATERCOUPLING_ADDOBJECT = %d\n", sizeof Hx::HXCONTROL_OBJECTWATERCOUPLING_ADDOBJECT);
	//DebugOutput("HXCONTROL_GUIDELINE = %d\n", sizeof Hx::HXCONTROL_GUIDELINE);
	//DebugOutput("HXCONTROL_WEATHER = %d\n", sizeof Hx::HXCONTROL_WEATHER);


	char script_path[] = "DemoScript.s";
	if(0 != _access(script_path, 0))
		return FALSE;

	esl.bind_i( &SimTime, "SimTime" );
	esl.bind_i( &SimFreq, "SimFreq" );
	esl.bind_i( &SimTick, "SimTick" );
	esl.bind_i( &SimTickSec, "SimTickSec" );
	esl.bind_f( &SimEventProgress, "SimEventProgress" );
	esl.bind_f( &SimEventProgressReturn, "SimEventProgressReturn" );
	esl.bind_f( &SimEventProgressSmooth, "SimEventProgressSmooth" );
	esl.bind_f( &SimEventProgressSmoothReturn, "SimEventProgressSmoothReturn" );
	

	esl.bind_function( (int (_stdcall*)())bindcall_NewEvent, "NewEvent" );
	esl.bind_function( (int (_stdcall*)())bindcall_NewFrameEvent, "NewFrameEvent" );
	esl.bind_function( (int (_stdcall*)())bindcall_DriveObjectByPath, "DriveObjectByPath" );
	esl.bind_function( (int (_stdcall*)())bindcall_ResetPath, "ResetPath" );
	esl.bind_function( (int (_stdcall*)())bindcall_AddPathNode, "AddPathNode" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetCameraPosOri, "SetCameraPosOri" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetCameraPos, "SetCameraPos" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetCameraOri, "SetCameraOri" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetCameraFollowOffset, "SetCameraFollowOffset" );
	esl.bind_function( (int (_stdcall*)())bindcall_StopCameraFollow, "StopCameraFollow" );
	esl.bind_function( (int (_stdcall*)())bindcall_QueryEntityInfo, "QueryEntityInfo" );
	esl.bind_function( (int (_stdcall*)())bindcall_UpdateDynamics_Harpoon, "UpdateDynamics_Harpoon" );
	esl.bind_function( (int (_stdcall*)())bindcall_UpdateDynamics_Torpedo, "UpdateDynamics_Torpedo" );
	esl.bind_function( (int (_stdcall*)())bindcall_BuildDynamicEntity, "BuildDynamicEntity" );
	esl.bind_function( (int (_stdcall*)())bindcall_RunDynamics, "RunDynamics" );
	esl.bind_function( (int (_stdcall*)())bindcall_StopDynamics, "StopDynamics" );
	esl.bind_function( (int (_stdcall*)())bindcall_ResetDynamics, "ResetDynamics" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetEntityPosOri, "SetEntityPosOri" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetWaterAttenuation, "SetWaterAttenuation" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetCameraLookat, "SetCameraLookat" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetCameraLookatOffset, "SetCameraLookatOffset" );
	esl.bind_function( (int (_stdcall*)())bindcall_UseEntity, "UseEntity" );
	esl.bind_function( (int (_stdcall*)())bindcall_ValidateEntity, "ValidateEntity" );
	esl.bind_function( (int (_stdcall*)())bindcall_InvalidateEntity, "InvalidateEntity" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetActiveCamera, "SetActiveCamera" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetCamera, "SetCamera" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetOneWave, "SetOneWave" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetScene, "SetScene" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetCameraPosLookat, "SetCameraPosLookat" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetSubmarineWaterCouplingRegion, "SetSubmarineWaterCouplingRegion" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetTOD, "SetTOD" );
	esl.bind_function( (int (_stdcall*)())bindcall_TranslateEntityPos, "TranslateEntityPos" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetFogAlt, "SetFogAlt" );
	esl.bind_function( (int (_stdcall*)())bindcall_DoExplosionByTorpedo, "DoExplosionByTorpedo" );
	esl.bind_function( (int (_stdcall*)())bindcall_DoExplosionByHarpoon, "DoExplosionByHarpoon" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetEntityLiveState, "SetEntityLiveState" );
	esl.bind_function( (int (_stdcall*)())bindcall_AutoExposure, "AutoExposure" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetCDTestPosition, "SetCDTestPosition" );
	esl.bind_function( (int (_stdcall*)())bindcall_ResetPathDriving, "ResetPathDriving" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetEntityModel, "SetEntityModel" );
	esl.bind_function( (int (_stdcall*)())bindcall_ResetGuidelines, "ResetGuidelines" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetFogColor, "SetFogColor" );
	esl.bind_function( (int (_stdcall*)())bindcall_UpdateDynamics_Decoy, "UpdateDynamics_Decoy" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetCloud, "SetCloud" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetRain, "SetRain" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetSnow, "SetSnow" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetWaterColor, "SetWaterColor" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetAirAttenuation, "SetAirAttenuation" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetExposure, "SetExposure" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetWaterReflection, "SetWaterReflection" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetUnderWaterExplosion, "SetUnderWaterExplosion" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetUnderWaterImpurity, "SetUnderWaterImpurity" );
	esl.bind_function( (int (_stdcall*)())bindcall_SetSwell, "SetSwell" );


	esl.init((char*)script_path, FALSE, vmCallGate, NULL);

	esl.init_block_port(21); // 21개의 구조체 연결을 준비함.
	esl.register_block(0, (void*)&svHXCONTROL_SCENECOMPOSITION, sizeof(svHXCONTROL_SCENECOMPOSITION));
	esl.register_block(1, (void*)&svHXCONTROL_CAMERA, sizeof(svHXCONTROL_CAMERA));
	esl.register_block(2, (void*)&svHXCONTROL_ACTIVECAMERA, sizeof(svHXCONTROL_ACTIVECAMERA));
	esl.register_block(3, (void*)&svHXCONTROL_MOVEENTITY, sizeof(svHXCONTROL_MOVEENTITY));
	esl.register_block(4, (void*)&svHXCONTROL_KILLENTITY, sizeof(svHXCONTROL_KILLENTITY));
	esl.register_block(5, (void*)&svHXCONTROL_ATMOSPHERE, sizeof(svHXCONTROL_ATMOSPHERE));
	esl.register_block(6, (void*)&svHXCONTROL_EMITPARTICLE, sizeof(svHXCONTROL_EMITPARTICLE));
	esl.register_block(7, (void*)&svHXCONTROL_ONEWAVE, sizeof(svHXCONTROL_ONEWAVE));
	esl.register_block(8, (void*)&svHXCONTROL_TORPEDOHIT, sizeof(svHXCONTROL_TORPEDOHIT));
	esl.register_block(9, (void*)&svHXCONTROL_EXPLOSION, sizeof(svHXCONTROL_EXPLOSION));
	esl.register_block(10, (void*)&svHXCONTROL_ROCKETTRAIL, sizeof(svHXCONTROL_ROCKETTRAIL));
	esl.register_block(11, (void*)&svHXCONTROL_CAVITATIONBUBBLE, sizeof(svHXCONTROL_CAVITATIONBUBBLE));
	esl.register_block(12, (void*)&svHXCONTROL_CAMERAOFFSET, sizeof(svHXCONTROL_CAMERAOFFSET));
	esl.register_block(13, (void*)&svHXCONTROL_OBJECTWATERCOUPLING_REGION, sizeof(svHXCONTROL_OBJECTWATERCOUPLING_REGION));
	esl.register_block(14, (void*)&svHXCONTROL_OBJECTWATERCOUPLING_ADDOBJECT, sizeof(svHXCONTROL_OBJECTWATERCOUPLING_ADDOBJECT));
	esl.register_block(15, (void*)&svHXCONTROL_GUIDELINE, sizeof(svHXCONTROL_GUIDELINE));
	esl.register_block(16, (void*)&svHXCONTROL_WEATHER, sizeof(svHXCONTROL_WEATHER));
	esl.register_block(17, (void*)&svHXCONTROL_UNDERWATEREXPLOSION, sizeof(svHXCONTROL_UNDERWATEREXPLOSION));
	esl.register_block(18, (void*)&svHXCONTROL_UNDERWATER_IMPURITY, sizeof(svHXCONTROL_UNDERWATER_IMPURITY));
	esl.register_block(19, (void*)&svHXCONTROL_SWELL, sizeof(svHXCONTROL_SWELL));
	esl.register_block(20, (void*)&svEntityInfoSnapshot, sizeof(svEntityInfoSnapshot));
	

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void DoSimulation()
{
	// 데이터 처리기 준비
	DataStream dsWrite;
	dsWrite.Init("G:\\DEV_SHIP\\J3\\HxDemo\\HX\\Run\\Rec.out");
	//dsWrite.Init("Rec.out");
	BOOL r = dsWrite.CreateToWrite();
	assert(r);


	// Initialize the main camera.
	bindcall_UseEntity("1", 1, 0.0, "", 0);


	// 내부 TimeStamp
	const unsigned __int64 TimeStampStep = (unsigned __int64)((1.0 / (double)SimFreq) * 1000000);

	// Run Simulation ---------------------------------------------------------------
	for(int kTickSec=0; kTickSec<SimTime; kTickSec++)
	{
		SimTickSec = kTickSec;

		for(int kTickFreq=0; kTickFreq<SimFreq; kTickFreq++)
		{
			SimTick = kTickSec*SimFreq + kTickFreq;
			SimTimeStamp += TimeStampStep;

			// Reset Frame Packet
			ResetPacket();

			// Event stage begins.
			g_OnEventStage = 1;

			// EVENT PROCESS BY SECONDS ----------------------------------------------------
			int nEvent = g_event_list.size();
			if(0 == (SimTick % SimFreq))
			{
				for(int kEvent=0; kEvent<nEvent; kEvent++)
				{
					EVENT& ev = g_event_list[kEvent];

					if(0 == ev.period || (SimTickSec > 0 && 0 == (SimTickSec % ev.period)))
						ev.triggered = TRUE;

					if(ev.triggered)
					{// on timing
						if(-1 == ev.repeat)
						{//INFINITE CALL
							int r = esl.call(ev.sip);
						}
						else if(ev.knocked < ev.repeat)
						{
							SimEventProgress = (double)(ev.knocked+1) / (double)ev.repeat;
							SimEventProgressReturn = SimEventProgress*2.0;
							if(SimEventProgress > 0.5)
								SimEventProgressReturn = (1.0-SimEventProgress)*2.0;
							SimEventProgressSmooth = 1.0 - (cos(PI*SimEventProgress) + 1.0) * 0.5;
							SimEventProgressSmoothReturn = 1.0 - (cos(2.0*PI*SimEventProgress) + 1.0) * 0.5;

							int r = esl.call(ev.sip);
							ev.knocked++;
						}
					}
				}
			}

			// EVENT PROCESS BY TICK COUNTS
			nEvent = g_frameevent_list.size();
			for(int kEvent=0; kEvent<nEvent; kEvent++)
			{
				EVENT& ev = g_frameevent_list[kEvent];

				if(0 == ev.period || (SimTickSec > 0 && 0 == (SimTickSec % ev.period)))
					ev.triggered = TRUE;

				if(ev.triggered)
				{// on timing
					if(-1 == ev.repeat)
					{//INFINITE CALL
						if(ev.sip_init > -1)
						{
							esl.call(ev.sip_init);
							ev.sip_init = -1;
						}
						
						int r = esl.call(ev.sip);
					}
					else if(ev.knocked < ev.repeat)
					{
						SimEventProgress = (double)(ev.knocked+1) / (double)ev.repeat;
						SimEventProgressReturn = SimEventProgress*2.0;
						if(SimEventProgress > 0.5)
							SimEventProgressReturn = (1.0-SimEventProgress)*2.0;
						SimEventProgressSmooth = 1.0 - (cos(PI*SimEventProgress) + 1.0) * 0.5;
						SimEventProgressSmoothReturn = 1.0 - (cos(2.0*PI*SimEventProgress) + 1.0) * 0.5;

						if(ev.sip_init > -1)
						{
							esl.call(ev.sip_init);
							ev.sip_init = -1;
						}

						int r = esl.call(ev.sip);
						ev.knocked++;
					}
				}
			}


			// Process entities --------------------------------
			int nEntity = g_entity_list.size();
			for(int kEntity=0; kEntity<nEntity; kEntity++)
			{
				ENTITY& ent = g_entity_list[kEntity];

				BOOL bCameraControlled = FALSE;
				BOOL bLookat = FALSE;


				if(ent.live && -1 != ent.sip_cdcb && ent.cdrange > 0.0)
				{// Collision Detection Check
					double d = HaanUtils::CCatmullRomInterp::CalcDistance((double*)&g_CDTesterPos,
						(double*)&ent.pos);

					if(d < ent.cdrange)
					{
						if(-1 == ent.cdrepeat)
						{
							esl.call(ent.sip_cdcb);
						}
						else if(ent.cdrepeat > 0)
						{
							esl.call(ent.sip_cdcb);
							ent.cdrepeat--;
						}
					}
				}


				if(1 == svHXCONTROL_ACTIVECAMERA.iCamera)
				{// main camera
					if(_stricmp(SimCameraFollowAngleLookAt, ent.name) == 0)
						bLookat = TRUE;

					if(SimCameraFollowTarget[0])
					{// 추적 타겟이 존재하는가?					
						if(_stricmp(SimCameraFollowTarget, ent.name) == 0)
						{// 카메라가 쫒아야 할 타겟인가?
							ENTITY& ce = g_entity_list[0];
							D3DXVECTOR3 vup(0, 0, 1);
							D3DXVECTOR3 vdir((float)ent.dir.x, (float)ent.dir.y, (float)ent.dir.z);
							D3DXVec3Normalize(&vdir, &vdir);
							D3DXVECTOR3 vright;
							D3DXVec3Cross(&vright, &vdir, &vup);
							D3DXVec3Cross(&vup, &vright, &vdir);

							ce.pos.x = ent.pos.x;
							ce.pos.y = ent.pos.y;
							ce.pos.z = ent.pos.z;

							ce.pos.x += vright.x*SimCameraFollowDist[0];
							ce.pos.y += vright.y*SimCameraFollowDist[0];
							ce.pos.z += vright.z*SimCameraFollowDist[0];

							ce.pos.x += vdir.x*SimCameraFollowDist[1];
							ce.pos.y += vdir.y*SimCameraFollowDist[1];
							ce.pos.z += vdir.z*SimCameraFollowDist[1];

							ce.pos.x += vup.x*SimCameraFollowDist[2];
							ce.pos.y += vup.y*SimCameraFollowDist[2];
							ce.pos.z += vup.z*SimCameraFollowDist[2];

							ce.ori.h = ent.ori.h + (float)SimCameraFollowAngleOffset[0];
							ce.ori.p = ent.ori.p + (float)SimCameraFollowAngleOffset[1];
							ce.ori.r = ent.ori.r + (float)SimCameraFollowAngleOffset[2];


							// Build up a camera packet.
							if(bLookat)
							{
								bindcall_SetCameraPos(ce.pos.x, ce.pos.y, ce.pos.z);
								nonexposed_SetCameraLookat(SimCameraFollowAngleLookAt);
							}
							else
							{
								bindcall_SetCameraPosOri(ce.pos.x, ce.pos.y, ce.pos.z,
														ce.ori.h, ce.ori.p, ce.ori.r);
							}
						}

						bCameraControlled = TRUE;
					}
					else
					{
						if(bLookat)
						{
							nonexposed_SetCameraLookat(SimCameraFollowAngleLookAt);
							bCameraControlled = TRUE;
						}
					}
				}


				if(SimTick < ent.ticklast && ent.live)
				{
					// 엔티티 처리
					HaanUtils::SIMDATA sim = g_path_build_list[ent.usepath][ent.tickcur++];

					// Build up an entity packet.
					if(bCameraControlled)
					{
						if(kEntity > 0)
							BuildEntity(ent, sim);
					}
					else
					{
						BuildEntity(ent, sim);
					}


					// Update Entity's Pos, Ori, Dir
					ent.pos.x = sim.Pos[0];
					ent.pos.y = sim.Pos[1];
					ent.pos.z = sim.Pos[2];
					ent.ori.h = sim.Ori[0];
					ent.ori.p = sim.Ori[1];
					ent.ori.r = sim.Ori[2];
					ent.dir.x = sim.Dir[0];
					ent.dir.y = sim.Dir[1];
					ent.dir.z = sim.Dir[2];

					// Callback init
					if(1 == ent.tickcur && ent.sip_init > -1)
						esl.call(ent.sip_init);

					// Callback
					if(ent.sip_during > -1)
						esl.call(ent.sip_during);

					// Callback finish
					if((SimTick+1) == ent.ticklast && ent.sip_finish > -1)
						esl.call(ent.sip_finish);
				}
			}


			// Event stage end.
			g_OnEventStage = 0;


			// accept booking events.
			int nBooking = g_booking_event_list.size();
			for(int kBooking=0; kBooking<nBooking; kBooking++)
				g_event_list.push_back(g_booking_event_list[kBooking]);
			for(int kBooking=0; kBooking<nBooking; kBooking++)
				g_booking_event_list.pop_back();

			nBooking = g_booking_frameevent_list.size();
			for(int kBooking=0; kBooking<nBooking; kBooking++)
				g_frameevent_list.push_back(g_booking_frameevent_list[kBooking]);
			for(int kBooking=0; kBooking<nBooking; kBooking++)
				g_booking_frameevent_list.pop_back();


			// Build packet
			dsWrite.PutData<int>(&PacketSize);
			dsWrite.PutData<unsigned char>(Packet, PacketSize);
		}
	}


	// 데이터 기록 종료
	dsWrite.Close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// PACKET MANAGEMENT
///////////////////////////////////////////////////////////////////////////////////////////////////////
void ResetPacket()
{
	memset(Packet, 0, PACKETMAX);
	PacketSize = 0;
}

void AddPacket(void* buf, int n)
{
	memcpy((void*)&Packet[PacketSize], buf, n);
	PacketSize += n;
	assert(PacketSize < PACKETMAX);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// UTILITIES
///////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugOutput(const char* fmt, ...)
{
	char		text[1024];			
	va_list		ap;					
	
	if (fmt == NULL)				
		return;						

	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf_s(text, 1024, "[%02d:%02d:%02d.%03d]", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	text[14] = ' ';

	va_start(ap, fmt);				
	vsprintf_s(text+15, 1009, fmt, ap);	
	va_end(ap);						

	OutputDebugStringA(text);
}

double _CalcLength(VEC& v)
{
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

double _CalcDistance(VEC& a, VEC& b)
{
	VEC v(a.x-b.x, a.y-b.y, a.z-b.z);
	return _CalcLength(v);
}

void _Normalize(VEC& v)
{
	double len = _CalcLength(v);
	v.x /= len;
	v.y /= len;
	v.z /= len;
}
