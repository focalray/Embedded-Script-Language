

#include "Def.s"

function main
{
	// SimTime, SimFreq, SimTickSec, SimTick은 
	// 호스트에서 생성되어 스크립트와 바인딩되어있는 변수들임.
	// SimTickSec은 초단위로 업데이트되며, SimTick은 주파수의 속도로
	// Tick값이 업데이트된다.  60Hz인경우 하나의 SimTick은 60분의 1초이다.

	SimTime = 450;	// 분
	SimFreq = 60; // 60Hz


	// 엔티티의 이동 궤적을 정의한 PATH는 총 100개(0~99)를 사전에 정의 가능.
	// Prerequisites all paths for moving entities!
	call Setup_All_Paths;	// Use 'CALL' to call script functions

	// Use prefix '$' to call Host-internal functions
	// 주의: 호스트내부함수-콜은 인자 타입이 직접적으로 매핑되므로(형변환 없이) 
	//      호스트내부함수의 인자가 정수면 정수표현으로 보내야하고,
	//      더블형이면 1.0 과 같은 식으로 명확히 보내야 함.


	// $NewEvent Parameters: <Event-Name, Period in Sec(must be an integer), Repetition(-1 for INFINITE)>


//////////////////////////////////////////////////////////////////////
// 시뮬레이션 초기화
//////////////////////////////////////////////////////////////////////

	//*** 시뮬레이션 시간 0초에서부터 매 초 호출됨
	$NewEvent "OnEventPeriodicTimer", 0, -1;
	$NewFrameEvent "OnEventPeriodicTimerTick", 0, -1, "";


	// 카메라 초기화
	$NewEvent "OnEvent_Init", 0, 1;


//////////////////////////////////////////////////////////////////////
// 시뮬레이션 시간대별 장면 정의
//////////////////////////////////////////////////////////////////////


	int TS = 0;

////////////////////////// 시작부터 3초간 Fade-In
	$NewFrameEvent "OnEvent_FadeIn", 0, 180, "";
	
	$NewFrameEvent "OnEvent_FogChange800TO400", 0, 180, "";


////////////////////////// 카메라 워킹 지상에서-잠수함으로(5초간)
	$NewEvent "OnEvent_CameraWalk_FlyToSubmarine", 0, 1;

	//goto start;


////////////////////////// 잠수함이 항만에서 바다 방향으로 1분간 이동
	$NewEvent "OnEvent_Submarine_잠수함이동", 2, 1;


	TS = 15;
	$Newframeevent "OnEvent_좀더낮게", TS, 120, "OnEvent_좀더낮게_INIT";
	TS += 3;
	$newevent "OnEvent_고정관찰카메라_잠수함", TS, 1;							// Play: 잠수함물속으로
	TS += 1;

////////////////////////// 환경 효과 제어

	$newframeevent "OnEvent_TOD_Slideto_Night", TS, 120, "";
	TS += 2;
	$newframeevent "OnEvent_TOD_Slideto_Daylight", TS, 120, "";
	TS += 3;
//	$newframeevent "OnEvent_FogChange",	TS, 240, "OnEvent_FogChange_Init";
	TS += 5;
//	$newevent "OnEvent_CloudChange", TS, 1;
	$newframeevent "OnEvent_FogChange",	TS, 240, "OnEvent_FogChange_Init";
	TS += 4;
	$NewEvent "OnEvent_Snow_On", TS, 1;
	TS += 3;
	$NewEvent "OnEvent_Snow_Off", TS, 1;
	TS += 3;
	$NewEvent "OnEvent_Rain_On", TS, 1;							// Play: 비오는소리
	TS += 3;
	$NewEvent "OnEvent_Rain_Off", TS, 1;						// Stop: 비오는소리
	TS += 3;


////////////////////////// 수중 잠수함 관련 효과 제어
	$NewFrameEvent "OnEvent_Submarine_잠수함따라가기", TS, 600, "";					// Stop: 잠수함물속으로, Play: 프로펠라
	TS += 10;
	$NewFrameEvent "OnEvent_Submarine_02", TS, 480, "";					// 물 밖으로 이동			// Stop: 프로펠라
	$newframeevent "OnEvent_WaterAttenuation_Brighter", TS, 180, "";
	TS += 9;
	$NewFrameEvent "OnEvent_Submarine_멀리보이게낮게", TS, 120, "OnEvent_Submarine_멀리보이게낮게_초기화";
	TS += 3;
	$newevent "OnEvent_SeaState_L5", TS, 1;
	TS += 5;
	$newevent "OnEvent_SeaState_L1", TS, 1;
	TS += 1;
	$NewFrameEvent "OnEvent_Submarine_좀더높이", TS, 180, "OnEvent_Submarine_좀더높이_초기화";  // 이 장면이 필요한가?????
	TS += 5;


//=== 잠망경 액션 ================================================================
	$newframeevent "OnEvent_카메라_잠수함옆으로_잠망경올라가는것보기", TS, 180;			
	TS += 1;
	$NewFrameEvent "OnEvent_PeriscopeUp", TS, 600, "";				// Play: 잠망경업다운

	$NewEvent "OnEvent_나진2호이동", TS, 1;							// Stop: 잠망경업다운

	TS += 2;
	$NewEvent "OnEvent_PeriscopeView", TS, 1;						
	TS += 8;
	$NewFrameEvent "OnEvent_잠망경_목표물탐색왼쪽으로", TS, 240, "";		// Play: 잠망경탐색
	TS += 4;															// Stop: 잠망경탐색
	$NewFrameEvent "OnEvent_잠망경_목표물탐색오른쪽으로", TS, 120, "";		// Play: 잠망경탐색
	TS += 3;															// Stop: 잠망경탐색
	$NewFrameEvent "OnEvent_PeriscopeZoomIn_적함포착_확대", TS, 180, "";	// Play: 잠망경탐색
	TS += 5;															// Stop: 잠망경탐색
	$newframeevent "OnEvent_잠망경_적함포착후_약간오른쪽으로", TS, 120, "";   // Play: 잠망경탐색
	TS += 4;															   // Stop: 잠망경탐색
	
//=== 미사일(하푼) 발사 ===========================================================
	$NewEvent "OnEvent_카메라_잠수함옆으로", TS, 1;
	$NewFrameEvent "OnEvent_PeriscopeDown", TS, 300, "";
	TS += 3;
	$newevent "OnEvent_Missile", TS, 1;		// 미사일 발사
	$newframeevent "OnEvent_카메라_미사일보게멀리", TS, 180, "";
	TS += 3;
	$newevent "OnEvent_카메라_미사일Lookat", TS, 1;
	TS += 11;
	//$newevent "OnEvent_카메라_미사일따라가기", TS, 1;
	$newframeevent "OnEvent_카메라_미사일따라가기", TS, 120, ""; //부드럽게
	TS += 5;
	$newframeevent "OnEvent_카메라_미사일따라가기_2", TS, 180, "";
	TS += 3;
	$newevent "OnEvent_카메라_나진호에서미사일보기", TS, 1;
	TS += 5;
	$newevent "OnEvent_카메라_미사일따라가기_2", TS, 1;


	//*** Play: 하푼 선상 폭발음

//=== 어뢰 발사 ==================================================================
	TS = 150;
	$newevent "OnEvent_카메라_잠수함수중정면", TS, 1;
	TS += 2;
	$newframeevent "OnEvent_1번해치열기", TS, 300, "";			// Play: 해치열기
	TS += 2;

	$newevent "OnEvent_Torpedo", TS, 1;		// 어뢰 발사			// Play: 어뢰진행음
	TS += 3;
	$newevent "OnEvent_카메라_잠수함수중정면_멀리", TS, 1;
	$newframeevent "OnEvent_1번해치닫기", TS, 300, "";
	TS += 3;
	$newevent "OnEvent_어뢰따라가기_1", TS, 1;
	TS += 5;
	$newframeevent "OnEvent_어뢰따라가기_2", TS, 300, "";
	TS += 5;
	$newframeevent "OnEvent_어뢰따라가기_3", TS, 300, "";
	TS += 9;
	$newframeevent "OnEvent_어뢰속도가속", TS, 300, "";
	TS += 1;
	$newframeevent "OnEvent_어뢰따라가기_4", TS, 600, "";
	TS += 28;
	$newframeevent "OnEvent_어뢰속도감속", TS, 150, "";
	TS += 5;
	$newframeevent "OnEvent_카메라_나진호에서어뢰보기", TS, 420, "";
	TS += 8;
	//$newframeevent "OnEvent_어뢰따라가기_4", TS, 300, "";


	////////////////////////////////////
	/////// 어뢰 명중 ///////////////////
	////////////////////////////////////

	//*** Play: 적함 폭발음


	//TS = 224;
	$newevent "OnEvent_카메라_나진2호옆에서_옆구리터지는것보기", TS, 1;
	TS += 3;
	$newevent "OnEvent_잠망경_적함폭발관찰", TS, 1;
	TS += 3;
	$newframeevent "OnEvent_Submarine_폭발관찰잠수함시점", TS, 300, "";
	TS += 5;



	$NewEvent "OnEvent_적잠수함1이동", TS, 1;	
	$NewEvent "OnEvent_적잠수함2이동", TS, 1;	
	$NewFrameEvent "OnEvent_적잠수함1이동_따라가기", TS, 300, "";
	TS += 5;
	$newevent "OnEvent_적잠수함1호_어뢰발사", TS, 1;		// 어뢰 발사 조금 후 가속함. (Chained-event)		// Play: 어뢰진행음	
	TS += 2;
	$newframeevent "OnEvent_적잠수함1이쏜_어뢰따라가기", TS, 180, "";
	TS += 3;
	$newframeevent "OnEvent_적잠수함1이쏜_어뢰따라가기_앞에서뒤로보기", TS, 180, "";
	TS += 3;
	$newevent "OnEvent_고정관찰카메라_적어뢰1", TS, 1;
	TS += 3;
	$newevent "OnEvent_고정관찰카메라_적어뢰2", TS, 1;
	TS += 3;
	$newevent "OnEvent_고정관찰카메라_적어뢰3", TS, 1;
	TS += 3;
	$newframeevent "OnEvent_적잠수함1이쏜_어뢰따라가기", TS, 180, "";

	TS += 16;
	$newframeevent "OnEvent_적잠수함1호_어뢰발사_감속", TS, 120, "";
	TS += 10;
	$newframeevent "OnEvent_적잠수함1이쏜_어뢰따라가기_수중", TS, 180, "";
	TS += 10;
	$newevent "OnEvent_고정관찰카메라_적어뢰2", TS, 1;

	TS += 10;
	$newevent "OnEvent_고정관찰카메라_적어뢰2", TS, 1;

	// 적어뢰 근접(500m)시 회피기동(이벤트)
	// 회피기동시 근처 고정카메라로 관찰

	TS += 8;


	// Play: 물표면폭발음(어뢰폭발음) (기만기와 충돌)


//@start;
//	TS = 7;


	
	$newevent "OnEvent_카메라_잠수함수중정면", TS, 1;
	TS += 2;
	$newframeevent "OnEvent_3번해치열기", TS, 300, "";							// Play: 해치열기
	TS += 2;
	$newevent "OnEvent_Torpedo_적잠수함1_향해발싸아", TS, 1;		// 어뢰 발사		// Play: 어뢰진행음	
	TS += 1;
	$newframeevent "OnEvent_4번해치열기", TS, 300, "";							// Play: 해치열기
	TS += 2;
	$newevent "OnEvent_Torpedo_적잠수함2_향해발싸아", TS, 1;		// 어뢰 발사		// Play: 어뢰진행음	
	TS += 2;
	$newevent "OnEvent_잠수함이쏜_어뢰따라가기_수중", TS, 1;
	$newframeevent "OnEvent_3번해치닫기", TS, 300, "";
	$newframeevent "OnEvent_4번해치닫기", TS, 300, "";
	TS += 1;

	$newevent "OnEvent_카메라_어뢰따라가기_2", TS, 1;

	$newframeevent "OnEvent_어뢰속도가속2", TS, 300, "";
	TS += 10;
	$newframeevent "OnEvent_어뢰속도감속2", TS, 300, "";
	TS += 5;

	$NewFrameEvent "OnEvent_적잠수함1이동_따라가기2", TS, 600, "";
	TS += 12;
//	$NewFrameEvent "OnEvent_적잠수함2이동_따라가기", TS, 300, "";
//	TS += 5;


	// Play: 물표면폭발음


//	$newframeevent "OnEvent_적잠수함1호관찰", TS, 300, "";
//	TS += 7;
//	$newframeevent "OnEvent_적잠수함2호관찰", TS, 300, "";
//	TS += 7;

	$newframeevent "OnEvent_잠수함부상관찰", TS, 3600, "";			// Play: 잠수함부상음
	TS += 30;

//	$NewFrameEvent "OnEvent_카메라_높게이동", TS, 180, "OnEvent_카메라_높게이동_초기화";


																	// Stop: 잠수함부상음
}



////////////////////////////////////////////////////////////////////////////////////////
// EVENT DEFINITIONS
////////////////////////////////////////////////////////////////////////////////////////
function OnEvent_Init
{
	$SetCamera(1, 1, 30.0, 2.0, 90000.0);
	$SetCamera(2, 112, 20.0, 1.0, 24000.0);
	
	$SetUnderWaterImpurity(1);
	$SetTOD(0.0);
	$SetWaterAttenuation(-0.03);
	$SetFogAlt(800.0);
	$SetSnow(0, 0.1);
	$SetRain(0, 0.01, 0.3);
	$SetCloud(1500, TRUE, 1000, 1000, 1000);

	CALL OnEvent_SeaState_L1;
	CALL OnEvent_EntityPositionInit;
	CALL OnEvent_PeriscopeInit;
	CALL OnEvent_CameraView;

	$SetSubmarineWaterCouplingRegion("100", 700.0);
}

function OnEvent_EntityPositionInit
{
	//UseEntity: EntityID, Validation, DetectionRange, "CollisionCallback", CallbackRepetition

	$UseEntity("100", 1, 500.0, "OnEvent_적어뢰근접", 1);	// 잠수함
	$UseEntity("30", 1, 0.0, "", 0);	// 자함(해양경찰)
	$UseEntity("300", 1, 0.0, "", 0);	// 미사일(Harpoon)
	$UseEntity("301", 1, 0.0, "", 0);	// Rocket Trail
	$UseEntity("111", 1, 0.0, "", 0);	// 잠망경
	$UseEntity("401", 1, 0.0, "", 0);	// 나진1
	$UseEntity("402", 1, 0.0, "", 0);	// 나진1파손
	$UseEntity("403", 1, 0.0, "", 0);	// 나진2
	$UseEntity("404", 1, 0.0, "", 0);	// 나진2파손
	$UseEntity("101", 1, 0.0, "", 0);	// 해치1
	$UseEntity("102", 1, 0.0, "", 0);	// 해치2
	$UseEntity("103", 1, 0.0, "", 0);	// 해치3
	$UseEntity("104", 1, 0.0, "", 0);	// 해치4
	$UseEntity("105", 1, 0.0, "", 0);	// 해치5
	$UseEntity("106", 1, 0.0, "", 0);	// 해치6
	$UseEntity("107", 1, 0.0, "", 0);	// 해치7
	$UseEntity("108", 1, 0.0, "", 0);	// 해치8
	$UseEntity("109", 1, 0.0, "", 0);	// 해치9
	$UseEntity("5000", 1, 0.0, "", 0);	// 어뢰(mk46)
	$UseEntity("5001", 1, 0.0, "", 0);	// 어뢰(mk46)
	$UseEntity("5002", 1, 0.0, "", 0);	// 어뢰(mk46)
	$UseEntity("5003", 1, 0.0, "", 0);	// 어뢰(mk46)
	$UseEntity("5004", 1, 0.0, "", 0);	// 어뢰(mk46)
	$UseEntity("5005", 1, 0.0, "", 0);	// 어뢰(mk46)
	$UseEntity("5006", 1, 0.0, "", 0);	// 어뢰(mk46)
	$UseEntity("5007", 1, 0.0, "", 0);	// 어뢰(mk46)
	$UseEntity("5008", 1, 0.0, "", 0);	// 적어뢰(mk46)
	$UseEntity("5009", 1, 0.0, "", 0);	// 적어뢰(mk46)
	$UseEntity("4100", 1, 0.0, "", 0);	// 적잠수함1
	$UseEntity("4200", 1, 0.0, "", 0);	// 적잠수함2
	$UseEntity("4101", 1, 0.0, "", 0);	// 적잠수함1(파손)
	$UseEntity("4201", 1, 0.0, "", 0);	// 적잠수함2(파손)
	$UseEntity("6000", 1, 0.0, "", 0);	// Decoy 1
	$UseEntity("6001", 1, 0.0, "", 0);	// Decoy 2


	// 모델 리셋
	$SetEntityModel("300", "400");	// 미사일 초기화
	$SetEntityModel("401", "200");	// 나진1호 초기화
	$SetEntityModel("402", "200");	// 나진2호 초기화
	$SetEntityModel("5000", "500");	// 어뢰초기화
	$SetEntityModel("5001", "500");	// 어뢰초기화
	$SetEntityModel("5002", "500");	// 어뢰초기화
	$SetEntityModel("5003", "500");	// 어뢰초기화
	$SetEntityModel("5004", "500");	// 어뢰초기화
	$SetEntityModel("5005", "500");	// 어뢰초기화
	$SetEntityModel("5006", "500");	// 어뢰초기화
	$SetEntityModel("5007", "500");	// 어뢰초기화
	$SetEntityModel("5008", "501");	// 어뢰초기화
	$SetEntityModel("5009", "501");	// 어뢰초기화
	$SetEntityModel("6000", "500");	// decoy 초기화
	$SetEntityModel("6001", "500");	// decoy 초기화
	$SetEntityModel("4100", "210");
	$SetEntityModel("4200", "210");

	// 유도선 초기화
	//$ResetGuidelines();


	// Decoy 위치 초기화
	$SetEntityPosOri("6000", -100000.0, -100000.0, 0.0, 0.0, 0.0, 0.0);
	$SetEntityPosOri("6001", -100000.0, -100000.0, 0.0, 0.0, 0.0, 0.0);


	// 적함 위치 초기화
	$SetEntityPosOri("401", 7028.0, -5453.0, 0.0, 0.0, 0.0, 0.0);
	$SetEntityPosOri("402", 7028.0, -5453.0, 0.0, 0.0, 0.0, 0.0);
	$SetEntityPosOri("403", 6500.0, -5000.0, 0.0, 0.0, 0.0, 0.0);
	$SetEntityPosOri("404", 6500.0, -5000.0, 0.0, 0.0, 0.0, 0.0);

	// 적잠수함 위치 초기화
	$SetEntityPosOri("4100", 4306.0, -5562.0, -10.0, 0.0, 0.0, 0.0);
	$SetEntityPosOri("4200", 4006.0, -5162.0, -30.0, 0.0, 0.0, 0.0);

	// 자함 위치 초기화
	//$SetEntityPosOri("30", 7018.0, -5453.0, 0.0, 0.0, 0.0, 0.0);

	// 미사일 위치 초기화
	$SetEntityPosOri("300", -100000.0, -100000.0, 1.0, 0.0, 0.0, 0.0);

	// 잠수함 위치 초기화
	$SetEntityPosOri("100", 2042.0, -2026.0, -3.8, 0.0, 0.0, 0.0);		// 원본
//	$SetEntityPosOri("100", 2561.0, -2617.0, -17.0, 0.0, 0.0, 0.0);		// start로 점프할경우


	
// <start>
//	$SetEntityPosOri("100", 2561.0, -2617.0, -17.0, 0.0, 0.0, 0.0);


	// 어뢰 속도 초기화
	g_TorpedoSpeed = 35.0; // 35kn/h
}

function OnEventPeriodicTimer
{
	echo "* Sec = "; echoln SimTickSec;
}
function OnEventPeriodicTimerTick
{
//	echo "* Simulation Time(Tick) = "; echoln SimTick;


	$SetActiveCamera(g_ActiveCamera);


	$AutoExposure;

	// Update Dynamics------------------------------------
	$QueryEntityInfo "401";
	MAPPORT	PORT_ENTITYINFO, svEntityInfo;
	temp[7] = svEntityInfo.pos.x;
	temp[8] = svEntityInfo.pos.y;
	temp[9] = svEntityInfo.pos.z;

	temp[0] = g_SubmarineInfo.pos.x;
	temp[1] = g_SubmarineInfo.pos.y;
	temp[2] = g_SubmarineInfo.pos.z;
	$UpdateDynamics_Harpoon("300", "100",
								//g_SubmarineInfo.pos.x, g_SubmarineInfo.pos.y, g_SubmarineInfo.pos.z,
								temp[0], temp[1], temp[2],
								temp[7], temp[8], 0.0,
								80.0,			// 최대상승고도
								70.0, 200.0,	// 속도1, 속도2
								"Harpoon_Collision");	


	
	$QueryEntityInfo "403"; // 나진 2호
	MAPPORT	PORT_ENTITYINFO, svEntityInfo;
	temp[7] = svEntityInfo.pos.x;
	temp[8] = svEntityInfo.pos.y;
	temp[9] = svEntityInfo.pos.z;


	$UpdateDynamics_Torpedo("5000", 0, "100", temp[0], temp[1], temp[2],
								temp[7], temp[8], 0.0, /*1.294*/1.6, 28.966, 0.552,	g_TorpedoSpeed, "Torpedo1_Collision");
	$UpdateDynamics_Torpedo("5001", 1, "100", temp[0], temp[1], temp[2],
								temp[7], temp[8], 0.0, /*-1.294*/-1.6, 28.966, 0.552, g_TorpedoSpeed, "Torpedo2_Collision");



	// 적잠수함 1호를 향해!
	$QueryEntityInfo "4100";
	MAPPORT	PORT_ENTITYINFO, svEntityInfo;
	temp[7] = svEntityInfo.pos.x;
	temp[8] = svEntityInfo.pos.y;
	temp[9] = svEntityInfo.pos.z;

//	echoln "SUB 4100";
//	echo "X = "; echoln temp[7];
//	echo "Y = "; echoln temp[8];
//	echo "Z = "; echoln temp[9];

	$UpdateDynamics_Torpedo("5002", 2, "100", temp[0], temp[1], temp[2],
								temp[7], temp[8], temp[9], 0.113, 29.318, 0.558, g_TorpedoSpeed, "Torpedo3_Collision");

	// 적잠수함 2호를 향해!
	temp[7] = g_EnemySubmarine2Info.pos.x;
	temp[8] = g_EnemySubmarine2Info.pos.y;
	temp[9] = g_EnemySubmarine2Info.pos.z;

	$UpdateDynamics_Torpedo("5003", 3, "100", temp[0], temp[1], temp[2],
								temp[7], temp[8], temp[9], -0.113, 29.318, 0.558, g_TorpedoSpeed, "Torpedo4_Collision");


	$UpdateDynamics_Torpedo("5004", 4, "100", temp[0], temp[1], temp[2],
								temp[7], temp[8], 0.0, 0.113, 29.18, 1.55, g_TorpedoSpeed, "Torpedo5_Collision");
	$UpdateDynamics_Torpedo("5005", 5, "100", temp[0], temp[1], temp[2],
								temp[7], temp[8], 0.0, -0.113, 29.18, 1.55, g_TorpedoSpeed, "Torpedo6_Collision");
	$UpdateDynamics_Torpedo("5006", 6, "100", temp[0], temp[1], temp[2],
								temp[7], temp[8], 0.0, 0.111, 28.804, 2.542, g_TorpedoSpeed, "Torpedo7_Collision");
	$UpdateDynamics_Torpedo("5007", 7, "100", temp[0], temp[1], temp[2],
								temp[7], temp[8], 0.0, -0.111, 28.804, 2.542, g_TorpedoSpeed, "Torpedo8_Collision");




	// 발사위치: 적잠수함1호
	temp[0] = g_EnemySubmarine1Info.pos.x;
	temp[1] = g_EnemySubmarine1Info.pos.y;
	temp[2] = g_EnemySubmarine1Info.pos.z;


	// 적어뢰의 목표 위치:
	if g_DecoyFired == 1;
	{
		// 기만기1호
		$QueryEntityInfo "6000";
		MAPPORT	PORT_ENTITYINFO, svEntityInfo;
		temp[7] = svEntityInfo.pos.x;
		temp[8] = svEntityInfo.pos.y;
		temp[9] = svEntityInfo.pos.z;

//		echoln "Decoy1:";
//		echo "X = "; echoln temp[7];
//		echo "Y = "; echoln temp[8];
//		echo "Z = "; echoln temp[9];
	else;
		// 잠수함이 정지하고 있는 위치
		$QueryEntityInfo "100";
		MAPPORT	PORT_ENTITYINFO, svEntityInfo;
		temp[7] = svEntityInfo.pos.x;
		temp[8] = svEntityInfo.pos.y;
		temp[9] = -60.0; //svEntityInfo.pos.z; // 아래쪽으로
	}

	$UpdateDynamics_Torpedo("5008", 8, "4100", temp[0], temp[1], temp[2],
								temp[7], temp[8], temp[9], 0.111, 28.804, 0.0/*2.542*/, g_TorpedoSpeed, "Torpedo9_Collision");
	$UpdateDynamics_Torpedo("5009", 9, "4200", temp[0], temp[1], temp[2],
								temp[7], temp[8], temp[9], -0.111, 28.804, 0.0/*2.542*/, g_TorpedoSpeed, "Torpedo10_Collision");


	// 어뢰위치갱신: 근접 및 충돌
	$QueryEntityInfo "5008";
	MAPPORT	PORT_ENTITYINFO, svEntityInfo;

	temp[0] = svEntityInfo.pos.x;
	temp[1] = svEntityInfo.pos.y;
	temp[2] = svEntityInfo.pos.z;
	$SetCDTestPosition(temp[0], temp[1], temp[2]);


	// 기만기 다이내믹스
	$UpdateDynamics_Decoy("6000", "100", 0, 50.0, temp[0], temp[1], temp[2]);

								
	$BuildDynamicEntity("Harpoon");
	$BuildDynamicEntity("Torpedo");
	$BuildDynamicEntity("Decoy");
	$DoExplosionByTorpedo;
	$DoExplosionByHarpoon;
}


string markmsg;
function OutputCameraInfo markmsg
{
	$QueryEntityInfo "1";
	MAPPORT	PORT_ENTITYINFO, g_CameraEntryInfo;

	echoln;
	echo "### TAG: <"; echo markmsg; echoln ">";
	echoln "--------------------------- CAM INFO -----------------------------";
	echo "X = "; echo g_CameraEntryInfo.pos.x; echo ", ";
	echo "Y = "; echo g_CameraEntryInfo.pos.y; echo ", ";
	echo "Z = "; echo g_CameraEntryInfo.pos.z; echoln;
	echo "H = "; echo g_CameraEntryInfo.ori.h; echo ", ";
	echo "P = "; echo g_CameraEntryInfo.ori.p; echo ", ";
	echo "R = "; echo g_CameraEntryInfo.ori.r; echoln;
	echoln "------------------------------------------------------------------";
}

function OutputTS markmsg
{
	echoln;
	echo "### TAG: <"; echo markmsg; echoln ">";
	echoln "--------------------------- TS INFO ------------------------------";
	echo "TS = "; echoln TS;
	echoln "------------------------------------------------------------------";
}


#include "EventHandlers.s"
#include "EventPeriscope.s"
#include "View.s"
#include "Screen.s"
#include "SeaState.s"
#include "Path.s"
#include "Cam.s"
#include "Hatch.s"

