#include "chess.hpp"

Bitboard to_bitboard(BoardPosition pos)
{
    return pos.column + pos.row * 8;
}

BoardPosition index_to_board_position(SquareIndex index)
{
    return BoardPosition(index / 8, index % 8);
}

SquareIndex board_position_to_index(BoardPosition pos)
{
    return pos.row * 8 + pos.column;
}

ChessState parse_fen_string(String fen)
{
    ChessState state;
    // @todo
    return state;
}

bool ChessGame::make_move(SquareIndex from, SquareIndex to)
{
    return false;
}

bool ChessGame::undo_move()
{
    if (!moves.size()) return false;

    moves.pop();
    return true;
}