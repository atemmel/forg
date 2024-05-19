#pragma once

#include <raylib.h>

#include <forg/core.hpp>

using namespace forg;

struct MyState : public State {
    enum Dir {
        Up,
        Down,
        Left,
        Right,
    };

    Image link_image;
    Texture link_texture;

    Dir link_dir            = Down;
    const int link_velocity = 2;

    int link_anim_step            = 0;
    const int link_anim_last_step = 8;

    int link_anim_ticks           = 0;
    const int link_anim_last_tick = 6;

    Vector2 link_position;
    Rectangle link_frame;

    MyState() {
        link_image = LoadImage("./link.png");
        defer(UnloadImage(link_image));
        ImageResizeNN(&link_image, link_image.width * 2, link_image.height * 2);
        link_texture = LoadTextureFromImage(link_image);

        link_position = {
            80,
            80,
        };
    }

    ~MyState() {
        println("Cleaning up...");
        UnloadTexture(link_texture);
    }

    auto update() -> bool override {
        bool is_walking = false;

        if (IsKeyDown(KEY_A)) {
            link_dir   = Left;
            is_walking = true;
            link_position.x -= link_velocity;
        }

        if (IsKeyDown(KEY_D)) {
            link_dir   = Right;
            is_walking = true;
            link_position.x += link_velocity;
        }

        if (IsKeyDown(KEY_W)) {
            link_dir   = Up;
            is_walking = true;
            link_position.y -= link_velocity;
        }

        if (IsKeyDown(KEY_S)) {
            link_dir   = Down;
            is_walking = true;
            link_position.y += link_velocity;
        }

        if (!is_walking) {
            link_anim_step = 0;
        }

        const auto looking_up    = Vector2{12, 9};
        const auto looking_down  = Vector2{12, 91};
        const auto looking_left  = Vector2{96, 91};
        const auto looking_right = Vector2{96, 9};

        const auto walking_up    = Vector2{184, 9};
        const auto walking_down  = Vector2{184, 91};
        const auto walking_left  = Vector2{842, 91};
        const auto walking_right = Vector2{842, 9};

        const auto offset_frame_x = 82;

        link_frame = {
            .x      = 0,
            .y      = 0,
            .width  = 48,
            .height = 70,
        };

        if (!is_walking) {
            switch (link_dir) {
                case Up:
                    link_frame.x = looking_up.x;
                    link_frame.y = looking_up.y;
                    break;
                case Down:
                    link_frame.x = looking_down.x;
                    link_frame.y = looking_down.y;
                    break;
                case Left:
                    link_frame.x = looking_left.x;
                    link_frame.y = looking_left.y;
                    break;
                case Right:
                    link_frame.x = looking_right.x;
                    link_frame.y = looking_right.y;
                    break;
            }
        } else {
            switch (link_dir) {
                case Up:
                    link_frame.x =
                        walking_up.x + (link_anim_step * offset_frame_x);
                    link_frame.y = walking_up.y;
                    break;
                case Down:
                    link_frame.x =
                        walking_down.x + (link_anim_step * offset_frame_x);
                    link_frame.y = walking_down.y;
                    break;
                case Left:
                    link_frame.x =
                        walking_left.x + (link_anim_step * offset_frame_x);
                    link_frame.y = walking_left.y;
                    break;
                case Right:
                    link_frame.x =
                        walking_right.x + (link_anim_step * offset_frame_x);
                    link_frame.y = walking_right.y;
                    break;
            }

            link_anim_ticks++;
            if (link_anim_ticks >= link_anim_last_tick) {
                link_anim_ticks = 0;
                link_anim_step++;
                if (link_anim_step == link_anim_last_step) {
                    link_anim_step = 0;
                }
            }
        }
        return true;
    }

    auto draw() -> void override {
        ClearBackground(Color{
            .r = 35,
            .g = 35,
            .b = 45,
            .a = 255,
        });

        DrawTextureRec(link_texture, link_frame, link_position, WHITE);
        DrawText("Running", 30, 30, 20, WHITE);
    }
};
