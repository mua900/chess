#ifndef _CHESS_H
#define _CHESS_H

#include "common.hpp"
#include "template.hpp"

#define PIECE_TYPE_PER_SIDE 6

enum PieceType {
    King   = 0,
    Queen  = 1,
    Rook   = 2,
    Bishop = 3,
    Knight = 4,
    Pawn   = 5,
};

enum class ChessColor {
    White,
    Black,
};

#define PIECE_COLOR_BIT BIT(3)
#define PIECE_TYPE_MASK 0b111

// 0 0 0 0 c p p p -> lowest three bits are the piece type as a number and the 3rd bit is the color
using Piece = uint8_t;

using Bitboard = uint64_t;

struct BoardPosition {
    uint8_t row = 0;
    uint8_t column = 0;

    BoardPosition() {}
    BoardPosition(uint8_t r, uint8_t c) : row(r), column(c) {}
};

using SquareIndex = uint8_t;
#define NullSquareIndex SquareIndex(0xff);

Bitboard board_position_to_bitboard(BoardPosition pos);
BoardPosition index_to_board_position(SquareIndex index);
SquareIndex board_position_to_index(BoardPosition pos);

struct ChessState {
    Bitboard white = {};
    Bitboard black = {};
    BoardPosition white_king = {};
    BoardPosition black_king = {};
    Bitboard queen = 0;
    Bitboard rook = 0;
    Bitboard bishop = 0;
    Bitboard pawn = 0;

    bool wcl;
    bool wcr;
    bool bcl;
    bool bcr;

    SquareIndex en_passant_square;

    void put_piece(PieceType type, ChessColor color, SquareIndex index);
    void put_piece(PieceType type, ChessColor color, BoardPosition position);
};

struct ChessPosition {
    ChessState board = {};
    Bitboard white_moves = {};
    Bitboard black_moves = {};
    Bitboard attack_squares_white = {};
    Bitboard attack_squares_black = {};
};

struct ChessMove {
    SquareIndex from;
    SquareIndex to;
    bool capture;
    bool check;
};

struct ChessGame {
    ChessState start;
    DArray<ChessMove> moves;

    bool make_move(SquareIndex from, SquareIndex to);
    bool undo_move();
};

ChessState parse_fen_string(String fen);
ChessPosition calculate_position(ChessState state);

Bitboard calculate_orthogonal_moves(Bitboard pieces, Bitboard blockers, Bitboard captures);
Bitboard calculate_diagonal_moves(Bitboard pieces, Bitboard blockers, Bitboard captures);
Bitboard calculate_knight_moves(Bitboard pieces, Bitboard blockers, Bitboard captures);

SquareIndex parse_square(char rank, char file);

#endif // _CHESS_H