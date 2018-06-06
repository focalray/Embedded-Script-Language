#pragma once
namespace Hx
{
#define HXCODE_BEGINPACKET          0
#define HXCODE_NEWFRAME	            1
#define HXCODE_MOVEENTITY           2
#define HXCODE_ACTIVECAMERA         3
#define HXCODE_KILLENTITY           4
#define HXCODE_ATMOSPHERE           5
#define HXCODE_SCENECOMPOSITION     6
#define HXCODE_CAMERA               7
#define HXCODE_CAMERAOFFSET         8
#define HXCODE_ONEWAVE              9

#define HXCODE_EMITPARTICLE        10
#define HXCODE_TORPEDOHIT          11 
#define HXCODE_EXPLOSION           12
#define HXCODE_ROCKETTRAIL         13
#define HXCODE_CAVITATIONBUBBLE    14
#define HXCODE_OBJECTWATERCOUPLING_REGION       15
#define HXCODE_OBJECTWATERCOUPLING_ADDOBJECT    16
#define HXCODE_GUIDELINE             17
#define HXCODE_WEATHER              18
#define HXCODE_UNDERWATEREXPLOSION 19
#define HXCODE_UNDERWATER_IMPURITY 20
#define HXCODE_SWELL 21

#pragma pack(4)

struct HXCONTROLHEADER
{
    WORD Code;    
    WORD Length;
};

struct HXCONTROL_BEGINPACKET : public HXCONTROLHEADER 
{
    DWORD TotalPacketLength;
    DWORD iChannel;  
};

struct HXCONTROL_NEWFRAME : public HXCONTROLHEADER 
{
    DWORD iFrame;
    unsigned __int64 TimeStamp;
};

struct HXCONTROL_MOVEENTITY : public HXCONTROLHEADER
{
    DWORD iEntity;
    DWORD Flags;
        // bit0: parent
        // bit1: model
        // bit2: light
        // bit3: timestamp
        // bit4: pos
        // bit5: velo
        // bit6: matrot
    DWORD iParentEntity;
    DWORD iModel;
    DWORD iLight;
    unsigned __int64 TimeStamp;
    double Pos[3];
    float Velo[3];
    float matRot[16];
};

struct HXCONTROL_ACTIVECAMERA : public HXCONTROLHEADER
{
    DWORD iCamera;
};

struct HXCONTROL_KILLENTITY : public HXCONTROLHEADER
{
    DWORD iEntity;
    DWORD Method;
};

struct HXCONTROL_ATMOSPHERE : public HXCONTROLHEADER
{
    float AirAttenuation;
    float WaterAttenuation;
    float FogHigh;
    float FogColor[3];
    float WaterColor[3];
    float SkyBright[3];
    float SunLight[3];
    float SunDir[3];
};

struct HXCONTROL_SCENECOMPOSITION : public HXCONTROLHEADER
{
    float Exposure;
    float WaterReflection;
    float WaterMixture;
};

struct HXCONTROL_EMITPARTICLE : public HXCONTROLHEADER
{
    unsigned int Type;
    unsigned int Life;
    double Pos[3];
    float Velo[3];
    float Color0[4];
    float Color1[4];
    float Radius0;
    float Radius1;
    float ColorTransitionStrength;
    float AlphaTransitionStrength;
    float RadiusTransitionStrength;
};

struct HXCONTROL_ONEWAVE : public HXCONTROLHEADER
{
    float WindDir;
    float WindSpeed;
    float WhiteCapStrength;
};

struct HXCONTROL_TORPEDOHIT : public HXCONTROLHEADER
{
    unsigned int TimeStamp;
    double Pos[3];
};

struct HXCONTROL_EXPLOSION : public HXCONTROLHEADER
{
    unsigned int TimeStamp;
    double Pos[3];
    unsigned int nParticleParFrame;
    float ExplosionRadius;
    float ParticleRadius;
    float Color0[4];
    float Color1[4];
};

struct HXCONTROL_ROCKETTRAIL : public HXCONTROLHEADER
{
    unsigned int iEntity;
    float ParticleRadius;
    float Speed;
    float Color[4];
};

struct HXCONTROL_CAVITATIONBUBBLE : public HXCONTROLHEADER
{
    unsigned int iEntity;
    float ParticleRadius;
    float Speed;
    float Color[4];
};

struct HXCONTROL_CAMERA : public HXCONTROLHEADER
{
    unsigned int iCamera;
    unsigned int iParent;
    float vFov;
    float Near;
    float Far;
};

struct HXCONTROL_CAMERAOFFSET : public HXCONTROLHEADER
{
    unsigned int iCamera;
    double Pos[3];
    float matRot[16];
};

struct HXCONTROL_OBJECTWATERCOUPLING_REGION : public HXCONTROLHEADER
{
    unsigned int BufferSize;
    double West;
    double South;
    double East;
    double North;
};

struct HXCONTROL_OBJECTWATERCOUPLING_ADDOBJECT : public HXCONTROLHEADER
{
    unsigned int AddRemove; // 0: add, 1: remove
    unsigned int iEntity;
};

#pragma pack(push)
#pragma pack(4)
struct HXCONTROL_GUIDELINE : public HXCONTROLHEADER
{
	unsigned int iEntityID;
	unsigned int bVisible;
	double Pos[3];
};

struct HXCONTROL_WEATHER : public HXCONTROLHEADER
{
	unsigned int nCountSnow;
	unsigned int bRain;
	float			fSnowSize;
	float			fRainWidth;
	float			fRainHeight;
	unsigned int nCloudCount;
	unsigned int bCloudVisible;
	unsigned int nCloudAltitude;
	unsigned int nCloudWidth;
	unsigned int nCloudHeight;
};

struct HXCONTROL_UNDERWATEREXPLOSION : public HXCONTROLHEADER
{
    float Progress;
    float Radius;
    float BaseColor[4];
    double Pos[3];
};


struct HXCONTROL_UNDERWATER_IMPURITY : public HXCONTROLHEADER
{
	unsigned int bImpurity;
};

struct HXCONTROL_SWELL : public HXCONTROLHEADER
{
	float fSwellDirection;
	float fSwellHeight;
	float fSwellLength;
	float fSwellFreq;
};

#pragma pack(pop)

}
