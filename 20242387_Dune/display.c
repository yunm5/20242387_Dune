/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "io.h"

// ����� ������� �»��(topleft) ��ǥ
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
const POSITION message_pos = { 19, 0 }; // �ý��� �޽��� ǥ�� ��ġ
const POSITION unit_status_pos = { 0, 61 }; // ���� ���� ������ ����� ��ġ


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

char messages[MAX_MESSAGES][100];  // �޽��� �迭
int message_count = 0;             // ���� �޽��� ��

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

				// ���� ����
				switch (element) {
				case BASE_P: color = COLOR_BLUE; break;  // ����
				case HARVESTER_P: color = COLOR_BLUE; break; // �Ϻ�����
				case BASE_H: color = COLOR_RED; break;  // ����
				case HARVESTER_H: color = COLOR_RED; break;
				case SPICE: color = COLOR_PURPLE; break; // �����̽� ������
				case PLATE: color = COLOR_WHITE; break;  // ����
				case SANDWORM: color = COLOR_YELLOW; break;  // �����
				case ROCK: color = COLOR_GRAY; break;   // ����
				default: color = COLOR_DEFAULT; break;
				}

				printc(padd(map_pos, pos), element, color);
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}

// frontbuf[][]���� Ŀ�� ��ġ�� ���ڸ� ���� �ٲ㼭 �״�� �ٽ� ���
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	char ch = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

// �ý��� �޽����� ȭ�鿡 ���
void display_system_message() {
	//set_color(COLOR_MESSAGE); // �޽��� ���� ����
	gotoxy(message_pos);      // �޽��� ��� ���� ��ġ�� �̵�

	printf("�ý��� �޽���:\n");
	for (int i = 0; i < message_count; i++) {
		printf("%s\n", messages[i]);
	}

}
void display_object_info(OBJECT_INFO* unit) {  // unit�� ���������� Ȯ��
	gotoxy(unit_status_pos);  // ���� ���� ��� ��ġ�� Ŀ�� �̵�

	if (unit) {  // ������ ���õ� ���
		// set_color(COLOR_UNIT_STATUS);
		printf("���� ����:\n");
		gotoxy((POSITION) { unit_status_pos.row + 1, unit_status_pos.column });
		printf("��ġ: (%d, %d)\n", unit->pos.row, unit->pos.column);
		gotoxy((POSITION) { unit_status_pos.row + 2, unit_status_pos.column });
		printf("��ǥ ��ġ: (%d, %d)\n", unit->dest.row, unit->dest.column);
		gotoxy((POSITION) { unit_status_pos.row + 3, unit_status_pos.column });
		printf("ü��: %d\n", unit->hp);
		gotoxy((POSITION) { unit_status_pos.row + 4, unit_status_pos.column });
		printf("���ݷ�: %d\n", unit->attack_power);
		gotoxy((POSITION) { unit_status_pos.row + 5, unit_status_pos.column });
		printf("�̵� �ֱ�: %d ms\n", unit->move_period);
		gotoxy((POSITION) { unit_status_pos.row + 5, unit_status_pos.column });
		printf("���� �̵� ����: %d ms\n", unit->next_move_time);
	}
	else {
		// set_color(COLOR_WARNING);
		gotoxy(unit_status_pos);
		printf("���õ� ������ �����ϴ�.\n");
	}
}
// �ý��� �޽��� �߰� �Լ�
void add_system_message(const char* msg) {
	// �޽����� �� �� ���, ������ �޽��� ���� �� �� �޽��� �߰�
	if (message_count >= MAX_MESSAGES) {
		for (int i = 1; i < MAX_MESSAGES; i++) {
			strcpy_s(messages[i - 1], sizeof(messages[i - 1]), messages[i]); // ������ ����
		}
		message_count = MAX_MESSAGES - 1;
	}

	// ���ο� �޽��� �߰�
	snprintf(messages[message_count], sizeof(messages[message_count]), "%s", msg);
	message_count++;
}

void display_command(const char* cmd, POSITION pos) {
	gotoxy(pos);
	printf("%s\n", cmd);
}

void display_commands() {
	set_color(COLOR_DEFAULT);
	display_command("Move", CMD1_POS);     // ù ��° ���â
	display_command("Attack", CMD2_POS);   // �� ��° ���â
	display_command("Build", CMD3_POS);    // �� ��° ���â
	display_command("Harvest", CMD4_POS);  // �� ��° ���â
}
