#include "AIE.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

struct Block{
	unsigned int spriteId, min, max, upKey, downKey;
	float width, height, x, y;

	void SetSize(float a_width, float a_height){
		width = a_width;
		height = a_height;
	}
	void SetPosition(float a_x, float a_y){
		x = a_x;
		y = a_y;
	}
	void SetMovementKeys(unsigned int a_up, unsigned int a_down){
		upKey = a_up;
		downKey = a_down;
	}
	void SetMoveExtremes(unsigned int a_min, unsigned int a_max){
		min = a_min;
		max = a_max;
	}
	void Move(float a_TimeStep, float a_Speed){
		if (IsKeyDown(upKey))
		{
			y += a_TimeStep * a_Speed;
			if (y > max - height * .5f){
				y = max - height * .5f;
			}
		}
		else if (IsKeyDown(downKey))
		{
			y -= a_TimeStep * a_Speed;
			if (y < min + height * .5f){
				y = min + height * .5f;
			}
		}
		MoveSprite(spriteId, x, y);
	}
};

enum GAMESTATE{
	MAIN_MENU,
	GAMEPLAY,
	HIGH_SCORE
};

GAMESTATE curGamestate;

//initialize
Block player1, player2;
Block ball;

const int screenWidth = 800;
const int screenHeight = 500;

const char* font = "./fonts/invaders.fnt";


//////////////////////functions//////////////////////////////////////////////////

int ballMove(float a_TimeStep, float &a_xSpeed, float &a_ySpeed, int direction);
/* increases the x and y of the ball sprite depending in where it is and 
   changed directions if hits paddle*/

void scoring(int &a_p1Score, int &a_p2Score, float &a_xSpeed);
/* increase respective player's score if reaches their end of screen*/

void displayWinner(int a_p1Score, int a_p2Score);

int countRally(int &a_rally, int a_highScore);
/////////////////////////////////////////////////////////////////////////////////

int main( int argc, char* argv[] )
{	

	//initialize screen settings
	Initialise(screenWidth, screenHeight, false, "Pong");
	SetBackgroundColour(SColour(0x00, 0x00, 0x00, 0xFF));

	
	// setup player 1
	player1.SetMovementKeys('Q', 'A');
	player1.SetSize(screenWidth * .01f ,screenHeight * .1f);
	player1.SetMoveExtremes(screenHeight * .02f, screenHeight * .98f);
	player1.SetPosition(screenWidth * .04f, screenHeight * .5f);
	player1.spriteId = CreateSprite("./images/block.png", player1.width, player1.height, true);
	MoveSprite(player1.spriteId, player1.x, player1.y);

	// setup player 2
	player2.SetMovementKeys('O', 'L');
	player2.SetSize(screenWidth * .01f, screenHeight * .1f);
	player2.SetMoveExtremes(screenHeight * .02f, screenHeight * .98f);
	player2.SetPosition(screenWidth * .96f, screenHeight * .5f);
	player2.spriteId = CreateSprite("./images/block.png", player2.width, player2.height, true);
	MoveSprite(player2.spriteId, player2.x, player2.y);

	//setup ball
	ball.SetSize(screenWidth * .013f, screenHeight * .02f);
	ball.SetPosition(screenWidth * .5f, screenHeight * .9f);
	ball.spriteId = CreateSprite("./images/ball.png", ball.width, ball.height, true);
	MoveSprite(ball.spriteId, ball.x, ball.y);


	AddFont(font);

	int direction = 1;

	//scores
	int p1Score = 0; 
	int p2Score = 0;

	//stuff for the ball
	int changeDir = -1;
	float ySpeed = 175;
	float xSpeed = 175;

	fstream file;

	char scores[32];
	int rally = 0;
	int highScore = 0;
	
	
    //***********************Game Loop************************
	do
	{
		ClearScreen();

		float fDeltaT = GetDeltaTime();

		// convert int to string
		string sP1Score = to_string(p1Score);
		string sP2Score = to_string(p2Score);
		char const *cP1Score = sP1Score.c_str();
		char const *cP2Score = sP2Score.c_str();

		string sHS = to_string(highScore);
		char const *cHS = sHS.c_str();

		SetFont(font);

		switch (curGamestate)
		{
		case MAIN_MENU:
			DrawString("PONG", screenWidth * .46, screenHeight *.7, SColour(0xFF, 0xFF, 0xFF, 0xFF));
			DrawString("1. Play", screenWidth * .35, screenHeight *.6, SColour(0xFF, 0xFF, 0xFF, 0xFF));
			DrawString("2. High Score", screenWidth * .35, screenHeight *.5, SColour(0xFF, 0xFF, 0xFF, 0xFF));
			DrawString("3. Exit", screenWidth * .35, screenHeight *.4, SColour(0xFF, 0xFF, 0xFF, 0xFF));
			if (IsKeyDown('1'))
			{
				curGamestate = GAMEPLAY;
			}
			if (IsKeyDown('2'))
			{
				curGamestate = HIGH_SCORE;
			}
			if (IsKeyDown('3'))
			{
				return 0;
			}
			break;

		case GAMEPLAY:
			// draw sprites
			DrawSprite(player1.spriteId);
			DrawSprite(player2.spriteId);
			DrawSprite(ball.spriteId);
			//draw board
			DrawString(cP1Score, screenWidth * .4f, screenHeight * .95f, SColour(0xFF, 0xFF, 0xFF, 0xFF));
			DrawString(cP2Score, screenWidth * .6f, screenHeight * .95f, SColour(0xFF, 0xFF, 0xFF, 0xFF));
			DrawLine(screenWidth*.5f, screenHeight * .02, screenWidth*.5f, screenHeight*.98f, SColour(0xFF, 0xFF, 0xFF, 0xFF));
			DrawLine(screenWidth*.02f, screenHeight * .98, screenWidth*.98f, screenHeight*.98f, SColour(0xFF, 0xFF, 0xFF, 0xFF));
			DrawLine(screenWidth*.02f, screenHeight * .02, screenWidth*.98f, screenHeight*.02f, SColour(0xFF, 0xFF, 0xFF, 0xFF));
			DrawLine(screenWidth*.02f, screenHeight * .02, screenWidth*.02f, screenHeight*.98f, SColour(0xFF, 0xFF, 0xFF, 0xFF));
			DrawLine(screenWidth*.98f, screenHeight * .02, screenWidth*.98f, screenHeight*.98f, SColour(0xFF, 0xFF, 0xFF, 0xFF));

			//moving things
			player1.Move(fDeltaT, 100);
			player2.Move(fDeltaT, 100);

			if (ball.y <= screenHeight * .03f || ball.y >= screenHeight * .97f){
				ySpeed *= changeDir;
			}

			direction = ballMove(fDeltaT, xSpeed, ySpeed, direction);
			highScore = countRally(rally, highScore);
			scoring(p1Score, p2Score, xSpeed);

			// display winner
			displayWinner(p1Score, p2Score);
			
			if (p1Score == 10 && IsKeyDown(257) || p2Score == 10 && IsKeyDown(257))
			{
				curGamestate = HIGH_SCORE;
			}
			break;

		case HIGH_SCORE:
			//file shit
			file.open("highScores.txt", std::fstream::in | std::fstream::out);

			if (file.is_open()){}
			else{
				cout << "file is not found or cannot be opened...";
				return 0;
			}
			
			file << highScore;
			file.getline(scores, 32);

			//draw file shit
			DrawString("Longest Rally", screenWidth * .4f, screenHeight *.6f, SColour(0xFF, 0xFF, 0xFF, 0xFF));
			DrawString(cHS, screenWidth * .5f, screenHeight *.5f, SColour(0xFF, 0xFF, 0xFF, 0xFF));

			//draw other shit
			DrawString("press (p) to play again", screenWidth * .33f, screenHeight *.2f, SColour(0xFF, 0xFF, 0xFF, 0xFF));
			DrawString("or (q) to quit", screenWidth * .4f, screenHeight *.1f, SColour(0xFF, 0xFF, 0xFF, 0xFF));

			file.close();

			if (IsKeyDown('P'))
			{
				cout << " hello";
				p1Score = 0;
				p2Score = 0;
				curGamestate = GAMEPLAY;
			}

			if (IsKeyDown('Q'))
			{
				return 0;
			}

			break;

		default:
			break;
		}
	

	} while(FrameworkUpdate() == false);
	//*******************************************************
		

    Shutdown();

    return 0;
}


int ballMove(float a_TimeStep, float &a_xSpeed, float &a_ySpeed, int a_direction)
{
	if (a_direction == 1)
	{
		ball.x += a_TimeStep * a_xSpeed ;
		ball.y += a_TimeStep * a_ySpeed *.5f;
		//collision
		if (ball.y <= player2.y + player2.height * .5f && ball.y >= player2.y - player2.height * .5f && ball.x >= screenWidth * .96f)
		{
			ball.x = screenWidth * .96f;
			a_xSpeed += 50;
			a_direction = 2;
		}
	}
	else
	{
		ball.x -= a_TimeStep * a_xSpeed;
		ball.y += a_TimeStep * a_ySpeed *.5f;
		//collision
		if (ball.y <= player1.y + player1.height * .5f && ball.y >= player1.y - player1.height * .5f && ball.x <= screenWidth * .04f){
			ball.x = screenWidth * .04f;
			a_xSpeed += 50;
			a_direction = 1;
		}
	}
	MoveSprite(ball.spriteId, ball.x, ball.y);
	return a_direction;
}

void scoring(int &a_p1Score, int &a_p2Score, float &a_xSpeed)
{
	if (ball.x > screenWidth * .98f){
		a_p1Score += 1;
		ball.SetPosition(screenWidth * .5f, screenHeight * .9f);
		MoveSprite(ball.spriteId, ball.x, ball.y);
		a_xSpeed = 175;
	}
	else if (ball.x < screenWidth * .02f){
		a_p2Score  += 1;
		ball.SetPosition(screenWidth * .5f, screenHeight * .9f);
		MoveSprite(ball.spriteId, ball.x, ball.y);
		a_xSpeed = 175;
	}
}

void displayWinner(int a_p1Score, int a_p2Score)
{
	char * winner;
	if (a_p1Score == 10){
		winner = "Player 1 Wins!";
		DrawString(winner, screenWidth * .4f, screenHeight *.5f, SColour(0xFF, 0xFF, 0xFF, 0xFF));
		DrawString("press enter to continue...", screenWidth * .3f, screenHeight *.1f, SColour(0xFF, 0xFF, 0xFF, 0xFF));
		ball.SetPosition(screenWidth * .5f, screenHeight * .9f);
	}
	else if (a_p2Score == 10){
		winner = "Player 2 Wins!";
		DrawString(winner, screenWidth * .4f, screenHeight *.5f, SColour(0xFF, 0xFF, 0xFF, 0xFF));
		DrawString("press enter to continue...", screenWidth * .3f, screenHeight *.1f, SColour(0xFF, 0xFF, 0xFF, 0xFF));
		ball.SetPosition(screenWidth * .5f, screenHeight * .9f);
	}

}

int countRally(int &a_rally, int a_highScore)
{
	//count rally
	if (ball.y <= player2.y + player2.height * .5f && ball.y >= player2.y - player2.height * .5f && ball.x >= screenWidth * .96f)
	{
		a_rally++;
		cout << a_rally << endl;
	}
	else if (ball.y <= player1.y + player1.height * .5f && ball.y >= player1.y - player1.height * .5f && ball.x <= screenWidth * .04f)
	{
		a_rally++;
		cout << a_rally << endl;
	}
	//reset rally
	if (ball.x > screenWidth * .98f || ball.x < screenWidth * .02f){
		if (a_rally > a_highScore){
			a_highScore = a_rally;
		}
		a_rally = 0;
	}
	return a_highScore;
}