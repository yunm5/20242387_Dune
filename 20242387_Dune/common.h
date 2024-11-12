#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <conio.h>
#include <assert.h>
/* =========== 유닛 정의 =========== */
#define EMPTY ' '
#define PLATE 'P'
#define ROCK 'R'
#define SPICE 'S'
#define BASE 'B'
#define AIBASE 'A'
#define HARVESTER 'H'
#define SANDWORM 'W'

/* ================= system parameters =================== */
#define TICK 10		// time unit(ms)

#define N_LAYER 2
#define MAP_WIDTH	60
#define MAP_HEIGHT	18

//extern char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH];

/* ===================== 메시지 관련 정의 =====================*/
#define MAX_MESSAGES 5  // 최대 메시지 수

/* =========== 색깔 정의 =========== */
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_YELLOW 3
#define COLOR_BLACK 4
#define COLOR_BROWN 5
#define COLOR_GRAY 6
#define COLOR_DEFAULT 0


/*======방향키 더블 클릭=======*/
#define DOUBLE_CLICK_THRESHOLD 300 // ms 기준
#define MOVE_DISTANCE 2 // 더블클릭 시 이동 거리

/* ================= 위치와 방향 =================== */
// 맵에서 위치를 나타내는 구조체
typedef struct {
	int row, column;
} POSITION;

// 커서 위치
typedef struct {
	POSITION previous;  // 직전 위치
	POSITION current;   // 현재 위치
} CURSOR;

// 입력 가능한 키 종류.
// 수업에서 enum은 생략했는데, 크게 어렵지 않으니 예제 검색
typedef enum {
	// k_none: 입력된 키가 없음. d_stay(안 움직이는 경우)에 대응
	k_none = 0, k_up, k_right, k_left, k_down,
	k_quit, k_cancel, k_select, // ESC 키 취소, 스페이스바 선택
	k_undef, // 정의되지 않은 키 입력	
} KEY;


// DIRECTION은 KEY의 부분집합이지만, 의미를 명확하게 하기 위해서 다른 타입으로 정의
typedef enum {
	d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION;

/*==================== 상태창 ===========================*/
typedef struct {
	POSITION pos;         // 유닛의 현재 위치
	POSITION dest;        // 유닛의 목표 위치
	int hp;           // 유닛의 현재 체력
	int attack_power;     // 유닛의 공격력
	int move_period;      // 유닛 이동 주기(ms 단위)
	int next_move_time;   // 다음 이동 시점
} OBJECT_INFO;
/*==================== 샌드 웜 정의  ===========================*/
typedef struct {
	POSITION pos;           // 샌드 웜의 현재 위치
	int health;             // 샌드 웜의 체력
	int move_period;        // 이동 주기 (ms 단위)
	int next_move_time;     // 다음 이동 시간
	int length;             // 샌드 웜 길이 (보너스 1)
} SAND_WORM;

/* ================= 위치와 방향(2) =================== */
// 편의성을 위한 함수들. KEY, POSITION, DIRECTION 구조체들을 유기적으로 변환

// 편의성 함수
inline POSITION padd(POSITION p1, POSITION p2) {
	POSITION p = { p1.row + p2.row, p1.column + p2.column };
	return p;
}

// p1 - p2
inline POSITION psub(POSITION p1, POSITION p2) {
	POSITION p = { p1.row - p2.row, p1.column - p2.column };
	return p;
}

// 방향키인지 확인하는 함수
#define is_arrow_key(k)		(k_up <= (k) && (k) <= k_down)

// 화살표 '키'(KEY)를 '방향'(DIRECTION)으로 변환. 정수 값은 똑같으니 타입만 바꿔주면 됨
#define ktod(k)		(DIRECTION)(k)

// DIRECTION을 POSITION 벡터로 변환하는 함수
inline POSITION dtop(DIRECTION d) {
	static POSITION direction_vector[] = { {0, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 0} };
	return direction_vector[d];
}

// p를 d 방향으로 이동시킨 POSITION
#define pmove(p, d)		(padd((p), dtop(d)))

/* ================= game data =================== */
typedef struct {
	int spice;		// 현재 보유한 스파이스
	int spice_max;  // 스파이스 최대 저장량
	int population; // 현재 인구 수
	int population_max;  // 수용 가능한 인구 수
} RESOURCE;


// 대강 만들어 봤음. 기능 추가하면서 각자 수정할 것
typedef struct {
	POSITION pos;		// 현재 위치(position)
	POSITION dest;		// 목적지(destination)
	char repr;			// 화면에 표시할 문자(representation)
	int move_period;	// '몇 ms마다 한 칸 움직이는지'를 뜻함
	int next_move_time;	// 다음에 움직일 시간
	int speed;
} OBJECT_SAMPLE;

DIRECTION get_direction(POSITION from, POSITION to); // get_direction 함수 선언
#endif
