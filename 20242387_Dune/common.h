#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <conio.h>
#include <assert.h>
/* =========== ���� ���� =========== */
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

/* ===================== �޽��� ���� ���� =====================*/
#define MAX_MESSAGES 5  // �ִ� �޽��� ��

/* =========== ���� ���� =========== */
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_YELLOW 3
#define COLOR_BLACK 4
#define COLOR_BROWN 5
#define COLOR_GRAY 6
#define COLOR_DEFAULT 0


/*======����Ű ���� Ŭ��=======*/
#define DOUBLE_CLICK_THRESHOLD 300 // ms ����
#define MOVE_DISTANCE 2 // ����Ŭ�� �� �̵� �Ÿ�

/* ================= ��ġ�� ���� =================== */
// �ʿ��� ��ġ�� ��Ÿ���� ����ü
typedef struct {
	int row, column;
} POSITION;

// Ŀ�� ��ġ
typedef struct {
	POSITION previous;  // ���� ��ġ
	POSITION current;   // ���� ��ġ
} CURSOR;

// �Է� ������ Ű ����.
// �������� enum�� �����ߴµ�, ũ�� ����� ������ ���� �˻�
typedef enum {
	// k_none: �Էµ� Ű�� ����. d_stay(�� �����̴� ���)�� ����
	k_none = 0, k_up, k_right, k_left, k_down,
	k_quit, k_cancel, k_select, // ESC Ű ���, �����̽��� ����
	k_undef, // ���ǵ��� ���� Ű �Է�	
} KEY;


// DIRECTION�� KEY�� �κ�����������, �ǹ̸� ��Ȯ�ϰ� �ϱ� ���ؼ� �ٸ� Ÿ������ ����
typedef enum {
	d_stay = 0, d_up, d_right, d_left, d_down
} DIRECTION;

/*==================== ����â ===========================*/
typedef struct {
	POSITION pos;         // ������ ���� ��ġ
	POSITION dest;        // ������ ��ǥ ��ġ
	int hp;           // ������ ���� ü��
	int attack_power;     // ������ ���ݷ�
	int move_period;      // ���� �̵� �ֱ�(ms ����)
	int next_move_time;   // ���� �̵� ����
} OBJECT_INFO;
/*==================== ���� �� ����  ===========================*/
typedef struct {
	POSITION pos;           // ���� ���� ���� ��ġ
	int health;             // ���� ���� ü��
	int move_period;        // �̵� �ֱ� (ms ����)
	int next_move_time;     // ���� �̵� �ð�
	int length;             // ���� �� ���� (���ʽ� 1)
} SAND_WORM;

/* ================= ��ġ�� ����(2) =================== */
// ���Ǽ��� ���� �Լ���. KEY, POSITION, DIRECTION ����ü���� ���������� ��ȯ

// ���Ǽ� �Լ�
inline POSITION padd(POSITION p1, POSITION p2) {
	POSITION p = { p1.row + p2.row, p1.column + p2.column };
	return p;
}

// p1 - p2
inline POSITION psub(POSITION p1, POSITION p2) {
	POSITION p = { p1.row - p2.row, p1.column - p2.column };
	return p;
}

// ����Ű���� Ȯ���ϴ� �Լ�
#define is_arrow_key(k)		(k_up <= (k) && (k) <= k_down)

// ȭ��ǥ 'Ű'(KEY)�� '����'(DIRECTION)���� ��ȯ. ���� ���� �Ȱ����� Ÿ�Ը� �ٲ��ָ� ��
#define ktod(k)		(DIRECTION)(k)

// DIRECTION�� POSITION ���ͷ� ��ȯ�ϴ� �Լ�
inline POSITION dtop(DIRECTION d) {
	static POSITION direction_vector[] = { {0, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 0} };
	return direction_vector[d];
}

// p�� d �������� �̵���Ų POSITION
#define pmove(p, d)		(padd((p), dtop(d)))

/* ================= game data =================== */
typedef struct {
	int spice;		// ���� ������ �����̽�
	int spice_max;  // �����̽� �ִ� ���差
	int population; // ���� �α� ��
	int population_max;  // ���� ������ �α� ��
} RESOURCE;


// �밭 ����� ����. ��� �߰��ϸ鼭 ���� ������ ��
typedef struct {
	POSITION pos;		// ���� ��ġ(position)
	POSITION dest;		// ������(destination)
	char repr;			// ȭ�鿡 ǥ���� ����(representation)
	int move_period;	// '�� ms���� �� ĭ �����̴���'�� ����
	int next_move_time;	// ������ ������ �ð�
	int speed;
} OBJECT_SAMPLE;

DIRECTION get_direction(POSITION from, POSITION to); // get_direction �Լ� ����
#endif
