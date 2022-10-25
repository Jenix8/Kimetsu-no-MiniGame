#include <iostream>
#include <vector>
#include <random>
#include <conio.h>
#include <algorithm>
#include <string>

// setting
constexpr int W = 10; // x, ↓     
constexpr int H = 12; // y , →  
int CureLimit = 3;
int deathCount = 0;		// korosareta hito
int deathLimit = 36;	// Limit 	
int turn = 1;
std::string frameUpdate = "";

enum object
{
	EMPTY = 0,
	TANZIRO,
	MUZAN,
	ONI,
	KISATSU,
	MINKAN,
	CHOUCHO,
	DIE
};

enum gameState
{
	PLAYING,
	VICTORY,
	HP0,
	MUZAN_KILL,
	HEARTLESS
};

class Character {
public:
	int CurX = -1, CurY= -1;
	object CharType = EMPTY;
	int power = 0, hp = 0;

	Character() {};
	Character(int x, int y, object T) : CurX(x), CurY(y), CharType(T) 
	{
		if (T == TANZIRO)
		{
			power = 5;
			hp = 10;
		}
		else if (T == MUZAN)
		{
			power = 10;
		}
	};
	Character(Character& cha) {};
	~Character() {};

	void printObj()
	{
		if (CharType == EMPTY) printf("□");
		if (CharType == TANZIRO) printf("★");
		if (CharType == MUZAN) printf("※");
		if (CharType == ONI) printf("◎");
		if (CharType == KISATSU) printf("☆");
		if (CharType == MINKAN) printf("■");
		if (CharType == CHOUCHO) printf("♥");
		if (CharType == DIE) printf("†");
	}
};

Character* grid[W][H] = { nullptr };
gameState Now = PLAYING;

Character* Tanziro;
Character* Muzan;
Character* Choucho;
std::vector<Character*> Oni, Kisatsu;
std::vector<int> deadOniIdx;

void printStart();
void explanation();
void printScreen();
void printEnd();
void deleteGrid();
void initialize();
void Update();
bool Restart();
void interaction(Character* A, int pos[2]);

void swap(Character** A, Character** B)
{
	Character* tmp = *A;
	*A = *B;
	*B = tmp;

	int tmpX = (*A)->CurX;
	(*A)->CurX = (*B)->CurX;
	(*B)->CurX = tmpX;

	int tmpY = (*A)->CurY;
	(*A)->CurY = (*B)->CurY;
	(*B)->CurY = tmpY;
}

int main()
{
	printStart();

	do
	{
		initialize();

		while (Now == PLAYING)
		{
			system("CLS");
			printScreen();
			frameUpdate = "";
			Update();
		}
		system("CLS");
		printScreen();

		deleteGrid();
		printEnd();
	} while (Restart());
}

void printStart()
{
	printf("***********************************************\n");
	printf("***********************************************\n");
	printf("********                               ********\n");
	printf("*******       탄지로가 되어 보자!       *******\n");
	printf("********                               ********\n");
	printf("***********************************************\n");
	printf("***********************************************\n\n\n");
														
	printf("	     Press any key to Start			 \n\n");

	printf("        Q를 눌러 설명을 볼 수 있습니다.        \n");

	char inputGame;
	inputGame = _getch();

	if (inputGame == 'Q' || inputGame == 'q')
		explanation();
	
	return;
}

void explanation()
{
	system("CLS");

	printf("-------------------------게임 설명-------------------------\n\n");
	printf(" ★ 당신이 탄지로가 되어 오니를 섬멸하는 게임입니다.    \n");
	printf(" ★ 오니를 잡아 전투력을 올리고, 최종적으로 무잔을 잡으면 승리!\n\n\n");

	printf("-------------------------게임 진행-------------------------\n\n");
	printf(" ★ 탄지로(★)는 체력 10과 전투력 5로 시작으로, 주변을 WASD로 돌아다니게 됩니다.\n");
	printf(" ★ 무잔(※)은 상하좌우 무작위로 움직이며, 민간인(■)을 만나면 일정 확률로 살해하거나 오니(◎)로 만듭니다.\n");
	printf(" ★ 탄지로는 무잔이 만든 오니를 잡아가면서 전투력을 올리고, 전투력이 10 이상이 되면 무잔을 처치할 수 있습니다.\n");
	printf(" ★ 탄지로는 10칸을 움직이면 체력이 1 감소하며, 오니와 만나면 서로 싸워 이기거나 지게 됩니다.\n");
	printf("    (한 턴에 두 번 이상의 싸움이 발생할 수 있습니다.)\n");
	printf(" ★ 탄지로가 오니에게 이기면 오니는 섬멸되며 탄지로의 전투력이 1 증가합니다.\n");
	printf("    또한 오니에게 당한 사람들을 기려, 죽은 민간인의 수가 3 감소됩니다..\n");
	printf("    하지만, 오니와의 싸움에서 피해를 입어 체력은 2만큼 감소합니다.\n");
	printf(" ★ 탄지로가 오니에게 지면 오니는 사라지지 않으며 탄지로의 체력은 5만큼 감소합니다.\n");
	printf(" ★ 탄지로는 나비 저택(♥)에서 총 %d번 체력을 최대로 회복시킬 수 있습니다.\n\n\n", CureLimit);


	printf("-------------------------패배 조건-------------------------\n\n");
	printf(" ★ 탄지로의 체력이 0이 됐을 때\n");
	printf(" ★ 민간인이 무잔과 오니에게 %d명 이상 죽었을 때\n", deathLimit);
	printf(" ★ 전투력이 10 미만인 상황에서 무잔을 만났을 때\n\n");

	printf("        ★ 무잔을 토벌하여 승리를 쟁취하세요! ★\n\n");

	if (_getch())
		return;
}

void printScreen()
{
	printf("\nturn : %d\n\n", turn);

	for (int i = 0; i < W; i++)
	{
		for (int j = 0; j < H; j++)
		{
			grid[i][j]->printObj();
		}
		printf("\n");
	}

	printf("\n");
	printf("체력\t: %d\n", Tanziro->hp);
	printf("전투력\t: %d\n", Tanziro->power);
	printf("남은 치료 횟수\t: ");
	for (int i = 0; i < CureLimit; i++) printf("♥");
	for (int i = 0; i < 3 - CureLimit; i++) printf("♡");
	printf("\n");
	printf("죽은 민간인 수\t: %d / %d\n\n", deathCount, deathLimit);
	if (Now == PLAYING)
		std::cout << frameUpdate << '\n';
}

void printEnd()
{
	std::cout << frameUpdate << '\n';

	if (Now == VICTORY)
	{
		printf("`8.`888b           ,8'  8 8888     ,o888888o.    8888888 8888888888     ,o888888o.     8 888888888o.   `8.`8888.      ,8' \n");
		printf(" `8.`888b         ,8'   8 8888    8888     `88.        8 8888        . 8888     `88.   8 8888    `88.   `8.`8888.    ,8'  \n");
		printf("  `8.`888b       ,8'    8 8888 ,8 8888       `8.       8 8888       ,8 8888       `8b  8 8888     `88    `8.`8888.  ,8'   \n");
		printf("   `8.`888b     ,8'     8 8888 88 8888                 8 8888       88 8888        `8b 8 8888     ,88     `8.`8888.,8'    \n");
		printf("    `8.`888b   ,8'      8 8888 88 8888                 8 8888       88 8888         88 8 8888.   ,88'      `8.`88888'     \n");
		printf("     `8.`888b ,8'       8 8888 88 8888                 8 8888       88 8888         88 8 888888888P'        `8. 8888      \n");
		printf("      `8.`888b8'        8 8888 88 8888                 8 8888       88 8888        ,8P 8 8888`8b             `8 8888      \n");
        printf("       `8.`888'         8 8888 `8 8888       .8'       8 8888       `8 8888       ,8P  8 8888 `8b.            8 8888      \n");
        printf("        `8.`8'          8 8888    8888     ,88'        8 8888        ` 8888     ,88'   8 8888   `8b.          8 8888      \n");
        printf("         `8.`           8 8888     `8888888P'          8 8888           `8888888P'     8 8888     `88.        8 8888      \n\n");
		
		printf("무잔을 토벌한 당신은 한 시대의 영웅이 될 것입니다.\n오니가 없는 세상에선 행복하기를.\n\n");
	}
	else
	{
		printf("8 888888888o.      8 8888888888   8 8888888888   8 8888888888            .8.          8888888 8888888888\n");
		printf("8 8888    `^888.   8 8888         8 8888         8 8888                 .888.               8 8888		\n");
		printf("8 8888        `88. 8 8888         8 8888         8 8888                :88888.              8 8888		\n");
		printf("8 8888         `88 8 8888         8 8888         8 8888               . `88888.             8 8888		\n");
		printf("8 8888          88 8 888888888888 8 888888888888 8 888888888888      .8. `88888.            8 8888		\n");
		printf("8 8888          88 8 8888         8 8888         8 8888             .8`8. `88888.           8 8888		\n");
		printf("8 8888,         88 8 8888         8 8888         8 8888            .8' `8. `88888.          8 8888      \n");
		printf("8 8888,        88' 8 8888         8 8888         8 8888           .8'   `8. `88888.         8 8888		\n");
		printf("8 8888,	   o88P'   8 8888         8 8888         8 8888          .888888888. `88888.        8 8888      \n");
		printf("8 888888888P'      8 888888888888 8 8888         8 888888888888 .8'       `8. `88888.       8 8888		\n\n");
	}

	if (Now == HP0)
		printf("탄지로는 귀살대에 적합하지 않았던 모양입니다..\n\n");

	if (Now == MUZAN_KILL)
		printf("무잔에게 덤비기에는 아직이었던 모양입니다..\n\n");

	if (Now == HEARTLESS)
		printf("너무 많은 사람들을 지켜주지 못했습니다..\n\n");
}

void deleteGrid()
{
	for (int i = 0; i < W; i++)
		for (int j = 0; j < H; j++)
		{
			delete grid[i][j];
			grid[i][j] = nullptr;
		}

	Oni.clear();
	Kisatsu.clear();
}

void initialize()
{
	Now = PLAYING;
	CureLimit = 3;
	frameUpdate = "";
	deathCount = 0;
	turn = 1;

	Tanziro = new Character(W - 2, H - 2, TANZIRO);
	Muzan = new Character(W / 2 - 1, H / 2 - 1, MUZAN);
	Choucho = new Character(W - 1, 0, CHOUCHO);

	grid[Tanziro->CurX][Tanziro->CurY] = Tanziro;
	grid[Muzan->CurX][Muzan->CurY] = Muzan;
	grid[Choucho->CurX][Choucho->CurY] = Choucho;

	srand((unsigned int)time(NULL));

	for (int i = 0; i < W; i++)
		for (int j = 0; j < H; j++)
		{
			if (grid[i][j] == nullptr)
			{
				int RND = rand();
				if (RND % 100 < 70)
				{
					Character* tmpMK = new Character(i, j, MINKAN);
					grid[i][j] = tmpMK;
				}
				else
				{
					Character* tmpEMT = new Character(i, j, EMPTY);
					grid[i][j] = tmpEMT;
				}
			}
		}
}

bool Restart()
{
	char Res; 

	printf("Restart? (R) : ");
	Res = getchar();
	while (getchar() != '\n');

	if (Res == 'R' || Res == 'r')
		return true;
	else
	{
		printf("\n게임이 종료됩니다.\n");
		return false;
	}
}

void Update()
{
	if (Now == PLAYING)
		turn++;

	if (turn % 10 == 0)
	{
		Tanziro->hp -= 1;
		frameUpdate += "너무 많이 걸은 탓에 체력이 소모됩니다.\n\n";
	}

	/////////////////////////////
	// Move Tanziro as your input
	char inputMove;
	bool correctInput;
	int T_x = Tanziro->CurX, T_y = Tanziro->CurY;
	int TN_x = Tanziro->CurX, TN_y = Tanziro->CurY;

	do
	{
		inputMove = _getch();

		correctInput = inputMove == 'W' || inputMove == 'A' || inputMove == 'S' || inputMove == 'D'
			|| inputMove == 'w' || inputMove == 'a' || inputMove == 's' || inputMove == 'd';

		if (!correctInput)
			printf("Wrong input\n");

	} while (!correctInput);

	if (inputMove == 'W' || inputMove == 'w')
	{
		if (T_x != 0)
			TN_x -= 1;
	}
	else if (inputMove == 'S' || inputMove == 's')
	{
		if (T_x != W - 1)
			TN_x += 1;
	}
	else if (inputMove == 'A' || inputMove == 'a')
	{
		if (T_y != 0)
			TN_y -= 1;
	}
	else if (inputMove == 'D' || inputMove == 'd')
	{
		if (T_y != H - 1)
			TN_y += 1;
	}

	int TNextPos[2] = { TN_x, TN_y };
	interaction(Tanziro, TNextPos);

	if (Now != PLAYING)
		return;

	///////////////////////////////
	//// Move Muzan randomly
	////srand(time(NULL));
	int M_x = Muzan->CurX, M_y = Muzan->CurY;
	int MN_x = Muzan->CurX, MN_y = Muzan->CurY;

	int MuzanMoveDir = rand() % 4;
	
	if (MuzanMoveDir == 0)
	{
		// down
		if (M_x != W - 1)
			MN_x += 1;
	}
	else if (MuzanMoveDir == 1)
	{
		// up
		if (M_x != 0)
			MN_x -= 1;
	}
	else if (MuzanMoveDir == 2)
	{
		// left
		if (M_y != 0)
			MN_y -= 1;
	}
	else
	{
		// right
		if (M_y != H - 1)
			MN_y += 1;
	}
	
	int MNextPos[2] = { MN_x, MN_y };
	interaction(Muzan, MNextPos);

	if (Now != PLAYING)
		return;

	/////////////////////////////
	// Move Onis randomly
	//srand(time(NULL));

	for(auto it = Oni.begin(); it != Oni.end(); it++)
	{
		int O_x = (*it)->CurX, O_y = (*it)->CurY;			// Oni's current position
		int ON_x = O_x, ON_y = O_y;		// Oni's next position
		int oniMoveDir = rand() % 4;
		
		if (oniMoveDir == 0)
		{
			// down
			if (O_x != W - 1)
				ON_x += 1;
		}
		else if (oniMoveDir == 1)
		{
			// up
			if (O_x != 0)
				ON_x -= 1;
		}
		else if (oniMoveDir == 2)
		{
			// left
			if (O_y != 0)
				ON_y -= 1;
		}
		else
		{
			// right
			if (O_y != H - 1)
				ON_y += 1;
		}
		
		int OniPos[2] = {ON_x, ON_y };
		interaction(*it, OniPos);

		if (Now != PLAYING)
			return;
	}

	if (deathCount >= deathLimit)
		Now = HEARTLESS;

	sort(deadOniIdx.begin(), deadOniIdx.end(), std::greater<int>());
	for (auto it = deadOniIdx.begin(); it != deadOniIdx.end(); it++)
	{
		Oni.erase(Oni.begin() + *it);
	}
	deadOniIdx.clear();
}


void interaction(Character* A, int pos[2])
{
	if (A->CharType == TANZIRO)
	{
		if (Tanziro->hp <= 0)
		{
			Now = HP0;
			return;
		}

		int T_x = A->CurX, T_y = A->CurY;
		int TN_x = pos[0], TN_y = pos[1];

		if (grid[TN_x][TN_y]->CharType == EMPTY || grid[TN_x][TN_y]->CharType == KISATSU || grid[TN_x][TN_y]->CharType == MINKAN)
		{
			swap(&grid[T_x][T_y], &grid[TN_x][TN_y]);
		}
		else if (grid[TN_x][TN_y]->CharType == MUZAN)
		{
			if (Tanziro->power < Muzan->power)
			{
				Now = MUZAN_KILL;
				frameUpdate += "무잔에게 패배하였습니다.\n\n";
				Tanziro->CharType = DIE;
			}
			else
			{
				Now = VICTORY;
				frameUpdate += "무잔에게 승리하였습니다!\n\n";
				Muzan->CharType = DIE;
			}
		}
		else if (grid[TN_x][TN_y]->CharType == ONI)
		{
			int battleProb = rand() % 100;

			if (battleProb <= Tanziro->power * 100 / Muzan->power)
			{
				grid[TN_x][TN_y]->CharType = EMPTY;
				Tanziro->hp = (Tanziro->hp >= 2) ? Tanziro->hp - 2 : 0;
				Tanziro->power += 1;
				deathCount = (deathCount - 3 >= 0) ? deathCount - 3 : 0;

				deadOniIdx.push_back(find(Oni.begin(), Oni.end(), grid[TN_x][TN_y]) - Oni.begin());

				swap(&grid[T_x][T_y], &grid[TN_x][TN_y]);


				frameUpdate += "토벌을 위해 오니에게 다가갑니다.\n";
				frameUpdate += "오니를 토벌하였습니다!\n탄지로가 더 강해졌습니다.\n하지만 전투 중에 받은 피해로 체력이 2 감소합니다.\n";
				frameUpdate += "오니에게 당한 3인의 넋을 기려주었습니다.\n\n";

				if (Tanziro->power == 10)
					frameUpdate += "\n이제 무잔과 싸워서 이길 수 있습니다.\n\n";
			}
			else
			{
				Tanziro->hp = (Tanziro->hp >= 5) ? Tanziro->hp - 5 : 0;
				frameUpdate += "토벌을 위해 오니에게 다가갑니다.\n";
				frameUpdate += "오니에게 패배하였습니다.\n체력이 5 감소합니다.\n\n";
			}
		}
		else if (grid[TN_x][TN_y]->CharType == CHOUCHO)
		{
			if (CureLimit > 0)
			{
				Tanziro->hp = 10;
				CureLimit -= 1;
				frameUpdate += "나비저택에서 치료를 받았습니다.\n체력이 최대로 회복됩니다.\n\n";
			}
			else
			{
				frameUpdate += "더 이상 치료받을 수 없습니다.\n\n";
			}
		}

		if (Tanziro->hp <= 0)
			Now = HP0;
	}

	if (A->CharType == MUZAN)
	{
		int M_x = A->CurX, M_y = A->CurY;
		int MN_x = pos[0], MN_y = pos[1];

		if (grid[MN_x][MN_y]->CharType == EMPTY || grid[MN_x][MN_y]->CharType == ONI)
		{
			swap(&grid[M_x][M_y], &grid[MN_x][MN_y]);
		}
		else if (grid[MN_x][MN_y]->CharType == TANZIRO)
		{
			if (Tanziro->power < Muzan->power)
			{
				Now = MUZAN_KILL;
				frameUpdate += "무잔에게 패배하였습니다.\n\n";
				Tanziro->CharType = DIE;
			}
			else
			{
				Now = VICTORY;
				frameUpdate += "무잔에게 승리하였습니다!\n\n";
				Muzan->CharType = DIE;
			}
	
			return;
		}
		else if (grid[MN_x][MN_y]->CharType == MINKAN || grid[MN_x][MN_y]->CharType == KISATSU)
		{
			int korosuProb = rand() % 100;
	
			if (korosuProb < 40)
			{
				grid[MN_x][MN_y]->CharType = EMPTY;
				deathCount = (deathCount < deathLimit) ? deathCount + 1 : deathLimit;
				frameUpdate += "무잔이 민간인을 무참히 살해하였습니다...\n\n";
			}
			else
			{
				grid[MN_x][MN_y]->CharType = ONI;
				Oni.push_back(grid[MN_x][MN_y]);
				deathCount = (deathCount < deathLimit) ? deathCount + 1 : deathLimit;
				frameUpdate += "무잔이 오니를 늘립니다.\n\n";
			}
	
			swap(&grid[M_x][M_y], &grid[MN_x][MN_y]);
		}
	}
	
	if (A->CharType == ONI)
	{
		int O_x = A->CurX, O_y = A->CurY;
		int ON_x = pos[0], ON_y = pos[1];
	
		if (grid[ON_x][ON_y]->CharType == EMPTY || grid[ON_x][ON_y]->CharType == ONI)
		{
			swap(&grid[O_x][O_y], &grid[ON_x][ON_y]);
		}
		else if (grid[ON_x][ON_y]->CharType == TANZIRO)
		{
			int battleProb = rand() % 100;
	
			if (battleProb <= Tanziro->power * 100 / Muzan->power)
			{
				grid[O_x][O_y]->CharType = EMPTY;
				Tanziro->hp = (Tanziro->hp >= 2) ? Tanziro->hp - 2 : 0;
				Tanziro->power += 1;
				deathCount = (deathCount - 3 >= 0) ? deathCount - 3 : 0;

				deadOniIdx.push_back(find(Oni.begin(), Oni.end(), grid[O_x][O_y]) - Oni.begin());

				frameUpdate += "오니가 싸움을 걸어왔습니다.\n";
				frameUpdate += "오니를 토벌하였습니다!\n탄지로가 더 강해졌습니다.\n하지만 전투 중에 받은 피해로 체력이 2 감소합니다.\n";
				frameUpdate += "오니에게 당한 3인의 넋을 기려주었습니다.\n\n";

				if (Tanziro->power == 10)
					frameUpdate += "\n이제 무잔과 싸워서 이길 수 있습니다.\n\n";
			}
			else
			{
				Tanziro->hp = (Tanziro->hp >= 5) ? Tanziro->hp - 5 : 0;
				frameUpdate += "오니가 싸움을 걸어왔습니다.\n";
				frameUpdate += "오니에게 패배하였습니다.\n체력이 5 감소합니다.\n\n";
			}
		}
		else if (grid[ON_x][ON_y]->CharType == MINKAN)
		{
			grid[ON_x][ON_y]->CharType = EMPTY;
			deathCount = (deathCount < deathLimit) ? deathCount + 1 : deathLimit;
			swap(&grid[O_x][O_y], &grid[ON_x][ON_y]);
			frameUpdate += "오니가 사람을 잡아먹었습니다.\n\n";
		}
		//else if (grid[ON_x][ON_y]->CharType == KISATSU)
		//{
		//	int korosuProb = rand() % 100;
		//
		//	if (korosuProb < 50)
		//	{
		//		grid[ON_x][ON_y] = EMPTY;
		//		swap(O_x, O_y, ON_x, ON_y);
		//	}
		//	else
		//		grid[O_x][O_y] = EMPTY;
		//}
		else if (grid[ON_x][ON_y]->CharType == CHOUCHO)
		{
			grid[O_x][O_y]->CharType = EMPTY;
			frameUpdate += "나비 저택에 발을 잘못 들인 오니가 죽었습니다.\n\n";
		}

		if (Tanziro->hp <= 0)
			Now = HP0;
	}
}