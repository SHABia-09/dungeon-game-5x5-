// oujitu2.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

bool g_bWonTheBoss = false;
bool g_bRunTheDungeon = true;
bool g_bMissionCompleted = false;
int g_iWonTheEnemy = 0;
int g_iRoomnumber = 7;
int g_iMovement = 0;
int g_iEnemyRandMIN = 0;
int g_iSanityTriggerRatio = 10;
int g_iSanityTrigger = -1;

struct roomevent {
	int enemy;
	int statusup;
	int weapon;
	int potion;
	int damage;
	int sanitydown;
	int nothing;
	//イベントの種類の構造体の型宣言
	//作成者…棟近、千原
};

struct roomevent event[] = {
	//部屋の種類
	{ 74, 0,79,89,97,98,99 },//1行目…強敵
	{ 44,46,50,58,71,73,99 },//2行目…通常
	{ 69, 0,71,79,87,89,99 },//3行目…弱敵
	{ 80,82,0,86,96,99,99 },//4行目…門番
	{ 49,54,59,69, 0, 0,99 },//5行目…罠なし
	{ 39,44,64,94,98,99,99 },//6行目…回復
	//作成者…棟近、千原
};


int player_status[9];
/*
	0:x座標
	1:y座標
	2:最大hp
	3:現在hp
	4:攻撃力
	5:防御力
	6:正気度
	7:武器耐久度
	8:ポーション所持数
*/

struct eventtable {
	int enemy[6];
	int statusup[6];
	int weapon[6];
	int potion[6];
	int damage[6];
	int sanitydown[6];
	int nothing[6];

};

void INIT_Status(int* p) {
	int i = 0;
	p[i] = 0;
	p[i + 1] = 0;
	p[i + 2] = 10;
	p[i + 3] = 10;
	p[i + 4] = 1;
	p[i + 5] = 1;
	p[i + 6] = 50;
	p[i + 7] = 3;
	p[i + 8] = 0;

	return;
}

typedef struct {
	char name[30];
	int max_hp;
	int attack;
	int defense;
	int Bonus;
}enemy;
enemy enemy_type[10];
//敵の構造体のタグ設定(遠藤)

void INIT_enemy() {
	enemy_type[0] = { "ゴースト",		15,3,2,1 };
	enemy_type[1] = { "スライム",		14,3,2,1 };
	enemy_type[10] = { "ゾンビ",		35,3,1,2 };
	enemy_type[11] = { "スケルトン",	26,5,4,2 };
	enemy_type[12] = { "サーペント",	31,5,2,2 };
	enemy_type[13] = { "インプ",		28,6,0,2 };
	enemy_type[14] = { "盗賊者",		29,5,2,2 };
	enemy_type[15] = { "ゴブリン",		33,4,2,2 };
	enemy_type[20] = { "ウィッチ",		73,8,7,5 };
	enemy_type[21] = { "オーガ",		98,10,3,5 };
	enemy_type[22] = { "ゴーレム",		85,7,10,5 };
	enemy_type[30] = { "デーモン",		126,13,5,4 };
	enemy_type[31] = { "ジャイアント",	130,15,4,4 };
	enemy_type[40] = { "ドラゴン",		235,21,7,0 };
}//敵の初期化(遠藤)

void INIT_var() {
	g_bWonTheBoss = false;
	g_bRunTheDungeon = true;
	g_bMissionCompleted = false;
	g_iWonTheEnemy = 0;
	g_iRoomnumber = 7;
	g_iMovement = 1;
	g_iEnemyRandMIN = 0;
	g_iSanityTriggerRatio = 10;
	g_iSanityTrigger = -1;
}

void Initialize() {
	INIT_Status(player_status);
	INIT_enemy();
	INIT_var();
	g_iSanityTrigger = player_status[6] - g_iSanityTriggerRatio;
	srand((unsigned int)time(NULL));
}

void Display_rule() {
	printf("\n!!ストーリー!!\
			\nドラゴンスレイヤーであるあなたはこのダンジョンに住むドラゴンを倒すためこのダンジョンにやってきました。\
			\nまた、このダンジョンに長くいると異形に近づいていくとのうわさも存在しています。\
			\n\n!!基本ルール説明!!\
			\nプレイヤーの基本的なステータスはHP,攻撃力,防御力,正気度の4つで、\
			\nプレイヤーはHP,攻撃,防御の三つのステータスを敵を倒す、アイテムを手に入れるなどして強化することができます。\
			\n正気度が0になるとプレイヤーは完全な異形となりゲームオーバーとなります。\
			\n同様にHPが0になってもゲームオーバーとなります。\
			\n\n!!マップ説明!!\
			\nプレイヤーはマップ上でPで表されます。\
			\n弱めの敵が出るエリアはW、普通の敵が出るエリアはo\
			\nアイテムが多く手に入るエリアはR、強敵が潜むエリアはS\
			\n門番がうろつくエリアはG、ボスのいる部屋はBで表されます。\
			\nまた、移動は矢印キーで行います。\
			\n\n!!戦闘ルール説明!!\
			\n戦闘はプレイヤーの先行で行われます。\
			\nプレイヤーが取れる行動は攻撃、回復、武器の使用です。\
			\n武器での攻撃は強力ですが、プレイヤーの武器には耐久度が存在し、三回使用すると壊れてしまいます。\
			\nしかし、新たな武器をダンジョン内で入手することができるでしょう。\
			\n最初ポーションは持っていませんがダンジョン内で入手することができます。\
			\nポーションの回復量は、現在HPの切り捨て半分です。\n\
			\nステータスの初期値は HP:%d 攻撃力:%d 防御力:%d 正気度:%dです。\n", player_status[3], player_status[4], player_status[5], player_status[6]);
	//ゲームルールの表示(小平)
}

void Display_Map(int* p) {
	for (int i = 0; i <= 4; i++) {
		if (i == 0 || i == 1) {
			for (int j = 0; j <= 4; j++) {
				if (p[1] == i && j == p[0]) {
					printf("P");
				}
				else if (j <= 1) {
					printf("W");
				}
				else if (j == 2) {
					printf("o");
				}
				else if (j >= 3) {
					printf("R");
				}
				if (j < 4) {
					printf(" - ");
				}
			}
			printf("\n");
		}
		else if (i == 2) {
			for (int j = 0; j <= 4; j++) {
				if (p[1] == i && j == p[0]) {
					printf("P");
				}
				else if (j == 2) {
					printf("N");
				}
				else {
					printf("o");
				}
				if (j < 4) {
					printf(" - ");
				}
			}
			printf("\n");

		}
		else if (i == 3 || i == 4) {
			for (int j = 0; j <= 4; j++) {
				if (p[1] == i && j == p[0]) {
					printf("P");
				}
				else if (i == 4 && j == 4) {
					printf("B");
				}
				else if (j <= 1) {
					printf("S");
				}
				else if (j == 2) {
					printf("o");
				}
				else if (j >= 3) {
					printf("G");
				}
				if (j < 4) {
					printf(" - ");
				}
			}
			printf("\n");

		}
		if (i < 4) {
			printf("|   |   |   |   |\n");
		}
	}

	//ダンジョンマップの表示(小平)
}

void Display_Status() {
	printf("最大HP…%d\n現在HP…%d\n攻撃力…%d\n防御力…%d\n正気度…%d\n武器使用回数…%d回\nポーション所持数…%d個\n\n", player_status[2], player_status[3], player_status[4], player_status[5], player_status[6], player_status[7], player_status[8]);
	//ステータスの表示(千原)
}

void Display_enemy_Status(char* name, int hp, int max_hp, int attack, int defense, int bonus, int damage) {
	if (player_status[6] <= 10) {
		printf("%sのステータス:\nHP:%d/%d、攻撃力:%d、防御力:%d、獲得経験値:%d\n次の攻撃の予測ダメージ:%d\n", name, hp, max_hp, attack, defense, bonus, damage);
	}
	else if (player_status[6] <= 20) {
		printf("%sのステータス:\nHP:%d/%d、攻撃力:%d、防御力:%d、獲得経験値:%d\n次の攻撃の予測ダメージ:???\n", name, hp, max_hp, attack, defense, bonus);
	}
	else if (player_status[6] <= 30) {
		printf("%sのステータス:\nHP:%d/%d、攻撃力:%d、防御力:???、獲得経験値:???\n次の攻撃の予測ダメージ:???\n", name, hp, max_hp, attack);
	}
	else if (player_status[6] <= 40) {
		printf("%sのステータス:\nHP:%d/%d、攻撃力:???、防御力:???、獲得経験値:???\n次の攻撃の予測ダメージ:???\n", name, hp, max_hp);
	}
	else {
		printf("%sのステータス:\nHP:???/???、攻撃力:???、防御力:???、獲得経験値:???\n次の攻撃の予測ダメージ:???\n", name);
	}
}

void first_Experience(int exp) {
	char key = 0;
	printf("あなたはゲームの最初に、ステータスに%dポイント割り振ることができます。\n「a」キー…最大HP、「s」キー…攻撃力、「d」キー…防御力が上がります。「w」キー…ステータス確認\n", exp);
	for (int i = 0; i < exp; i++) {
		key = _getch();
		if (key == 'a') {
			player_status[2] += 2;
			player_status[3] += 2;
			printf("最大HPと現在HPが2上がった!\n");
			printf("現在のHP:%d/%d\n", player_status[3], player_status[2]);
		}
		else if (key == 's') {
			player_status[4] += 1;
			printf("攻撃力が1上がった!\n");
			printf("現在の攻撃力:%d\n", player_status[4]);
		}
		else if (key == 'd') {
			player_status[5] += 1;
			printf("防御力が1上がった!\n");
			printf("現在の防御力:%d\n", player_status[5]);
		}
		else if (key == 'w') {
			Display_Status();
			i--;
			continue;
		}
		else {
			printf("あなたは間違った値を入力した!\n力が失われていく感覚がした。\n");
		}
	}
	printf("ステータスを割り振り終わりました!\nさあダンジョンを進みましょう!\n");
}//作成者…千原

void Experience(int p) {
	char key = 0;
	printf("あなたはステータスに%dポイント割り振ることができます。\n「a」キー…最大HP、「s」キー…攻撃力、「d」キー…防御力が上がります。「w」キー…ステータス確認\n", p);
	for (int i = 0; i < p; i++) {
		key = _getch();
		if (key == 'a') {
			player_status[2] += 2;
			player_status[3] += 2;
			printf("最大HPと現在HPが2上がった!\n");
			printf("現在のHP:%d/%d\n", player_status[3], player_status[2]);
		}
		else if (key == 's') {
			player_status[4] += 1;
			printf("攻撃力が1上がった!\n");
			printf("現在の攻撃力:%d\n", player_status[4]);
		}
		else if (key == 'd') {
			player_status[5] += 1;
			printf("防御力が1上がった!\n");
			printf("現在の防御力:%d\n", player_status[5]);
		}
		else if (key == 'w') {
			Display_Status();
			i--;
			continue;
		}
		else {
			printf("あなたは間違った値を入力した!\n力が失われていく感覚がした。\n");
		}
	}
}//作成者…千原


void Used_Potion(int* p) {
	int i = 0;
	int a, b;
	if (p[i + 8] != 0) {
		if (p[i + 2] == p[i + 3]) {
			printf("あなたのHPは最大値です!\n");
			return;
		}
		p[i + 8] -= 1;
		a = p[i + 2];
		b = a / 2;
		p[i + 3] += b;
		if (p[i + 3] > a) {
			p[i + 3] = a;
		}
		printf("\nポーションを使った。%dポイント回復した\n", b);
		printf("HP:%d/%d\n", player_status[3], player_status[2]);
	}
	else {
		printf("ポージョンを持っていません!\n");
		//ポーションの使用(小平)
	}
}

void Battle_Enemy() {
	int randomint;
	char key = 0;
	double random;
	int damage = 0;
	int enemy_damage = 0;
	int sanity_minus;
	int sanity_ratio = 0;
	int turn = 0;
	int nothing_turn = 0;

	switch (g_iRoomnumber) {
	case 0:
	case 2:
	case 3:
		randomint = rand() % 2 + g_iEnemyRandMIN;
		break;
	case 4:
	case 5:
		randomint = rand() % 3 + g_iEnemyRandMIN;
		break;
	case 1:
		randomint = rand() % 6 + g_iEnemyRandMIN;
	default:
		break;
	}
	printf("%sが現れた!\n", enemy_type[randomint].name);

	int enemy_hp = enemy_type[randomint].max_hp;
	int defense = enemy_type[randomint].defense;
	int attack = enemy_type[randomint].attack;


	while (true) {
		turn++;
		printf("\n%dターン目の行動を決めてください。\n", turn);
		random = rand() % 4 + 8;
		enemy_damage = (attack * 3 - player_status[5] * 2) * (random * 0.1);
		if (enemy_damage < 0) {
			enemy_damage = 0;
		}
		Display_enemy_Status(enemy_type[randomint].name, enemy_hp, enemy_type[randomint].max_hp, enemy_type[randomint].attack, enemy_type[randomint].defense, enemy_type[randomint].Bonus, enemy_damage);
		printf("\nあなたのステータス:\nHP:%d/%d、武器使用可能回数:%d/3、ポーション所持数:%d/5\n「a」キー…攻撃、「s」キー…武器使用攻撃、「d」キー…ポーションを使用、「w」キー…ステータス確認\n", player_status[3], player_status[2], player_status[7], player_status[8]);

		do {
			key = _getch();
			if (key == 'a')
			{
				random = rand() % 5 + 8;
				damage = (player_status[4] * 3 - defense * 2) * (random * 0.1);
				if (damage < 0) {
					damage = 0;
				}
				printf("プレイヤーの攻撃!\n%dダメージ与えた!\n", damage);
				enemy_hp = enemy_hp - damage;
				break;
			}
			else if (key == 's') {
				if (player_status[7] <= 0) {
					printf("武器は壊れている…。\n");
					continue;
				}
				random = rand() % 4 + 15;
				damage = (player_status[4] * 3 - defense * 2) * (random * 0.1);
				if (damage < 0) {
					damage = 0;
				}
				printf("プレイヤーの武器攻撃!\n%dダメージ与えた!\n", damage);
				player_status[7]--;
				printf("武器の耐久値は%dになった。\n", player_status[7]);
				if (player_status[7] <= 0) {
					printf("武器の耐久度が0になり、壊れてしまった。\n");
				}
				enemy_hp = enemy_hp - damage;
				break;
			}
			else if (key == 'w') {
				Display_Status();
				continue;
			}
			else if (key == 'd') {
				Used_Potion(player_status);
				continue;
			}
			else {
				printf("正しい文字を入力してください!\n");
				continue;
			}
		} while (true);
		if (enemy_hp <= 0) {
			break;
		}
		if (enemy_type[randomint].name == "盗賊者" && turn % 2 == 1) {
			printf("%sのスティール!\n", enemy_type[randomint].name);
			if (player_status[8] > 0) {
				player_status[8]--;
				printf("ポーションを1つ奪われた!\n現在のポーションの所持数…%d\n", player_status[8]);
			}
		}
		else {
			printf("%sの攻撃!\n%dダメージ受けた!\n", enemy_type[randomint].name, enemy_damage);
			player_status[3] = player_status[3] - enemy_damage;
			if (player_status[3] <= 0) {
				break;
			}
			else if (damage == 0 && enemy_damage == 0) {
				nothing_turn++;
				if (nothing_turn >= 2) {
					sanity_minus = 2 ^ sanity_ratio;
					player_status[6] = player_status[6] - sanity_minus;
					printf("無意味に続く戦闘に、あなたは正気を%d失った。\n現在の正気度…%d\n", sanity_minus, player_status[6]);
					sanity_ratio++;
					if (player_status[6] <= 0) {
						break;
					}
				}
			}
		}
		printf("\nキーを押してください\n");
		key = _getch();
		if (key == 0xFFFFFFE0)
		{
			_getch();
		}
	}
	if (enemy_hp <= 0) {
		printf("%sを倒した!\n", enemy_type[randomint].name);
		g_iWonTheEnemy++;
		player_status[6] = player_status[6] - enemy_type[randomint].Bonus;
		printf("\nキーを押してください\n");
		key = _getch();
		if (key == 0xFFFFFFE0)
		{
			_getch();
		}
		printf("\nあなたは力を得ると同時に、正気を%d失った。\n現在の正気度…%d\n", enemy_type[randomint].Bonus, player_status[6]);
		if (player_status[6] <= 0) {
			return;
		}
		else if (g_iSanityTrigger >= player_status[6]) {
			printf("あなたは正気が失われたことで、ダンジョンの適応能力を得た。\n新たに敵のステータスが分かるようになった。\n");
			g_iSanityTrigger = g_iSanityTrigger - g_iSanityTriggerRatio;
		}
		Experience(enemy_type[randomint].Bonus);
	}
}

void Battle_Boss() {
	int randomint;
	char key = 0;
	double random;
	int damage = 0;
	int enemy_damage = 0;
	int sanity_minus;
	int sanity_ratio = 0;
	int turn = 0;
	int nothing_turn = 0;
	int attack_pattern = 0;

	randomint = rand() % 1 + 40;
	printf("%sが現れた!\n", enemy_type[randomint].name);

	int enemy_hp = enemy_type[randomint].max_hp;
	int defense = enemy_type[randomint].defense;
	int attack = enemy_type[randomint].attack;

	while (true) {
		turn++;
		printf("\n%dターン目の行動を決めてください。\n", turn);
		random = rand() % 4 + 8;
		enemy_damage = (attack * 3 - player_status[5] * 2) * (random * 0.1);
		if (rand() % 10 >= 2) {
			attack_pattern = 0;
		}
		else {
			attack_pattern = 1;
			enemy_damage += 5;
		}
		if (enemy_damage < 0) {
			enemy_damage = 0;
		}
		Display_enemy_Status(enemy_type[randomint].name, enemy_hp, enemy_type[randomint].max_hp, enemy_type[randomint].attack, enemy_type[randomint].defense, enemy_type[randomint].Bonus, enemy_damage);
		printf("あなたのステータス:\nHP:%d/%d、武器使用可能回数…%d/3、ポーション所持数:%d/5\n「a」キー…攻撃、「s」キー…武器使用攻撃、「d」キー…ポーションを使用、「w」キー…ステータス確認\n", player_status[3], player_status[2], player_status[7], player_status[8]);

		do {
			key = _getch();
			if (key == 'a')
			{
				random = rand() % 5 + 8;
				damage = (player_status[4] * 3 - defense * 2) * (random * 0.1);
				if (damage < 0) {
					damage = 0;
				}
				printf("プレイヤーの攻撃!\n%dダメージ与えた!\n", damage);
				enemy_hp = enemy_hp - damage;
				break;
			}
			else if (key == 's') {
				if (player_status[7] <= 0) {
					printf("武器は壊れている…。\n");
					continue;
				}
				random = rand() % 4 + 15;
				damage = (player_status[4] * 3 - defense * 2) * (random * 0.1);
				if (damage < 0) {
					damage = 0;
				}
				printf("プレイヤーの武器攻撃!\n%dダメージ与えた!\n", damage);
				player_status[7]--;
				printf("武器の耐久値は%dになった。\n", player_status[7]);
				if (player_status[7] <= 0) {
					printf("武器の耐久度が0になり、壊れてしまった。\n");
				}
				enemy_hp = enemy_hp - damage;
				break;
			}
			else if (key == 'w') {
				Display_Status();
				continue;
			}
			else if (key == 'd') {
				Used_Potion(player_status);
				continue;
			}
			else {
				printf("正しい文字を入力してください!\n");
			}
		} while (true);
		if (enemy_hp <= 0) {
			break;
		}
		if (turn % 3 == 0) {
			int more_power = 3;
			attack += more_power;
			printf("%sはダンジョンの力を取り込んだ。\n攻撃力が%d上がった!\n", enemy_type[randomint].name, more_power);
		}
		else if (attack_pattern == 0) {
			printf("%sは爪で引き裂いてきた!\n%dダメージ受けた!\n", enemy_type[randomint].name, enemy_damage);
		}
		else {
			printf("%sの火炎ブレス!\n%dダメージ受けた!\n", enemy_type[randomint].name, enemy_damage);
		}
		player_status[3] = player_status[3] - enemy_damage;
		if (player_status[3] <= 0) {
			break;
		}
		else if (damage == 0 && enemy_damage == 0) {
			nothing_turn++;
			if (nothing_turn >= 2) {
				sanity_minus = 2 ^ sanity_ratio;
				player_status[6] = player_status[6] - sanity_minus;
				printf("無意味に続く戦闘に、あなたは正気を%d失った。\n現在の正気度…%d\n", sanity_minus, player_status[6]);
				sanity_ratio++;
				if (player_status[6] <= 0) {
					break;
				}
			}
		}
		printf("\nキーを押してください\n");
		key = _getch();
		if (key == 0xFFFFFFE0)
		{
			_getch();
		}
	}
	if (enemy_hp <= 0) {
		printf("%sを倒した!\n", enemy_type[randomint].name);
		printf("\nキーを押してください\n");
		key = _getch();
		if (key == 0xFFFFFFE0)
		{
			_getch();
		}
		g_iWonTheEnemy++;
		g_bMissionCompleted = true;
	}
}

void Triggerd_Map() {

	int randomint = rand() % 100;
	if (g_iRoomnumber == 6) {			//ボスの部屋なら
		Battle_Boss();
	}
	else if (g_iRoomnumber == 7) {		//初期部屋なら
	}
	else if (randomint < event[g_iRoomnumber].enemy) {
		Battle_Enemy();
	}
	else if (randomint < event[g_iRoomnumber].statusup) {
		printf("小さな祠をみつけた。あなたは手を合わせ、お祈りをした。すると不思議なことに、体の内側から力が湧き出てくるように感じた。\n");
		int p = 1;
		Experience(p);
	}
	else if (randomint < event[g_iRoomnumber].weapon) {
		player_status[7] = 3;
		printf("あなたは新品の武器を見つけた。\n武器の耐久度が全回復した。\n");
	}
	else if (randomint < event[g_iRoomnumber].potion) {
		printf("あなたは小瓶が落ちている事に気づいた。拾い上げてみると、まだ未開封の回復ポーションのようだ。\n");
		if (player_status[8] < 5) {
			player_status[8]++;
			printf("回復ポーションの所持数が1増えた。\n");
			printf("回復ポーションの所持数:%d\n", player_status[8]);
		}
		else {
			printf("しかし、これ以上回復ポーションを持てないことに気づいたあなたは、手に入れるのを諦めた。\n");
		}
	}
	else if (randomint < event[g_iRoomnumber].damage) {
		player_status[3] -= 1;
		printf("あなたが部屋にはいると、床を踏んだ感触がしなかった。足下を見て、落とし穴があることに気づいたときにはもう遅かった。なんとか落とし穴から這い上がってこれたが、全身が軋むように痛い。\nあなたは1ダメージを受けた。\n");
		printf("現在のHP:%d\n", player_status[3]);
	}
	else if (randomint < event[g_iRoomnumber].sanitydown) {
		player_status[6] -= 1;
		printf("あなたが部屋に入ったときひどい異臭が鼻を突いた。匂いに耐えながら部屋を進むと五感がおかしくなっていくようだ。部屋が七色に歪んで見え、ささやき声や叫び声が木霊してるように聞こえる…\n正気度を1失った。\n");
		printf("現在の正気度:%d\n", player_status[6]);
	}
	else if (randomint <= event[g_iRoomnumber].nothing) {
		printf("部屋に入ったが、特に何も起こらなかった。\n");
	}
	else {
		printf("###エラー発生###\nTrigger_Map関数のイベント選択でエラー\n");
	}
}//作成者…千原


void Player_Movement() {
	char key = 0;
	g_iMovement++;

	printf("%d回目の移動\nどの方向に移動するか矢印キーで決めてください。\n「d」キー…ポーションを使用、「w」キー…ステータス確認\n", g_iMovement);
	while (true)
	{
		key = _getch();
		if (key == 0xFFFFFFE0)
		{
			key = _getch();
			if (key == 0x4b) {
				if (player_status[0] <= 0) {
					printf("左隣の部屋はありません!\n");
				}
				else {
					player_status[0] -= 1;
					printf("\n左隣の部屋に移動した。\n");
					break;
				}
			}// ←
			else if (key == 0x4d) {
				if (player_status[0] >= 4) {
					printf("右隣の部屋はありません!\n");
				}
				else {
					player_status[0] += 1;
					printf("\n右隣の部屋に移動した。\n");
					break;
				}

			}// →
			else if (key == 0x48) {
				if (player_status[1] <= 0) {
					printf("上隣の部屋はありません!\n");
				}
				else {
					player_status[1] -= 1;
					printf("\n上隣の部屋に移動した。\n");
					break;
				}

			}// ↑
			else if (key == 0x50) {
				if (player_status[1] >= 4) {
					printf("下隣の部屋はありません!\n");
				}
				else {
					player_status[1] += 1;
					printf("\n下隣の部屋に移動した。\n");
					break;
				}

			}// ↓
			else {
				break;
			}
		}
		else if (key == 'w') {
			Display_Status();
		}
		else if (key == 'd') {
			Used_Potion(player_status);
		}
		else {
			printf("矢印キーを入力してください。\n");
		}
	}

	if (player_status[0] == 0 && player_status[1] == 0) {
		g_iRoomnumber = 7;
		printf("この部屋は安全な部屋だ。\n");
	}
	else if (player_status[0] == 4 && player_status[1] == 4) {
		g_iRoomnumber = 6; //ボス部屋
		printf("ボスの部屋だ!\n");
	}
	else if (player_status[0] == 2 && player_status[1] == 2) {
		g_iRoomnumber = 4;	//罠なし部屋
		printf("この部屋は罠のない部屋のようだ。\n");
	}
	else if (player_status[0] <= 1 && player_status[1] <= 1) {
		g_iRoomnumber = 2;	//弱敵
		printf("この部屋は弱敵が出現する部屋だ。\n");
		g_iEnemyRandMIN = 0;
	}
	else if (player_status[0] <= 1 && player_status[1] >= 3) {
		g_iRoomnumber = 0;	//強敵
		printf("この部屋は強敵が出現する部屋だ。\n");
		g_iEnemyRandMIN = 20;
	}
	else if (player_status[0] >= 3 && player_status[1] <= 1) {
		g_iRoomnumber = 5;	//回復部屋
		printf("この部屋はアイテムがより多く獲得できる部屋だ。\n");
		g_iEnemyRandMIN = 10;
	}
	else if (player_status[0] >= 3 && player_status[1] >= 3) {
		g_iRoomnumber = 3;	//門番の部屋
		printf("この部屋は門番が出現する部屋だ。\nボスの部屋までもうすぐだ\n");
		g_iEnemyRandMIN = 30;
	}
	else {
		g_iRoomnumber = 1;	//通常部屋
		printf("この部屋は普通の部屋だ。\n");
		g_iEnemyRandMIN = 10;
	}
	printf("\nキーを押してください\n");
	key = _getch();
	if (key == 0xFFFFFFE0)
	{
		_getch();
	}
	printf("\n");
}//作成者…千原

bool Finish_Game() {
	int key = 0;
	if (g_bMissionCompleted == true) {
		printf("\n!!ゲームクリア!!\n\
				\nあなたはダンジョンのボスであるドラゴンを打ち倒し生還するすることができました。\
				\nダンジョン内で手にした数々の財と異形へと踏み込んだその力を手に、\
				\nあなたは次の獲物へと歩みを進めるでしょう。\n");
	}
	else {
		printf("\nあなたの意識は暗い底へと沈む、それは肉体的な死を意味するのか\
				\nそれともこのダンジョンを闊歩する異形の一人と化してしまったのかは誰にもわからない\
				\n!!ゲームオーバー!!\n");
	}
	printf("リザルト:\n部屋の移動回数…%d回\n倒した敵の数…%d体\n終了時の正気度…%d\n", g_iMovement, g_iWonTheEnemy, player_status[6]);
	printf("「c」キーを押すともう一度遊びます。それ以外のキーを押すと終了します。\n");
	key = _getch();
	if (key == 'c') {
		return true;
	}
	else {
		return false;
	}
}//エンド分岐(小平)

int main() {
	while (g_bRunTheDungeon) {
		Initialize();
		Display_rule();
		first_Experience(5);
		while (player_status[3] > 0 && player_status[6] > 0 && g_bMissionCompleted == false) {
			Display_Map(player_status);
			Player_Movement();
			Triggerd_Map();
		}
		g_bRunTheDungeon = Finish_Game();
	}
	return 0;
}