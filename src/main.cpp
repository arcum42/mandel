/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "SDL.h"
#include <complex>
#include <vector>

struct point
{
    double x, y;
};

point m_min = {-2.0, -2.0};
point m_max = {2.0, 2.0};

const int initial_width = 640;
const int initial_height = 480;

int m_width = initial_width;
int m_height = initial_height;

const u_int32_t m_max_iter = 100;

std::vector<u_int8_t> pixels(m_width *m_height * 4, 0);

double map(double x, double in_min, double in_max, double out_min, double out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

u_int32_t mandel(double x, double y)
{
    std::complex<double> c{x, y};
    std::complex<double> z{0, 0};
    u_int32_t i = 0;

    while ((abs(z) < 2) && (i < m_max_iter))
    {
        z = z * z + c;
        i++;
    }

    return i;
}

void draw_pixel_alpha(u_int32_t x, u_int32_t y, u_int8_t r, u_int8_t g, u_int8_t b, u_int8_t a)
{
    const unsigned int offset = (m_width * 4 * y) + x * 4;

    pixels[offset + 0] = r;
    pixels[offset + 1] = g;
    pixels[offset + 2] = b;
    pixels[offset + 3] = a;
}

void draw_pixel(u_int32_t x, u_int32_t y, u_int8_t r, u_int8_t g, u_int8_t b)
{
    draw_pixel_alpha(x, y, r, g, b, SDL_ALPHA_OPAQUE);
}

void draw_mandel(SDL_Texture *texture)
{
    pixels.clear();
    for (int screen_x = 0; screen_x < m_width; screen_x++)
    {
        for (int screen_y = 0; screen_y < m_height; screen_y++)
        {
            double real_x = map(screen_x, 0, m_width, m_min.x, m_max.x);
            double real_y = map(screen_y, 0, m_height, m_min.y, m_max.y);

            uint32_t i = mandel(real_x, real_y);

            if (i < m_max_iter)
            {
                int grad_r = map(i, 0, m_max_iter, 0, 255);
                int grad_g = map(i, 0, m_max_iter, 0, 128);
                int grad_b = map(i, 0, m_max_iter, 0, 255);

                draw_pixel(screen_x, screen_y, grad_r, grad_g, grad_b);
            }
            else
            {
                draw_pixel(screen_x, screen_y, 0, 0, 0);
            }
        }
    }
    SDL_UpdateTexture(texture, nullptr, &pixels[0], m_width * 4);
}

int main(int argc, char *argv[])
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    int err = SDL_CreateWindowAndRenderer(m_width, m_height, SDL_WINDOW_RESIZABLE, &window, &renderer);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, m_width, m_height);

    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    bool closing_time = false;
    bool redraw = true;

    while (!closing_time)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                //case SDL_KEYUP:
                if (!event.key.repeat)
                {
                    redraw = true;
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_PLUS:
                    case SDLK_EQUALS:
                        m_min.x *= 0.9;
                        m_min.y *= 0.9;
                        m_max.x *= 0.9;
                        m_max.y *= 0.9;
                        break;

                    case SDLK_MINUS:
                        m_min.x /= 0.9;
                        m_min.y /= 0.9;
                        m_max.x /= 0.9;
                        m_max.y /= 0.9;
                        break;

                    case SDLK_LEFT:
                        m_min.x /= 0.9;
                        m_max.x *= 0.9;
                        break;

                    case SDLK_RIGHT:
                        m_min.x *= 0.9;
                        m_max.x /= 0.9;
                        break;

                    case SDLK_UP:
                        m_min.y /= 0.9;
                        m_max.y *= 0.9;
                        break;

                    case SDLK_DOWN:
                        m_min.y *= 0.9;
                        m_max.y /= 0.9;
                        break;

                    default:
                        break;
                    }
                }
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    m_width = event.window.data1;
                    m_height = event.window.data2;

                    SDL_DestroyTexture(texture);
                    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, m_width, m_height);
                    pixels.resize(m_width * m_height * 4);

                    redraw = true;
                }
                break;

            case SDL_QUIT:
                closing_time = true;
                break;

            default:
                break;
            }
        }

        if (!closing_time)
        {
            if (redraw)
            {
                draw_mandel(texture);
                redraw = false;
            }

            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
