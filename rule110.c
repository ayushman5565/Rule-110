#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define WINDOW_WIDTH 1260
#define WINDOW_HEIGHT 720
#define ROW_SIZE 252
#define ITERATIONS 144
#define RECT_WIDTH WINDOW_WIDTH / ROW_SIZE
#define RECT_HEIGHT WINDOW_HEIGHT / ITERATIONS

#define BG_COLOR 0xFFFFFFFF
#define FG_COLOR 0x00000000

/* uint8_t varibles for FOREGROUND and BACKGROUND color */
static uint8_t fr, fg, fb, fa;
static uint8_t br, bg, bb, ba;

typedef enum
{
    O = 0,
    I = 1
} Cell;

typedef struct
{
    Cell cells[ROW_SIZE];
} Row;

/* Initializes SDL subsystems */
void init_sdl_subsystems(void)
{
    SDL_InitSubSystem(SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_VIDEO);
}

/* Creates a SDL Window */
SDL_Window *init_window(void)
{
    SDL_Window *window = SDL_CreateWindow("Game Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (window == NULL)
    {
        fprintf(stderr, "Error while creating window.\n");
        exit(1);
    }
    return window;
}

/* Creates an SDL renderer */
SDL_Renderer *init_renderer(SDL_Window *window)
{
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL)
    {
        fprintf(stderr, "Error while creating Renderer : %s.\n", SDL_GetError());
        exit(1);
    }
    return renderer;
}

/* Generate a randow row */
Row randow_row(void)
{
    Row result = {0};
    for (uint8_t i = 0; i < ROW_SIZE; i++)
    {
        result.cells[i] = rand() % 2;
    }
    return result;
}

/* Ruleset for the automaton */
Cell patterns[1 << 3] = {
    [0b000] = 0,
    [0b001] = 1,
    [0b010] = 1,
    [0b011] = 1,
    [0b100] = 0,
    [0b101] = 1,
    [0b110] = 1,
    [0b111] = 0,
};

/* Generate next row */
Row next_row(Row prev)
{
    Row next = {0};
    for (uint8_t i = 1; i < ROW_SIZE - 1; i++)
    {
        uint8_t neighbourhood = ((prev.cells[i - 1] << 2) | (prev.cells[i] << 1) | (prev.cells[i + 1]));
        next.cells[i] = patterns[neighbourhood];
    }
    /* for cell 0 */
    const uint8_t neighbourhood = ((prev.cells[ROW_SIZE - 1] << 2) | (prev.cells[0] << 1) | (prev.cells[1]));
    next.cells[0] = patterns[neighbourhood];

    /* for cell ROW_SIZE - 1 */
    const uint8_t neighbourhood2 = ((prev.cells[ROW_SIZE - 2] << 2) | (prev.cells[ROW_SIZE - 1] << 1) | (prev.cells[0]));
    next.cells[ROW_SIZE - 1] = patterns[neighbourhood2];

    return next;
}

/* Get RGBA value from hex value of color */
void get_uint8_rgba_value(uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a, uint32_t Color)
{
    *r = (Color >> 24) & 0xFF;
    *g = (Color >> 16) & 0xFF;
    *b = (Color >> 8) & 0xFF;
    *a = (Color) & 0xFF;
}

/* Set initial bg-color and add title to the window */
void set_bg_color(SDL_Window *window, SDL_Renderer *renderer)
{
    uint8_t r, g, b, a;
    get_uint8_rgba_value(&r, &g, &b, &a, BG_COLOR);
    SDL_SetWindowTitle(window, "Rule 110");
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderClear(renderer);
}

/* Print each row one by one on screen */
void update_screen(SDL_Renderer *renderer, Row *row, SDL_Rect *rect, uint16_t* x_pos, uint16_t* y_pos)
{

    for (int i = 0; i < ROW_SIZE; i++)
    {
        if (*x_pos >= WINDOW_WIDTH)
            *x_pos = 0;
        if (*y_pos >= WINDOW_HEIGHT)
            *y_pos = 0;
        rect->x = *x_pos;
        rect->y = *y_pos;
        if (row->cells[i] == 0)
        {
            SDL_SetRenderDrawColor(renderer, br, bg, bb, ba);
            SDL_RenderFillRect(renderer,rect);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, fr, fg, fb, fa);
            SDL_RenderFillRect(renderer, rect);
        }
        *x_pos += WINDOW_WIDTH/ROW_SIZE;
    }
    *y_pos += WINDOW_HEIGHT/ITERATIONS;
    *row = next_row(*row);
}

int main(void)
{
    init_sdl_subsystems();
    SDL_Window *window = init_window();
    SDL_Renderer *renderer = init_renderer(window);
    srand(time(NULL));

    SDL_Rect *rect = malloc(sizeof(*rect));
    rect->h = RECT_HEIGHT;
    rect->w = RECT_WIDTH;
    rect->x = 0;
    rect->y = 0;    

    uint16_t x_pos = 0, y_pos = 0;

    Row row = randow_row();

    set_bg_color(window, renderer);
    get_uint8_rgba_value(&fr, &fa, &fb, &fa, FG_COLOR);
    get_uint8_rgba_value(&br, &bg, &bb, &ba, BG_COLOR);

    bool running = true;
    SDL_Event Event;
    while (running)
    {
        while (SDL_PollEvent(&Event))
        {
            if (Event.type == SDL_QUIT)
            {
                running = false;
            }
            if (Event.type == SDL_KEYDOWN)
            {
                if (Event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = false;
                    exit(1);
                }
            }
        }
        update_screen(renderer,&row,rect,&x_pos,&y_pos);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_Quit();
    return 0;
}
