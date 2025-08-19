#ifndef _CHESS_H
#define _CHESS_H

#include "common.h"

typedef uint64_t Bitboard;

#define BITBOARD(x) ((Bitboard)1 << (x))

#define MOVE_PIECE_MASK     0b111
#define MOVE_FLAG_CAPTURE   BIT(3)
#define MOVE_FLAG_PROMOTION BIT(4)

typedef struct {
  u8 from;
  u8 to;
  // 0..2 bits -> associated piece if this is a capture or a promotion
  // 3 -> capture flag
  // 4 -> promotion flag
  u8 info;
} Chess_Move;

typedef struct {
  Chess_Move* moves;
  int length;
  int cap;
} Chess_History;

typedef enum {
  CHESS_PIECE_TYPE_KING,
  CHESS_PIECE_TYPE_QUEEN,
  CHESS_PIECE_TYPE_ROOK,
  CHESS_PIECE_TYPE_BISHOP,
  CHESS_PIECE_TYPE_KNIGHT,
  CHESS_PIECE_TYPE_PAWN,
  CHESS_PIECE_TYPE_COUNT
} Chess_Piece_Type;

typedef enum {
    CHESS_WHITE = 0,
    CHESS_BLACK = 1,
} Chess_Color;

#define FEN_START "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define CHESS_SIDE_TO_MODE          BIT(0)
#define CHESS_EN_PASSANT           BIT(1)
#define CHESS_SQUARE(sq)            (sq & 0b111111) // 6 bits to represent a square
#define CHESS_EN_PASSANT_SQUARE(sq) (CHESS_SQUARE(sq) << 2)

#define CHESS_CASTLE_WKS BIT(8)
#define CHESS_CASTLE_WQS BIT(9)
#define CHESS_CASTLE_BKS BIT(10)
#define CHESS_CASTLE_BQS BIT(11)

typedef struct {
  Bitboard white;
  Bitboard black;
  Bitboard pieces[CHESS_PIECE_TYPE_COUNT];
  u16 info;
  int half_move_clock;
  int move;
} Chess_Board;

typedef struct {
  Bitboard moves[64];
} Chess_Moves;

typedef struct {
  Chess_Board board;
  Chess_Moves moves;
} Chess_Position;

typedef struct {
  Chess_Position position;
  Chess_History history;
} Chess_Game;
 

typedef struct {
  Chess_Game* data;
  int size;
  int cap;
} Chess_Game_List;

typedef struct {
  Chess_Game_List game_list;
  int selected;
} Chess_Context;

typedef struct {
  u8 index;
  u8 file;
  u8 rank;
} Board_Location;

Chess_Context create_chess_context_pgn(String pgn, bool* success);
Chess_Context create_chess_context_fen(String fen, bool* success);
Chess_Context create_chess_context();

void chess_context_add();

Bitboard get_next_piece(Bitboard* board);

void chess_game_list_add(Chess_Game_List* list, Chess_Game* game);
void eliminate_moves(Chess_Position* position);
void generate_moves(Chess_Position* position);

Board_Location get_board_location(Bitboard x);

void print_board(Chess_Board* board);

#endif
