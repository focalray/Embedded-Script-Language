function OnEvent_CameraWalk_00
{
	$DriveObjectByPath "1", PATH_IDX_00, 120, 1, "", "OnEvent_CameraWalk_00_sub", ""; // Name, Path-Index, Sec, Gen-Fwd-Dir(Bool), Callback
}

function OnEvent_CameraWalk_00_sub
{
	$QueryEntityInfo "1";
	MAPPORT	PORT_ENTITYINFO, g_CameraEntryInfo;

	temp[0] = g_CameraEntryInfo.pos.x;
	temp[1] = g_CameraEntryInfo.pos.y;
	temp[2] = g_CameraEntryInfo.pos.z;
	temp[3] = g_CameraEntryInfo.ori.h;
	temp[4] = g_CameraEntryInfo.ori.p;
	temp[5] = g_CameraEntryInfo.ori.r;

	$SetCameraPosOri("1", temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
}


function OnEvent_CameraWalk_FlyToSubmarine
{
	// Name, Path-Index, Sec, Gen-Fwd-Dir(Bool), Callback
	$DriveObjectByPath "1", PATH_IDX_02, 
		6, // Duration
		0, // Gen-Fwd-Dir
		"SUB_TEST_INIT", "SUB_OnEvent_CameraWalk_FlyToSubmarine", "SUB_TEST_FINISH";
}
function SUB_OnEvent_CameraWalk_FlyToSubmarine
{
//	call OutputCameraInfo "SUB_OnEvent_CameraWalk_FlyToSubmarine";
}
function SUB_TEST_INIT
{
	// Lookat
	$SetCameraLookat "100";
}
function SUB_TEST_FINISH
{
	//tickstart = SimTickSec + 1;
	//$NewFrameEvent "OnEvent_Submarine_잠수함이동멈추기", tickstart, 180, "";
}


function OnEvent_Submarine_잠수함이동멈추기
{
	echoln "OnEvent_Submarine_잠수함이동멈추기";

	$QueryEntityInfo "100";
	MAPPORT	PORT_ENTITYINFO, g_SubmarineInfo;
	
	temp[0] = g_SubmarineInfo.pos.x;
	temp[1] = g_SubmarineInfo.pos.y;
	temp[2] = g_SubmarineInfo.pos.z;
	temp[3] = g_SubmarineInfo.ori.h;
	temp[4] = 0.0;
	temp[5] = g_SubmarineInfo.ori.r;

	double vel[3];
	vel[0] = save[0] * (1.0 - SimEventProgressSmooth);
	vel[1] = save[1] * (1.0 - SimEventProgressSmooth);
	vel[2] = save[2] * (1.0 - SimEventProgressSmooth);

	temp[0] += save[0];
	temp[1] += save[1];
	temp[2] += save[2];

	$SetEntityPosOri("100", temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
}


function OnEvent_좀더낮게
{
	temp[0] = g_CameraEntryInfo.pos.x;
	temp[1] = g_CameraEntryInfo.pos.y;
	temp[2] = g_CameraEntryInfo.pos.z + -50.0 * SimEventProgressSmooth;
	$SetCameraPos(temp[0], temp[1], temp[2]);
}
function OnEvent_좀더낮게_INIT
{
	$QueryEntityInfo "1";
	MAPPORT	PORT_ENTITYINFO, g_CameraEntryInfo;
	$SetCameraLookat("100");
}



function OnEvent_CloudChange
{
	$SetCloud(1500, TRUE, 1000, 1000, 1000);
}

function OnEvent_Snow_On
{
	$SetAirAttenuation(-0.001);
	g_AutoExposure = 0;
	$SetExposure(0.04);
	$SetCloud(3500, TRUE, 2000, 2000, 2000);
	$SetSnow(20000, 0.1);
}
function OnEvent_Snow_Off
{
	$SetAirAttenuation(-0.0002);
	g_AutoExposure = 1;
	$SetSnow(0, 0.1);
	$SetCloud(1500, TRUE, 1000, 1000, 1000);
}

function OnEvent_Rain_On
{
	$SetAirAttenuation(-0.005);
	g_AutoExposure = 0;
	$SetExposure(0.04);
	$SetCloud(3500, TRUE, 2000, 2000, 2000);
	$SetRain(20000, 0.02, 0.1);
}
function OnEvent_Rain_Off
{
	$SetAirAttenuation(-0.0002);
	g_AutoExposure = 1;
	$SetCloud(1500, TRUE, 1000, 1000, 1000);
	$SetRain(0, 0.03, 0.1);
}

function OnEvent_Submarine_잠수함이동
{
	echoln "OnEvent_Submarine_잠수함이동";
	$DriveObjectByPath "100", PATH_IDX_01, 60, 1, "", "OnEvent_Submarine_00_going", "";
}
function OnEvent_Submarine_00_going
{
	$QueryEntityInfo "100";
	MAPPORT	PORT_ENTITYINFO, g_SubmarineInfo;
	
	temp[0] = g_SubmarineInfo.pos.x;
	temp[1] = g_SubmarineInfo.pos.y;
	temp[2] = g_SubmarineInfo.pos.z;
	temp[3] = g_SubmarineInfo.ori.h;
	temp[4] = 0.0;
	temp[5] = g_SubmarineInfo.ori.r;

	save[0] = g_SubmarineInfo.vel.x;
	save[1] = g_SubmarineInfo.vel.y;
	save[2] = g_SubmarineInfo.vel.z;

	$SetEntityPosOri("100", temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
}

function OnEvent_Submarine_잠수함따라가기
{
	echoln "OnEvent_Submarine_잠수함따라가기";
	temp[0] = -80.0 + 30.0 * SimEventProgress;
	$SetCameraFollowOffset "100", 10.0, temp[0], 5.0, 0.0, 0.0, 0.0;
}
function OnEvent_Submarine_02
{
	echoln "OnEvent_Submarine_02";
	temp[0] = 90.0 - 50.0 * SimEventProgress;
	temp[1] = 20.0 - 10.0 * SimEventProgress;
	temp[2] = 6.0 + 30.0 * SimEventProgress;
	$SetCameraFollowOffset "100", temp[0], temp[1], temp[2], 0.0, 0.0, 0.0;

	save[0] = temp[0]; // save
	save[1] = temp[1]; // save
	save[2] = temp[2]; // save
}

function OnEvent_Submarine_좀더높이_초기화
{
	echoln "OnEvent_Submarine_좀더높이_초기화";

	$SetCameraLookat "100";

	$QueryEntityInfo "100";
	MAPPORT	PORT_ENTITYINFO, g_SubmarineInfo;
}
function OnEvent_Submarine_좀더높이
{
	echoln "OnEvent_Submarine_좀더높이";

	temp[0] = -90.0 * SimEventProgress;
	temp[1] = 0.0;
	temp[2] = 50.0 * SimEventProgress;
	$SetCameraFollowOffset "100", temp[0], temp[1], temp[2], 0.0, 0.0, 0.0;
	//sleep 100;
}

function OnEvent_Submarine_멀리보이게낮게_초기화
{
	echoln "OnEvent_Submarine_멀리보이게낮게_초기화";

	$QueryEntityInfo "1";
	MAPPORT	PORT_ENTITYINFO, g_CameraEntryInfo;
}
function OnEvent_Submarine_멀리보이게낮게
{
	$SetCameraLookat ""; // Lookat off

	temp[4] = g_CameraEntryInfo.ori.h;
	temp[5] = g_CameraEntryInfo.ori.p + 37.0 * SimEventProgressSmooth;
	temp[6] = g_CameraEntryInfo.ori.r;

	temp[0] = 0.0;
	temp[1] = 0.0;
	temp[2] = g_CameraEntryInfo.pos.z + 10.0 * SimEventProgressSmooth;

	//temp[2] = g_CameraEntryInfo.pos.z - 5.0 * SimEventProgressSmooth;  *********** 마이너스 부호 문법 처리 오류!!

	$SetCameraFollowOffset "100", temp[0], temp[1], temp[2], temp[4], temp[5], temp[6];

	save[0] = temp[0]; // save
	save[1] = temp[1]; // save
	save[2] = temp[2]; // save
}


function OnEvent_카메라_잠수함옆으로
{
	echoln "OnEvent_카메라_잠수함옆으로";
	Sleep 2000;

	temp[0] = 100.0;
	temp[1] = 0.0;
	temp[2] = 25.0;

	g_ActiveCamera = 1;
	$SetCameraLookat("100");
	$SetCameraFollowOffset("100", temp[0], temp[1], temp[2], 0.0, 0.0, 0.0);
}


function OnEvent_카메라_미사일보게멀리
{
	echoln "OnEvent_카메라_미사일보게멀리";

	temp[0] = 100.0 + 300.0 * SimEventProgressSmooth;
	temp[1] = 0.0;
	temp[2] = 25.0 + 25.0 * SimEventProgressSmooth;;

	$SetCameraLookat("100");
	$SetCameraFollowOffset("100", temp[0], temp[1], temp[2], 0.0, 0.0, 0.0);
}

function OnEvent_카메라_미사일Lookat
{
	echoln "OnEvent_카메라_미사일Lookat";

	$StopCameraFollow;
	$SetCameraLookat("300");
}

function OnEvent_카메라_미사일따라가기
{
	echoln "OnEvent_카메라_미사일따라가기";

	$SetCameraLookat("300");
	$SetCameraFollowOffset("300", 100.0, -100.0, 50.0, 0.0, 0.0, 0.0);
}

function OnEvent_카메라_미사일따라가기_2
{
	echoln "OnEvent_카메라_미사일따라가기_2";

	temp[0] = 100.0 + 100.0 * SimEventProgressSmooth;
	temp[1] = -100.0 - 100.0 * SimEventProgressSmooth;
	temp[2] = 50.0 + 50.0 * SimEventProgressSmooth;
	$SetCameraLookat("300");
	$SetCameraFollowOffset("300", temp[0], temp[1], temp[2], 0.0, 0.0, 0.0);
}

function OnEvent_카메라_잠수함수중정면
{
	echoln "OnEvent_카메라_잠수함수중정면";
	$SetCameraLookat("100");
	$SetCameraFollowOffset("100", -5.0, 50.0, 0.0, 0.0, 0.0, 0.0);
}

function OnEvent_카메라_잠수함수중정면_멀리
{
	echoln "OnEvent_카메라_잠수함수중정면_멀리";

	$SetCameraLookat("100");
	$SetCameraFollowOffset("100", -15.0, 150.0, 0.0, 0.0, 0.0, 0.0);
}

function OnEvent_카메라_나진호에서미사일보기
{
	echoln "OnEvent_카메라_나진호에서미사일보기";

	$QueryEntityInfo "403";
	MAPPORT	PORT_ENTITYINFO, svEntityInfo;

	temp[0] = svEntityInfo.pos.x;
	temp[1] = svEntityInfo.pos.y;
	temp[2] = svEntityInfo.pos.z + 30.0;

	$StopCameraFollow;
	$SetCameraLookat("300");
	$SetCameraPosLookat(temp[0], temp[1], temp[2], "300");
}


function OnEvent_Missile
{
	echoln "미사일 발사!";
	$QueryEntityInfo "100";
	MAPPORT	PORT_ENTITYINFO, g_SubmarineInfo;

	$RunDynamics("Harpoon");
}

function OnEvent_Torpedo
{
	echoln "어뢰 발사!";
	$QueryEntityInfo "100";
	MAPPORT	PORT_ENTITYINFO, g_SubmarineInfo;
	$RunDynamics("Torpedo1");
}

function OnEvent_어뢰따라가기_1
{
	echoln "OnEvent_어뢰따라가기_1";
	$SetCameraLookat("5000");
	$SetCameraFollowOffset("5000", 20.0, 20.0, 1.0, 0.0, 0.0, 0.0);
}

function OnEvent_어뢰따라가기_2
{
	echoln "OnEvent_어뢰따라가기_2";

	temp[0] = 20.0 - 19.0 * SimEventProgressSmooth;
	temp[1] = 20.0 - 19.0 * SimEventProgressSmooth;
	temp[2] = 1.0 + 20.0 * SimEventProgressSmooth;

	$SetCameraLookat("5000");
	$SetCameraFollowOffset("5000", temp[0], temp[1], temp[2], 0.0, 0.0, 0.0);
}

function OnEvent_어뢰따라가기_3
{
	echoln "OnEvent_어뢰따라가기_3";

	temp[0] = 1.0 + 19.0 * SimEventProgressSmooth;
	temp[1] = 1.0 - 50.0 * SimEventProgressSmooth;
	temp[2] = 21.0 - 20.0 * SimEventProgressSmooth;

	$SetCameraLookat("5000");
	$SetCameraFollowOffset("5000", temp[0], temp[1], temp[2], 0.0, 0.0, 0.0);
}

function OnEvent_어뢰따라가기_4
{
	echoln "OnEvent_어뢰따라가기_4";

	temp[0] = 20.0 - 10.0 * SimEventProgressSmooth;
	temp[1] = -49.0 - 20.0 * SimEventProgressSmooth;
	temp[2] = 1.0 + 20.0 * SimEventProgressSmooth;

	$SetCameraLookat("5000");
	$SetCameraFollowOffset("5000", temp[0], temp[1], temp[2], 0.0, 20.0, 0.0);
}


function OnEvent_어뢰속도가속
{
	echoln "OnEvent_어뢰속도가속";
	g_TorpedoSpeed = 35.0 + 180.0 * SimEventProgressSmooth;
//	g_TorpedoSpeed = 35.0 + 580.0 * SimEventProgressSmooth;
}

function OnEvent_어뢰속도감속
{
	echoln "OnEvent_어뢰속도감속";
//	sleep 10;
	g_TorpedoSpeed = 215.0 - 180.0 * SimEventProgressSmooth;
//	g_TorpedoSpeed = 615.0 - 580.0 * SimEventProgressSmooth;
}

function OnEvent_카메라_나진호에서어뢰보기
{
	echoln "OnEvent_카메라_나진호에서어뢰보기";
	//sleep 1;

	$QueryEntityInfo "403";
	MAPPORT	PORT_ENTITYINFO, svEntityInfo;

	temp[0] = svEntityInfo.pos.x;
	temp[1] = svEntityInfo.pos.y;
	temp[2] = svEntityInfo.pos.z + 30.0;

	$StopCameraFollow;
	$SetCameraLookat("5000");
	$SetCameraPosLookat(temp[0], temp[1], temp[2], "5000");
}


function OnEvent_카메라_나진2호옆에서_옆구리터지는것보기
{
	echoln "OnEvent_카메라_나진2호옆에서_옆구리터지는것보기";
	Sleep 2000;

	temp[0] = 500.0;
	temp[1] = 0.0;
	temp[2] = 25.0;

	g_ActiveCamera = 1;
	$SetCameraLookat("403");
	$SetCameraFollowOffset("403", temp[0], temp[1], temp[2], 0.0, 0.0, 0.0);
}






function OnEvent_카메라_잠수함옆으로_잠망경올라가는것보기
{
	echoln "OnEvent_카메라_잠수함옆으로_잠망경올라가는것보기";
	temp[0] = 100.0 - 30.0 * SimEventProgress;
	temp[1] = 0.0;
	temp[2] = -5.0;

	g_ActiveCamera = 1;
	$SetCameraLookat("100");
	$SetCameraFollowOffset("100", temp[0], temp[1], temp[2], 0.0, 0.0, 0.0);
}


function OnEvent_Submarine_폭발관찰잠수함시점
{
	echoln "OnEvent_Submarine_폭발관찰잠수함시점";

	g_ActiveCamera = 1;
//	temp[0] = -90.0 * SimEventProgress;
//	temp[1] = 0.0;
//	temp[2] = 50.0 * SimEventProgress;
//	$SetCameraFollowOffset "100", temp[0], temp[1], temp[2], 0.0, 0.0, 0.0;
}




function OnEvent_WaterAttenuation_Transparent
{
	temp[0] = -0.03 + 0.028 * SimEventProgress;
	$SetWaterAttenuation(temp[0]);
}
function OnEvent_WaterAttenuation_Normal
{
	temp[0] = -0.001 + -0.029 * SimEventProgress;
	$SetWaterAttenuation(temp[0]);
}
function OnEvent_WaterAttenuation_Brighter
{
	temp[0] = -0.03 + 0.015 * SimEventProgress;
	$SetWaterAttenuation(temp[0]);
}

function OnEvent_TOD_Slideto_Night
{
	temp[0] = 0.7 * SimEventProgress;
	$SetTOD(temp[0]);
}
function OnEvent_TOD_Slideto_Daylight
{
	temp[0] = 0.7 - 0.7 * SimEventProgress;
	$SetTOD(temp[0]);
}
function OnEvent_FogChange800TO400
{
	temp[0] = 800.0 - 400.0 * SimEventProgressSmooth;
	$SetFogAlt(temp[0]);
}
function OnEvent_FogChange
{
	temp[0] = 400.0 - 300.0 * SimEventProgressSmoothReturn;
	$SetFogAlt(temp[0]);
}
function OnEvent_FogChange_Init
{
	// cloud off when fog change works.
	$SetCloud(1500, FALSE, 1000, 1000, 1000);
}


//////////////////////////////////////////// 충돌 ////////////////////////////////////////

function Harpoon_Collision
{
	echoln "Harpoon_Collision";

	$SetEntityLiveState("401", 0);

	sleep 2000;
}

function Torpedo1_Collision
{
	echoln "Torpedo1_Collision";

	// 나진2호 폭발에 의한 운항 중단.
	$SetEntityLiveState("403", 0);

	sleep 2000;
}
function Torpedo2_Collision
{
	echoln "Torpedo2_Collision";
	sleep 2000;
}
function Torpedo3_Collision
{
	echoln "Torpedo3_Collision";
	sleep 2000;
	tickstart = SimTickSec + 1;
	$newframeevent "OnEvent_적잠수함폭발", tickstart, 60, "";
	$newevent "OnEvent_적잠수함박살", tickstart, 1;
}

function OnEvent_적잠수함박살
{
	$SetEntityModel("4100", "211");
}

function OnEvent_적잠수함폭발
{
	echoln "OnEvent_적잠수함폭발";

	$QueryEntityInfo "4100";
	MAPPORT	PORT_ENTITYINFO, g_EnemySubTorpedo1;

	temp[0] = g_EnemySubTorpedo1.pos.x;
	temp[1] = g_EnemySubTorpedo1.pos.y;
	temp[2] = g_EnemySubTorpedo1.pos.z;

	$SetUnderWaterExplosion(temp[0], temp[1], temp[2], SimEventProgress);
}

function Torpedo4_Collision
{
	echoln "Torpedo4_Collision";
	sleep 2000;
}
function Torpedo5_Collision
{
	echoln "Torpedo5_Collision";
	sleep 2000;
}
function Torpedo6_Collision
{
	echoln "Torpedo6_Collision";
	sleep 2000;
}
function Torpedo7_Collision
{
	echoln "Torpedo7_Collision";
	sleep 2000;
}
function Torpedo8_Collision
{
	echoln "Torpedo8_Collision";
	sleep 2000;
}
function Torpedo9_Collision
{
	echoln "Torpedo9_Collision";
	sleep 2000;

	tickstart = SimTickSec + 1;
	$newframeevent "OnEvent_기만기어뢰충돌폭발", tickstart, 60, "";
}
function Torpedo10_Collision
{
	echoln "Torpedo10_Collision";
	sleep 2000;
}


function OnEvent_기만기어뢰충돌폭발
{
	echoln "OnEvent_기만기어뢰충돌폭발";

	$QueryEntityInfo "5008";
	MAPPORT	PORT_ENTITYINFO, g_EnemySubTorpedo1;

	temp[0] = g_EnemySubTorpedo1.pos.x;
	temp[1] = g_EnemySubTorpedo1.pos.y;
	temp[2] = g_EnemySubTorpedo1.pos.z;

	$SetUnderWaterExplosion(temp[0], temp[1], temp[2], SimEventProgress);
}



////////////////////////////////// 적잠수함 1호 ///////////////////////////////////
function OnEvent_적잠수함1이동
{
	echoln "OnEvent_적잠수함1이동";
	$DriveObjectByPath "4100", PATH_IDX_03, 120, 1, "", "OnEvent_적잠수함1이동_sub", "";
}
function OnEvent_적잠수함1이동_sub
{
	$QueryEntityInfo "4100";
	MAPPORT	PORT_ENTITYINFO, g_EnemySubmarine1Info;
	
	temp[0] = g_EnemySubmarine1Info.pos.x;
	temp[1] = g_EnemySubmarine1Info.pos.y;
	temp[2] = g_EnemySubmarine1Info.pos.z;
	temp[3] = g_EnemySubmarine1Info.ori.h;
	temp[4] = 0.0;
	temp[5] = g_EnemySubmarine1Info.ori.r;


//	echoln "적잠수함위치:"
//	echoln g_EnemySubmarine1Info.pos.x;
//	echoln g_EnemySubmarine1Info.pos.y;
//	echoln g_EnemySubmarine1Info.pos.z;

	$SetEntityPosOri("4100", temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
}

function OnEvent_적잠수함1이동_따라가기
{
	echoln "OnEvent_적잠수함1이동_따라가기";
	temp[0] = -80.0 + 30.0 * SimEventProgress;
	$SetCameraFollowOffset "4100", 10.0, temp[0], 5.0, 0.0, 0.0, 0.0;
}

function OnEvent_적잠수함1이동_따라가기2
{
	echoln "OnEvent_적잠수함1이동_따라가기2";
	temp[0] = -50.0 - 20.0 * SimEventProgress;
	$SetCameraFollowOffset "4100", 10.0, temp[0], 5.0, 0.0, 0.0, 0.0;
}

function OnEvent_카메라_적잠수함1호_수중정면
{
	echoln "OnEvent_카메라_적잠수함1호_수중정면";

	$SetCameraLookat("4100");
	$SetCameraFollowOffset("4100", -5.0, 60.0, 0.0);
}


function OnEvent_적잠수함1호_어뢰발사
{
	echoln "OnEvent_적잠수함1호_어뢰발사!";

	$QueryEntityInfo "4100";
	MAPPORT	PORT_ENTITYINFO, g_EnemySubmarine1Info;

	g_TorpedoSpeed = 35.0;

	$RunDynamics("Torpedo9");

	tickstart = SimTickSec + 3;
	$newevent "OnEvent_적잠수함1호_어뢰발사_가속", tickstart, 1;
}
function OnEvent_적잠수함1호_어뢰발사_가속
{
	echoln "OnEvent_적잠수함1호_어뢰발사_가속";
	sleep 2000;
	g_TorpedoSpeed = 200.0;
}

function OnEvent_적잠수함1호_어뢰발사_감속
{
	echoln "OnEvent_적잠수함1호_어뢰발사_감속";
	g_TorpedoSpeed = 150.0 - 115.0 * SimEventProgressSmooth;
}



function OnEvent_적잠수함1이쏜_어뢰따라가기
{
	echoln "OnEvent_적잠수함1이쏜_어뢰따라가기";

	temp[0] = 20.0 - 10.0 * SimEventProgressSmooth;
	temp[1] = -49.0 - 20.0 * SimEventProgressSmooth;
	temp[2] = 1.0 + 15.0 * SimEventProgressSmooth;

	$SetCameraLookat("5008");
	$SetCameraFollowOffset("5008", temp[0], temp[1], temp[2], 0.0, 0.0, 0.0);
}

function OnEvent_적잠수함1이쏜_어뢰따라가기_수중
{
	echoln "OnEvent_적잠수함1이쏜_어뢰따라가기_수중";

//	$QueryEntityInfo "5008";
//	MAPPORT	PORT_ENTITYINFO, g_EnemySubTorpedo1;
//	echoln "g_EnemySubTorpedo1";
//	echo "x = "; echoln g_EnemySubTorpedo1.pos.x;
//	echo "y = "; echoln g_EnemySubTorpedo1.pos.y;
//	echo "z = "; echoln g_EnemySubTorpedo1.pos.z;

	temp[0] = -30.0;
	temp[1] = -50.0;
	temp[2] = 0.0;

	$SetCameraLookat("5008");
	$SetCameraFollowOffset("5008", temp[0], temp[1], temp[2], 0.0, 0.0, 0.0);
}


function OnEvent_잠수함이쏜_어뢰따라가기_수중
{
	echoln "OnEvent_잠수함이쏜_어뢰따라가기_수중";

	temp[0] = -30.0;
	temp[1] = -150.0;
	temp[2] = 1.0;

	$SetCameraLookat("5002");
	$SetCameraFollowOffset("5002", temp[0], temp[1], temp[2], 0.0, 0.0, 0.0);
}



function OnEvent_적잠수함1이쏜_어뢰따라가기_앞에서뒤로보기
{
	echoln "OnEvent_적잠수함1이쏜_어뢰따라가기_앞에서뒤로보기";

	temp[0] = 10.0;
	temp[1] = -69.0 + 120.0 * SimEventProgressSmooth;
	temp[2] = 16.0 - 15.0 * SimEventProgressSmooth;

	$SetCameraLookat("5008");
	$SetCameraFollowOffset("5008", temp[0], temp[1], temp[2], 0.0, 0.0, 0.0);
}

function OnEvent_적잠수함1이쏜_어뢰따라가기_좀더높이
{
	echoln "OnEvent_적잠수함1이쏜_어뢰따라가기_좀더높이";

	temp[0] = 10.0;
	temp[1] = -69.0;
	temp[2] = 16.0 - 5.0 * SimEventProgressSmooth;

	$SetCameraLookat("5008");
	$SetCameraFollowOffset("5008", temp[0], temp[1], temp[2], 0.0, 0.0, 0.0);
}



////////////////////////////////// 적잠수함 2호 ///////////////////////////////////
function OnEvent_적잠수함2이동
{
	echoln "OnEvent_적잠수함2이동";
	$DriveObjectByPath "4200", PATH_IDX_04, 160, 1, "", "OnEvent_적잠수함2이동_sub", "";
}
function OnEvent_적잠수함2이동_sub
{
	$QueryEntityInfo "4200";
//	MAPPORT	PORT_ENTITYINFO, g_EnemySubmarine1Info;
	MAPPORT	PORT_ENTITYINFO, g_EnemySubmarine2Info;
	
//	echoln "적잠수함위치:"
//	echoln g_EnemySubmarineInfo[1].pos.x;  // 스크립트 오류발생!
//	echoln g_EnemySubmarineInfo[1].pos.y;
//	echoln g_EnemySubmarineInfo[1].pos.z;

//	echoln "적잠수함위치:"
//	echoln g_EnemySubmarine2Info.pos.x;
//	echoln g_EnemySubmarine2Info.pos.y;
//	echoln g_EnemySubmarine2Info.pos.z;

	temp[0] = g_EnemySubmarine2Info.pos.x;
	temp[1] = g_EnemySubmarine2Info.pos.y;
	temp[2] = g_EnemySubmarine2Info.pos.z;
	temp[3] = g_EnemySubmarine2Info.ori.h;
	temp[4] = 0.0;
	temp[5] = g_EnemySubmarine2Info.ori.r;

	$SetEntityPosOri("4200", temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
}

function OnEvent_적잠수함2이동_따라가기
{
	echoln "OnEvent_적잠수함2이동_따라가기";
	temp[0] = 80.0 - 30.0 * SimEventProgress;
	$SetCameraFollowOffset "4200", 10.0, temp[0], 5.0, 0.0, 0.0, 0.0;
}

function OnEvent_카메라_적잠수함2호_수중정면
{
	echoln "OnEvent_카메라_적잠수함1호_수중정면";

	$SetCameraLookat("4200");
	$SetCameraFollowOffset("4200", -5.0, 60.0, 0.0, 0.0, 0.0, 0.0);
}



////////////////////////////////// 나진 2호 이동 ///////////////////////////////////
function OnEvent_나진2호이동
{
	echoln "OnEvent_나진2호이동";
	$DriveObjectByPath "403", PATH_IDX_05, 280, 1, "", "OnEvent_나진2호이동_sub", "";
}
function OnEvent_나진2호이동_sub
{
	$QueryEntityInfo "403";
	MAPPORT	PORT_ENTITYINFO, svEntityInfo;
	
	temp[0] = svEntityInfo.pos.x;
	temp[1] = svEntityInfo.pos.y;
	temp[2] = svEntityInfo.pos.z;
	temp[3] = svEntityInfo.ori.h;
	temp[4] = 0.0;
	temp[5] = svEntityInfo.ori.r;

	$SetEntityPosOri("403", temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
}


////////////////////////////////// 잠수함: 적어뢰근접 ///////////////////////////////////
function OnEvent_적어뢰근접 
{
	echoln "OnEvent_적어뢰근접:  500m 이하";
	sleep 2000;

	tickstart = SimTickSec+1;
	$newevent "OnEvent_Submarine_잠수함회피기동", tickstart, 1;

	echoln "적어뢰 가속 중단: 35노트 복원";
	g_TorpedoSpeed = 35.0;

	$newframeevent "OnEvent_WaterAttenuation_Transparent", tickstart, 120, "";
	//$newframeevent "OnEvent_WaterAttenuation_Brighter", tickstart, 120, "";

	// 잠수함 위치
	$QueryEntityInfo "100";
	MAPPORT	PORT_ENTITYINFO, g_SubmarineInfo;
	temp[0] = g_SubmarineInfo.pos.x + 50;
	temp[1] = g_SubmarineInfo.pos.y + 90;
	temp[2] = g_SubmarineInfo.pos.z + 0;

	$StopCameraFollow;
	$SetCameraLookat "100";
	$SetCameraPos(temp[0], temp[1], temp[2]);

	// 기만기발사!
	$RunDynamics("Decoy1");
	g_DecoyFired = 1;

	echoln "기만기발사!";
	sleep 2000;
}
function OnEvent_Submarine_잠수함회피기동
{
	echoln "OnEvent_Submarine_잠수함회피기동!";

	$DriveObjectByPath "100", PATH_IDX_06, 100, 1, "", "OnEvent_Submarine_잠수함회피기동_sub", "";
	$ResetPathDriving("100");
}
function OnEvent_Submarine_잠수함회피기동_sub
{
	$SetCameraLookat "100";

	$QueryEntityInfo "100";
	MAPPORT	PORT_ENTITYINFO, g_SubmarineInfo;
	
	temp[0] = g_SubmarineInfo.pos.x;
	temp[1] = g_SubmarineInfo.pos.y;
	temp[2] = g_SubmarineInfo.pos.z;
	temp[3] = g_SubmarineInfo.ori.h;
	temp[4] = 0.0;//g_SubmarineInfo.ori.p;
	temp[5] = g_SubmarineInfo.ori.r;

	$SetEntityPosOri("100", temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function OnEvent_Torpedo_적잠수함1_향해발싸아
{
	echoln "OnEvent_Torpedo_적잠수함1_향해발싸아";
	$QueryEntityInfo "100";
	MAPPORT	PORT_ENTITYINFO, g_SubmarineInfo;
	g_TorpedoSpeed = 70.0;
	$RunDynamics("Torpedo3");
}

function OnEvent_Torpedo_적잠수함2_향해발싸아
{
	echoln "OnEvent_Torpedo_적잠수함2_향해발싸아";
	$QueryEntityInfo "100";
	MAPPORT	PORT_ENTITYINFO, g_SubmarineInfo;
	g_TorpedoSpeed = 70.0;
	$RunDynamics("Torpedo4");
}

function OnEvent_어뢰따라가기_1
{
	echoln "OnEvent_어뢰따라가기_1";
	$SetCameraLookat("5000");
	$SetCameraFollowOffset("5000", 20.0, 20.0, 1.0, 0.0, 0.0, 0.0);
}


function OnEvent_적잠수함1이쏜_어뢰따라가기_수중
{
	echoln "OnEvent_적잠수함1이쏜_어뢰따라가기_수중";

	temp[0] = -30.0;
	temp[1] = -50.0;
	temp[2] = 0.0;

	$SetCameraLookat("5002");
	$SetCameraFollowOffset("5002", temp[0], temp[1], temp[2], 0.0, 0.0, 0.0);
}



function OnEvent_카메라_어뢰따라가기_2
{
	echoln "OnEvent_카메라_어뢰따라가기_2";

	$SetCameraLookat("5002");
	$SetCameraFollowOffset("5002", -20.0, -70.0, 1.0, 0.0, 0.0, 0.0);
}


function OnEvent_적잠수함1호관찰
{
	$SetCameraLookat("4100");
	$SetCameraFollowOffset("4100", -20.0, 50.0, 1.0, 0.0, 0.0, 0.0);
}

function OnEvent_적잠수함2호관찰
{
	$SetCameraLookat("4200");
	$SetCameraFollowOffset("4200", 20.0, -50.0, 5.0, 0.0, 0.0, 0.0);
}


function OnEvent_잠수함부상관찰
{
	$SetSubmarineWaterCouplingRegion("100", 700.0);

	echoln "OnEvent_잠수함부상관찰";
	$SetCameraLookat("100");
	$SetCameraFollowOffset("100", 50.0, 50.0, 7.0, 0.0, 0.0, 0.0);
}



function OnEvent_카메라_높게이동_초기화
{
	echoln "OnEvent_카메라_높게이동_초기화";

	$SetCameraLookat "100";
	$QueryEntityInfo "100";
	MAPPORT	PORT_ENTITYINFO, g_SubmarineInfo;
}
function OnEvent_카메라_높게이동
{
	echoln "OnEvent_카메라_높게이동";

	temp[0] = -90.0 * SimEventProgress;
	temp[1] = 0.0;
	temp[2] = 150.0 * SimEventProgress;
	$SetCameraFollowOffset "100", temp[0], temp[1], temp[2], 0.0, 0.0, 0.0;
}



function OnEvent_어뢰속도가속2
{
	echoln "OnEvent_어뢰속도가속2";
	g_TorpedoSpeed = 35.0 + 250.0 * SimEventProgressSmooth;
}

function OnEvent_어뢰속도감속2
{
	echoln "OnEvent_어뢰속도감속2";
	g_TorpedoSpeed = 285.0 - 250.0 * SimEventProgressSmooth;
}

