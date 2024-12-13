#ifndef GAME_H
#define GAME_H
#include "struct.h"
#include "Tools.h"


Blocgame createGameBloc(Game g);
void read_bloc_game(TOF f, Blocgame *bg, int bIndx);
void write_bloc_game(TOF *f, Blocgame bg, int bIndx);

// Core operations
Game createGame(int gID, char title[], float rentalPrice, TOF *f);
Game search_game(TOF tof, int gID, int index);

// List operations
void loadGameFile(TOF *f, GameNode **strt);
void saveGameFile(TOF f, GameNode *strt);

// Display functions
void displayGame(Game game);
void display_all_games(TOF tof);

#endif // GAME_H
