#ifndef _PGN_H
#define _PGN_H

#include "common.h"
#include "chess.h"

bool parse_pgn_game(String s, int* cursor, Chess_Game_List* games);
bool parse_pgn(String s, Chess_Game_List* games);

#endif // _PGN_H
