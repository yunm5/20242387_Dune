/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
const POSITION message_pos = { 19, 0 }; // 시스템 메시지 표시 위치
const POSITION unit_status_pos = { 0, 61 }; // 유닛 상태 정보를 출력할 위치


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

char messages[MAX_MESSAGES][100];  // 메시지 배열
int message_count = 0;             // 현재 메시지 수

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);


void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], 
	CURSOR cursor,
	OBJECT_INFO obj_info
)
{
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	display_system_message();
	//display_object_info(obj_info);
	//display_commands();
	// ...
}

void display_resource(RESOURCE resource) {
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}

void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				POSITION pos = {i, j };
				printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}

// frontbuf[][]에서 커서 위치의 문자를 색만 바꿔서 그대로 다시 출력
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	char ch = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

// 시스템 메시지를 화면에 출력
void display_system_message() {
	//set_color(COLOR_MESSAGE); // 메시지 색상 설정
	gotoxy(message_pos);      // 메시지 출력 시작 위치로 이동

	printf("system message:\n");

}
void display_object_info(OBJECT_INFO* unit) {
	gotoxy(unit_status_pos);  // 유닛 상태 출력 위치로 커서 이동

	if (unit) {  // 유닛이 선택된 경우
		//set_color(COLOR_UNIT_STATUS);
		printf("유닛 상태:\n");
		printf("위치: (%d, %d)\n", unit->pos.row, unit->pos.column);
		printf("목표 위치: (%d, %d)\n", unit->dest.row, unit->dest.column);
		printf("체력: %d\n", unit->health);
		printf("공격력: %d\n", unit->attack_power);
		printf("이동 주기: %d ms\n", unit->move_period);
		printf("다음 이동 시점: %d ms\n", unit->next_move_time);
	}
	else {
		//set_color(COLOR_WARNING);
		printf("선택된 유닛이 없습니다.\n");
	}
}


