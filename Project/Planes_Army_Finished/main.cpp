#include <graphics.h>
#include <conio.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <easyx.h>
#include<string>
#include<bits/stdc++.h>

using namespace std;

// ���� Windows Multimedia API
#pragma comment(lib,"Winmm.lib")

#define High 800  
#define Width 800			//��Ϸ����ߴ�
int numbers[100];			//��������
int save_data[3] = { 0 };	//��������

IMAGE img_bk;		//����ͼƬ

IMAGE img_plane_Normal1, img_plane_Normal2; //��ҷɻ�ͼƬ
IMAGE img_planeExplode1, img_planeExplode2; //��ҷɻ���ըͼƬ
struct Player {								////��ҷɻ�λ������
	int x;
	int y;
}player;

IMAGE img_enemyPlane1, img_enemyPlane2;		//�л�ͼƬ
float enemy_x, enemy_y;						//�л�λ��

IMAGE img_bullet[2];						//����ӵ�ͼƬ
float bullet_x, bullet_y;					//��ҷɻ��ӵ�λ��

IMAGE img_enemy_bullet[2];                  //�л��ӵ�ͼƬ
float enemy_bullet_x, enemy_bullet_y;       //�л��ӵ�λ��

int isExpolde = 0;							//�ɻ��Ƿ�ը
int score = 0;								//�÷�
int position = 10;							//���ڶ�̬�������а�λ��

IMAGE img_begin;							//��������ͼƬ
IMAGE img_over;							    //��������ͼƬ

bool isEscPressed = false;					//�Ƿ�ESC���з����浵


void begin(IMAGE img_begin) {				//��������չʾ����

	putimage(0, 0, &img_begin);

	//TOP10������ʾ����
	outtextxy(Width * 0.85, High * 0.1, "TOP10 score:");
	char temp[5];
	for (int i = 0; i < 10; i++) {
		sprintf_s(temp, "%d", numbers[i]);
		outtextxy(Width * 0.85, High * (0.13 + (float)(i * 0.03)), temp);
	}

	//���������壺�ж������λ��
	ExMessage msg;
	while (1) {
		peekmessage(&msg);
		if (msg.message == WM_LBUTTONDOWN) {		//���������ʱ��������λ���ж�
			if (msg.x >= 165 && msg.x <= 639 && msg.y >= 145 && msg.y <= 266) {
				break;
			}
			if (msg.x >= 172 && msg.x <= 640 && msg.y >= 411 && msg.y <= 534) {
				FILE* fp = fopen("./data.txt", "r");
				char buffer[10] = { 0 };
				int tmp_cnt = 0;
				while (fgets(buffer, sizeof(buffer), fp)) {
					save_data[tmp_cnt++] = atoi(buffer);
				}
				fclose(fp);
				score = save_data[0];                 //����score 
				player.x = save_data[1];
				player.y = save_data[2];
				FILE* ffp = fopen("./data.txt", "w");
				int newnum = 0;
				int new_x = Width * 0.55;
				int new_y = High * 0.8;
				fprintf(ffp, "%d\n", newnum);		   //��ȡ֮���Ƚ�save��0   
				fprintf(ffp, "%d\n", new_x);
				fprintf(ffp, "%d", new_y);

				fclose(ffp);
				break;
			}
		}
	}
}

void load()
{
	mciSendString("open ./sourse/game_music.mp3 alias bkmusic", NULL, 0, NULL);//�򿪱�������
	mciSendString("play bkmusic repeat", NULL, 0, NULL);					   //���ñ�������ѭ������

	initgraph(Width, High);

	//���о�ΪͼƬͼƬ����
	loadimage(&img_bk, "./sourse/background_image.jpg", 800, 800);
	loadimage(&img_plane_Normal1, "./sourse/planeNormal_1.jpg");
	loadimage(&img_plane_Normal2, "./sourse/planeNormal_2.jpg");
	loadimage(img_bullet + 0, "./sourse/bullet1.jpg");
	loadimage(img_bullet + 1, ",/sourse/bullet2.jpg");
	loadimage(&img_enemyPlane1, "./sourse/enemyPlane1.jpg");
	loadimage(&img_enemyPlane2, "./sourse/enemyPlane2.jpg");
	loadimage(&img_planeExplode1, "./sourse/planeExplode_1.jpg");
	loadimage(&img_planeExplode2, "./sourse/planeExplode_2.jpg");
	loadimage(&img_over, "./sourse/over.jpg");
	loadimage(img_enemy_bullet + 0, "./sourse/enemy_bullet1.jpg");
	loadimage(img_enemy_bullet + 1, "./sourse/enemy_bullet2.jpg");
	loadimage(&img_over, "./sourse/over.jpg", 800, 800);
	loadimage(&img_begin, "./sourse/begin.jpg", 800, 800);

	player.x = Width * 0.5;
	player.y = High * 0.8;			  //��ҷɻ���ʼλ��

	bullet_x = player.x;
	bullet_y = -85;					  //��ҷɻ��ӵ�λ��

	enemy_x = Width * 0.6;
	enemy_y = 10;					  //�л���ʼλ��

	enemy_bullet_x = enemy_x + 45;
	enemy_bullet_y = enemy_y + 125;	  //�л��ӵ�λ��

	begin(img_begin);

	BeginBatchDraw();
}

bool cmp(int a, int b) {
	return a > b;
}

void show()
{
	putimage(0, 0, &img_bk);		//��ʾ����	
	if (isExpolde == 0) {			//��ҷɻ�δ��ը	
		putimage(player.x - 50, player.y - 60, &img_plane_Normal1, SRCPAINT);		//��ʾ��������ɻ�	
		putimage(player.x - 50, player.y - 60, &img_plane_Normal2, SRCAND);

		putimage(bullet_x - 7, bullet_y, img_bullet + 0, SRCINVERT);				//��ʾ�ӵ�	         
		putimage(bullet_x - 7, bullet_y, img_bullet + 1, NOTSRCERASE);

		putimage(enemy_x, enemy_y, &img_enemyPlane1, SRCPAINT);						//��ʾ�л�	
		putimage(enemy_x, enemy_y, &img_enemyPlane2, SRCAND);

		putimage(enemy_bullet_x, enemy_bullet_y, img_enemy_bullet + 0, SRCPAINT);   //��ʾ�л��ӵ�
		putimage(enemy_bullet_x, enemy_bullet_y, img_enemy_bullet + 1, SRCAND);
	}
	else {							//��ҷɻ���ը	
		putimage(player.x - 50, player.y - 60, &img_planeExplode1, SRCPAINT);		//��ʾ��ը�ɻ�	
		putimage(player.x - 50, player.y - 60, &img_planeExplode2, SRCAND);

	}

	//������̬����	
	numbers[position] = score;														//positionΪȫ�ֱ���
	for (int i = 0; i <= position - 1; i++) {
		if (score == numbers[i]) {
			numbers[position] = numbers[i];
			position = i;
		}
	}

	//TOP10������ʾ����
	outtextxy(Width * 0.85, High * 0.1, "TOP10 score:");
	char temp[5];
	for (int i = 0; i < 10; i++) {
		sprintf_s(temp, "%d", numbers[i]);
		outtextxy(Width * 0.85, High * (0.13 + (float)(i * 0.03)), temp);
	}

	//���������̷���չʾ
	outtextxy(Width * 0.48, High * 0.95, "Score��");
	char s[5];
	sprintf_s(s, "%d", score);
	outtextxy(Width * 0.55, High * 0.95, s);
	FlushBatchDraw();
	Sleep(2);
}

void Auto_Update()
{
	if (isExpolde == 0) {

		if (bullet_y > -25) {
			bullet_y = bullet_y - 5;	  //����ӵ�λ��
		}

		if (enemy_y < High - 25) {        //�л�λ��
			enemy_y = enemy_y + (double)0.5 + (double)(score / 20) * 0.25;
			//�л����ӵ��͵ĵл����ٶ�ÿ20���ٶȱ��һ��
		}
		else {
			enemy_y = 10;
		}

		if (enemy_bullet_y == -20 || enemy_bullet_y >= High) {		//��ʼ���л��ӵ�λ��
			enemy_bullet_y = enemy_y + 125;
		}

		if (enemy_bullet_y < High) {							//�л��ӵ�λ��
			enemy_bullet_y = enemy_bullet_y + (double)1.5 + (double)(score / 20) * 0.4;
		}
		if (bullet_x >= enemy_x && bullet_x <= enemy_x + 100 && bullet_y <= enemy_y + 100) {  //�ӵ����ел�

			enemy_x = rand() % Width;				//���к����õл�x����
			if (enemy_x >= 700) enemy_x -= 100;
			enemy_bullet_x = enemy_x + 45;			//���õл�x�����ͬʱ ���õл��ӵ���x
			enemy_y = -40;
			bullet_y = -85;							//���ел������л����ӵ���������ȥ
			enemy_bullet_y = -20;

			//��ҷɻ���ը����
			mciSendString("close gemusic", NULL, 0, NULL);
			mciSendString("open ./sourse/gotEnemy.mp3 alias gemusic", NULL, 0, NULL);
			mciSendString("play gemusic", NULL, 0, NULL);
			score++;
		}

		if (enemy_y >= 700) {	//�л�δ��ը�ɵ���������⣺�����õл�����

			enemy_x = rand() % Width;
			if (enemy_x >= 700) enemy_x -= 100;
			enemy_bullet_x = enemy_x + 45;
			enemy_y = -40;
			bullet_y = -85;
			enemy_bullet_y = -20;
		}
		if (player.x >= enemy_x && player.x <= enemy_x + 100 && player.y >= enemy_y && player.y <= enemy_y + 180 || (enemy_bullet_y >= player.y && enemy_bullet_y <= player.y + 30 && (enemy_bullet_x >= player.x - 50 && enemy_bullet_x <= player.x + 50)))
		{	//��ҷɻ���ײ�л����ߵл��ӵ�������ҷɻ�
			isExpolde = 1;

			//��ҷɻ���ը����
			mciSendString("close exmusic", NULL, 0, NULL);
			mciSendString("open ./sourse/explode.mp3 alias exmusic", NULL, 0, NULL);
			mciSendString("play exmusic", NULL, 0, NULL);
		}
	}
}

void Update_Input()
{
	if (isExpolde == 0) {
		Sleep(1);
		if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W')) {
			if (player.y > 0) {
				player.y -= 2;
			}
		}
		if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S')) {
			if (player.y < 800) {
				player.y += 2;
			}
		}
		if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A')) {
			if (player.x > 0) {
				player.x -= 2;
			}
		}
		if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D')) {
			if (player.x < 800) {
				player.x += 2;
			}
		}
	}
	if (GetAsyncKeyState(VK_SPACE) && bullet_y <= 0) {
		bullet_x = player.x;
		bullet_y = player.y - 85;			//������ҷɻ��ӵ�����

		//���ӵ���������
		mciSendString("close fgmusic", NULL, 0, NULL);
		mciSendString("open ./sourse/f_gun.mp3 alias fgmusic", NULL, 0, NULL);
		mciSendString("play fgmusic", NULL, 0, NULL);

	}
}

void gameover() {

	EndBatchDraw();        //����˫�����ͼ

	putimage(0, 0, &img_over);
	//�˴���ʾ����ͼƬ��Ϊ���ݴ����ṩʱ��/�˴���ʾ����ͼƬ
	if (isExpolde == 1) {
		//���������ӵ�list����
		FILE* fp;
		fp = fopen("list.txt", "a"); // ��׷��ģʽ���ļ�
		fprintf(fp, "%d", score);	 // ���ļ�ĩβд�����ݣ��µ�һ�У�
		fclose(fp);
	}

	MOUSEMSG m;					 // ���������Ϣ
	while (1) {
		setbkcolor(WHITE);
		settextcolor(BLUE);
		settextstyle(30, 0, "����");
		if (isEscPressed) {
			char str1[] = { "����ESC������,�����ѱ���" };
			outtextxy(Width * 0.28, High * 0.35, str1);
		}
		else {
			char str1[] = { "��ķɻ�������" };
			outtextxy(Width * 0.37, High * 0.35, str1);
		}
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN) {
			closegraph();       //����ͼƬ����
			break;				//����whileѭ����������Ϸ
		}
	}
}


void data_initialize() {

	FILE* fp = fopen("./list.txt", "r");			 //��ȡlist�еķ�������number���鴢��
	int cnt = 0;
	char buffer[10] = { 0 };
	while (fgets(buffer, sizeof(buffer), fp)) {
		numbers[cnt++] = atoi(buffer);               //�����ַ�תΪint
	}

	sort(numbers, numbers + cnt, cmp);				 //��list�ж���ķ�������

	fclose(fp);

	FILE* fp2 = fopen("./list.txt", "w");			 //������õķ�������ԭ���ķ������ǵ�TOP10���մӴ�С��������
	for (int i = 0; i < cnt; i++) {
		fprintf(fp2, "%d\n", numbers[i]);
		//�ļ�����  ����fp  ��ʽint �ӻ���  �������� numbers����
	}
	fclose(fp);
}

int main() {

	data_initialize();		//�������ݳ�ʼ��

	load();					//���ݳ�ʼ��	//��������


	while (1) {				//��Ϸ��ѭ��ִ��

		show();				//ͼƬ����	
		Auto_Update();		//λ�������Զ�������	    
		Update_Input();     //��Ҳ�������

		cleardevice();		//ˢ�¼���ͼƬ����ֹ��Ӱ

		if (isExpolde == 1) {			//��ҷɻ���ײ���߱�����
			show();						//�����ټ��ر�ըͼƬ
			Sleep(1000);				//��ʾ1��ı�ը˲��
			break;
		}

		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {	//����ESC���д浵��������ǰ��Ϸ
			isEscPressed = true;
			FILE* ffp = fopen("./data.txt", "w");
			int newnum = score;
			fprintf(ffp, "%d\n", newnum);
			fprintf(ffp, "%d\n", player.x);
			fprintf(ffp, "%d", player.y);

			fclose(ffp);
			break;
		}
	}

	gameover();
	//��Ϸ��������������        
	//��¼���������ж��̲���  
	return 0;
}