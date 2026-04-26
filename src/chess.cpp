#include "chess.hpp"
#include "log.hpp"

Bitboard board_position_to_bitboard(BoardPosition pos)
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

bool parse_fen_string(ChessState* state, String fen)
{
    int cursor = 0;

    if (fen.size < 25)  // shortest possible fen string
        return false;

#define ADVANCE()   \
    do { cursor += 1; if (cursor >= fen.size) return false; } while(0)

    int row = 7;
    while (row >= 0)
    {
        int column = 0;
        while (column < 8)
        {
            if (is_digit(fen[cursor]))
            {
                int n = fen[cursor] - '0';
                if (column + n > 8)
                {
                    return false;
                }

                column += n;
            }
            else if (is_alpha_upper(fen[cursor]))
            {
                PieceType type;

                switch (fen[cursor])
                {
                case 'K':
                    type = PieceType::King;
                    break;
                case 'Q':
                    type = PieceType::Queen;
                    break;
                case 'R':
                    type = PieceType::Rook;
                    break;
                case 'B':
                    type = PieceType::Bishop;
                    break;
                case 'N':
                    type = PieceType::Knight;
                    break;
                case 'P':
                    type = PieceType::Pawn;
                    break;
                default:
                    return false;
                }

                state->put_piece(type, ChessColor::White, BoardPosition(row, column));
                column += 1;
            }
            else if (is_alpha_lower(fen[cursor]))
            {
                PieceType type;

                switch (fen[cursor])
                {
                case 'k':
                    type = PieceType::King;
                    break;
                case 'q':
                    type = PieceType::Queen;
                    break;
                case 'r':
                    type = PieceType::Rook;
                    break;
                case 'b':
                    type = PieceType::Bishop;
                    break;
                case 'n':
                    type = PieceType::Knight;
                    break;
                case 'p':
                    type = PieceType::Pawn;
                    break;
                default:
                    return false;
                }

                state->put_piece(type, ChessColor::Black, BoardPosition(row, column));
                column += 1;
            }
            else if (fen[cursor] == '/')
            {
                if (column != 0 && row != 7)
                {
                    return false;
                }
            }

            ADVANCE();
        }

        if (column != 8)
            return false;
        row -= 1;
    }

    if (fen[cursor] != ' ')
        return false;
    ADVANCE();

    ChessColor side_to_move = ChessColor::White;
    if (fen[cursor] == 'w')
        side_to_move = ChessColor::White;
    else if (fen[cursor] == 'b')
        side_to_move = ChessColor::Black;
    else
        return false;

    state->side_to_move = side_to_move;

    ADVANCE();

    if (fen[cursor] != ' ')
        return false;
    ADVANCE();

    bool wck = false;
    bool wcq = false;
    bool bck = false;
    bool bcq = false;
    {
        int save = cursor;
        if (fen[cursor] == 'K') { wck = true; ADVANCE(); }
        if (fen[cursor] == 'Q') { wcq = true; ADVANCE(); }
        if (fen[cursor] == 'k') { bck = true; ADVANCE(); }
        if (fen[cursor] == 'q') { bcq = true; ADVANCE(); }

        if (cursor == save)
        {
            if (fen[cursor] == '-')
            {
                ADVANCE();
            }
            else
                return false;
        }
    }
    state->wck = wck;
    state->wcq = wcq;
    state->bck = bck;
    state->bcq = bcq;

    if (fen[cursor] != ' ')
        return false;
    ADVANCE();

    if (fen[cursor] == '-')
    {
        ADVANCE();
    }
    else
    {
        // bounds check
        if (cursor + 2 >= fen.size)
            return false;

        SquareIndex square = parse_square(fen[cursor + 1], fen[cursor]);
        if (square == NullSquareIndex)
        {
            return false;
        }

        state->en_passant_square = square;
        cursor += 2;
    }

    if (fen[cursor] != ' ')
    {
        return false;
    }
    cursor += 1;

    String half_move_string = string_slice_to_character(fen, cursor, ' ');
    bool half_move_success = false;
    int half_move_counter = string_to_integer(half_move_string, &half_move_success);
    cursor += half_move_string.size;
    if (!half_move_success)
    {
        return false;
    }

    state->half_move = half_move_counter;

    if (fen[cursor] != ' ')
    {
        return false;
    }
    ADVANCE();

    String full_move_string = string_slice(fen, cursor, fen.size);
    bool full_move_success = false;
    int full_move_clock = string_to_integer(full_move_string, &full_move_success);
    cursor += full_move_string.size;
    if (!full_move_success)
    {
        return false;
    }

    state->move_clock = full_move_clock;

    return true;
}

bool ChessGame::make_move(SquareIndex from, SquareIndex to)
{
    Bitboard source = BIT(from);
    Bitboard destination = BIT(to);

    Bitboard friendly = 0;
    Bitboard opponent = 0;

    ASSERT(!(state.white & state.black));

    bool is_white_move = state.white & source;

    friendly = is_white_move ? state.white : state.black;
    opponent = is_white_move ? state.black : state.white;

    if (friendly & destination)
    {
        return false;
    }

    if (is_white_move)
    {
        if (state.side_to_move != ChessColor::White)
        {
            return false;
        }
        state.white &= ~source;
        state.white |= destination;

        if (from == state.white_king)
        {
            state.white_king = to;
        }
    }
    else
    {
        if (state.side_to_move != ChessColor::Black)
        {
            return false;
        }
        state.black &= ~source;
        state.black |= destination;

        if (from == state.black_king)
        {
            state.black_king = to;
        }
    }

    if (state.queen & source)
    {
        state.queen = bitboard_move(state.queen, source, destination);
    }
    else if (state.rook & source)
    {
        state.rook = bitboard_move(state.rook, source, destination);
    }
    else if (state.bishop & source)
    {
        state.bishop = bitboard_move(state.bishop, source, destination);
    }
    else if (state.knight & source)
    {
        state.knight = bitboard_move(state.knight, source, destination);
    }
    else if (state.pawn & source)
    {
        state.pawn = bitboard_move(state.pawn, source, destination);
    }
    else  // empty square
    {
        panic("Invalid board state");
    }

    ChessMove move = {};
    move.from = from;
    move.to = to;
    move.capture = opponent & destination;

    moves.add(move);

    state.side_to_move = is_white_move ? ChessColor::Black : ChessColor::White;

    return true;
}

bool ChessGame::undo_move()
{
    if (!moves.size()) return false;

    moves.pop();
    return true;
}

void ChessState::put_piece(PieceType type, ChessColor color, SquareIndex index)
{
    Bitboard square = BIT(index);

    if (color == ChessColor::White)
    {
        white |= square;
    }
    else if (color == ChessColor::Black)
    {
        black |= square;
    }

    switch (type)
    {
        case PieceType::King: {
            BoardPosition position = index_to_board_position(index);
            if ((color == ChessColor::White &&
                 white_king != NullSquareIndex &&
                 index_to_board_position(white_king) != position) ||
                (color == ChessColor::Black &&
                 black_king != NullSquareIndex &&
                 index_to_board_position(black_king) != position)
                 )
            {
                log_warning("Placing more than one king on the board");
            }

            if (color == ChessColor::White)
                white_king = index;
            else if (color == ChessColor::Black)
                black_king = index;
            break;
        }
        case PieceType::Queen: {
            queen |= square;
            break;
        }
        case PieceType::Rook: {
            rook |= square;
            break;
        }
        case PieceType::Bishop: {
            bishop |= square;
            break;
        }
        case PieceType::Knight: {
            knight |= square;
            break;
        }
        case PieceType::Pawn: {
            pawn |= square;
            break;
        }
    }
}

void ChessState::put_piece(PieceType type, ChessColor color, BoardPosition position)
{
    put_piece(type, color, board_position_to_bitboard(position));
}

void ChessState::clear_square(SquareIndex index)
{
    if (white_king == index)
        white_king = NullSquareIndex;
    if (black_king == index)
        black_king = NullSquareIndex;
    Bitboard c = ~BIT(index);
    white &= c;
    black &= c;
    queen &= c;
    rook &= c;
    bishop &= c;
    knight &= c;
    pawn &= c;
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

Bitboard bitboard_move(Bitboard b, Bitboard source, Bitboard destination)
{
    b &= ~source;
    b |= destination;
    return b;
}

bool operator==(BoardPosition pos0, BoardPosition pos1) {
    return pos0.row == pos1.row && pos1.column == pos1.column;
}

bool operator!=(BoardPosition pos0, BoardPosition pos1) {
    return pos0.row != pos1.row || pos1.column != pos1.column;
}

void print_board_state(ChessState state)
{
    printf("     a  b  c  d  e  f  g  h \n");

    for (int row = 7; row >= 0; row--)
    {
        printf("  %d ", row + 1);
        for (int column = 0; column < 8; column++)
        {
            SquareIndex index = row * 8 + column;
            Bitboard square = BIT(index);

            char character = '!';
            if (state.white_king == index || state.black_king == index) {
                character = 'K';
            }
            else if (state.queen & square) {
                character = 'Q';
            }
            else if (state.rook & square) {
                character = 'R';
            }
            else if (state.bishop & square) {
                character = 'B';
            }
            else if (state.knight & square) {
                character = 'N';
            }
            else if (state.pawn & square) {
                character = 'P';
            }
            else {
                character = '#';
            }

            if (state.white & square) { character = to_upper_ascii(character); }
            else if (state.black & square) { character = to_lower_ascii(character); }

            printf(" %c ", character);
        }
        printf("\n");
    }

    printf("\n");
    printf(" WKS: %d, WQS: %d, BKS: %d, BQS: %d \n", state.wck, state.wcq, state.bck, state.bcq);
    printf(" En passant: %s\n", square_identifier_string(state.en_passant_square));
}

SquareIndex parse_square(char rank, char file)
{
    int row = 0;
    int column = 0;
    switch (rank) {
        case '1': row = 0; break;
        case '2': row = 1; break;
        case '3': row = 2; break;
        case '4': row = 3; break;
        case '5': row = 4; break;
        case '6': row = 5; break;
        case '7': row = 6; break;
        case '8': row = 7; break;
        default: return NullSquareIndex;
    }
    switch (to_lower_ascii(file)) {
        case 'a': column = 0; break;
        case 'b': column = 1; break;
        case 'c': column = 2; break;
        case 'd': column = 3; break;
        case 'e': column = 4; break;
        case 'f': column = 5; break;
        case 'g': column = 6; break;
        case 'h': column = 7; break;
        default: return NullSquareIndex;
    }

    return row * 8 + column;
}

const char* square_identifier_string(SquareIndex index)
{
    if (index == NullSquareIndex) return "-";
    if (index < 0 || index >= 64) return "!!";  // respect two characters

    const char* square_names[] = {
        "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8",
        "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8",
        "c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8",
        "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8",
        "e1", "e2", "e3", "e4", "e5", "e6", "e7", "e8",
        "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8",
        "g1", "g2", "g3", "g4", "g5", "g6", "g7", "g8",
        "h1", "h2", "h3", "h4", "h5", "h6", "h7", "h8",
    };

    return square_names[index];
}
