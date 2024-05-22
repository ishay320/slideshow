#include <getopt.h>
#include <stdlib.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <string>
#include <vector>

#include "effect.h"
#include "getters.h"
#include "logger.h"
#include "raylib.h"

// steps:
// [ ] 1. check updates online
// [ ] 2. check config for folders of images
// [x] 3. create class of files
// [x] 4. create class of texture getter
// [x] 5. create class of effect
// [x] 6. using effect class fill it with the image and transition
// [x] 7. loop til the world end
// [ ] 8. if drive connect/disconnect search again

void usage(int argc, char* argv[])
{
    std::cout << "Usage: '" << argv[0] << "'\n";
    std::cout << "      -v --valgrind <seconds> | optional\n";
}

static const char optstr[]                = "r:h";
static const struct option long_options[] = {
    {"run-time-seconds", required_argument, nullptr, 'r'},
    {nullptr, 0, nullptr, 0},
};
struct argsSettings {
    std::optional<int> run_time_in_sec;
};
argsSettings init(int argc, char* argv[])
{
    argsSettings args;
    int c;
    while ((c = getopt_long(argc, argv, optstr, long_options, NULL)) != -1) {
        const char* tmp_optarg = optarg;
        switch (c) {
            case 'r':
                args.run_time_in_sec = std::stoi(tmp_optarg);
                break;
            case 'h':
                usage(argc, argv);
                exit(0);
            default:
                usage(argc, argv);
                exit(1);
        }
    }
    return args;
}

int main(int argc, char* argv[])
{
    auto args = init(argc, argv);

    struct {
        int swap_time = 8;
        std::vector<std::string> file_types{"png", "jpg", "heic"};
        float effect_speed      = 50;
        int fps                 = 60;
        std::string images_path = "./data";
    } config;

    LOG_INFO("starting window");
    InitWindow(800, 450, "slideshow");
    const int display = GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
    SetTargetFPS(config.fps);
    ToggleFullscreen();

    LOG_INFO("loading files from '" << config.images_path << '\'');
    FileGetter fg{config.images_path, config.file_types};
    ImageGetter ig{fg};
    Effect effect{std::move(ig), config.effect_speed};

    LOG_INFO("starting the loop");
    int next_swap_time     = config.swap_time;
    double prev_frame_time = GetTime();
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        double current_time = GetTime();
        double delta_time   = current_time - prev_frame_time;
        prev_frame_time     = current_time;
        effect.update(delta_time);

        DrawFPS(50, 50);
        EndDrawing();

        if (GetTime() > next_swap_time) {
            next_swap_time += config.swap_time;
            effect.next();
            LOG_INFO("changed image");
        }

        if (args.run_time_in_sec.has_value() &&
            GetTime() > args.run_time_in_sec.value()) {
            break;
        }
    }

    LOG_INFO("closing the window");
    CloseWindow();
    return 0;
}
