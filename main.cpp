#include <graphics.h>
#include <conio.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <easyx.h>
#include<string>

using namespace std;

// 引用 Windows Multimedia API
#pragma comment(lib,"Winmm.lib")

#define High 800  
#define Width 800			//游戏画面尺寸
int numbers[100];			//分数数据
int save_data[3] = { 0 };	//读档数据

IMAGE img_bk;		//背景图片

IMAGE img_plane_Normal1, img_plane_Normal2; //玩家飞机图片
IMAGE img_planeExplode1, img_planeExplode2; //玩家飞机爆炸图片
struct Player {								////玩家飞机位置坐标
	int x;
	int y;
}player;

IMAGE img_enemyPlane1, img_enemyPlane2;		//敌机图片
float enemy_x, enemy_y;						//敌机位置

IMAGE img_bullet[2];						//玩家子弹图片
float bullet_x, bullet_y;					//玩家飞机子弹位置

IMAGE img_enemy_bullet[2];                  //敌机子弹图片
float enemy_bullet_x, enemy_bullet_y;       //敌机子弹位置

int isExpolde = 0;							//飞机是否爆炸
int score = 0;								//得分
int position = 10;							//用于动态更新排行榜定位用

IMAGE img_begin;							//启动背景图片
IMAGE img_over;							    //结束背景图片

bool isEscPressed = false;					//是否按ESC进行分数存档


void begin(IMAGE img_begin) {				//启动界面展示函数

	putimage(0, 0, &img_begin);

	//TOP10分数显示处理
	outtextxy(Width * 0.85, High * 0.1, "TOP10 score:");
	char temp[5];
	for (int i = 0; i < 10; i++) {
		sprintf_s(temp, "%d", numbers[i]);
		outtextxy(Width * 0.85, High * (0.13 + (float)(i * 0.03)), temp);
	}

	//鼠标变量定义：判断鼠标点击位置
	ExMessage msg;
	while (1) {
		peekmessage(&msg);
		if (msg.message == WM_LBUTTONDOWN) {		//按下左键的时候进行鼠标位置判断
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
				score = save_data[0];                 //更新score 
				player.x = save_data[1];
				player.y = save_data[2];
				FILE* ffp = fopen("./data.txt", "w");
				int newnum = 0;
				int new_x = Width * 0.55;
				int new_y = High * 0.8;
				fprintf(ffp, "%d\n", newnum);		   //读取之后先将save清0   
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
	mciSendString("open ./sourse/game_music.mp3 alias bkmusic", NULL, 0, NULL);//打开背景音乐
	mciSendString("play bkmusic repeat", NULL, 0, NULL);					   //设置背景音乐循环播放

	initgraph(Width, High);

	//下列均为图片图片加载
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
	player.y = High * 0.8;			  //玩家飞机初始位置

	bullet_x = player.x;
	bullet_y = -85;					  //玩家飞机子弹位置

	enemy_x = Width * 0.6;
	enemy_y = 10;					  //敌机初始位置

	enemy_bullet_x = enemy_x + 45;
	enemy_bullet_y = enemy_y + 125;	  //敌机子弹位置

	begin(img_begin);

	BeginBatchDraw();
}

bool cmp(int a, int b) {
	return a > b;
}

void show()
{
	putimage(0, 0, &img_bk);		//显示背景	
	if (isExpolde == 0) {			//玩家飞机未爆炸	
		putimage(player.x - 50, player.y - 60, &img_plane_Normal1, SRCPAINT);		//显示玩家正常飞机	
		putimage(player.x - 50, player.y - 60, &img_plane_Normal2, SRCAND);

		putimage(bullet_x - 7, bullet_y, img_bullet + 0, SRCINVERT);				//显示子弹	         
		putimage(bullet_x - 7, bullet_y, img_bullet + 1, NOTSRCERASE);

		putimage(enemy_x, enemy_y, &img_enemyPlane1, SRCPAINT);						//显示敌机	
		putimage(enemy_x, enemy_y, &img_enemyPlane2, SRCAND);

		putimage(enemy_bullet_x, enemy_bullet_y, img_enemy_bullet + 0, SRCPAINT);   //显示敌机子弹
		putimage(enemy_bullet_x, enemy_bullet_y, img_enemy_bullet + 1, SRCAND);
	}
	else {							//玩家飞机爆炸	
		putimage(player.x - 50, player.y - 60, &img_planeExplode1, SRCPAINT);		//显示爆炸飞机	
		putimage(player.x - 50, player.y - 60, &img_planeExplode2, SRCAND);

	}

	//分数动态更新	
	numbers[position] = score;														//position为全局变量
	for (int i = 0; i <= position - 1; i++) {
		if (score == numbers[i]) {
			numbers[position] = numbers[i];
			position = i;
		}
	}

	//TOP10分数显示处理
	outtextxy(Width * 0.85, High * 0.1, "TOP10 score:");
	char temp[5];
	for (int i = 0; i < 10; i++) {
		sprintf_s(temp, "%d", numbers[i]);
		outtextxy(Width * 0.85, High * (0.13 + (float)(i * 0.03)), temp);
	}

	//玩家游玩过程分数展示
	outtextxy(Width * 0.48, High * 0.95, "Score：");
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
			bullet_y = bullet_y - 5;	  //玩家子弹位移
		}

		if (enemy_y < High - 25) {        //敌机位移
			enemy_y = enemy_y + (double)0.5 + (double)(score / 20) * 0.25;
			//敌机的子弹和的敌机的速度每20分速度变快一次
		}
		else {
			enemy_y = 10;
		}

		if (enemy_bullet_y == -20 || enemy_bullet_y >= High) {		//初始化敌机子弹位置
			enemy_bullet_y = enemy_y + 125;
		}

		if (enemy_bullet_y < High) {							//敌机子弹位移
			enemy_bullet_y = enemy_bullet_y + (double)1.5 + (double)(score / 20) * 0.4;
		}
		if (bullet_x >= enemy_x && bullet_x <= enemy_x + 100 && bullet_y <= enemy_y + 100) {  //子弹击中敌机

			enemy_x = rand() % Width;				//击中后重置敌机x坐标
			if (enemy_x >= 700) enemy_x -= 100;
			enemy_bullet_x = enemy_x + 45;			//重置敌机x坐标的同时 重置敌机子弹的x
			enemy_y = -40;
			bullet_y = -85;							//击中敌机，将敌机和子弹丢到外面去
			enemy_bullet_y = -20;

			//玩家飞机爆炸音乐
			mciSendString("close gemusic", NULL, 0, NULL);
			mciSendString("open ./sourse/gotEnemy.mp3 alias gemusic", NULL, 0, NULL);
			mciSendString("play gemusic", NULL, 0, NULL);
			score++;
		}

		if (enemy_y >= 700) {	//敌机未爆炸飞到程序界面外：则重置敌机坐标

			enemy_x = rand() % Width;
			if (enemy_x >= 700) enemy_x -= 100;
			enemy_bullet_x = enemy_x + 45;
			enemy_y = -40;
			bullet_y = -85;
			enemy_bullet_y = -20;
		}
		if (player.x >= enemy_x && player.x <= enemy_x + 100 && player.y >= enemy_y && player.y <= enemy_y + 180 || (enemy_bullet_y >= player.y && enemy_bullet_y <= player.y + 30 && (enemy_bullet_x >= player.x - 50 && enemy_bullet_x <= player.x + 50)))
		{	//玩家飞机碰撞敌机或者敌机子弹击中玩家飞机
			isExpolde = 1;

			//玩家飞机爆炸音乐
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
		bullet_y = player.y - 85;			//设置玩家飞机子弹坐标

		//打开子弹发射音乐
		mciSendString("close fgmusic", NULL, 0, NULL);
		mciSendString("open ./sourse/f_gun.mp3 alias fgmusic", NULL, 0, NULL);
		mciSendString("play fgmusic", NULL, 0, NULL);

	}
}

void gameover() {

	EndBatchDraw();        //结束双缓冲绘图

	putimage(0, 0, &img_over);
	//此处显示结束图片，为数据处理提供时间/此处显示结束图片
	if (isExpolde == 1) {
		//将数据添加到list即可
		FILE* fp;
		fp = fopen("list.txt", "a"); // 以追加模式打开文件
		fprintf(fp, "%d", score);	 // 向文件末尾写入数据（新的一行）
		fclose(fp);
	}

	MOUSEMSG m;					 // 定义鼠标消息
	while (1) {
		setbkcolor(WHITE);
		settextcolor(BLUE);
		settextstyle(30, 0, "黑体");
		if (isEscPressed) {
			char str1[] = { "按了ESC，分数,坐标已保存" };
			outtextxy(Width * 0.28, High * 0.35, str1);
		}
		else {
			char str1[] = { "你的飞机已陨落" };
			outtextxy(Width * 0.37, High * 0.35, str1);
		}
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN) {
			closegraph();       //结束图片加载
			break;				//跳出while循环，结束游戏
		}
	}
}


void data_initialize() {

	FILE* fp = fopen("./list.txt", "r");			 //读取list中的分数：用number数组储存
	int cnt = 0;
	char buffer[10] = { 0 };
	while (fgets(buffer, sizeof(buffer), fp)) {
		numbers[cnt++] = atoi(buffer);               //数字字符转为int
	}

	sort(numbers, numbers + cnt, cmp);				 //将list中读入的分数排序

	fclose(fp);

	FILE* fp2 = fopen("./list.txt", "w");			 //将排序好的分数覆盖原来的分数：是的TOP10按照从大到小进行排序
	for (int i = 0; i < cnt; i++) {
		fprintf(fp2, "%d\n", numbers[i]);
		//文件操作  对象fp  方式int 加换行  处理数据 numbers数组
	}
	fclose(fp);
}

int main() {

	data_initialize();		//分数数据初始化

	load();					//数据初始化	//数据载入


	while (1) {				//游戏死循环执行

		show();				//图片加载	
		Auto_Update();		//位置坐标自动化函数	    
		Update_Input();     //玩家操作函数

		cleardevice();		//刷新加载图片：防止重影

		if (isExpolde == 1) {			//玩家飞机碰撞或者被击中
			show();						//用来再加载爆炸图片
			Sleep(1000);				//显示1秒的爆炸瞬间
			break;
		}

		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {	//按下ESC进行存档：结束当前游戏
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
	//游戏结束、后续处理        
	//记录分数，进行读盘操作  
	return 0;
}