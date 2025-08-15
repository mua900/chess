#ifndef _CHESS_H
#define _CHESS_H

#include "common.h"

typedef uint64_t Bitboard;

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

typedef struct {
  Bitboard white;
  Bitboard black;
  Bitboard pieces[CHESS_PIECE_TYPE_COUNT];
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


struct Chess_Game_List_Node {
  Chess_Game game;
  struct Chess_Game_List_Node* next;
};

typedef struct {
  struct Chess_Game_List_Node* head;
} Chess_Game_List;

typedef struct {
  u8 index;
  u8 file;
  u8 rank;
} Board_Location;

Chess_Game_List make_chess_game_list();
void chess_game_list_add(Chess_Game_List* list, Chess_Game* game);
void eliminate_moves(Chess_Position* position);
void generate_moves(Chess_Position* position);

Board_Location get_board_location(Bitboard x);

#endif
