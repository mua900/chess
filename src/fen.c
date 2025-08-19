#include "fen.h"

int parse_number(String s, int* cursor, bool* success)
{
    int accum = 0;
    while (is_digit(s.data[*cursor]))
    {
        int d = s.data[*cursor] - '0';
        accum *= 10;
        accum += d;

        *cursor += 1;
    }

    return accum;
}

Board_Location parse_chess_square(String s, int* cursor, bool* success)
{
    Board_Location location = {};
    if (s.size + 2 <= *cursor){
        *success = false;
        return location;
    }

    if (!(*cursor >= 'a' && *cursor <= 'h'))
    {
        *success = false;
        return location;
    }
    location.file = s.data[*cursor] - 'a';
    if (!(*cursor >= '1' && *cursor <= '8'))
    {
        *success = false;
        return location;
    }
    location.file = s.data[*cursor] - '1';

    location.index = location.file+location.rank*8;
    *cursor += 2;

    return location;
}

Chess_Board parse_fen(String fen, bool* success)
{
    Chess_Board board = {};

#define PARSE_FAIL          \
        LOG_ERRORF("Invalid fen string %d", cursor);    \
        *success = false;   \
        return board;       \

#define CONSUME(p_character)        \
        if (!(p_character == fen.data[cursor])){  \
            PARSE_FAIL              \
        }                           \
        cursor += 1;                \


    int cursor = 0;

    const char white_chars[] = {
        'K','Q','R','B','N','P',
    };
    const char black_chars[] = {
        'k','q','r','b','n','p',
    };

    int file = 0;
    int rank = 7;

    while (rank >= 0 && cursor < fen.size)
    {
        char ch = fen.data[cursor];
        switch (ch)
        {
            case 'K': {
                Bitboard p = BITBOARD(file+rank*8);
                board.white |= p; board.pieces[CHESS_PIECE_TYPE_KING] |= p;
                break;
            }
            case 'Q': {
                Bitboard p = BITBOARD(file+rank*8);
                board.white |= p; board.pieces[CHESS_PIECE_TYPE_QUEEN] |= p;
                break;
            }
            case 'R': {
                Bitboard p = BITBOARD(file+rank*8);
                board.white |= p; board.pieces[CHESS_PIECE_TYPE_ROOK] |= p;
                break;
            }
            case 'B': {
                Bitboard p = BITBOARD(file+rank*8);
                board.white |= p; board.pieces[CHESS_PIECE_TYPE_BISHOP] |= p;
                break;
            }
            case 'N': {
                Bitboard p = BITBOARD(file+rank*8);
                board.white |= p; board.pieces[CHESS_PIECE_TYPE_KNIGHT] |= p;
                break;
            }
            case 'P': {
                Bitboard p = BITBOARD(file+rank*8);
                board.white |= p; board.pieces[CHESS_PIECE_TYPE_PAWN] |= p;
                break;
            }
            case 'k': {
                Bitboard p = BITBOARD(file+rank*8);
                board.white |= p; board.pieces[CHESS_PIECE_TYPE_KING] |= p;
                break;
            }
            case 'q': {
                Bitboard p = BITBOARD(file+rank*8);
                board.white |= p; board.pieces[CHESS_PIECE_TYPE_QUEEN] |= p;
                break;
            }
            case 'r': {
                Bitboard p = BITBOARD(file+rank*8);
                board.white |= p; board.pieces[CHESS_PIECE_TYPE_ROOK] |= p;
                break;
            }
            case 'b': {
                Bitboard p = BITBOARD(file+rank*8);
                board.white |= p; board.pieces[CHESS_PIECE_TYPE_BISHOP] |= p;
                break;
            }
            case 'n': {
                Bitboard p = BITBOARD(file+rank*8);
                board.white |= p; board.pieces[CHESS_PIECE_TYPE_KNIGHT] |= p;
                break;
            }
            case 'p': {
                Bitboard p = BITBOARD(file+rank*8);
                board.white |= p; board.pieces[CHESS_PIECE_TYPE_PAWN] |= p;
                break;
            }
            default: {
                if ('1' <= ch && '8' >= ch)
                {
                    file += ch - '0';
                }

                if (ch == '/')
                {
                    if (file != 8)
                    {
                        PARSE_FAIL
                    }

                    file = 0;
                    rank -= 1;
                }
            }

            cursor += 1;
        }
    }

    if (!(rank >= 0))
    {
        *success = false;
        return board;
    }

    CONSUME(' ')
    if (fen.data[cursor] == 'w') {
        board.info |= CHESS_WHITE;
    }
    else if (fen.data[cursor] == 'b') {
        board.info |= CHESS_BLACK;
    }
    else {
        PARSE_FAIL
    }

    CONSUME(' ')

    if (fen.data[cursor] == '-') {
        board.info &= ~CHESS_CASTLE_WKS;
        board.info &= ~CHESS_CASTLE_WQS;
        board.info &= ~CHESS_CASTLE_BKS;
        board.info &= ~CHESS_CASTLE_BQS;
    }
    else {
        if (fen.data[cursor] == 'K') {
            board.info |= CHESS_CASTLE_WKS;
            cursor++;
        }
        if (fen.data[cursor] == 'Q') {
            board.info |= CHESS_CASTLE_WQS;
            cursor++;
        }
        if (fen.data[cursor] == 'k') {
            board.info |= CHESS_CASTLE_BKS;
            cursor++;
        }
        if (fen.data[cursor] == 'q') {
            board.info |= CHESS_CASTLE_BQS;
            cursor++;
        }
    }

    CONSUME(' ')

    if (fen.data[cursor] == '-')
    {
        board.info &= ~CHESS_EN_PASSANT;
    }
    else {
        bool en_passant_success = false;
        Board_Location en_passant = parse_chess_square(fen, &cursor, &en_passant_success);
        if (en_passant_success)
        {
            PARSE_FAIL
        }
        board.info |= CHESS_EN_PASSANT;
        board.info |= CHESS_EN_PASSANT_SQUARE(en_passant.index);
    }

    bool move_success = false;
    board.half_move_clock = parse_number(fen, &cursor, &move_success);
    CONSUME(' ')
    board.move = parse_number(fen, &cursor, &move_success);

    if (!move_success)
    {
        PARSE_FAIL
    }

    return board;

#undef PARSE_FAIL
#undef CONSUME
}
