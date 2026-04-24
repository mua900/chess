#include "piece_set.hpp"

void render_piece_set(const RenderContext& context, SDL_Texture* targets[], NSVGimage* images[])
{
	ColorF white_tint = ColorF(0.8, 0.8, 0.8);
	ColorF black_tint = ColorF(0.3, 0.3, 0.3);
	for (int i = 0; i < PIECE_TYPE_PER_SIDE * 2; i++)
	{
		bool is_white = i < PIECE_TYPE_PER_SIDE;
		SDL_SetRenderTarget(context.renderer, targets[i]);
		draw_svg_image(context, images[i], 1, vec2(0, 0), is_white ? white_tint : black_tint);
	}

	SDL_SetRenderTarget(context.renderer, nullptr);
}
