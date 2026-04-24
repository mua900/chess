#include "application.hpp"
#include "log.hpp"

#include <iostream>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <nanosvg.h>

bool Application::initialize()
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("Failed to init SDL: %s\n", SDL_GetError());
        return false;
    }

    // window
    {
        float scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

        SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE |
                                SDL_WINDOW_HIDDEN;  // show the window after the initialization is complete
        SDL_Window* window = SDL_CreateWindow("chess", 1440, 810, flags);
        if (!window)
        {
            SDL_Log("Failed to create window with SDL: %s\n", SDL_GetError());
            return false;
        }

        SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
        if (!renderer)
        {
            SDL_Log("Failed to create renderer with SDL: %s\n", SDL_GetError());
            return false;
        }

        SDL_ShowWindow(window);

        m_window = { window };

        int render_size_x, render_size_y;
        SDL_GetRenderOutputSize(renderer, &render_size_x, &render_size_y);
        m_render = { vec2(render_size_x, render_size_y), renderer };
    }

    {
        if (!TTF_Init())
        {
            fprintf(stderr, "Could not initialize TTF: %s\n", SDL_GetError());
            return false;
        }

        if (!MIX_Init())
        {
            fprintf(stderr, "Could not initialize MIX: %s\n", SDL_GetError());
            return false;
        }
    }

    String_Builder path(256);

    get_base_path(path);
    if (!read_asset_catalog(path)) {
        log_error("Could not read asset catalog\n");
        // return false;
    }

    get_to_run_tree_path(path, "asset/piece_set/chessnut/");
    if (!load_piece_set(path))
    {
        // @todo a fallback simple procedural piece set
        log_error("Could not load default piece set");
        return false;
    }

    ChessState state;
    bool success = parse_fen_string(&state, String("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    if (!success)
    {
        return false;
    }

    print_board_state(state);
    game.state = state;

    quit = false;

    return true;
}

Text create_text(SDL_Renderer* renderer, String text, Font font, Color color)
{
    SDL_Color sdl_color = { color.r, color.g, color.b, color.a };
    SDL_Surface* surface = TTF_RenderText_Solid(font.font, text.data, text.size, sdl_color);

    if (!surface)
        return Text();

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (!texture)
    {
        SDL_DestroySurface(surface);
        return Text();
    }

    return Text(texture, text);
}

bool Application::read_asset_catalog(String_Builder& path)
{
    const char* desc_name = "run_tree.txt";
    path.append(make_string(desc_name));
    bool parse_description = parse_assets(path.c_string(), m_catalog);

    return parse_description;
}

void get_base_path(String_Builder& builder)
{
    const char* base_path = SDL_GetBasePath();
    builder.clear_and_append(make_string(base_path));
}

void get_to_run_tree_path(String_Builder& builder, const char* path)
{
    get_base_path(builder);
    builder.append(String(path));
}

bool Application::load_piece_set(String_Builder& path)
{
    const char* expected_names[PIECE_TYPE_PER_SIDE * 2] = {
        "wK", "wQ", "wR", "wB", "wN", "wP",
        "bK", "bQ", "bR", "bB", "bN", "bP",
    };

    for (int i = 0; i < ARRAY_SIZE(expected_names); i++)
    {
        int file = 0;
        file += path.append(String(expected_names[i]));
        file += path.append(String(".svg"));

        NSVGimage* image = NULL;
        // @todo think about dpi
        image = nsvgParseFromFile(path.c_string(), "px", 96);

        if (!image)
        {
            path.remove(file);
            return false;
        }
        log_info("Piece: %s, Width: %.1f, Height: %.1f\n", path.c_string(), image->width, image->height);
        path.remove(file);

        if (piece_set.pieces[i])
        {
            nsvgDelete(piece_set.pieces[i]);
        }
        piece_set.pieces[i] = image;
    }

    return true;
}

void Application::handle_events()
{
    SDL_Event e = {};
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
            case SDL_EVENT_QUIT:
            {
                quit = true;
                break;
            }
            case SDL_EVENT_KEY_DOWN:
            {
                SDL_KeyboardEvent keyboard = e.key;

                keyboard_input(keyboard);

                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                SDL_MouseButtonEvent mouse = e.button;
                if (mouse_input())
                {
                    break;
                }

                break;
            }
            case SDL_EVENT_MOUSE_MOTION:
            {
                m_input.mouse.flags = SDL_GetMouseState(&m_input.mouse.pos.x, &m_input.mouse.pos.y);
                break;
            }
            case SDL_EVENT_WINDOW_RESIZED:
            {
                int render_size_x, render_size_y;
                SDL_GetRenderOutputSize(m_render.renderer, &render_size_x, &render_size_y);
                m_render.render_size = vec2(render_size_x, render_size_y);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    update_keyboard_state();
}

bool Application::keyboard_input(SDL_KeyboardEvent keyboard)
{
    switch (keyboard.scancode)
    {
        case SDL_SCANCODE_ESCAPE:
        {
            quit = true;
            return true;
        }
        case SDL_SCANCODE_F11:
        {
            SDL_SetWindowFullscreen(m_window.window, !is_fullscreen());
            return true;
        }
    }

    return false;
}

bool Application::mouse_input()
{
    vec2 mouse_pos = m_input.mouse.pos;

    vec2 render_size = m_render.render_size;
    float board_size = calculate_board_size();
    float square_size = board_size / 8.0f;
    vec2 margin = calculate_board_margin(render_size, board_size);

    if (Rectangle(margin.x, margin.y, board_size, board_size).contains_top_left(mouse_pos))
    {
        int row = 7 - int(floorf((mouse_pos.y - margin.y) / square_size));
        int column = int(floorf((mouse_pos.x - margin.x) / square_size));

        m_selected_square = row * 8 + column;
        return true;
    }

    return false;
}

void Application::update_keyboard_state()
{
    m_input.keyboard.keys = SDL_GetKeyboardState(&m_input.keyboard.num_keys);
    m_input.keyboard.mod_state = SDL_GetModState();
}

void Application::update()
{
    // update time
    SDL_Time time = SDL_GetTicksNS();
    double time_sec = (double)time / NS_PER_SECONDS;
    m_time = time;
    m_time_seconds = time_sec;

    timeout();
}

void Application::timeout()
{
    for (int i = 0; i < ARRAY_SIZE(m_events); i++)
    {
        if (m_events[i].active)
        {
            if (m_events[i].event < m_time)
            {
                m_events[i].active = false;
            }
        }
    }
}

void Application::set_event_active(int event_index, double timeout_seconds)
{
    s64 timeout = (s64)(timeout_seconds * NS_PER_SECONDS);
    m_events[event_index].active = true;
    m_events[event_index].event = m_time + timeout;
}

void Application::set_event_deactive(int event_index)
{
    m_events[event_index].active = false;
}

void Application::cleanup()
{
    MIX_Quit();
    SDL_Quit();
}

void Application::draw()
{
    SDL_Renderer* renderer = m_render.renderer;

    if (SDL_GetWindowFlags(m_window.window) & SDL_WINDOW_MINIMIZED) {
        // don't draw anything if the window is minimized
        return;
    }

    SDL_SetRenderDrawColor(renderer, COLOR_ARG(m_background_color));
    SDL_RenderClear(renderer);

    draw_board();
    draw_ui();

    SDL_RenderPresent(renderer);
}

void Application::draw_board()
{
    float board_size = calculate_board_size();
    float square_size = board_size / 8.0f;
    vec2 margin = calculate_board_margin(m_render.render_size, board_size);

    const Color WhiteSquareColor = Color(0xAA, 0xAA, 0xAA);
    const Color BlackSquareColor = Color(0x22, 0x22, 0x22);

    for (int j = 0; j < 8; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            bool is_white = ((i + j) % 2 == 0);
            Color color = is_white ? WhiteSquareColor : BlackSquareColor;
            int row = 7 - j;
            int column = i;
            SquareIndex index = row * 8 + column;
            if (index == m_selected_square)
            {
                color = is_white ? ColorF(0.8, 0.5, 0.5) : ColorF(0.6, 0.3, 0.3);
            }
            render_rectangle(Rectangle(margin.x + i * square_size, margin.y + j * square_size, square_size, square_size), color, false);
        }
    }

    ChessState state = game.state;
    while (state.white)
    {
        int index = pop_lsb(&state.white);
        BoardPosition position = index_to_board_position(index);
        Bitboard square = BIT(index);

        Rectangle area = calculate_square_area(position.row, position.column);
        vec2 translate = vec2(area.x, area.y);
        float scale = 0.1;

        ColorF color = ColorF(0.8, 0.8, 0.8);
        NSVGimage* image = nullptr;
        if (index_to_board_position(state.white_king) == position) {
            image = piece_set.pieces[PieceType::King];
        }
        else if (state.queen & square) {
            image = piece_set.pieces[PieceType::Queen];
        }
        else if (state.rook & square) {
            image = piece_set.pieces[PieceType::Rook];
        }
        else if (state.bishop & square) {
            image = piece_set.pieces[PieceType::Bishop];
        }
        else if (state.knight & square) {
            image = piece_set.pieces[PieceType::Knight];
        }
        else if (state.pawn & square) {
            image = piece_set.pieces[PieceType::Pawn];
        }
        else {
            log_error("Invalid chess board state");
            continue;
        }

        draw_svg_image(m_render, image, scale, translate, color);
    }
    while (state.black)
    {
        int index = pop_lsb(&state.black);
        BoardPosition position = index_to_board_position(index);
        Bitboard square = BIT(index);

        Rectangle area = calculate_square_area(position.row, position.column);
        vec2 translate = vec2(area.x, area.y);
        float scale = 0.1;

        ColorF color = ColorF(0.4, 0.4, 0.4);
        NSVGimage* image = nullptr;
        if (index_to_board_position(state.black_king) == position) {
            image = piece_set.pieces[PieceType::King];
        }
        else if (state.queen & square) {
            image = piece_set.pieces[PieceType::Queen];
        }
        else if (state.rook & square) {
            image = piece_set.pieces[PieceType::Rook];
        }
        else if (state.bishop & square) {
            image = piece_set.pieces[PieceType::Bishop];
        }
        else if (state.knight & square) {
            image = piece_set.pieces[PieceType::Knight];
        }
        else if (state.pawn & square) {
            image = piece_set.pieces[PieceType::Pawn];
        }
        else {
            log_error("Invalid chess board state");
            continue;
        }

        draw_svg_image(m_render, image, scale, translate, color);
    }
}

float Application::calculate_board_size() const
{
    float percent = 0.8;
    float board_size = MIN(m_render.render_size.x, m_render.render_size.y) * percent;
    return board_size;
}

Rectangle Application::calculate_square_area(int row, int column) const
{
    vec2 render_size = m_render.render_size;
    float board_size = calculate_board_size();
    float square_size = board_size / 8.0f;
    vec2 margin = calculate_board_margin(render_size, board_size);
    return Rectangle(margin.x + column * square_size, margin.y + (7 - row) * square_size, square_size, square_size);
}

vec2 Application::calculate_board_margin(vec2 render_size, float board_size)
{
    return vec2((render_size.x - board_size) / 2, (render_size.y - board_size) / 2);
}

bool Application::is_fullscreen() const
{
    SDL_WindowFlags flags = SDL_GetWindowFlags(m_window.window);
    return flags & SDL_WINDOW_FULLSCREEN;
}

void Application::draw_ui()
{
    // @todo
}

void Application::render_rectangle(Rectangle rect, Color color, bool center)
{
    SDL_SetRenderDrawColor(m_render.renderer, COLOR_ARG(color));
    SDL_FRect area = center ?
                    SDL_FRect { rect.x - rect.w / 2, rect.y - rect.h / 2, rect.w, rect.h } :
                    SDL_FRect { rect.x, rect.y, rect.w, rect.h };
    SDL_RenderFillRect(m_render.renderer, &area);
}

void Application::render_slider(Rectangle area, vec2 knob_scale, float value, Color slider_color, Color knob_color, const Text& text)
{
    float slider_knob_width = area.w * knob_scale.x;
    float slider_knob_height = area.h * knob_scale.y;

    SDL_SetRenderDrawColor(m_render.renderer, COLOR_ARG(slider_color));
    SDL_FRect slider = { area.x, area.y, area.w, area.h };
    SDL_RenderFillRect(m_render.renderer, &slider);
    float percentage = value;
    SDL_SetRenderDrawColor(m_render.renderer, COLOR_ARG(knob_color));
    SDL_FRect slider_knob = {
        slider.x - (slider_knob_width / 2) + (slider.w * percentage), slider.y + slider.h / 2 - slider_knob_height / 2,
        slider_knob_width, slider_knob_height
    };
    SDL_RenderFillRect(m_render.renderer, &slider_knob);

    // text
    {
        const int margin = 10;
        render_text_scale(m_render.renderer, text,
            vec2(slider.x + slider.w / 2, slider.y + slider.h * 2 + margin), vec2(0.6, 0.6));
    }
}

void Application::render_text_field(const Text_Field& text_field)
{
    SDL_FRect tf_area = { text_field.m_area.x, text_field.m_area.y, text_field.m_area.w, text_field.m_area.h };
    SDL_RenderFillRect(m_render.renderer, &tf_area);

    SDL_Texture* text_texture = text_field.m_texture;
    float texture_width;
    float texture_height;
    SDL_GetTextureSize(text_texture, &texture_width, &texture_height);

    if (text_texture)
    {
        int line_count = text_field.m_line_count;
        float font_size = text_field.m_font_size;

        SDL_FRect string_area = { tf_area.x, tf_area.y, texture_width, texture_height };
        SDL_FRect texture_area = { 0, 0, texture_width, texture_height };
        SDL_RenderTexture(m_render.renderer, text_texture, &texture_area, &string_area);

        SDL_SetRenderDrawColor(m_render.renderer, 0x33, 0x56, 0x74, 0xff);

        SDL_FRect cursor = SDL_FRect { tf_area.x + text_field.m_cursor_pixel_x,
                                       tf_area.y + text_field.m_cursor_pixel_y,
                                       tf_area.w / 100,
                                       font_size };
    }
}

void Application::render_dropdown(const Drop_Down_List& list, Color title_color, Color option_color) {
    SDL_SetRenderDrawColor(m_render.renderer, COLOR_ARG(title_color));

    SDL_FRect header_area = {
        list.pos.x - list.scale.x/2, list.pos.y - list.scale.y / 2,
        list.scale.x, list.scale.y
    };
    SDL_RenderFillRect(m_render.renderer, &header_area);
    render_text_size(m_render.renderer, list.title,
        vec2(header_area.x + header_area.w / 2, header_area.y + header_area.h / 2), vec2(header_area.w, header_area.h));

    if (list.open) {
        SDL_SetRenderDrawColor(m_render.renderer, COLOR_ARG(option_color));

        for (int i = 0; i < list.options.size(); i++) {
            SDL_FRect area = header_area;
            area.y += area.h * (i + 1);
            SDL_RenderFillRect(m_render.renderer, &area);
            render_text_size(m_render.renderer, list.get_option_label(i),
                vec2(area.x + area.w/2, area.y + area.h/2), vec2(area.w, area.h));
        }
    }
}

void Application::render_textured_rectangle(Rectangle rect, SDL_Texture* texture, Color color, bool center) {
    SDL_SetRenderDrawColor(m_render.renderer, COLOR_ARG(color));
    SDL_FRect area = center ?
        SDL_FRect { rect.x - rect.w / 2, rect.y - rect.h / 2, rect.w, rect.h } :
        SDL_FRect { rect.x, rect.y, rect.w, rect.h };
    SDL_RenderFillRect(m_render.renderer, &area);

    float tex_w, tex_h;
    SDL_GetTextureSize(texture, &tex_w, &tex_h);
    SDL_FRect src = {0,0,tex_w,tex_h};
    SDL_FRect dst = area;
    SDL_RenderTexture(m_render.renderer, texture, &src, &dst);
}

void render_text_size(SDL_Renderer* renderer, Text text, vec2 where, vec2 absolute_scale)
{
    float tex_w, tex_h;
    SDL_GetTextureSize(text.texture, &tex_w, &tex_h);

    if (!absolute_scale.x)
    {
        absolute_scale = vec2(tex_w, tex_h);
    }

    SDL_FRect src = { 0,0,tex_w,tex_h };
    SDL_FRect dst = {where.x - absolute_scale.x/2, where.y - absolute_scale.y/2, absolute_scale.x, absolute_scale.y};

    SDL_RenderTexture(renderer, text.texture, &src, &dst);
}

void render_text_scale(SDL_Renderer* renderer, Text text, vec2 where, vec2 scale_factor)
{
    float tex_w, tex_h;
    SDL_GetTextureSize(text.texture, &tex_w, &tex_h);

    if (!scale_factor.x)
    {
        scale_factor = vec2(1,1);
    }

    vec2 scale = vec2(tex_w * scale_factor.x, tex_h * scale_factor.y);

    SDL_FRect src = { 0,0,tex_w,tex_h };
    SDL_FRect dst = {where.x - scale.x/2, where.y - scale.y/2, scale.x, scale.y};

    SDL_RenderTexture(renderer, text.texture, &src, &dst);
}
