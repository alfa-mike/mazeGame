

#include <iostream>
#include <math.h>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "GamePlayer.h"
#include "Maze.h"


#define IMG_TITLE "src/Title.png"
#define IMG_GAMEOVER "src/GameOver.png"
#define TEXTFILE "src/TitleFonts.ttf"


int curWindowWidth = 1024;
int curWindowHeight = 800;

SDL_Rect FullScreen{ 0, 0, curWindowWidth, curWindowHeight };

//Gameplay Area Display Parameters
int GameArea_Width = 720;
int GameArea_Height = 640;
int GameArea_xOffset = 0;
int GameArea_yOffset = (curWindowHeight - GameArea_Height);

SDL_Rect GameAreaFillRect{	GameArea_xOffset, 
							GameArea_yOffset, 
							GameArea_Width, 
							GameArea_Height };

//Game Title Parameters
int GameTitle_Width = curWindowWidth;
int GameTitle_Height = curWindowHeight - GameArea_Height;
int GameTitle_xOffset = 0;
int GameTitle_yOffset = 0;
int GameTitle_FontSize = 72;

SDL_Rect GameTitleFillRect
{ 
	GameTitle_xOffset,
	GameTitle_yOffset,
	GameTitle_Width,
	GameTitle_Height 
};

//Game Info GUI Parameters
int GameInfo_Width = curWindowWidth - GameArea_Width;
int GameInfo_Height = curWindowHeight - GameTitle_Height;
int GameInfo_xOffset = GameArea_Width;
int GameInfo_yOffset = GameTitle_Height;

SDL_Rect GameInfoFillRect
{ 
	GameInfo_xOffset,
	GameInfo_yOffset,
	GameInfo_Width,
	GameInfo_Height 
};

//Level Complete Screen Information
int LevelComplete_Width = curWindowWidth * 8 / 10;
int LevelComplete_Height = curWindowHeight * 8 / 10;
int LevelComplete_xOffset = curWindowWidth / 10;
int LevelComplete_yOffset = curWindowHeight / 10;;

SDL_Rect levelCompleteFillRect
{	
	LevelComplete_xOffset,
	LevelComplete_yOffset,
	LevelComplete_Width,
	LevelComplete_Height
};

SDL_Rect levelCompleteTitleRect
{	
	levelCompleteFillRect.x,
	levelCompleteFillRect.y,
	levelCompleteFillRect.w,
	levelCompleteFillRect.h / 3 
};

SDL_Rect levelCompleteInfoRect
{
	levelCompleteTitleRect.x,
	levelCompleteTitleRect.y + levelCompleteTitleRect.h,
	levelCompleteTitleRect.w,
	levelCompleteFillRect.h - levelCompleteTitleRect.h
};
//Game Window
SDL_Window* window = NULL;

//Game Renderer
SDL_Renderer* renderer = NULL;

enum GameState{Menu, InGame, LevelComplete, GameOver};
GameState curGameState = Menu;

//Initialize the SDL2
bool initialization()
{
	bool success = true;
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{	
		//Create window
		window = SDL_CreateWindow("Maze Simulator Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, curWindowWidth, curWindowHeight, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if (renderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			//Initialize Fonts
			else if (TTF_Init() == -1)
			{
				printf("SDL TTF could not initialize");
				success = false;
			}
		}
	}
	return success;
}


void FullScreenTexture(const char* fileName)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_Texture* screenTexture = IMG_LoadTexture(renderer, fileName);
	if (screenTexture == NULL)
	{
		printf("SDL could not load the Texture from ");
		printf(fileName);
		return;
	}
	SDL_RenderCopy(renderer, screenTexture, NULL, &FullScreen);
	SDL_RenderPresent(renderer);
}

void DrawTextFromRect(std::string text, SDL_Rect * textRect, int fontSize)
{
	
	TTF_Font *font = TTF_OpenFont(TEXTFILE, fontSize);
	if (font == NULL)
	{
		printf("SDL could not load the Font from ");
		printf(TEXTFILE);
		return;
	}
	SDL_Color fontColor = { 255, 255, 0, 255 };

	SDL_Surface * textSurface = TTF_RenderText_Solid(font, text.c_str(), fontColor);

	
	int text_xOffset = textRect->x + (textRect->w - textSurface->w) / 2;
	int text_yOffset = textRect->y + (textRect->h - textSurface->h) / 2;

	
	SDL_Texture * textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_Rect DisplayTextRect = { text_xOffset, text_yOffset, textSurface->w, textSurface->h };

	
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, textRect);
	SDL_RenderCopy(renderer, textTexture, NULL, &DisplayTextRect);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(renderer, textRect);
}


void DrawTextFromRectParams(std::string text, int xOffset, int yOffset, int width, int height, int fontSize)
{
	
	SDL_Rect textRect = { 
		xOffset,
		yOffset,
		width,
		height 
	};
	DrawTextFromRect(text, &textRect, fontSize);
}


void DrawTextWithAdjCenteredNumber(std::string text, std::string count, int xOffset, int yOffset, int width, int height, int fontSize)
{
	//Write Text Portion
	int textXOffset = xOffset;
	int textYOffset = yOffset;
	int textWidth = width - height;
	int textHeight = height;
	DrawTextFromRectParams(text, textXOffset, textYOffset, textWidth, textHeight, fontSize);
	
	//WriteIntPortion
	int intXOffset = textXOffset + textWidth;
	int intYOffset = yOffset;
	int intWidth = height;
	int intHeight = height;
	DrawTextFromRectParams(count, intXOffset, intYOffset, intWidth, intHeight, fontSize);	
}

 
void DrawTextMultipleRows(std::vector<std::string> &text, std::vector<std::string> &data, int xOffset, int yOffset, int width, int height, int fontSize)
{
	int rowCount = text.size();
	int rowHeight = height / rowCount;
	for (int i = 0; i < rowCount; i++)
	{
		
		if(i < static_cast<int>( data.size()))
		{
			DrawTextWithAdjCenteredNumber(text[i], data[i], xOffset, yOffset + rowHeight * i, width, rowHeight, fontSize);
		}
		else
		{
			DrawTextFromRectParams(text[i], xOffset, yOffset + rowHeight * i, width, rowHeight, fontSize);
		}
	}
}

//Draws in-game information
void DrawInGameGUI(std::string level, std::string steps, std::string lives)
{
	std::vector<std::string> infoTexts = {"Level", "Steps", "Lives", "Press R to Reset", "Press ESC to Menu" };
	std::vector<std::string> dataTexts =  { level, steps, lives };
	DrawTextFromRect("MAZE SIMULATOR GAME", &GameTitleFillRect, GameTitle_FontSize);
	DrawTextMultipleRows(infoTexts, dataTexts, GameInfo_xOffset, GameInfo_yOffset, GameInfo_Width, GameInfo_Height, 40);
}


void DrawInGameGUI(int level, int steps, int lives)
{
	DrawInGameGUI(std::to_string(level), std::to_string(steps), std::to_string(lives));
}


void DrawInGameGUI()
{
	DrawInGameGUI("-", "-", "-");
}

bool FrontScreen()
{
	//Create Front End Screen
	FullScreenTexture(IMG_TITLE);
	
	SDL_Event * startEvent = new SDL_Event;
	while (startEvent->type != SDL_KEYDOWN && startEvent->type != SDL_QUIT)
	{
		
		SDL_WaitEvent(startEvent);
	}
	
	return startEvent->key.keysym.sym == SDLK_ESCAPE || startEvent->type == SDL_QUIT;
	
}

 
bool GameOverScreen()
{
	FullScreenTexture(IMG_GAMEOVER);
	SDL_Event * event = new SDL_Event;

	while (event->type != SDL_KEYDOWN && event->type != SDL_QUIT)
	{
		SDL_WaitEvent(event);
	}

	//Quits the Game
	return event->type == SDL_QUIT;
}


bool LevelCompleteScreen(int steps, int deaths)
{
	int LevelComplete_FontSize = 64;
	
	//Add Background Rect and Outline to Renderer
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &levelCompleteFillRect);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(renderer, &levelCompleteFillRect);

	
	DrawTextFromRect("Level Complete!", &levelCompleteTitleRect, LevelComplete_FontSize);

	//Information in Level Complete Panel
	std::vector<std::string> levelComplete_TextRows = { "Steps Taken", "Lives Lost", "Press Space To Continue" };
	std::vector<std::string> levelComplete_DataRows = { std::to_string(steps), std::to_string(deaths) };

	
	DrawTextMultipleRows(levelComplete_TextRows, levelComplete_DataRows, levelCompleteInfoRect.x, levelCompleteInfoRect.y, levelCompleteInfoRect.w, levelCompleteInfoRect.h, 50);
	SDL_RenderPresent(renderer);
	SDL_Event * continueEvent = new SDL_Event;
	
	
	while (continueEvent->key.keysym.sym != SDLK_SPACE && continueEvent->type != SDL_QUIT )
	{
		
		SDL_WaitEvent(continueEvent);
	}

	//Quits the Game
	return continueEvent->type == SDL_QUIT;
}

//Renders all necessary objects to the screen
void RenderAllGameObjects(std::shared_ptr<Maze> & maze, std::shared_ptr<Player> & player, int &steps)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	DrawInGameGUI(maze->mazeLevel, steps, player->playerLives);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	
	maze->AddMazeRoomsToRenderer(0);
	player->AddObjToRenderer();
	
	//Only start rendering obstacles after a particular level
	if (maze->mazeLevel > TRAPS_START)
		maze->AddMazeObstaclesToRenderer();
	
	//Check if the player has the key
	if (!player->hasKey)
		maze->mazeKeyPtr->AddObjToRenderer();
	
	maze->mazeDoorPtr->AddObjToRenderer();
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderPresent(renderer);
}

//Reset the maze to the Initial Conditions
void ResetToStart(std::shared_ptr<Maze> & maze, std::shared_ptr<Player> & player, int *level, int* steps, int* lives)
{
	*level = 1;
	*steps = 0;
	maze->SetMaze(3, 3, 1);
	player->ResetPlayer(maze->FindRoomByPos(maze->startPos), 5);
	*lives = player->playerLives;
	RenderAllGameObjects(maze, player, *steps);
}

int main(int argc, char *argv[])
{
	//Initialize SDL2
	if (!initialization())
	{
		printf("Failed to initialize!\n");
	}
	else 
	{
		//Sets the Level, step count, and starting lives
		int level = 1, steps = 0;
		int levelStartLives = 0;

		//Determines Starting Size of Maze 
		int mazeX = 3, mazeY = 3;

		bool firstLoad = true;
		bool quit = false;
		
		std::shared_ptr<Maze> curMaze;
		std::shared_ptr<Player> curPlayer;
		
		//Get the SDL Starting Event
		SDL_Event * gameEvent = new SDL_Event;

		//Game Running Loop
		while (!quit && gameEvent->type != SDL_QUIT && curGameState >= Menu)
		{
			//Front End Menu Loop
			while(!quit && gameEvent->type != SDL_QUIT && curGameState == Menu)
			{
				//Show the Front Screen
				quit = FrontScreen();

				//If Close Window or ESC key is pressed, quit the game
				if (quit)
				{
					break;
				}
				else 
				{
					curGameState = InGame;
					DrawInGameGUI();

					//First time the game is loaded
					if (firstLoad)
					{
						curMaze = std::shared_ptr<Maze>(new Maze(mazeX, mazeY, 
							GameArea_xOffset, GameArea_yOffset, GameArea_Width, 
							GameArea_Height, renderer, level, false));
						curPlayer = std::shared_ptr<Player>
							(new Player(curMaze->FindRoomByPos(curMaze->startPos), 
										renderer));
						levelStartLives = curPlayer->playerLives;
						firstLoad = false;
					}

					//Every subsequent time
					else
					{
						ResetToStart(curMaze, curPlayer, &level, &steps, 
							&levelStartLives);
					}
				}
				RenderAllGameObjects(curMaze, curPlayer, steps);
			}
			
			//Gameplay Loop
			while (!quit && gameEvent->type != SDL_QUIT && curGameState == InGame)
			{
				//Do something when there is an event
				SDL_WaitEvent(gameEvent);

				//If the Close Window button is pressed, quit the game
				if (gameEvent->type == SDL_KEYDOWN)
				{
					//Gets the Key Input
					auto keyInput = gameEvent->key.keysym.sym;

					//If "ESC" was pressed, leave the game loop
					if (keyInput == SDLK_ESCAPE)
					{
						curGameState = Menu;
						break;
					}

					//If "R" was pressed, reset the maze, reset the player position, and reduce the player life count by 1
					else if (keyInput == SDLK_r && curPlayer->playerLives > 1)
					{
						curPlayer->playerLives--;
						curMaze->ResetMaze();
						curPlayer->SetPlayerToStart(curMaze->FindRoomByPos(curMaze->startPos));
					}
				
					//If the player presses an arrow key, check for a movement
					else if(keyInput == SDLK_DOWN  ||
							keyInput == SDLK_LEFT ||
							keyInput == SDLK_RIGHT ||
							keyInput == SDLK_UP)
					{
						//Moves the Player According to the Key Input
						if (curPlayer->PlayerMove(keyInput, renderer))
						{
							steps++;
							//Update the Maze
							curMaze->NextMazeCycle();

							if(curPlayer->CheckForObjects())
							{
								//If the player had damage dealt, check if 
								//they still have enough lives
								if (curPlayer->playerLives == 0)
								{
									curGameState = GameOver;
								}
							}
							if ((curPlayer->objPos == curMaze->finalPos &&
								curPlayer->hasKey))
							{
								curGameState = LevelComplete;
							}
						}
					}
					else {
						std::cout << "Invalid Key input!" << std::endl;
					}
					RenderAllGameObjects(curMaze, curPlayer, steps);
				}
			}
			
			//Level Complete Loop
			while (!quit && gameEvent->type != SDL_QUIT && curGameState == LevelComplete)
			{
				//Display level complete stats
				quit = LevelCompleteScreen(steps, std::max(0, levelStartLives - curPlayer->playerLives));
				if (quit)
				{
					break;
				}
				curGameState = InGame;

				//Next Level Changes
				SDL_RenderClear(renderer);
				DrawInGameGUI();

				//Store how many lives the player started with
				levelStartLives = curPlayer->playerLives;
				//Increment level count
				level++;
				//Reset Step Count
				steps = 0;
				//Increment Maze Size
				mazeX += 1;
				mazeY += 1;

				curMaze->NextLevelMaze();

				curPlayer->SetPlayerToStart(curMaze->FindRoomByPos(curMaze->startPos));

				RenderAllGameObjects(curMaze, curPlayer, steps);
			}
			
			//Game Over Loop
			while (!quit && gameEvent->type != SDL_QUIT && curGameState == GameOver)
			{
				quit = GameOverScreen();
				if (quit)
				{
					break;
				}
				else
				{
					curGameState = Menu;
				}
			}
		}
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	return 0;
}
