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

ChessPosition calculate_position(ChessState state)
{
    ChessPosition position;
    return position;
}

Bitboard calculate_direction_moves(Bitboard pieces, Bitboard blockers, Bitboard captures, Array<int> offsets, int depth)
{
    Bitboard moves = 0;

    while (pieces)
    {
        SquareIndex index = pop_lsb(&pieces);

        Bitboard square = BIT(index);
        BoardPosition position = index_to_board_position(index);

        for (int i = 0; i < offsets.size; i++)
        {
            int step = offsets[i];
            int square_index = index + step;
            int row    = square_index / 8;
            int column = square_index % 8;

            int iter = 0;

            while ((row < 8 && row >= 0 && column < 8 && column >= 0) && iter < depth)
            {
                Bitboard m = BIT(square_index);
                if (m & blockers)
                {
                    break;
                }
                if (m & captures)
                {
                    moves |= m;
                    break;
                }

                moves |= m;

                square_index += step;
                row = square_index / 8;
                column = square_index % 8;

                iter += 1;
            }
        }
    }

    return moves;
}

Bitboard calculate_orthogonal_moves(Bitboard pieces, Bitboard blockers, Bitboard captures)
{
    int offsets[4] = { -8, -1, 1, 8 };
    return calculate_direction_moves(pieces, blockers, captures, make_array(offsets), 8);
}

Bitboard calculate_diagonal_moves(Bitboard pieces, Bitboard blockers, Bitboard captures)
{
    int offsets[4] = { -9, -7, 7, 9 };
    return calculate_direction_moves(pieces, blockers, captures, make_array(offsets), 8);
}

Bitboard calculate_knight_moves(Bitboard pieces, Bitboard blockers, Bitboard captures)
{
    int offsets[8] = { -17, -15, -10, -6, 6, 10, 15, 17 };
    return calculate_direction_moves(pieces, blockers, captures, make_array(offsets), 1);
}
