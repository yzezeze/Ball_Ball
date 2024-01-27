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
* ��Ŀ���ƣ��������ս
* ����������VS2022+EasyX
* ��       �ߣ�Kathwa
* ����ʱ�䣺2023/6/1
*/

#define WIDTH 1024
#define HEIGHT 768
//��Ϸ֡�� 1000ms/n = 60FPS
#define FPS 1000/60.0
//ʳ������
#define FOOD_NUM 100
//�˻�����
#define GAME_NUM 10
//�궨�庯�� ���ɶ�����Բ�ľ���
#define DISTANCE(p1,p2) sqrt(pow((p1->x - p2->x), 2) + pow((p1->y - p2->y), 2))
//ʳ�� ��Ҹ���ʱ�� nS=60FPS 10S=10*nFPS
#define RESET_TIME 5*60
//��Ϸʱ��N���� n*60*60FPS 
#define GAME_TIME 1.5*60*60
//���ֱ�����С 20
#define PROTECT_R 20

struct Ball
{
	//�������
	double x;
	double y;
	//�뾶
	double r;
	//�ƶ��ٶ�
	double speed;
	//������
	bool life;
	//��ɫ
	COLORREF color;
	//�˻����� ׷���±�
	int index;
	//��ʱ
	int cnt;
};

//�������
Ball game[GAME_NUM];
//ʳ������
Ball food[FOOD_NUM];
//�Ծ�ʱ��
int gameTime = GAME_TIME;

//��ʼ������
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

//�����ʼ��ʳ�� �˻� ��ֹ��������Ҷ�����
void initBallAuto(Ball* ball, double r, double speed, Ball* game)
{
	//�ݹ� ֱ��������ɵ�ʳ�� �˻������������
	int x = rand() % (getwidth() - (int)r);
	int y = rand() % (getheight() - (int)r);
	initBall(ball, x, y, r, speed);
	if (DISTANCE(ball, game) < game->r)
	{
		initBallAuto(ball, r, speed, game);
	}
}

//��ʼ�������������
void init()
{
	//�����������
	srand(time(NULL));

	//��ʼ������ʳ��
	for (int i = 0; i < FOOD_NUM; i++)
	{
		initBallAuto(food + i, 3, 0, game);
	}

	//��ʼ���������
	initBall(game, WIDTH / 2, HEIGHT / 2, 10, 2);

	//��ʼ���������
	for (int i = 1; i < GAME_NUM; i++)
	{
		initBallAuto(game + i, 10, 1.5, game);
	}
}

//���ƽ�ɫ
void drawBall(Ball* ball)
{
	if (ball->life)
	{
		//���������ɫ
		setfillcolor(ball->color);

		//����Բ
		solidcircle(ball->x, ball->y, ball->r);
	}
}

//������������ �Զ�������
bool cmp(const Ball& a, const Ball& b)
{
	return a.r < b.r;
}
void draw()
{
	//��������ʳ��
	for (int i = 0; i < FOOD_NUM; i++)
	{
		drawBall(food + i);
	}

	//����������� Ҫ��֤��ĸ���С��
	//�����������ʱ����
	Ball temp[GAME_NUM];
	for (int i = 0; i < GAME_NUM; i++)
	{
		temp[i] = game[i];
	}
	//�Զ�������
	sort(temp, temp + GAME_NUM, cmp);
	//�����������
	for (int i = 0; i < GAME_NUM; i++)
	{
		drawBall(temp + i);
		//showName(temp + i);
	}
}

//��������
void moveKey(Ball* game)
{
	if (game)
	{
		//б���ƶ�
		double realMove = game->speed;
		if (GetAsyncKeyState(VK_UP) && GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState(VK_UP) && GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState(VK_DOWN) && GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState(VK_DOWN) && GetAsyncKeyState(VK_RIGHT))
		{
			//б���ٶ�
			realMove *= sqrt(2) / 2;
		}
		//��
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
		//��
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
		//��
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
		//��
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

//��ʳ��
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
			//ʳ����� �� Բ�ľ���С���������뾶 ��
			if (DISTANCE((game + i), (food + j)) < game[i].r && game[i].life && food[j].life)
			{
				//���
				game[i].r += sqrt(food[j].r);
				//ʳ��û��
				food[j].life = false;
				food[j].cnt -= RESET_TIME;
			}
		}
	}
}

//�����
void eatGame(Ball* game)
{
	//���ij������ �� ���ij�������ֱ��� �� ���i�뾶�������j��110% Բ�ľ���С�����i�뾶 i��j
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
				//���
				game[i].r += sqrt(game[j].r);
				//���[j]û��
				game[j].life = false;
				game[j].cnt -= RESET_TIME;
				//������ʳ��Ч
				mciSendString(_T("stop eatVoice"), 0, 0, 0);
				mciSendString(_T("close eatVoice"), 0, 0, 0);
				mciSendString(_T("open ./Resource/Eat.wav alias eatVoice type mpegvideo"), 0, 0, 0);
				mciSendString(_T("play eatVoice"), 0, 0, 0);
			}
		}
	}
}

//׷��
void chaseGame(Ball* gameA, Ball* gameB)
{
	//gameA׷
	//б���ƶ��ж�
	double realMove = gameA->speed;
	if (gameA->x > gameB->x && gameA->y > gameB->y || gameA->x > gameB->x && gameA->y < gameB->y || gameA->x < gameB->x && gameA->y > gameB->y || gameA->x < gameB->x && gameA->y < gameB->y)
	{
		realMove *= sqrt(2) / 2;
	}
	//�ж�runλ�� ׷
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

//����
void runGame(Ball* gameA, Ball* gameB)
{
	//gameA��
	//б���ƶ��ж�
	double realMove = gameA->speed;
	if (gameA->x > gameB->x && gameA->y > gameB->y || gameA->x > gameB->x && gameA->y < gameB->y || gameA->x < gameB->x && gameA->y > gameB->y || gameA->x < gameB->x && gameA->y < gameB->y)
	{
		realMove *= sqrt(2) / 2;
	}
	//�ж�runλ�� ��
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

//�˻�Ŀ������
void moveGame(Ball* game, Ball* food)
{
	//���� ���˻������(�����˻�)�� ��׷��� ��֮ ���ʳ�� 
	for (int i = 1; i < GAME_NUM; i++)
	{
		if (!game[i].life)
		{
			continue;
		}
		//���˻�����Ҵ� �� ��ҹ������ֱ���  ����һ��� �˻�����׷���
		//indexȱʡֵ-1 �˻�indexֵ���Ϊ-2 ׷���
		if (game[i].r > game->r && game->r > PROTECT_R && game->life)
		{
			game[i].index = -2;
			continue;
		}
		//���˻������С ׷ʳ��
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
			//��ÿ����Ҷ��������������ʳ��
		}
	}
	//ս����
	for (int i = 1; i < GAME_NUM; i++)
	{
		if (!game[i].life)
		{
			continue;
		}
		if (game[i].index < 0)
		{
			//׷�����
			chaseGame(game + i, game);
			continue;
		}
		//���������С �Ҿ���С�ڵ�����Ұ뾶��300% ��· ֱ���ܳ��˷�ΧΪֹ
		if (game[i].r < game->r && DISTANCE((game + i), game) <= game->r * 3)
		{
			//����
			runGame(game + i, game);
			continue;
		}
		//׷��ʳ��
		chaseGame(game + i, food + game[i].index);
	}
}

//���򸴻�
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

//�Զ�����
void autoSlimGame(Ball* game)
{
	//���û�Է� ������ �����������
	//ÿ60FPS����һ�� ����ֵ����������������������
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

//�ַ���ʾ
void showString()
{
	//��ʾ����
	TCHAR grade[100];
	sprintf_s(grade, _T("%d"), int(game->r));//��sprintf�����ͷ���ת�ַ���
	setbkmode(TRANSPARENT);	//͸����ʽ���,���ı䱳��
	settextcolor(BLACK);
	//settextstyle(20, 0, _T("΢���ź�"));
	outtextxy(40, 20, _T("����:"));
	outtextxy(80, 20, grade);

	//��ʾ����ʱ
	TCHAR m[100];
	TCHAR s[100];
	sprintf_s(m, _T("%d"), gameTime / 3600);
	sprintf_s(s, _T("%d"), (gameTime % 3600) / 60);
	outtextxy(WIDTH / 2 - 30, 20, m);
	outtextxy(WIDTH / 2 - 20, 20, _T(":"));
	outtextxy(WIDTH / 2 - 10, 20, s);
	gameTime--;
}

//ͼƬ��Դ
IMAGE bk1;
IMAGE bk2;
IMAGE bk3;

//��ȡͼƬ
void loadRes()
{
	loadimage(&bk1, _T("Resource\\bk1.png"), WIDTH, HEIGHT);
	loadimage(&bk2, _T("Resource\\bk2.png"), WIDTH, HEIGHT);
	loadimage(&bk3, _T("Resource\\bk3.png"), WIDTH, HEIGHT);
}

//��ʼ��Ϸ
void start()
{
	//����ͼ�δ���
	initgraph(WIDTH, HEIGHT);
	loadRes();

	// ��������
	putimage(0, 0, &bk1);
	while (!_kbhit()) {}
	putimage(0, 0, &bk2);
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	outtextxy(WIDTH / 2 - 50, 20, _T("��Ϸ������......"));
	Sleep(3000);

	//���ñ�����ɫ
	setbkcolor(RGB(230, 230, 230));
	//ȫ�ֳ�ʼ��
	init();
	cleardevice();

	//���ű�������
	mciSendString(_T("open ./Resource/BGM.wav alias bkMusic type mpegvideo"), 0, 0, 0);
	mciSendString(_T("play bkMusic repeat"), 0, 0, 0);
}

//������Ϸ
void end()
{
	mciSendString(_T("close bkMusic"), 0, 0, 0);
	putimage(0, 0, &bk3);

	int constY = 130;

	settextstyle(50, 0, "��Բ");
	outtextxy(WIDTH / 2 - 100, constY+20, _T("��Ϸ����"));

	TCHAR grade[100];
	sprintf_s(grade, _T("%d"), int(game->r));
	settextstyle(30, 0, "΢���ź�");
	outtextxy(WIDTH / 2 - 50, constY + 180, _T("����������"));

	settextstyle(50, 0, "΢���ź�");
	outtextxy(WIDTH / 2 - 30, constY + 220, grade);

	settextstyle(30, 0, "΢���ź�");
	if (game[0].life)
	{
		outtextxy(WIDTH / 2 - 90, constY + 280, "��ϲ��������");
	}
	else
	{
		outtextxy(WIDTH / 2 - 140, constY + 280, "��û�д���󣬽����ؿ�");
	}
	Sleep(30000);
	//������Ⱦͼ��
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
		//����˫���� ��������
		BeginBatchDraw();
		//�������ٻ�
		cleardevice();
		//���������� i+=20 20px*20px
		setlinecolor(RGB(204, 204, 204));
		for (int i = 0; i < getwidth(); i += 20)
		{
			line(i, 0, i, getheight());//������ ����������(��) ����ȷ��һ���߶�, ������(�߶ȣ�
			line(0, i, getwidth(), i); //������(�߶ȣ�
		}
		//������
		draw();
		//����ַ�
		showString();

		//����ƶ�
		moveKey(game);
		moveGame(game, food);
		eatFood(game, food);
		eatGame(game);

		//�Զ�����
		autoSlimGame(game);
		//���򸴻�
		resetBall(game, food);

		//�ر�˫����
		EndBatchDraw();

		//�̶�֡��ΪFPS
		int frameTime = clock() - startTime;
		if (frameTime < FPS)
		{
			Sleep(FPS - frameTime);
		}
	}

	end();
	return 0;
}

