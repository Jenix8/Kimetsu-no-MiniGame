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
int deathLimit = 36;		
std::string frameUpdate = "";

enum object
{
	EMPTY = 0,
	TANZIRO,
	MUZAN,
	ONI,
	KISATSU,
	MINKAN,
	CHOUCHO
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
	}
};

Character* grid[W][H] = { nullptr };
gameState Now = PLAYING;

Character* Tanziro;
Character* Muzan;
Character* Choucho;
std::vector<Character*> Oni, Kisatsu;
std::vector<int> deadOniIdx;

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

		deleteGrid();
		printEnd();
	} while (Restart());

}

void printScreen()
{
	for (int i = 0; i < W; i++)
	{
		for (int j = 0; j < H; j++)
		{
			grid[i][j]->printObj();
		}
		printf("\n");
	}

	printf("\n");
	printf("HP\t: %d\n", Tanziro->hp);
	printf("Power\t: %d\n", Tanziro->power);
	printf("Cure\t: ");
	for (int i = 0; i < CureLimit; i++) printf("♥");
	printf("\n");
	printf("people dead\t: %d / %d\n\n", deathCount, deathLimit);
	std::cout << frameUpdate << '\n';
}

void printEnd()
{
	std::cout << frameUpdate << '\n';

	if (Now == VICTORY)
		printf("무잔을 토벌하였습니다!\n\n");

	if (Now == HP0)
		printf("탄지로는 너무나도 나약했습니다..\n\n");

	if (Now == MUZAN_KILL)
		printf("무잔은 너무나도 큰 존재였습니다..\n\n");

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

	Tanziro = new Character(W - 2, H - 2, TANZIRO);
	Muzan = new Character(1, H / 2 - 1, MUZAN);
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
				if (RND % 100 < 60)
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
		return false;
}

void Update()
{
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
			}
			else Now = VICTORY;
		}
		else if (grid[TN_x][TN_y]->CharType == ONI)
		{
			int battleProb = rand() % 100;

			if (battleProb <= Tanziro->power * 100 / Muzan->power)
			{
				grid[TN_x][TN_y]->CharType = EMPTY;
				Tanziro->hp -= 2;
				Tanziro->power += 1;
				deathCount = (deathCount - 5 >= 0) ? deathCount - 5 : 0;

				deadOniIdx.push_back(find(Oni.begin(), Oni.end(), grid[TN_x][TN_y]) - Oni.begin());

				swap(&grid[T_x][T_y], &grid[TN_x][TN_y]);

				frameUpdate += "오니를 토벌하였습니다!\n탄지로가 더 강해졌습니다.\n하지만 전투 중에 받은 피해로 체력이 2 감소합니다.\n";
				frameUpdate += "오니에게 당한 5인의 넋을 기려주었습니다.\n\n";
			}
			else
			{
				Tanziro->hp -= 5;
				frameUpdate += "오니에게 패배하였습니다.\n체력이 5 감소합니다.\n체력이 0이 되기 전에 나비 저택에서 치료를 받으세요.\n\n";
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
			}
			else Now = VICTORY;
	
			return;
		}
		else if (grid[MN_x][MN_y]->CharType == MINKAN || grid[MN_x][MN_y]->CharType == KISATSU)
		{
			int korosuProb = rand() % 100;
	
			if (korosuProb < 40)
			{
				grid[MN_x][MN_y]->CharType = EMPTY;
				deathCount += 1;
				frameUpdate += "무잔이 민간인을 무참히 살해하였습니다...\n\n";
			}
			else
			{
				grid[MN_x][MN_y]->CharType = ONI;
				Oni.push_back(grid[MN_x][MN_y]);
				deathCount += 1;
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
				Tanziro->hp -= 2;
				Tanziro->power += 1;
				deathCount = (deathCount - 5 >= 0) ? deathCount - 5 : 0;

				deadOniIdx.push_back(find(Oni.begin(), Oni.end(), grid[O_x][O_y]) - Oni.begin());

				frameUpdate += "오니를 토벌하였습니다!\n탄지로가 더 강해졌습니다.\n하지만 전투 중에 받은 피해로 체력이 2 감소합니다.\n";
				frameUpdate += "오니에게 당한 5인의 넋을 기려주었습니다.\n\n";
			}
			else
			{
				Tanziro->hp -= 5;
				frameUpdate += "오니에게 패배하였습니다.\n체력이 5 감소합니다.\n체력이 0이 되기 전에 나비 저택에서 치료를 받으세요.\n\n";
			}
		}
		else if (grid[ON_x][ON_y]->CharType == MINKAN)
		{
			grid[ON_x][ON_y]->CharType = EMPTY;
			deathCount += 1;
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
		}
	}
}