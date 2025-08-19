#ifndef _FEN_H
#define _FEN_H

#include "common.h"
#include "chess.h"

Chess_Board parse_fen(String fen, bool* success);

#endif // _FEN_H
