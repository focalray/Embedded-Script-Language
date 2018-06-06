
////////////////////////////////////////////////////////////////////////////////////////
// PATH DEFINITIONS
////////////////////////////////////////////////////////////////////////////////////////
function Setup_All_Paths
{
	CALL PATH00_CAMERAWALK_00;
	CALL PATH01_SUBMARINE_00; // 이동
	CALL PATH01_SUBMARINE_01; // 회피
	CALL PATH02_CAMERAWALK_FLYTOSUBMARINE;
	CALL PATH03_ENEMY_SUBMARINE_01;
	CALL PATH04_ENEMY_SUBMARINE_02;
	call PATH_ENEMY_나진2호;
}


function PATH00_CAMERAWALK_00
{
	// Node의 갯수는 제한이 없습니다.
	// X, Y, Z, H, P, R 모두 거리기준으로 Catmull-rom spline으로 근사됩니다.

	// PATH INDEX RANGE: 0~99
	//$ResetPath PATH_IDX_00;

	// $AddPathNode: Path-Index, X, Y, Z, Heading, Pitch, Roll
	$AddPathNode PATH_IDX_00, 3559.0, -2950.0, -4.8, 0.0, 0.0, 0.0;
	$AddPathNode PATH_IDX_00, -4019.0, -4827.0, 700.0, 0.0, 0.0, 0.0;
	$AddPathNode PATH_IDX_00, -4547.0, -2454.0, 50.0, 0.0, -10.0, 0.0;
	$AddPathNode PATH_IDX_00, -2965.0, 444.0, 100.0, 0.0, 0.0, 0.0;
	$AddPathNode PATH_IDX_00, 1087.0, -1169.0, 300.0, 90.0, 0.0, 0.0;
	$AddPathNode PATH_IDX_00, 5634.0, -115.0, 300.0, -60.0, 0.0, 0.0;
	$AddPathNode PATH_IDX_00, 3888.0, 4629.0, 200.0, -60.0, 0.0, 0.0;
	$AddPathNode PATH_IDX_00, -1779.0, 4398.0, 200.0, 60.0, 0.0, 0.0;
	$AddPathNode PATH_IDX_00, -1812.0, 543.0, 300.0, 60.0, 0.0, 0.0;
	$AddPathNode PATH_IDX_00, 2075.0, -2355.0, 500.0, 60.0, 0.0, 0.0;
}


function PATH01_SUBMARINE_00	// 이동
{
	$AddPathNode PATH_IDX_01, 2042.0, -2026.0, -4.8, 0.0, 0.0, 0.0;
	$AddPathNode PATH_IDX_01, 2561.0, -2617.0, -17.0, 0.0, 0.0, 0.0;
}

function PATH01_SUBMARINE_01	// 회피
{
	$AddPathNode PATH_IDX_06, 2561.0, -2617.0, -17.0, 0.0, 0.0, 0.0;
	$AddPathNode PATH_IDX_06, 2936.0, -2791.0, -17.0, 0.0, 0.0, 0.0;
	$AddPathNode PATH_IDX_06, 3269.0, -2892.0, -17.0, 0.0, 0.0, 0.0;
	$AddPathNode PATH_IDX_06, 3559.0, -2950.0, -4.5, 0.0, 0.0, 0.0;
}


function PATH02_CAMERAWALK_FLYTOSUBMARINE
{
	$AddPathNode PATH_IDX_02, -5271.0, -3509.0, 500.0, 0.0, 0.0, 0.0;
	$AddPathNode PATH_IDX_02, 2042.0, -2026.0, 100.0, 0.0, 0.0, 0.0;
}


function PATH03_ENEMY_SUBMARINE_01
{
	$AddPathNode PATH_IDX_03, 4306.0, -5562.0, -10.0, 0.0, 0.0, 0.0;
	$AddPathNode PATH_IDX_03, 3784.0, -4420.0, -10.0, 0.0, 0.0, 0.0;
}

function PATH04_ENEMY_SUBMARINE_02
{
	$AddPathNode PATH_IDX_04, 4006.0, -5162.0, -30.0, 0.0, 0.0, 0.0;
	$AddPathNode PATH_IDX_04, 4013.0, -5301.0, -10.0, 0.0, 0.0, 0.0;
}


function PATH_ENEMY_나진2호
{
	$AddPathNode PATH_IDX_05, 7028.0, -5353.0, 0.0, 0.0, 0.0, 0.0;
	$AddPathNode PATH_IDX_05, 4633.0, -4062.0, 0.0, 0.0, 0.0, 0.0;
}
