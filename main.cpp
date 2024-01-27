#include<iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <algorithm>
#include <graphics.h>
#include <conio.h>
#include <tchar.h>	
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"MSIMG32.lib")
using namespace std;

/*
* 项目名称：表情大作战
* 开发环境：VS2022+EasyX
* 作       者：Kathwa
* 开发时间：2023/6/1
*/

#define WIDTH 1024
#define HEIGHT 768
//游戏帧率 1000ms/n = 60FPS
#define FPS 1000/60.0
//食物数量
#define FOOD_NUM 100
//人机数量
#define GAME_NUM 10
//宏定义函数 勾股定理求圆心距离
#define DISTANCE(p1,p2) sqrt(pow((p1->x - p2->x), 2) + pow((p1->y - p2->y), 2))
//食物 玩家复活时间 nS=60FPS 10S=10*nFPS
#define RESET_TIME 5*60
//游戏时长N分钟 n*60*60FPS 
#define GAME_TIME 1.5*60*60
//新手保护大小 20
#define PROTECT_R 20

struct Ball
{
	//玩家属性
	double x;
	double y;
	//半径
	double r;
	//移动速度
	double speed;
	//生或死
	bool life;
	//颜色
	COLORREF color;
	//人机参数 追击下标
	int index;
	//计时
	int cnt;
};

//玩家数组
Ball game[GAME_NUM];
//食物数组
Ball food[FOOD_NUM];
//对局时长
int gameTime = GAME_TIME;

//初始化球球
void initBall(Ball* ball, double x, double y, double r, double speed)
{
	ball->x = x;
	ball->y = y;
	ball->r = r;
	ball->speed = speed;
	ball->color = RGB(rand() % 256, rand() % 256, rand() % 256);
	ball->life = true;
	ball->index = -1;
	ball->cnt = 0;
	//strcpy_s(ball->name, sizeof(ball->name) / sizeof(char), name);
}

//随机初始化食物 人机 防止生成在玩家肚子里
void initBallAuto(Ball* ball, double r, double speed, Ball* game)
{
	//递归 直到随机生成的食物 人机不在玩家体内
	int x = rand() % (getwidth() - (int)r);
	int y = rand() % (getheight() - (int)r);
	initBall(ball, x, y, r, speed);
	if (DISTANCE(ball, game) < game->r)
	{
		initBallAuto(ball, r, speed, game);
	}
}

//初始化所有球球参数
void init()
{
	//置随机数种子
	srand(time(NULL));

	//初始化所有食物
	for (int i = 0; i < FOOD_NUM; i++)
	{
		initBallAuto(food + i, 3, 0, game);
	}

	//初始化玩家球球
	initBall(game, WIDTH / 2, HEIGHT / 2, 10, 2);

	//初始化所有玩家
	for (int i = 1; i < GAME_NUM; i++)
	{
		initBallAuto(game + i, 10, 1.5, game);
	}
}

//绘制角色
void drawBall(Ball* ball)
{
	if (ball->life)
	{
		//设置填充颜色
		setfillcolor(ball->color);

		//绘制圆
		solidcircle(ball->x, ball->y, ball->r);
	}
}

//绘制所有球球 自定义排序
bool cmp(const Ball& a, const Ball& b)
{
	return a.r < b.r;
}
void draw()
{
	//绘制所有食物
	for (int i = 0; i < FOOD_NUM; i++)
	{
		drawBall(food + i);
	}

	//绘制所有玩家 要保证大的覆盖小的
	//用于输出的临时数组
	Ball temp[GAME_NUM];
	for (int i = 0; i < GAME_NUM; i++)
	{
		temp[i] = game[i];
	}
	//自定义排序
	sort(temp, temp + GAME_NUM, cmp);
	//绘制所有玩家
	for (int i = 0; i < GAME_NUM; i++)
	{
		drawBall(temp + i);
		//showName(temp + i);
	}
}

//按键操作
void moveKey(Ball* game)
{
	if (game)
	{
		//斜向移动
		double realMove = game->speed;
		if (GetAsyncKeyState(VK_UP) && GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState(VK_UP) && GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState(VK_DOWN) && GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState(VK_DOWN) && GetAsyncKeyState(VK_RIGHT))
		{
			//斜向速度
			realMove *= sqrt(2) / 2;
		}
		//上
		if (GetAsyncKeyState(VK_UP))
		{
			//gamer->y -= realMove;
			for (int i = 0; i < FOOD_NUM; i++)
			{
				food[i].y += realMove;
			}
			for (int i = 1; i < GAME_NUM; i++)
			{
				game[i].y += realMove;
			}
		}
		//下
		if (GetAsyncKeyState(VK_DOWN))
		{
			//gamer->y += realMove;
			for (int i = 0; i < FOOD_NUM; i++)
			{
				food[i].y -= realMove;
			}
			for (int i = 1; i < GAME_NUM; i++)
			{
				game[i].y -= realMove;
			}
		}
		//左
		if (GetAsyncKeyState(VK_LEFT))
		{
			//gamer->x -= realMove;
			for (int i = 0; i < FOOD_NUM; i++)
			{
				food[i].x += realMove;
			}
			for (int i = 1; i < GAME_NUM; i++)
			{
				game[i].x += realMove;
			}
		}
		//右
		if (GetAsyncKeyState(VK_RIGHT))
		{
			//gamer->x += realMove;
			for (int i = 0; i < FOOD_NUM; i++)
			{
				food[i].x -= realMove;
			}
			for (int i = 1; i < GAME_NUM; i++)
			{
				game[i].x -= realMove;
			}
		}
	}
}

//吃食物
void eatFood(Ball* game, Ball* food)
{
	for (int i = 0; i < GAME_NUM; i++)
	{
		if (!game[i].life)
		{
			continue;
		}
		for (int j = 0; j < FOOD_NUM; j++)
		{
			if (!food[j].life)
			{
				continue;
			}
			//食物存在 且 圆心距离小于玩家球球半径 吃
			if (DISTANCE((game + i), (food + j)) < game[i].r && game[i].life && food[j].life)
			{
				//变肥
				game[i].r += sqrt(food[j].r);
				//食物没了
				food[j].life = false;
				food[j].cnt -= RESET_TIME;
			}
		}
	}
}

//吃玩家
void eatGame(Ball* game)
{
	//玩家ij都活着 且 玩家ij过了新手保护 且 玩家i半径大于玩家j的110% 圆心距离小于玩家i半径 i吃j
	for (int i = 0; i < GAME_NUM; i++)
	{
		if (!game[i].life)
		{
			continue;
		}
		for (int j = 0; j < GAME_NUM; j++)
		{
			if (!game[j].life || i == j)
			{
				continue;
			}
			if (game[i].r < PROTECT_R || game[j].r < PROTECT_R)
			{
				continue;
			}
			if (game[i].r <= game[j].r * 1.1)
			{
				continue;
			}
			if (DISTANCE((game + i), (game + j)) < game[i].r)
			{
				//变肥
				game[i].r += sqrt(game[j].r);
				//玩家[j]没了
				game[j].life = false;
				game[j].cnt -= RESET_TIME;
				//播放吞食音效
				mciSendString(_T("stop eatVoice"), 0, 0, 0);
				mciSendString(_T("close eatVoice"), 0, 0, 0);
				mciSendString(_T("open ./Resource/Eat.wav alias eatVoice type mpegvideo"), 0, 0, 0);
				mciSendString(_T("play eatVoice"), 0, 0, 0);
			}
		}
	}
}

//追击
void chaseGame(Ball* gameA, Ball* gameB)
{
	//gameA追
	//斜向移动判定
	double realMove = gameA->speed;
	if (gameA->x > gameB->x && gameA->y > gameB->y || gameA->x > gameB->x && gameA->y < gameB->y || gameA->x < gameB->x && gameA->y > gameB->y || gameA->x < gameB->x && gameA->y < gameB->y)
	{
		realMove *= sqrt(2) / 2;
	}
	//判断run位置 追
	if (gameA->x > gameB->x)
	{
		gameA->x -= realMove;
	}
	else
	{
		gameA->x += realMove;
	}
	if (gameA->y > gameB->y)
	{
		gameA->y -= realMove;
	}
	else
	{
		gameA->y += realMove;
	}
}

//逃跑
void runGame(Ball* gameA, Ball* gameB)
{
	//gameA逃
	//斜向移动判定
	double realMove = gameA->speed;
	if (gameA->x > gameB->x && gameA->y > gameB->y || gameA->x > gameB->x && gameA->y < gameB->y || gameA->x < gameB->x && gameA->y > gameB->y || gameA->x < gameB->x && gameA->y < gameB->y)
	{
		realMove *= sqrt(2) / 2;
	}
	//判断run位置 逃
	if (gameA->x > gameB->x)
	{
		gameA->x += realMove;
	}
	else
	{
		gameA->x -= realMove;
	}
	if (gameA->y > gameB->y)
	{
		gameA->y += realMove;
	}
	else
	{
		gameA->y -= realMove;
	}
}

//人机目标锁定
void moveGame(Ball* game, Ball* food)
{
	//规则 若人机比玩家(包含人机)大 则追玩家 反之 则吃食物 
	for (int i = 1; i < GAME_NUM; i++)
	{
		if (!game[i].life)
		{
			continue;
		}
		//若人机比玩家大 且 玩家过了新手保护  且玩家活着 人机优先追玩家
		//index缺省值-1 人机index值标记为-2 追玩家
		if (game[i].r > game->r && game->r > PROTECT_R && game->life)
		{
			game[i].index = -2;
			continue;
		}
		//若人机比玩家小 追食物
		double minDistance = getwidth();
		for (int j = 0; j < FOOD_NUM; j++)
		{
			if (!food[j].life)
			{
				continue;
			}
			double distance = DISTANCE((game + i), (food + j));
			if (distance < minDistance)
			{
				minDistance = distance;
				game[i].index = j;
			}
			//让每个玩家都关联离它最近的食物
		}
	}
	//战或逃
	for (int i = 1; i < GAME_NUM; i++)
	{
		if (!game[i].life)
		{
			continue;
		}
		if (game[i].index < 0)
		{
			//追击玩家
			chaseGame(game + i, game);
			continue;
		}
		//如果球比玩家小 且距离小于等于玩家半径的300% 跑路 直到跑出了范围为止
		if (game[i].r < game->r && DISTANCE((game + i), game) <= game->r * 3)
		{
			//逃跑
			runGame(game + i, game);
			continue;
		}
		//追击食物
		chaseGame(game + i, food + game[i].index);
	}
}

//球球复活
void resetBall(Ball* game, Ball* food) {
	for (int i = 0; i < GAME_NUM; i++)
	{
		if (game[i].life)
		{
			continue;
		}
		if (game[i].cnt == 0)
		{
			initBallAuto(game + i, 10, game[i].speed, game);
			game[i].life = true;
		}
		game[i].cnt++;
	}
	for (int i = 0; i < FOOD_NUM; i++)
	{
		if (food[i].life)
		{
			continue;
		}
		if (food[i].cnt == 0)
		{
			initBallAuto(food + i, 3, food[i].speed, game);
			food[i].life = true;
		}
		food[i].cnt++;
	}
}

//自动瘦身
void autoSlimGame(Ball* game)
{
	//如果没吃饭 就瘦身 避免球球过大
	//每60FPS瘦身一次 瘦身值根据体重增长而线性增长
	for (int i = 0; i < GAME_NUM; i++)
	{
		if (!game[i].life)
		{
			continue;
		}
		if (game[i].r > PROTECT_R && !(game[i].cnt % 60))
		{
			game[i].r -= game[i].r / 100;
		}
		game[i].cnt++;
	}
}

//字符显示
void showString()
{
	//显示分数
	TCHAR grade[100];
	sprintf_s(grade, _T("%d"), int(game->r));//用sprintf把整型分数转字符串
	setbkmode(TRANSPARENT);	//透明方式输出,不改变背景
	settextcolor(BLACK);
	//settextstyle(20, 0, _T("微软雅黑"));
	outtextxy(40, 20, _T("体重:"));
	outtextxy(80, 20, grade);

	//显示倒计时
	TCHAR m[100];
	TCHAR s[100];
	sprintf_s(m, _T("%d"), gameTime / 3600);
	sprintf_s(s, _T("%d"), (gameTime % 3600) / 60);
	outtextxy(WIDTH / 2 - 30, 20, m);
	outtextxy(WIDTH / 2 - 20, 20, _T(":"));
	outtextxy(WIDTH / 2 - 10, 20, s);
	gameTime--;
}

//图片资源
IMAGE bk1;
IMAGE bk2;
IMAGE bk3;

//读取图片
void loadRes()
{
	loadimage(&bk1, _T("Resource\\bk1.png"), WIDTH, HEIGHT);
	loadimage(&bk2, _T("Resource\\bk2.png"), WIDTH, HEIGHT);
	loadimage(&bk3, _T("Resource\\bk3.png"), WIDTH, HEIGHT);
}

//开始游戏
void start()
{
	//创建图形窗口
	initgraph(WIDTH, HEIGHT);
	loadRes();

	// 开场界面
	putimage(0, 0, &bk1);
	while (!_kbhit()) {}
	putimage(0, 0, &bk2);
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	outtextxy(WIDTH / 2 - 50, 20, _T("游戏加载中......"));
	Sleep(3000);

	//设置背景颜色
	setbkcolor(RGB(230, 230, 230));
	//全局初始化
	init();
	cleardevice();

	//播放背景音乐
	mciSendString(_T("open ./Resource/BGM.wav alias bkMusic type mpegvideo"), 0, 0, 0);
	mciSendString(_T("play bkMusic repeat"), 0, 0, 0);
}

//结束游戏
void end()
{
	mciSendString(_T("close bkMusic"), 0, 0, 0);
	putimage(0, 0, &bk3);

	int constY = 130;

	settextstyle(50, 0, "幼圆");
	outtextxy(WIDTH / 2 - 100, constY+20, _T("游戏结束"));

	TCHAR grade[100];
	sprintf_s(grade, _T("%d"), int(game->r));
	settextstyle(30, 0, "微软雅黑");
	outtextxy(WIDTH / 2 - 50, constY + 180, _T("最终体重是"));

	settextstyle(50, 0, "微软雅黑");
	outtextxy(WIDTH / 2 - 30, constY + 220, grade);

	settextstyle(30, 0, "微软雅黑");
	if (game[0].life)
	{
		outtextxy(WIDTH / 2 - 90, constY + 280, "恭喜你存活到了最后");
	}
	else
	{
		outtextxy(WIDTH / 2 - 140, constY + 280, "你没有存活到最后，建议重开");
	}
	Sleep(30000);
	//结束渲染图形
	closegraph();
}

int main(void)
{
	start();
	while (true)
	{
		int startTime = clock();
		if (gameTime <= 0 || !game[0].life)
		{
			end();
		}
		//开启双缓冲 避免闪屏
		BeginBatchDraw();
		//先清屏再画
		cleardevice();
		//画背景格子 i+=20 20px*20px
		setlinecolor(RGB(204, 204, 204));
		for (int i = 0; i < getwidth(); i += 20)
		{
			line(i, 0, i, getheight());//画竖线 有两个参数(点) 两点确定一个线段, 画竖线(高度）
			line(0, i, getwidth(), i); //画竖线(高度）
		}
		//画球球
		draw();
		//输出字符
		showString();

		//玩家移动
		moveKey(game);
		moveGame(game, food);
		eatFood(game, food);
		eatGame(game);

		//自动瘦身
		autoSlimGame(game);
		//球球复活
		resetBall(game, food);

		//关闭双缓冲
		EndBatchDraw();

		//固定帧率为FPS
		int frameTime = clock() - startTime;
		if (frameTime < FPS)
		{
			Sleep(FPS - frameTime);
		}
	}

	end();
	return 0;
}

