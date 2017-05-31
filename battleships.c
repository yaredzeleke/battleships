#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#define SIZE 12
#define NUMBER_OF_SHIPS 8
#define HITS_TO_WIN 18
#define NAMELENGTH 17

typedef struct point{
  int x;
  int y;
} Point;

typedef struct ship{
  int size;
  int direction;
  char *name;
  Point pt;
} Ship;

typedef struct player{
  int hits;
  int world1[SIZE][SIZE];
  int world2[SIZE][SIZE];
  char name[NAMELENGTH];
  Ship ships[NUMBER_OF_SHIPS];
  bool won;
} Player;

// clear the input, if the user puts more values than expectet in
void clearInput()
{
  char c;
  
  while ((c = getchar()) != '\n' && c != EOF) { }
}

// clear the terminal screen
void clearScreen()
{
  printf("\033[2J\033[1;1H");
}

int printIntro(int *gametype)
{
  int scan = 0;
  
  printf("                                     |__\n"
	 "                                     |\\/\n"
	 "                                     ---\n"
	 "                                     / | [\n"
	 "                              !      | |||\n"
	 "                            _/|     _/|-++'\n"
	 "                        +  +--|    |--|--|_ |-\n"
	 "                     { /|__|  |/\\__|  |--- |||__/\n"
	 "                    +---------------___[}-_===_.'____                 /\\\n"
	 "                ____`-' ||___-{]_| _[}-  |     |_[___\\==--            \\/   _\n"
	 " __..._____--==/___]_|__|_____________________________[___\\==--____,------' .7\n"
	 "|                                                                     BB-61/\n"
	 "\\_________________________________________________________________________|\n");

  printf("\nWelcome to Battleships!\n");
  
  printf("\nRules for Singleplayer:\n"
	 "1) The Computer places its ships on the board.\n"
	 "2) You shoot at the board until you sunk every ship.\n"
	 "3) You win!\n");

  printf("\nRules for Multiplayer:\n"
	 "1) Every Player gets 1 Carrier, 1 Battleship, 1 Cruiser, 2 Submarines and 3 Destroyer.\n"
	 "2) The Players shoot in turns, after you shot, the other Players Turn begin.\n"
	 "3) The Player who first sunk all enemy ships wins.\n");
  
  while(scan != 1 || (*gametype != 1 && *gametype != 2))
    {
      printf("\nPlease select a game type 1) Singleplayer 2) Multiplayer: ");
      scan = scanf("%d", gametype);
      clearInput();
    }
  
  return *gametype;
}

void initBoard(int board[SIZE][SIZE])
{
  // set the board to all water
  for(int i = 0; i < SIZE; i++)
    for(int j = 0; j < SIZE; j++)
      board[i][j] = 0;

  // set a border for the world
  for(int i = 0; i < SIZE; i++)
    {
      board[i][SIZE - 1] = 3;
      board[SIZE - 1][i] = 3;
      board[i][0] = 3;
      board[0][i] = 3;
    }
}

// every player gets a certain number of ships
void initShips(Player *player)
{
  int i;
  Ship carrier, battleship, cruiser, submarine, destroyer;
  
  carrier.size = 5;
  carrier.name = "Carrier";
  
  battleship.size = 4;
  battleship.name = "Battleship";
  
  cruiser.size = 3;
  cruiser.name = "Cruiser";
  
  submarine.size = 2;
  submarine.name = "Submarine";
  
  destroyer.size = 1;
  destroyer.name = "Destroyer";
  
  player->ships[0] = carrier;
  player->ships[1] = battleship;
  player->ships[2] = cruiser;

  for(i = 3; i < 5; i++)
    player->ships[i] = submarine;
  
  for(i = 5; i < NUMBER_OF_SHIPS; i++)
    player->ships[i] = destroyer;
}

void initPlayer(Player *player)
{
  int scan;
  
  while(true)
    {
      printf("\nPlease enter your Name Player %c (1 - 16 characters): ", player->name[0]);
      scan = scanf("%16s", player->name);
      clearInput();
      if(scan != 1)
	{
	  printf("Please enter a valid name.\n");
	  continue;
	}
      else
	break;
    }
  initBoard(player->world1);
  initBoard(player->world2);
  initShips(player);
  player->hits = 0;
  player->won = false;
}

void printBoard(int board[SIZE][SIZE])
{
  printf("\n   1 2 3 4 5 6 7 8 9 10\n"); // y-coordinates
  
  for(int i = 1; i < SIZE - 1; i++)
    {
      printf("%2d ", i); // x-coordinates
      for(int j = 1; j < SIZE - 1; j++)
	{
	  switch(board[i][j])
	    {
	    case 0:
	      {
		printf("~ "); // water
		break;
	      }
	      
	    case 1:
	      {
		printf("X "); // ship/hit
		break;
	      }
	      
	    case 2:
	    default:
	      printf("O "); // miss
	    }
	}
      printf("\n");
    }
  printf("\n");
}

// check if the ship is not colliding
bool nextNeighbour(Player *player, Ship ship)
{
  int i;
  
  switch(ship.direction)
    {
    case 0: // north
      {
	for(i = 0; i < ship.size; i++)
	  if(player->world2[ship.pt.x - i][ship.pt.y] == 1
	     || player->world2[ship.pt.x - i][ship.pt.y] == 3)
	    return false;
	break;
      }
  
    case 1: // east
      {
	for(i = 0; i < ship.size; i++)
	  if(player->world2[ship.pt.x][ship.pt.y + i] == 1
	     || player->world2[ship.pt.x][ship.pt.y + i] == 3)
	    return false;
	break;
      }
  
    case 2: // south
      {
	for(i = 0; i < ship.size; i++)
	  if(player->world2[ship.pt.x + i][ship.pt.y] == 1
	     || player->world2[ship.pt.x + i][ship.pt.y] == 3)
	    return false;
	break;
      }
      
    case 3: // west
      {
	for(i = 0; i < ship.size; i++)
	  if(player->world2[ship.pt.x][ship.pt.y - i] == 1
	     || player->world2[ship.pt.x][ship.pt.y - i] == 3)
	    return false;
	break;
      }
      
    default:
      return false;
    }
  
  return true;
}

// deploy ships in given direction
bool deployShip(Player *player, Ship ship)
{
  int i, endpoint;
  bool shipDeployed = false;

  switch(ship.direction)
    {
    case 0: // north
      {
	if(nextNeighbour(player, ship))
	  {
	    endpoint = ship.pt.x - ship.size;
	    for(i = ship.pt.x; i > endpoint; i--)
	      player->world1[i][ship.pt.y] = 1;
	    shipDeployed = true;
	  }
	break;
      }
      
    case 1: // east
      {
	if(nextNeighbour(player, ship))
	  {
	    endpoint = ship.pt.y + ship.size;
	    for(i = ship.pt.y; i < endpoint; i++)
	      player->world1[ship.pt.x][i] = 1;
	    shipDeployed = true;
	  }
	break;
      }
      
    case 2: // south
      if(nextNeighbour(player, ship))
	{
	  endpoint = ship.pt.x + ship.size;
	  for(i = ship.pt.x; i < endpoint; i++)
	    player->world1[i][ship.pt.y] = 1;
	  shipDeployed = true;
	}
      break;
      
    case 3: // west
      {
	if(nextNeighbour(player, ship))
	  {
	    endpoint = ship.pt.y - ship.size;
	    for(i = ship.pt.y; i > endpoint; i--)
	      player->world1[ship.pt.x][i] = 1;
	    shipDeployed = true;
	  }
	break;
      }
      
    default:
      {
	shipDeployed = false;
	break;
      }
    }

  // setting border to ship so ships don't collide
  for(i = 1; i < SIZE - 1; i++)
    {
      for(int j = 1; j < SIZE - 1; j++)
	{
	  if(player->world1[i][j])
	    {
	      player->world2[i][j] = 1;
	      player->world2[i - 1][j] = 1;
	      player->world2[i + 1][j] = 1;
	      player->world2[i][j - 1] = 1;
	      player->world2[i][j + 1] = 1;
	    }
	}
    }
  
  return shipDeployed;
}

// only for singleplayer needed
void randomlyDeployShips(Player *computer)
{
  bool shipDeployed;
  time_t t;

  initShips(computer);
  
  srand((unsigned) time(&t));

  for(int i = 0; i < NUMBER_OF_SHIPS; i++)
    {
      computer->ships[i].direction = rand() % 3;
      computer->ships[i].pt.x = rand() % (SIZE - 1);
      computer->ships[i].pt.y = rand() % (SIZE - 1);
      
      shipDeployed = deployShip(computer, computer->ships[i]);
      
      if(!shipDeployed)
	i--;
    }
  
  initBoard(computer->world2);
}

bool isValidCoordinate(Point coordinate)
{
  if(coordinate.x > 0 && coordinate.x < SIZE - 1)
    if(coordinate.y > 0 && coordinate.y < SIZE - 1)
      return true;

  return false;
}

// only for multiplayer needed
void manuallyDeployShips(Player *player)
{
  int nbCoordinates, nbDirection;
  bool error, shipDeployed;

  printf("\n%s\n", player->name);
  printf("Please place your ships.\n");
  printBoard(player->world1);
  
  for(int i = 0; i < NUMBER_OF_SHIPS; i++)
    {
      shipDeployed = true;
      error = false;
      printf("Please set a starting point for your %s(%d) (y x): \n",
	     player->ships[i].name, player->ships[i].size);
      nbCoordinates = scanf("%d %d", &player->ships[i].pt.x, &player->ships[i].pt.y);
      clearInput();

      /*
       * scanf exception handling
       * Note: trying to avoid goto with the error variable
       */
      if(nbCoordinates < 2 || !isValidCoordinate(player->ships[i].pt))
	{
	  printf("\nPlease enter valid coordinates (between 1-10)!\n");
	  error = true;
	  i--;
	}
      
      if(!error)
	{
	  printf("Please set a direction for your %s (North(0), East(1), South(2), West(3)): \n"
		 , player->ships[i].name);
	  
	  nbDirection = scanf("%d", &player->ships[i].direction);
	  clearInput();
      
	  if(nbDirection < 1 || player->ships[i].direction > 3 || player->ships[i].direction < 0)
	    {
	      printf("\nPlease enter a valid direction (between 1-4)\n");
	      i--;
	    }
	  else
	    shipDeployed = deployShip(player, player->ships[i]);
	}
      
      if(!shipDeployed)
	{
	  printf("Couldn't deploy ship!\n");
	  i--;
	}
      
      printBoard(player->world1);
    }
  
  // clearing the board you shoot at
  initBoard(player->world2);

  clearScreen();
}

bool isSunk(Player *player, Point *point)
{
  int nb1 = 0;
  int nb2 = 0;
  int i = 1;
  
  while(player->world1[point->x + i][point->y] == 1)
    {
      nb1++;
      i++;
    }
  i = 1;
  while(player->world1[point->x - i][point->y] == 1)
    {
      nb1++;
      i++;
    }
  i = 1;
  while(player->world1[point->x][point->y + i] == 1)
    {
      nb1++;
      i++;
    }
  i = 1;
  while(player->world1[point->x][point->y - i] == 1)
    {
      nb1++;
      i++;
    }

  
  i = 1;
  while(player->world2[point->x + i][point->y] == 1)
    {
      nb2++;
      i++;
    }
  i = 1;
  while(player->world2[point->x - i][point->y] == 1)
    {
      nb2++;
      i++;
    }
  i = 1;
  while(player->world2[point->x][point->y + i] == 1)
    {
      nb2++;
      i++;
    }
  i = 1;
  while(player->world2[point->x][point->y - i] == 1)
    {
      nb2++;
      i++;
    }    
    
  if(nb1 == nb2)
    return true;
  return false;
}

void shoot(Player *player)
{
  int nbShotCoordinates;
  Point shot;
  bool validShot = false;

  while(!validShot)
    {
      validShot = true;
      
      printf("Please put in the coordinates you want to shoot (y x): ");
      nbShotCoordinates = scanf("%d %d", &shot.x, &shot.y);
      clearInput();

      // scanf exception handling
      if(nbShotCoordinates < 2)
	validShot = false;

      else if(!isValidCoordinate(shot))
	{
	  printf("\nPlease enter valid coordinates (between 1-10)!\n");
	  validShot = false;
	}

      else if(player->world2[shot.x][shot.y] != 0)
	{
	  printf("\nYou already shot those coordinates!\n");
	  validShot = false;
	}
    }

  if(player->world1[shot.x][shot.y] == 1)
    {
      if(isSunk(player, &shot))
	printf("\nShip sunk!\n");
      else
	printf("\nHit!\n");
      player->world2[shot.x][shot.y] = 1;
      player->hits++;
    }
  else
    {
      printf("\nMiss!\n");
      player->world2[shot.x][shot.y] = 2;
    }
  
  printBoard(player->world2);
}

void isWon(Player *player)
{
  if(player->hits > HITS_TO_WIN)
    player->won = true;
  else
    player->won = false;
}

void turn(Player *player)
{
  printBoard(player->world2);
  shoot(player);
  isWon(player);
}

void waitingToContinue()
{
  printf("Press any key to continue.");
  getchar();
  clearScreen();
}

void playGame(int *gametype)
{
  switch(*gametype)
    {
    case 1: // Singleplayer
      {
	Player computer;
	
	computer.won = false;
	computer.hits = 0;
	initBoard(computer.world1);
	initBoard(computer.world2);

	randomlyDeployShips(&computer);
	printBoard(computer.world2);
	
	while(!computer.won)
	  {
	    shoot(&computer);
	    isWon(&computer);
	  }
	printf("Congratulations! You Won!\n");
	break;
      }
      
    case 2: // Multiplayer
      {
	Player player1;
	Player player2;

	player1.name[0] = '1';
	player2.name[0] = '2';
	
	initPlayer(&player1);
	initPlayer(&player2);
	
	manuallyDeployShips(&player1);
	manuallyDeployShips(&player2);

	while(!player1.won && !player2.won)
	  {

	    printf("%s, your turn.\n", player1.name);
	    turn(&player2);

	    /*
	     * it counts the hits on the board you're shooting at, 
	     * so if Player 2 wins it's saved in the Player 1 object
	     */
	    if(player2.won)
	      {
		printf("Congratulations Player 1!\n");	    
		break;
	      }

	    waitingToContinue();
	    
	    printf("%s, your turn.\n", player2.name);
	    turn(&player1);
	    
	    if(player1.won)
	      {
		printf("Congratulations Player 2!\n");
		break;
	      }
	    
	    waitingToContinue();
	  }
      }
    }
}

bool wantsToPlayAgain()
{
  int scan;
  char answer;

  printf("\nDo you want to play again? (y/n): ");
  scan = scanf("%c", &answer);
  clearInput();

  if(scan != 1 || answer == 'n')
    return false;
  else if(answer == 'y')
    {
      clearScreen();
      return true;
    }
  else
    {
      printf("Unknown Input. The Programm will close now.\n");
      return false;
    }
}

int main(void)
{
  int gametype = 0;
  bool playAgain = true;

  while(playAgain)
    {
      printIntro(&gametype);
      playGame(&gametype);
      playAgain = wantsToPlayAgain();
    }
  
  return EXIT_SUCCESS;
}
