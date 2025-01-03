﻿#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
void sample_obj_move(void);
POSITION sample_obj_next_position(void);
void check_double_click(KEY key); // 더블 클릭 감지 및 커서 이동 처리
// 함수 프로토타입 선언
void initialize_map(void);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };

/* ============ 커서 조작 ============ */
POSITION selected_position = { -1, -1 }; // 선택된 오브젝트 위치 (없을 때 -1)
bool is_selected = false;           // 선택 상태 추적

/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = { 
	.spice = 0,
	.spice_max = 10,
	.population = 0,
	.population_max = 10
};

OBJECT_SAMPLE obj = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'o',
	.speed = 300,
	.next_move_time = 300
};

OBJECT_INFO unit = {
	.pos = {5, 10},           // 현재 위치
	.dest = {8, 15},           // 목표 위치
	.hp = 100,               // 체력
	.attack_power = 20,                // 공격력
	.move_period = 2000,              // 이동 주기
	.next_move_time = 1000               // 다음 이동 시점
};

Sandworm sandworm; //샌드 웜 
/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();
	display(resource, map, cursor,&unit);
	initialize_map();
	display_map(map); // 맵을 화면에 출력하는 함수 호출

	while (1) {
		// loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
		KEY key = get_key();

		// 키 입력이 있으면 처리
		if (is_arrow_key(key)) {
			check_double_click(key);  // 더블 클릭 감지 함수 호출
		}
		else {
			// 방향키 외의 입력
			switch (key) {
			case k_quit: outro();
			case k_none:
			case k_undef:
				break;
			default: break;
			}
		}  
		// 샌드웜 동작?

		// 샘플 오브젝트 동작
		sample_obj_move();

		// 화면 출력
		display(resource, map, cursor, &unit);
		Sleep(TICK);
		sys_clock += 10;
	}
}

/* ================= subfunctions =================== */
void intro(void) {
	printf("DUNE 1.5\n");
	Sleep(1000);
	system("cls");
	printf("Wait for a moment");
	Sleep(2000);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}

void init(void) {
	// layer 0(map[0])에 지형 생성
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH-1; j++) {
			map[0][i][j] = ' ';
		}
	}

	// layer 1(map[1])은 비워 두기(-1로 채움)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	// object sample
	map[1][obj.pos.row][obj.pos.column] = 'o';
}

// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	POSITION new_pos = pmove(curr, dir);

	// validation check
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 && \
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

		cursor.previous = cursor.current;
		cursor.current = new_pos;
	}
}

/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}
	
	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}
	
	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {
		
		return next_pos;
	}
	else {
		return obj.pos;  // 제자리
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// 아직 시간이 안 됐음
		return;
	}

	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.speed;
}

void initialize_map(void) {
	// 좌하단 본진(2x2)
	map[0][16][1] = BASE_P;
	map[0][16][2] = BASE_P;
	map[0][15][1] = BASE_P;
	map[0][15][2] = BASE_P;
	// 우상단 본진(2x2)
	map[0][1][57] = BASE_H;
	map[0][1][58] = BASE_H;
	map[0][2][57] = BASE_H;
	map[0][2][58] = BASE_H;

	// 하베스터 배치
	map[1][14][1] = HARVESTER_P;
	map[1][3][58] = HARVESTER_H;

	// 스파이스 매장지
	map[0][12][1] = SPICE;
	map[0][5][58] = SPICE;

	// 장판(2x2)
	map[0][15][3] = PLATE;
	map[0][15][4] = PLATE;
	map[0][16][3] = PLATE;
	map[0][16][4] = PLATE;

	
	map[0][1][55] = PLATE;
	map[0][1][56] = PLATE;
	map[0][2][55] = PLATE;
	map[0][2][56] = PLATE;

	// 중립 샌드웜 배치
	map[1][2][10] = SANDWORM;
	map[1][10][43] = SANDWORM;

	// 바위 배치
	map[0][5][15] = ROCK;
	map[0][5][16] = ROCK;
	map[0][6][15] = ROCK;
	map[0][6][16] = ROCK;

	map[0][12][29] = ROCK;
	map[0][12][30] = ROCK;
	map[0][13][29] = ROCK;
	map[0][13][30] = ROCK;

	map[0][12][12] = ROCK;
	map[0][4][49] = ROCK;
	map[0][13][50] = ROCK;

	// 기타 초기 설정 필요 시 추가
}
void check_double_click(KEY key) {
	static KEY last_key = k_none;
	static clock_t last_time = 0;
	clock_t now = clock();
	double diff = (double)(now - last_time) * 1000 / CLOCKS_PER_SEC;

	if (key != last_key) {
		last_key = key;
		last_time = now;
	}
	else if (diff < DOUBLE_CLICK_THRESHOLD) {
		// 더블 클릭인 경우
		cursor_move(ktod(key) * MOVE_DISTANCE);  // 두 칸 이동
	}
	else {
		// 더블 클릭이 아닌 경우
		cursor_move(ktod(key));  // 한 칸 이동
	}
}
// get_direction 함수 구현
DIRECTION get_direction(POSITION from, POSITION to) {
	if (from.row < to.row) return d_down;
	if (from.row > to.row) return d_up;
	if (from.column < to.column) return d_right;
	if (from.column > to.column) return d_left;
	return d_stay; // 목적지와 현재 위치가 같은 경우
}

POSITION find_nearest_unit(POSITION worm_pos) {
	POSITION closest_unit = { -1, -1 };
	int min_distance = MAP_WIDTH + MAP_HEIGHT;

	for (int row = 0; row < MAP_HEIGHT; row++) {
		for (int col = 0; col < MAP_WIDTH; col++) {
			if (map[1][row][col] == 'H') {  // 하베스터 유닛 예시
				int distance = abs(row - worm_pos.row) + abs(col - worm_pos.column);
				if (distance < min_distance) {
					min_distance = distance;
					closest_unit.row = row;
					closest_unit.column = col;
				}
			}
		}
	}
	return closest_unit;
}
// 거리 계산 함수
int calculate_distance(POSITION a, POSITION b) {
	return abs(a.row - b.row) + abs(a.column - b.column);
}

// 가장 가까운 유닛 탐색
POSITION find_closest_unit(POSITION sandworm_pos, POSITION* units, int unit_count) {
	POSITION closest = units[0];
	int min_distance = calculate_distance(sandworm_pos, units[0]);

	for (int i = 1; i < unit_count; i++) {
		int distance = calculate_distance(sandworm_pos, units[i]);
		if (distance < min_distance) {
			closest = units[i];
			min_distance = distance;
		}
	}
	return closest;
}
void create_spice(POSITION pos) {
	// 스파이스를 생성하는 로직 구현 (예: 맵 데이터에 추가)
	map[0][pos.row][pos.column] = SPICE; // 예시로 스파이스 위치를 'S'로 표시
}
// 샌드웜 움직임 업데이트
void update_sandworm_position(Sandworm* sandworm, POSITION* units, int unit_count) {
	if (unit_count == 0) return;

	// 가장 가까운 유닛 찾기
	POSITION closest_unit = find_closest_unit(sandworm->pos, units, unit_count);

	// 방향 결정 및 이동
	if (closest_unit.row > sandworm->pos.row) sandworm->pos.row++;
	else if (closest_unit.row < sandworm->pos.row) sandworm->pos.row--;
	if (closest_unit.column > sandworm->pos.column) sandworm->pos.column++;
	else if (closest_unit.column < sandworm->pos.column) sandworm->pos.column--;

	// 배설 주기 조건
	int current_time = TICK;
	if (current_time - sandworm->last_defecate_time > rand() % 5000 + 5000) { // 랜덤 배설 주기
		create_spice(sandworm->pos);
		sandworm->last_defecate_time = current_time;
		add_system_message("A sandworm has left spice.");
	}

}
