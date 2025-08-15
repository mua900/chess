#include "chess.h"

#define BITBOARD(x) ((Bitboard)1 << x)

ivec2 get_board_coord(u8 index)
{
  return (ivec2){index%8,index/8};
}

int get_board_index(ivec2 c)
{
  return c.y*8+c.x;
}

Bitboard get_lowest_piece(Bitboard* board)
{
  Bitboard b = *board-1;
  Bitboard p = *board & ~(b);
  *board &= b;
  return p;
}

Board_Location get_board_location(Bitboard x)
{
  // assert(hamming(x) == 1);
  int index = pop_lsb(&x);
  int file = index % 8;
  int rank = index / 8;
  return (Board_Location){index,file,rank};
}

Chess_Game_List make_chess_game_list()
{
  struct Chess_Game_List_Node* head = malloc(sizeof(struct Chess_Game_List_Node));
  head->game = (Chess_Game){};
  head->next = NULL;
  return (Chess_Game_List){.head=head};
}

void chess_game_list_add(Chess_Game_List* list, Chess_Game* game)
{
  struct Chess_Game_List_Node** node_ptr = &list->head;
  while (*node_ptr)
    {
      node_ptr = &(*node_ptr)->next;
    }

  (*node_ptr)->next = malloc(sizeof(struct Chess_Game_List_Node));
  (*node_ptr)->next->game = *game;
  (*node_ptr)->next->next = NULL;
}

#define SCAN_DIRECTION(p_start,p_step,p_cond,p_index,p_moves,p_friendly,p_opponent)     \
  for (int it = p_start; p_cond; it+=p_step)  {                                         \
    Bitboard move = BITBOARD(p_index);                                                  \
    if (move & friendly) break;                                                         \
    moves |= move;                                                                      \
    if (move & opponent) break;                                                         \
  }

void generate_orthogonal_moves(Chess_Moves* pos_moves, Bitboard pieces, Bitboard friendly, Bitboard opponent)
{
    while (pieces)
    {
        Bitboard square = get_lowest_piece(&pieces);
        Board_Location location = get_board_location(square);
        Bitboard moves = 0;
  
  
        SCAN_DIRECTION(location.file+1, +1, it<8,  location.rank*8+it, moves, friendly, opponent)
        SCAN_DIRECTION(location.file-1, -1, it>=0, location.rank*8+it, moves, friendly, opponent)
        SCAN_DIRECTION(location.rank+1, +1, it<8,  it*8+location.file, moves, friendly, opponent)
        SCAN_DIRECTION(location.rank-1, -1, it>=0, it*8+location.file, moves, friendly, opponent)
        
        pos_moves->moves[location.index] |= moves;
    }
}

void generate_diagonal_moves(Chess_Moves* pos_moves, Bitboard pieces, Bitboard friendly, Bitboard opponent)
{
    while (pieces)
    {
        Bitboard piece = get_lowest_piece(&pieces);
        Board_Location location = get_board_location(piece);
        Bitboard moves = 0;


        int r,f;
        for (r = location.rank+1, f = location.file+1; r < 8 && f < 8; r++, f++)
        {
          Bitboard move = BITBOARD(r*8+f);
          if (move & friendly) break;
          moves |= move;
          if (move & opponent) break;
        }
        for (r = location.rank+1, f = location.file-1; r < 8 && f >= 0; r++, f--)
        {
          Bitboard move = BITBOARD(r*8+f);
          if (move & friendly) break;
          moves |= move;
          if (move & opponent) break;
        }
        for (r = location.rank-1, f = location.file+1; r >= 0 && f < 8; r--, f++)
        {
          Bitboard move = BITBOARD(r*8+f);
          if (move & friendly) break;
          moves |= move;
          if (move & opponent) break;
        }
        for (r = location.rank-1, f = location.file-1; r >= 0 && f >= 0; r--, f--)
        {
          Bitboard move = BITBOARD(r*8+f);
          if (move & friendly) break;
          moves |= move;
          if (move & opponent) break;
        }

        pos_moves->moves[location.index] |= moves;
    }
}

void generate_pawn_moves(Chess_Moves* pos_moves, Bitboard pieces, Bitboard friendly, Bitboard opponent, bool white)
{
  int direction = white ? 1 : -1;
  Bitboard occupancy = friendly | opponent;
  while (pieces)
  {
    Bitboard p = pieces & ~(pieces-1);
    pieces &= pieces-1;
    Board_Location location = get_board_location(p);

    Bitboard step = BITBOARD((location.rank+direction)*8+location.file);
    if (location.rank+direction >= 8 || location.rank+direction < 0)
    {
      continue;
    }
    if (step & occupancy)
    {
      continue;
    }

    pos_moves->moves[location.index] |= step;

    if (white ? (location.rank == 1) : (location.rank == 6))
    {
      Bitboard step_two = BITBOARD((location.rank+direction*2)*8+location.file);
      if (location.rank+direction*2 >= 8 || location.rank+direction*2 < 0)
      {
        continue;
      }
      if (step_two & occupancy)
      {
        continue;
      }

      pos_moves->moves[location.index] |= step_two;
    }

    
  }
}

void generate_knight_moves(Chess_Moves* pos_moves, Bitboard pieces, Bitboard friendly, Bitboard opponent)
{
  const ivec2 offsets[] = {
    {-1,-2},{1,-2},{-2,-1},{2,-1},{-2,1},{2,1},{-1,2},{1,2}
  };

  while (pieces)
  {
    Bitboard moves = 0;
    Bitboard p = pieces & ~(pieces-1);
    pieces &= pieces-1;

    Board_Location board_location = get_board_location(p);
    for (int i = 0; i < ARRAY_SIZE(offsets); i++)
    {
      Bitboard square = BITBOARD(board_location.index + offsets[i].x+offsets[i].y*8);

      if (board_location.file + offsets[i].x < 0 || board_location.file + offsets[i].x >= 8 || board_location.rank + offsets[i].y < 0 || board_location.rank + offsets[i].y >= 8)
      {
        continue;
      }

      if (friendly & square)
      {
        continue;
      }

      moves |= square;
    }

    pos_moves->moves[board_location.index] = moves;
  }
}

void generate_king_moves(Chess_Moves* pos_moves, Bitboard king, Bitboard friendly, Bitboard opponent)
{
  if (hamming(king) != 1)
  {
    LOG_ERROR("Multiple kings of the same side on the board");
  }

  Board_Location board_location = get_board_location(king);

  Bitboard moves = 0;
  const ivec2 offsets[] = {{-1,-1},{0,-1},{1,-1},{-1,0},{1,0},{-1,1},{0,1},{1,1}};
  for (int i = 0; i < ARRAY_SIZE(offsets); i++)
  {
    Bitboard square = BITBOARD(board_location.index + offsets[i].x+offsets[i].y*8);

    if (board_location.file + offsets[i].x < 0 || board_location.file + offsets[i].x >= 8 || board_location.rank + offsets[i].y < 0 || board_location.rank + offsets[i].y >= 8)
    {
      continue;
    }

    if (friendly & square)
    {
      continue;
    }

    // if (opponent & square) 

    moves |= square;
  }

  pos_moves->moves[board_location.index] = moves;
}

void generate_moves(Chess_Position* position)
{
  memset(&position->moves, 0, sizeof(Chess_Moves));

  generate_orthogonal_moves(&position->moves, (position->board.pieces[CHESS_PIECE_TYPE_ROOK] | position->board.pieces[CHESS_PIECE_TYPE_QUEEN]) & position->board.white, position->board.white, position->board.black);
  generate_diagonal_moves(&position->moves, (position->board.pieces[CHESS_PIECE_TYPE_BISHOP] | position->board.pieces[CHESS_PIECE_TYPE_QUEEN]) & position->board.white, position->board.white, position->board.black);
  generate_pawn_moves(&position->moves, (position->board.pieces[CHESS_PIECE_TYPE_PAWN]) & position->board.white, position->board.white, position->board.black,true);
  generate_knight_moves(&position->moves, (position->board.pieces[CHESS_PIECE_TYPE_KNIGHT]) & position->board.white, position->board.white, position->board.black);
  generate_king_moves(&position->moves, (position->board.pieces[CHESS_PIECE_TYPE_KING]) & position->board.white, position->board.white, position->board.black);

  generate_orthogonal_moves(&position->moves, (position->board.pieces[CHESS_PIECE_TYPE_ROOK] | position->board.pieces[CHESS_PIECE_TYPE_QUEEN]) & position->board.black, position->board.black, position->board.white);
  generate_diagonal_moves(&position->moves, (position->board.pieces[CHESS_PIECE_TYPE_BISHOP] | position->board.pieces[CHESS_PIECE_TYPE_QUEEN]) & position->board.black, position->board.black, position->board.white);
  generate_pawn_moves(&position->moves, (position->board.pieces[CHESS_PIECE_TYPE_PAWN]) & position->board.black, position->board.black, position->board.white,true);
  generate_knight_moves(&position->moves, (position->board.pieces[CHESS_PIECE_TYPE_KNIGHT]) & position->board.black, position->board.black, position->board.white);
  generate_king_moves(&position->moves, (position->board.pieces[CHESS_PIECE_TYPE_KING]) & position->board.black, position->board.black, position->board.white);
}
