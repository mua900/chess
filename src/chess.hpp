#ifndef _CHESS_H
#define _CHESS_H

#include "common.hpp"
#include "template.hpp"

#define PIECE_TYPE_PER_SIDE 6

enum PieceType : u8 {
    King   = 0,
    Queen  = 1,
    Rook   = 2,
    Bishop = 3,
    Knight = 4,
    Pawn   = 5,
    Count  = 6,
    Sentinel = 7,
};

enum class ChessColor {
    White,
    Black,
};

#define PIECE_COLOR_BIT BIT(3)
#define PIECE_TYPE_MASK 0b111

// @todo maybe use this
// 0 0 0 0 c p p p -> lowest three bits are the piece type as a number and the 3rd bit is the color
using Piece = uint8_t;

using Bitboard = uint64_t;

Bitboard bitboard_move(Bitboard b, Bitboard source, Bitboard destination);

struct BoardPosition {
    uint8_t row = 0;
    uint8_t column = 0;

    BoardPosition() {}
    BoardPosition(uint8_t r, uint8_t c) : row(r), column(c) {}
};

bool operator==(BoardPosition pos0, BoardPosition pos1);
bool operator!=(BoardPosition pos0, BoardPosition pos1);

using SquareIndex = uint8_t;
#define NullSquareIndex SquareIndex(0xff)
const char* square_identifier_string(SquareIndex index);

Bitboard board_position_to_bitboard(BoardPosition pos);
BoardPosition index_to_board_position(SquareIndex index);
SquareIndex board_position_to_index(BoardPosition pos);

struct ChessState {
    PieceType squares[64] = {};

    Bitboard white = {};
    Bitboard black = {};
    Bitboard pieces[PieceType::Count] = {};

    bool wck = false;
    bool wcq = false;
    bool bck = false;
    bool bcq = false;

    SquareIndex en_passant_square = NullSquareIndex;
    u32 half_move = 0;
    u32 move_clock = 0;
    ChessColor side_to_move = ChessColor::White;

    void put_piece(PieceType type, ChessColor color, SquareIndex index);
    void put_piece(PieceType type, ChessColor color, BoardPosition position);

    void clear_square(SquareIndex index);
};

void print_board_state(ChessState state);

struct ChessPosition {
    ChessState board = {};
    Bitboard white_moves = 0;
    Bitboard black_moves = 0;
    Bitboard moves[64];
    Bitboard attacks[64];
};

struct ChessMove {
    SquareIndex from;
    SquareIndex to;
    bool capture;
    bool check;
};

struct ChessGame {
    ChessPosition position = {};
    DArray<ChessMove> moves = {};

    bool make_move(SquareIndex from, SquareIndex to);
    bool undo_move();

    bool set_position(ChessState state);
    void calculate_moves();
    void calculate_king_moves();
};

bool parse_fen_string(ChessState* state, String fen);
ChessPosition calculate_position(ChessState state);

Bitboard calculate_orthogonal_moves(Bitboard pieces, Bitboard blockers, Bitboard captures);
Bitboard calculate_diagonal_moves(Bitboard pieces, Bitboard blockers, Bitboard captures);
Bitboard calculate_knight_moves(Bitboard pieces, Bitboard blockers, Bitboard captures);
Bitboard calculate_pawn_moves(Bitboard pieces, Bitboard blockers, Bitboard captures);

SquareIndex parse_square(char rank, char file);

#endif // _CHESS_H