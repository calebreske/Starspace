#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <random>

const int xsize = 180;//x size of the map
const int ysize = 49;// y size of the map

std::random_device rd; // Use a hardware entropy source if available, otherwise use PRNG
std::mt19937 mersenne(rd()); // initialize the mersenne twister with a random seed

int probAdd = 11;//literally adjusts the amount of land
int groups = 160;//this adjusts the number of times you "clump" the land
				 //a 2d array
				 //land or water?
bool land[ysize][xsize];

//probs for creatinig probabilities
//another 2d array
unsigned int probs[ysize][xsize];

//emums for terrain generation
enum Temperature
{
	COLD = 0,
	TEMPERATE = 1,
	HOT = 2
};

Temperature zone[ysize][xsize];

enum Terrain
{
	ICE_PLAINS = 0,
	SNOWY_FOREST = 1,
	MOUNTAINS = 2,
	FOREST = 3,
	PLAINS = 4,
	JUNGLE = 5,
	DESERT = 6,
	WATER = 7
};

Terrain terraType[ysize][xsize];

std::string terrainName[ysize][xsize];

enum CONCOL //enum for setting colors
{
	black = 0,
	dark_blue = 1,
	dark_green = 2,
	dark_aqua, dark_cyan = 3,
	dark_red = 4,
	dark_purple = 5, dark_pink = 5, dark_magenta = 5,
	dark_yellow = 6,
	dark_white = 7,
	gray = 8,
	blue = 9,
	green = 10,
	aqua = 11, cyan = 11,
	red = 12,
	purple = 13, pink = 13, magenta = 13,
	yellow = 14,
	white = 15,
};

CONCOL textColor = white; //set color initially to white

enum PLANETTYPE
{
	ocean,
	earthlike,
	lava,
	ice,
	swamp,
	forest,
	cloud
};

CONCOL clouds[ysize][xsize];

const int starchance = 250;

bool planet[ysize][xsize];

int planetNumber = 0;

char ID[ysize][xsize];

int seed[ysize][xsize];

PLANETTYPE planetType[ysize][xsize];

void color(CONCOL colorChange)
{
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	textColor = colorChange;
	SetConsoleTextAttribute(hConsole, textColor);
}

void curPos(int x, int y) {
	HANDLE hStdout;
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hStdout, &csbiInfo);
	csbiInfo.dwCursorPosition.X = x;
	csbiInfo.dwCursorPosition.Y = y;
	SetConsoleCursorPosition(hStdout, csbiInfo.dwCursorPosition);
}

void printG()
{
	using namespace std;

	srand(static_cast <unsigned int>(time(0)));

	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			if (planet[i][j] == true)
			{
				curPos(j, i);

				switch (planetType[i][j])
				{
				case earthlike:
					color(green);
					break;
				case ocean:
					color(blue);
					break;
				case forest:
					color(dark_green);
					break;
				case ice:
					color(gray);
					break;
				case cloud:
					color(yellow);
					break;
				case lava:
					color(red);
					break;
				case swamp:
					color(dark_yellow);
					break;
				}

				cout << "#";
			}
		}
	}

	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			if (planet[i][j - 1] == true)
			{
				curPos(j, i);

				color(white);
				std::cout << ID[i][j - 1];
			}
		}
	}

	curPos(xsize - 1, ysize - 1);

	color(white);

	for (int i = 0; i < xsize - 2; ++i)
	{
		cout << "-";
	}
	cout << "\n\t\t\t\t\t\t\tThis is the Galaxy Map, where you can see the nearest planets.\n\t\t\t\t\t\t\tA pound sign indicates a habitable planet in that area.\n\t\t\t\t\t\t\t";

	color(green);
	cout << "#";
	color(white);
	cout << " indicates an earth-like planet.\n\t\t\t\t\t\t\t";

	color(blue);
	cout << "#";
	color(white);
	cout << " indicates an ocean-covered planet.\n\t\t\t\t\t\t\t";

	color(dark_green);
	cout << "#";
	color(white);
	cout << " indicates a forest planet.\n\t\t\t\t\t\t\t";

	color(gray);
	cout << "#";
	color(white);
	cout << " indicates an ice-covered planet.\n\t\t\t\t\t\t\t";

	color(yellow);
	cout << "#";
	color(white);
	cout << " indicates a cloud planet.\n\t\t\t\t\t\t\t";

	color(red);
	cout << "#";
	color(white);
	cout << " indicates a lava planet.\n\t\t\t\t\t\t\t";

	color(dark_yellow);
	cout << "#";
	color(white);
	cout << " indicates a swamp-covered planet.\n\t\t\t\t\t\t\t";

	cout << "Type the coresponding letter of the planet you wish to explore.\n\t\t\t\t\t\t\t";

}

/*
- # of random #'s required to generate earthlike & island: 16
	- # of random #'s required to generate cloud: 1
*/

void clump(int ii, int jj)
{
	using namespace std;

	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			int chance = mersenne() % 1000;
			int average = (probs[i - 1][j] + probs[i + 1][j] + probs[i][j - 1] + probs[i][j + 1]) / 4; //4 for # of tiles around it

			if (chance > average)
			{
				probs[i][j] = (probs[i][j] - probAdd);
			}

			if (chance <= average)
			{
				probs[i][j] = (probs[i][j] + probAdd);
			}
		}
	}
}

void clear(int ii, int jj)
{
	using namespace std;

	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			if (land[i + 1][j] == false && land[i - 1][j] == false)
			{
				land[i][j] = false;
			}

			if (land[i][j + 1] == false && land[i][j - 1] == false)
			{
				land[i][j] = false;
			}
		}
	}
}

void smooth(int ii, int jj)
{
	using namespace std;

	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			if (land[i][j] == false)
			{
				if (land[i + 1][j] == true && land[i][j - 1] == true && land[i + 1][j - 1] == false)
				{
					land[i][j] = true;
				}
			}

			if (land[i][j] == false)
			{
				if (land[i - 1][j] == true && land[i][j - 1] == true && land[i - 1][j - 1] == false)
				{
					land[i][j] = true;
				}
			}

			if (land[i][j] == false)
			{
				if (land[i - 1][j] == true && land[i][j + 1] == true && land[i - 1][j + 1] == false)
				{
					land[i][j] = true;
				}
			}

			if (land[i][j] == false)
			{
				if (land[i + 1][j] == true && land[i][j + 1] == true && land[i + i][j + 1] == false)
				{
					land[i][j] = true;
				}
			}

			if (land[i][j] == true)//gets rid of square islands
			{
				if (land[i + 1][j] == true && land[i][j + 1] == true && land[i + 1][j + 1] == true)
				{
					if (land[i + 2][j] == false && land[i + 2][j + 1] == false)
					{
						if (land[i][j + 2] == false && land[i + 1][j + 2] == false)
						{
							if (land[i][j - 1] == false && land[i + 1][j - 1] == false)
							{
								if (land[i - 1][j] == false && land[i - 1][j + 1] == false)
								{
									land[i][j] = false;
									land[i + 1][j] = false;
									land[i + 1][j + 1] = false;
									land[i][j + 1] = false;
								}
							}
						}
					}
				}
			}

			if (land[i][j] == true)//gets rid of rectangle islands
			{
				if (land[i + 1][j] == true && land[i][j + 1] == true && land[i + 1][j + 1] == true && land[i][j + 2] == true && land[i + 1][j + 2] == true)
				{
					if (land[i + 2][j] == false && land[i + 2][j + 1] == false && land[i + 2][j + 2] == false)
					{
						if (land[i][j + 3] == false && land[i + 1][j + 3] == false)
						{
							if (land[i][j - 1] == false && land[i + 1][j - 1] == false)
							{
								if (land[i - 1][j] == false && land[i - 1][j + 1] == false && land[i - 1][j + 2] == false)
								{
									land[i][j] = false;
									land[i + 1][j] = false;
									land[i + 1][j + 1] = false;
									land[i][j + 1] = false;
									land[i][j + 2] = false;
									land[i + 1][j + 2] = false;
								}
							}
						}
					}
				}
			}

			if (land[i][j] == true)//gets rid of big square islands
			{
				if (land[i + 1][j] == true && land[i][j + 1] == true && land[i + 1][j + 1] == true && land[i][j + 2] == true && land[i + 1][j + 2] == true && land[i + 2][j] == true && land[i + 2][j + 1] == true && land[i + 2][j + 2] == true)
				{
					if (land[i + 3][j] == false && land[i + 3][j + 1] == false && land[i + 3][j + 2] == false)
					{
						if (land[i][j + 3] == false && land[i + 1][j + 3] == false && land[i + 2][j + 3] == false)
						{
							if (land[i][j - 1] == false && land[i + 1][j - 1] == false && land[i + 2][j - 1] == false)
							{
								if (land[i - 1][j] == false && land[i - 1][j + 1] == false && land[i - 1][j + 2] == false)
								{
									land[i][j] = false;
									land[i + 1][j] = false;
									land[i + 1][j + 1] = false;
									land[i][j + 1] = false;
									land[i][j + 2] = false;
									land[i + 1][j + 2] = false;
									land[i + 2][j] = false;
									land[i + 2][j + 1] = false;
									land[i + 2][j + 2] = false;
								}
							}
						}
					}
				}
			}

			if (land[i][j] == true)//gets rid of tall rectangles
			{
				if (land[i + 1][j] == true && land[i][j + 1] == true && land[i + 1][j + 1] == true && land[i + 2][j] == true && land[i + 2][j + 1] == true)
				{
					if (land[i + 3][j] == false && land[i + 3][j + 1] == false)
					{
						if (land[i][j + 2] == false && land[i + 1][j + 2] == false && land[i + 2][j + 2] == false)
						{
							if (land[i][j - 1] == false && land[i + 1][j - 1] == false && land[i + 2][j - 1] == false)
							{
								if (land[i - 1][j] == false && land[i - 1][j + 1] == false)
								{
									land[i][j] = false;
									land[i + 1][j] = false;
									land[i + 1][j + 1] = false;
									land[i][j + 1] = false;
									land[i + 2][j] = false;
									land[i + 2][j + 1] = false;
								}
							}
						}
					}
				}
			}

			if (land[i][j] == false && land[i][j + 1] == true && land[i][j - 1] == true)
			{
				land[i][j] = true;
			}

			if (land[i][j] == false && land[i + 1][j] == true && land[i - 1][j] == true)
			{
				land[i][j] = true;
			}
		}
	}
}

void add(int ii, int jj)
{
	using namespace std;

	bool checked[ysize][xsize];
	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			checked[i][j] = false;
		}
	}


	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			int chance = mersenne() % 10;

			if (land[i][j] == false && chance < 3)
			{
				if ((land[i][j - 1] == true && checked[i][j - 1] == false) || (land[i - 1][j] == true && checked[i - 1][j] == false)
					|| (land[i][j + 1] == true && checked[i][j + 1] == false) || (land[i + 1][j] == true && checked[i + 1][j] == false))
				{
					land[i][j] = true;
					checked[i][j] = true;
				}
			}
		}
	}
}

void islandIt(int ii, int jj)
{
	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			int chance = mersenne() % 1000;
			int chance2 = mersenne() % 1000;

			if (land[i][j] == false)
			{
				if (chance < 1)
				{
					land[i][j] = true;
				}
			}

		}
	}
}

void getTerrain(int ii, int jj)
{
	//ok so this function generates the terrain into clumps. It also has sort-of seamless transfer between TEMPERATE and HOT climates
	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			int chance = mersenne();

			if (i <= 2)
			{
				zone[i][j] = COLD;
			}
			if (i > 2 && i <= 37)
			{
				zone[i][j] = TEMPERATE;
			}
			if (i > 37)
			{
				zone[i][j] = HOT;
			}

			if (land[i][j] == false)
				terraType[i][j] = WATER;


			if (zone[i][j] == COLD)
			{
				switch (chance % 3)
				{
				case 0:
					terraType[i][j] = ICE_PLAINS;
					break;
				case 1:
					terraType[i][j] = SNOWY_FOREST;
					break;
				case 2:
					terraType[i][j] = MOUNTAINS;
					break;
				}
			}

			if (zone[i][j] == TEMPERATE)
			{
				switch (chance % 55)
				{
				case 0:
					terraType[i][j] = MOUNTAINS;
					break;
				}

				int mountChance = mersenne() % 5;
				if (terraType[i][j] == MOUNTAINS)
				{
					if (mountChance <= 2)
					{
						terraType[i][j + 1] = MOUNTAINS;
						terraType[i + 1][j] = MOUNTAINS;
						terraType[i][j - 1] = MOUNTAINS;
						terraType[i - 1][j] = MOUNTAINS;
					}
				}
				switch (chance % 80)
				{
				case 0:
					terraType[i][j] = PLAINS;
					break;
				}

				int plainChance = mersenne();
				if (terraType[i][j] == PLAINS)
				{
					if (plainChance % 3 == 0 || plainChance % 3 == 1)
					{
						terraType[i][j + 1] = PLAINS;
						terraType[i + 1][j] = PLAINS;
						terraType[i][j - 1] = PLAINS;
						terraType[i - 1][j] = PLAINS;
					}
				}
				if (terraType[i][j] != MOUNTAINS && terraType[i][j] != PLAINS)
				{
					terraType[i][j] = FOREST;
				}
			}

			if (zone[i][j] == HOT)
			{
				switch (chance % 40)
				{
				case 0:
					terraType[i][j] = MOUNTAINS;
					break;
				}

				int mountChanceH = mersenne() % 5;
				if (terraType[i][j] == MOUNTAINS)
				{
					if (mountChanceH <= 2)
					{
						terraType[i][j + 1] = MOUNTAINS;
						terraType[i + 1][j] = MOUNTAINS;
						terraType[i][j - 1] = MOUNTAINS;
						terraType[i - 1][j] = MOUNTAINS;
					}
				}

				switch (chance % 30)
				{
				case 0:
					terraType[i][j] = JUNGLE;
					break;
				}

				int jungChance = mersenne() % 5;
				if (terraType[i][j] == JUNGLE)
				{
					if (jungChance <= 2)
					{
						terraType[i][j + 1] = JUNGLE;
						terraType[i + 1][j] = JUNGLE;
						terraType[i][j - 1] = JUNGLE;
						terraType[i - 1][j] = JUNGLE;
					}
				}

				switch (chance % 55)
				{
				case 0:
					terraType[i][j] = PLAINS;
					break;
				}

				int plainChanceH = mersenne();
				if (terraType[i][j] == PLAINS)
				{
					if (plainChanceH % 3 <= 1)
					{
						terraType[i][j + 1] = PLAINS;
						terraType[i + 1][j] = PLAINS;
						terraType[i][j - 1] = PLAINS;
						terraType[i - 1][j] = PLAINS;
					}
				}

				if (terraType[i][j] != MOUNTAINS && terraType[i][j] != PLAINS && terraType[i][j] != JUNGLE)
				{
					terraType[i][j] = DESERT;
				}

			}

		}
	}
}

void generateWorldE(int ii, int jj)
{
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	srand(static_cast <unsigned int>(time(0)));
	SetConsoleTextAttribute(hConsole, 15);//text color initially to white
	using namespace std;

	std::mt19937 mersenne(seed[ii][jj]);;

	std::string type = "c";

	if (type == "i")
	{
		probAdd = 11;
		groups = 190;
	}

	else if (type == "c")
	{
		probAdd = 11;
		groups = 85;
	}

	system("CLS");


	//set all probobilities to 48 out of 100
	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			probs[i][j] = 480;

			if (type == "c")
			{
				probs[i][j] = 1;
			}
		}
	}

	if (type == "c")
	{
		int chance[2] = { mersenne() % ysize, mersenne() % xsize };
		int chance2[2] = { mersenne() % ysize, mersenne() % xsize };

		for (int i = 0; i < ysize; ++i)
		{
			for (int j = 0; j < xsize; ++j)
			{
				if (i == chance[1] && j == chance[2])
				{
					probs[i][j] = 999;
				}

				if (i == chance2[1] && j == chance2[2])
				{
					probs[i][j] = 999;
				}
			}
		}
	}


	for (int i = 0; i < groups; ++i)//this is where the probabilities are done
	{
		clump(ii, jj);
	}

	for (int i = 0; i < ysize; ++i)//tells whether the land is t or f based on probs
	{
		for (int j = 0; j < xsize; ++j)
		{
			int chance = mersenne() % 100;
			int prob = probs[i][j];

			if (chance < prob)
			{
				land[i][j] = true;

			}
			if (chance >= prob)
			{
				land[i][j] = false;
			}
		}
	}

	//clear gets rid of the tiny islands
	clear(ii, jj);
	clear(ii, jj);

	//smooth gets rid of those annoying corner blocks
	smooth(ii, jj);
	smooth(ii, jj);
	smooth(ii, jj);

	clear(ii, jj);

	for (int i = 0; i < ysize; ++i)//gets rid of the land at the bottom that is always there due to a relationship error
	{
		for (int j = 0; j < xsize; ++j)
		{
			if (i >= ysize - 1)
			{
				land[i][j] = false;
			}
		}
	}

	if (type == "c")//If there are continents, this is where they grow. Well, add() is where they grow.
	{
		for (int a = 0; a < 25; ++a)
		{
			add(ii, jj);
		}

		islandIt(ii, jj);
	}

	smooth(ii, jj);//just smoothing out the newly created continents. This is fine even with the islands type, so it's not in an if()



	getTerrain(ii, jj);

	//actually prints the land
	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			if (land[i][j] == true)
			{
				curPos(j, i);

				switch (terraType[i][j])
				{
				case ICE_PLAINS:
					SetConsoleTextAttribute(hConsole, 11);
					cout << "#";
					break;
				case SNOWY_FOREST:
					SetConsoleTextAttribute(hConsole, 7);
					cout << static_cast<char>(6);
					break;
				case MOUNTAINS:
					SetConsoleTextAttribute(hConsole, 8);
					cout << static_cast<char>(30);
					break;
				case FOREST:
					SetConsoleTextAttribute(hConsole, 2);
					cout << static_cast<char>(24);
					break;
				case PLAINS:
					SetConsoleTextAttribute(hConsole, 6);
					cout << "#";
					break;
				case JUNGLE:
					SetConsoleTextAttribute(hConsole, 10);
					cout << static_cast<char>(244);
					break;
				case DESERT:
					SetConsoleTextAttribute(hConsole, 6);
					cout << static_cast<char>(247);
					break;
				}

			}


			if (land[i][j] == false)
			{
				curPos(j, i);

				SetConsoleTextAttribute(hConsole, 1);
				cout << static_cast<char>(247);

			}
		}
	}

	SetConsoleTextAttribute(hConsole, 15);


	//What appears below the world
	for (int i = 0; i < xsize; ++i)
		cout << "-";

	cout << "\t\t\t\t\t\tThis is the earth-like world you have selected.";
	cout << "\n\t\t\t\t\t\tType 'back' to go back to the Galaxy Map\n\t\t\t\t\t\t";
	string direction = "string";
	redo7:
	cin >> direction;

	if (direction == "back")
	{
		system("CLS");
		printG();
	}
	else
	{
		cout << "Try Again!\n\t\t\t\t\t\t";
		goto redo7;
	}
}

void generateWorldI(int ii, int jj)
{
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	srand(static_cast <unsigned int>(time(0)));
	SetConsoleTextAttribute(hConsole, 15);//text color initially to white
	using namespace std;

	std::string type = "i";

	if (type == "i")
	{
		probAdd = 11;
		groups = 200;
	}

	else if (type == "c")
	{
		probAdd = 11;
		groups = 85;
	}

	system("CLS");


	//set all probobilities to 48 out of 100
	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			probs[i][j] = 480;

			if (type == "c")
			{
				probs[i][j] = 1;
			}
		}
	}

	if (type == "c")
	{
		bool chance[2] = { mersenne() % ysize, mersenne() % xsize };
		bool chance2[2] = { mersenne() % ysize, mersenne() % xsize };

		for (int i = 0; i < ysize; ++i)
		{
			for (int j = 0; j < xsize; ++j)
			{
				if (i == chance[1] && j == chance[2])
				{
					probs[i][j] = 999;
				}

				if (i == chance2[1] && j == chance2[2])
				{
					probs[i][j] = 999;
				}
			}
		}
	}


	for (int i = 0; i < groups; ++i)//this is where the probabilities are done
	{
		clump(ii, jj);
	}

	for (int i = 0; i < ysize; ++i)//tells whether the land is t or f based on probs
	{
		for (int j = 0; j < xsize; ++j)
		{
			int chance = mersenne() % 100;
			int prob = probs[i][j];

			if (chance < prob)
			{
				land[i][j] = true;

			}
			if (chance >= prob)
			{
				land[i][j] = false;
			}
		}
	}

	//clear gets rid of the tiny islands
	clear(ii, jj);
	clear(ii, jj);

	//smooth gets rid of those annoying corner blocks
	smooth(ii, jj);
	smooth(ii, jj);
	smooth(ii, jj);

	clear(ii, jj);

	for (int i = 0; i < ysize; ++i)//gets rid of the land at the bottom that is always there due to a relationship error
	{
		for (int j = 0; j < xsize; ++j)
		{
			if (i >= ysize - 1)
			{
				land[i][j] = false;
			}
		}
	}

	if (type == "c")//If there are continents, this is where they grow. Well, add() is where they grow.
	{
		for (int a = 0; a < 25; ++a)
		{
			add(ii, jj);
		}

		islandIt(ii, jj);
	}

	smooth(ii, jj);//just smoothing out the newly created continents. This is fine even with the islands type, so it's not in an if()



	getTerrain(ii, jj);

	//actually prints the land
	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			curPos(j, i);

			if (land[i][j] == true)
			{
				switch (terraType[i][j])
				{
				case ICE_PLAINS:
					SetConsoleTextAttribute(hConsole, 11);
					cout << "#";
					break;
				case SNOWY_FOREST:
					SetConsoleTextAttribute(hConsole, 7);
					cout << static_cast<char>(6);
					break;
				case MOUNTAINS:
					SetConsoleTextAttribute(hConsole, 8);
					cout << static_cast<char>(30);
					break;
				case FOREST:
					SetConsoleTextAttribute(hConsole, 2);
					cout << static_cast<char>(24);
					break;
				case PLAINS:
					SetConsoleTextAttribute(hConsole, 6);
					cout << "#";
					break;
				case JUNGLE:
					SetConsoleTextAttribute(hConsole, 10);
					cout << static_cast<char>(244);
					break;
				case DESERT:
					SetConsoleTextAttribute(hConsole, 6);
					cout << static_cast<char>(247);
					break;
				}

			}


			if (land[i][j] == false)
			{
				SetConsoleTextAttribute(hConsole, 1);
				cout << static_cast<char>(247);

			}
		}
	}

	SetConsoleTextAttribute(hConsole, 15);


	//What appears below the world
	for (int i = 0; i < xsize; ++i)
		cout << "-";

	
	cout << "\t\t\t\t\t\tThis is the water world you have selected.";

	cout << "\n\t\t\t\t\t\tType 'back' to go back to the Galaxy Map\n\t\t\t\t\t\t";
	string direction = "string";
redo7:
	cin >> direction;

	if (direction == "back")
	{
		system("CLS");
		printG();
	}
	else
	{
		cout << "\t\t\t\t\t\tTry Again!\n\t\t\t\t\t\t";
		goto redo7;
	}
}


//end of earthlike and and ocean generations
//----------------------------------------------
//beginning of cloud generation


void generateWorldC(int ii, int jj)
{
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	srand(static_cast <unsigned int>(time(0)));
	SetConsoleTextAttribute(hConsole, 15);//text color initially to white
	using namespace std;

	system("CLS");

	std::mt19937 mersenne(seed[ii][jj]);;

	int randomSeed = mersenne() % 5;

	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			switch (randomSeed)
			{
			case 0:
				clouds[i][j] = dark_red;
				break;
			case 1:
				clouds[i][j] = dark_green;
				break;
			case 2:
				clouds[i][j] = dark_yellow;
				break;
			case 3:
				clouds[i][j] = purple;
				break;
			case 4:
				clouds[i][j] = gray;
				break;
			}
		}

	}

	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			switch (clouds[i][j])
			{
			case dark_red:
				color(dark_red);
				cout << "#";
				break;
			case dark_green:
				color(dark_green);
				cout << "#";
				break;
			case dark_yellow:
				color(dark_yellow);
				cout << "#";
				break;
			case purple:
				color(dark_purple);
				cout << "#";
				break;
			case gray:
				color(gray);
				cout << "#";
				break;
			}

		}
	}

	color(white);

	//What appears below the world
	for (int i = 0; i < xsize; ++i)
		cout << "-";


	cout << "\t\t\t\t\t\tThis is the gas world you have selected.";

	cout << "\n\t\t\t\t\t\tType 'back' to go back to the Galaxy Map\n\t\t\t\t\t\t";
	string direction = "string";
redo7:
	cin >> direction;

	if (direction == "back")
	{
		system("CLS");
		printG();
	}
	else
	{
		cout << "Try Again!\n\t\t\t\t\t\t";
		goto redo7;
	}


}

//----------------------------------------------
//beginning of main program


void generatePlanets()
{
	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			int chance = mersenne() % starchance;

			if (chance == 0)
			{
				planet[i][j] = 1;
				
				int starChance = mersenne() % 100;

				if (starChance <= 29)
				{
					planetType[i][j] = earthlike;
				}
				else if (starChance >= 30 && starChance <= 44)
				{
					planetType[i][j] = ocean;
				}
				else if (starChance >= 45 && starChance <= 59)
				{
					planetType[i][j] = forest;
				}
				else if (starChance >= 60 && starChance <= 79)
				{
					planetType[i][j] = ice;
				}
				else if (starChance >= 80 && starChance <= 87)
				{
					planetType[i][j] = cloud;
				}
				else if (starChance >= 88 && starChance <= 95)
				{
					planetType[i][j] = lava;
				}
				else if (starChance >= 96)
				{
					planetType[i][j] = swamp;
				}

			}

			else if (chance >= 1)
			{
				planet[i][j] = 0;
			}
		}
	}
}

void assignID() // My complete Planet Identification System v. 1.0.2
{
	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			if (planet[i][j] == true && planet[i][j + 1] == true)
			{
				planet[i][j + 1] = false;
			}
		}
	}

	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			if (planet[i][j] == true)
			{
				planetNumber++;
			}
		}
	}

	std::vector<char>symbol = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S'};
	symbol.resize(planetNumber);

	int planety = 0;
	int planetx = 0;

	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			ID[i][j] = '0';
		}
	}


	for (int h = 0; h < planetNumber; ++h)
	{
		for (int i = 0; i < ysize; ++i)
		{
			for (int j = 0; j < xsize; ++j)
			{
				if (planet[i][j] == true && ID[i][j] == '0')
				{
					planety = i;
					planetx = j;

					goto thing;
				}

			}
		}
	thing:

		ID[planety][planetx] = symbol[h];

	}

	// PRNG seed generation for planet concistency

	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			seed[i][j] = mersenne();
		}
	}
}

void redirect()
{
	using namespace std;

	char response = '0';

	redo:

	cin >> response;

	for (int i = 0; i < ysize; ++i)
	{
		for (int j = 0; j < xsize; ++j)
		{
			if (ID[i][j] == response)
			{
				if (planet[i][j] == true)
				{
					switch (planetType[i][j])
					{
					case ocean:
						generateWorldI(i, j);
						break;
					case earthlike:
						generateWorldE(i, j);
						break;
					case lava:

						break;
					case ice:

						break;
					case swamp:

						break;
					case forest:

						break;
					case cloud:
						generateWorldC(i, j);
						break;
					}
				}
				else
				{
					cout << "\t\t\t\t\t\t\tNOT A VALID ID\n\t\t\t\t\t\t\t";
					goto redo;
				}
			}
			else if (i == ysize - 1 && j == xsize - 1)
			{
				cout << "\t\t\t\t\t\t\tNOT A VALID ID\n\t\t\t\t\t\t\t";
				goto redo;
			}
		}
	}
	
}

int main()
{
	using namespace std;
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	color(white);//text color initially to white
	system("MODE 180, 80");//set  the screen size to fullscreen

	cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\t\tMAKE SURE TO TURN THIS FULLSCREEN BEFORE YOU CONTINUE";

	Sleep(3000);

	system("CLS");

	generatePlanets();
	assignID();
	printG();
	redirect();

	curPos(180, 50);

	std::cin.clear(); // reset any error flags
	std::cin.ignore(32767, '\n'); // ignore any characters in the input buffer until we find an enter character
	std::cin.get(); // get one more char from the user

	return 0;
}