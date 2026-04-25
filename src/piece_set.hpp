#ifndef _PIECE_SET_H
#define _PIECE_SET_H

#include "common.hpp"
#include "chess.hpp"
#include "draw.hpp"

struct PieceSet {
	String name = {};
	// @todo color can be applied with tint
	SDL_Texture* pieces[PIECE_TYPE_PER_SIDE * 2] = {};
};

bool load_piece_set(const RenderContext& context, SDL_Texture* pieces[], String_Builder& path);

#endif // _PIECE_SET_H