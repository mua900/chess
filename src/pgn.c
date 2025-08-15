#include "pgn.h"
bool parse_pgn_game(String s, int* cursor, Chess_Game_List* games)
{  
  return true;
}

bool parse_pgn(String s, Chess_Game_List* games)
{
  int cursor = 0;
  if (!parse_pgn_game(s, &cursor, games))
    {
      return false;
    }
  

  
  return true;
}
