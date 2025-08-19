#include "chess.h"
#include "fen.h"

#define BITBOARD(x) ((Bitboard)1 << (x))

Chess_Game_List make_chess_game_list()
{
  int cap = 10;
  Chess_Game* data = (Chess_Game*)malloc(sizeof(Chess_Game)*cap);
  return (Chess_Game_List){.data=data,.size=0,.cap=cap};
}

void chess_game_list_add(Chess_Game_List* list, Chess_Game* game)
{
  if (list->size+1>=list->cap)
  {
    int ncap = list->cap*2;
    Chess_Game* ndata = (Chess_Game*)malloc(sizeof(Chess_Game)*ncap);
    memcpy(ndata,list->data,sizeof(Chess_Game)*list->size);
    list->cap = ncap;
  }

  list->data[list->size] = *game;
  list->size += 1;
}

ivec2 get_board_coord(u8 index)
{
  return (ivec2){index%8,index/8};
}

int get_board_index(ivec2 c)
{
  return c.y*8+c.x;
}

Bitboard get_next_piece(Bitboard* board)
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
        Bitboard square = get_next_piece(&pieces);
        Board_Location location = get_board_location(square);
        Bitboard moves = 0;
  
  
        SCAN_DIRECTION(location.file+1, +1, it<8,  (location.rank*8+it), moves, friendly, opponent)
        SCAN_DIRECTION(location.file-1, -1, it>=0, (location.rank*8+it), moves, friendly, opponent)
        SCAN_DIRECTION(location.rank+1, +1, it<8,  (it*8+location.file), moves, friendly, opponent)
        SCAN_DIRECTION(location.rank-1, -1, it>=0, (it*8+location.file), moves, friendly, opponent)
        
        pos_moves->moves[location.index] |= moves;
    }
}

void generate_diagonal_moves(Chess_Moves* pos_moves, Bitboard pieces, Bitboard friendly, Bitboard opponent)
{
    while (pieces)
    {
        Bitboard piece = get_next_piece(&pieces);
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
  (void)opponent;
  const ivec2 offsets[] = {
    {-1,-2},{1,-2},{-2,-1},{2,-1},{-2,1},{2,1},{-1,2},{1,2}
  };

  while (pieces)
  {
    Bitboard moves = 0;
    Bitboard p = pieces & ~(pieces-1);
    pieces &= pieces-1;

    Board_Location board_location = get_board_location(p);
    for (size_t i = 0; i < ARRAY_SIZE(offsets); i++)
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
  (void)opponent;
  if (hamming(king) != 1)
  {
    LOG_ERROR("Multiple kings of the same side on the board");
  }

  Board_Location board_location = get_board_location(king);

  Bitboard moves = 0;
  const ivec2 offsets[] = {{-1,-1},{0,-1},{1,-1},{-1,0},{1,0},{-1,1},{0,1},{1,1}};
  for (size_t i = 0; i < ARRAY_SIZE(offsets); i++)
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


Chess_Position make_chess_position(Chess_Board* board)
{
  Chess_Position position = (Chess_Position){.board=*board};
  generate_moves(&position);
  return position;
}

Chess_Context create_chess_context_pgn(String pgn, bool* success)
{
  panic("Not implemented");
}

Chess_Context create_chess_context_fen(String fen, bool* success)
{
  Chess_Context context = {};
  Chess_Board board = parse_fen(fen, success);
  if (!(*success)) return context;
  context = (Chess_Context){
    .game_list = make_chess_game_list(),
    .selected = 0,
  };

  Chess_Game game = {};
  game.position = make_chess_position(&board);
  chess_game_list_add(&context.game_list, &game);

  return context;
}

Chess_Context create_chess_context()
{
  Chess_Context context;

  context.game_list = make_chess_game_list();
  context.selected = 0;

  return context;
}



void print_board(Chess_Board* board)
{
    char buffer[73];
    const char piece_chars[] = {
        'K','Q','R','B','N','P',
        'k','q','r','b','n','p',
    };

    for (int rank = 0; rank < 8; rank++)
    {
        for (int col = 0; col < 8; col++)
        {
            int index = rank*8+col;
            Bitboard p = BITBOARD(index);

            for (int i = 0; i < CHESS_PIECE_TYPE_COUNT; i++)
            {
                if (p & board->pieces[i])
                {
                    int color = (p & board->white) ? (0) : (1);
                    buffer[rank*9+col] = piece_chars[color*CHESS_PIECE_TYPE_COUNT+i];
                    break;
                }
            }
        }

        buffer[(rank+1)*8] = '\n';
    }

    buffer[sizeof(buffer)-1] = '\0';
    printf("%s",buffer);
}
