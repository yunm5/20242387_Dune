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
void display_object_info(OBJECT_INFO* unit);


void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], 
	CURSOR cursor,
	OBJECT_INFO* unit
)
{
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	display_system_message();
	display_object_info(unit);
	display_commands();
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
				POSITION pos = { i, j };
				char element = backbuf[i][j];
				int color = COLOR_DEFAULT;

				// 색깔 지정
				switch (element) {
				case BASE_P: color = COLOR_BLUE; break;  // 본진
				case HARVESTER_P: color = COLOR_BLUE; break; // 하베스터
				case BASE_H: color = COLOR_RED; break;  // 본진
				case HARVESTER_H: color = COLOR_RED; break;
				case SPICE: color = COLOR_PURPLE; break; // 스파이스 매장지
				case PLATE: color = COLOR_WHITE; break;  // 장판
				case SANDWORM: color = COLOR_YELLOW; break;  // 샌드웜
				case ROCK: color = COLOR_GRAY; break;   // 바위
				default: color = COLOR_DEFAULT; break;
				}

				printc(padd(map_pos, pos), element, color);
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

	printf("시스템 메시지:\n");
	for (int i = 0; i < message_count; i++) {
		printf("%s\n", messages[i]);
	}

}
void display_object_info(OBJECT_INFO* unit) {  // unit이 포인터임을 확인
	gotoxy(unit_status_pos);  // 유닛 상태 출력 위치로 커서 이동

	if (unit) {  // 유닛이 선택된 경우
		// set_color(COLOR_UNIT_STATUS);
		printf("유닛 상태:\n");
		gotoxy((POSITION) { unit_status_pos.row + 1, unit_status_pos.column });
		printf("위치: (%d, %d)\n", unit->pos.row, unit->pos.column);
		gotoxy((POSITION) { unit_status_pos.row + 2, unit_status_pos.column });
		printf("목표 위치: (%d, %d)\n", unit->dest.row, unit->dest.column);
		gotoxy((POSITION) { unit_status_pos.row + 3, unit_status_pos.column });
		printf("체력: %d\n", unit->hp);
		gotoxy((POSITION) { unit_status_pos.row + 4, unit_status_pos.column });
		printf("공격력: %d\n", unit->attack_power);
		gotoxy((POSITION) { unit_status_pos.row + 5, unit_status_pos.column });
		printf("이동 주기: %d ms\n", unit->move_period);
		gotoxy((POSITION) { unit_status_pos.row + 5, unit_status_pos.column });
		printf("다음 이동 시점: %d ms\n", unit->next_move_time);
	}
	else {
		// set_color(COLOR_WARNING);
		gotoxy(unit_status_pos);
		printf("선택된 유닛이 없습니다.\n");
	}
}
// 시스템 메시지 추가 함수
void add_system_message(const char* msg) {
	// 메시지가 꽉 찬 경우, 오래된 메시지 삭제 후 새 메시지 추가
	if (message_count >= MAX_MESSAGES) {
		for (int i = 1; i < MAX_MESSAGES; i++) {
			strcpy_s(messages[i - 1], sizeof(messages[i - 1]), messages[i]); // 안전한 복사
		}
		message_count = MAX_MESSAGES - 1;
	}

	// 새로운 메시지 추가
	snprintf(messages[message_count], sizeof(messages[message_count]), "%s", msg);
	message_count++;
}

void display_command(const char* cmd, POSITION pos) {
	gotoxy(pos);
	printf("%s\n", cmd);
}

void display_commands() {
	set_color(COLOR_DEFAULT);
	display_command("Move", CMD1_POS);     // 첫 번째 명령창
	display_command("Attack", CMD2_POS);   // 두 번째 명령창
	display_command("Build", CMD3_POS);    // 세 번째 명령창
	display_command("Harvest", CMD4_POS);  // 네 번째 명령창
}
