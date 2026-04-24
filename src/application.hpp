#pragma once

#include "common.hpp"
#include "template.hpp"
#include "math_util.hpp"
#include "ui.hpp"
#include "asset.hpp"
#include "input.hpp"
#include "draw.hpp"
#include "chess.hpp"
#include "piece_set.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <nanosvg.h>

struct Window {
    SDL_Window* window;
};

struct Event_Timeout {
    s64 event = 0;
    bool active = false;
};

enum Events {
    EVENT_DUMMY,
    EVENT_COUNT,
};

#define NS_PER_SECONDS 1'000'000'000

class Application {
public:
    ChessGame game = {};
    SquareIndex m_selected_square = NullSquareIndex;
    SquareIndex m_target_square = NullSquareIndex;

    Window m_window = {};
    RenderContext m_render = {};
    Input m_input = {};
    AssetCatalog m_catalog = {};

    Ui_State m_ui = {};
    Color m_background_color = DEFAULT_BACKGROUND_COLOR;

    s64 m_time = 0;
    double m_time_seconds = 0;

    Event_Timeout m_events[EVENT_COUNT] = {};

    Array<Text> m_rendered_text = {};

    PieceSet piece_set = {};

    bool quit = false;
    // bool doing_text_input = false;

    bool initialize();

    void handle_events();
    void update();
    void draw();

    void cleanup();
private:
    void timeout();
    void update_ui_state(vec2 window_size);
    void set_event_active(int event_index, double timeout_seconds);
    void set_event_deactive(int event_index);

    void draw_board();
	void draw_ui();

    bool mouse_input();

    void update_keyboard_state();
    bool keyboard_input(SDL_KeyboardEvent keyboard);

    bool gen_static_text(Color color);

    // void text_input_start();
    // void text_input_stop();
    // void toggle_text_input();
    // bool update_input_string();

    bool read_asset_catalog(String_Builder& path);

    bool set_eval_string(String s);
    bool set_eval_string_left(String s);
    bool set_eval_string_right(String s);

    void render_rectangle(Rectangle rect, Color color, bool center = true);
    void render_textured_rectangle(Rectangle rect, SDL_Texture* texture, Color color, bool center = true);

    void render_slider(Rectangle area, vec2 knob_scale, float value, Color slider_color, Color knob_color, const Text& text);
    void render_text_field(const Text_Field& text_field);
    void render_dropdown(const Drop_Down_List& list, Color title_color, Color option_color);

    void clear_text_input_selection();

    float calculate_board_size() const;
    Rectangle calculate_square_area(int row, int column) const;

    bool is_fullscreen() const;

    static vec2 calculate_board_margin(vec2 render_size, float board_size);
};

void get_base_path(String_Builder& builder);

Text create_text(SDL_Renderer* renderer, String text, Font font, Color color);

void render_text_size(SDL_Renderer* renderer, Text text, vec2 where, vec2 absolute_scale = vec2(0, 0));
void render_text_scale(SDL_Renderer* renderer, Text text, vec2 where, vec2 scale_factor = vec2(0,0));
