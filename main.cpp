// Copyright (C) 2015 Krzysztof Stachowiak

#include "config.h"
#include "platform.h"
#include "kulki_context.h"

int main()
{
    KulkiConfig config("config.mn");

    Platform platform {
        config.get_integer("SCREEN_W"),
        config.get_integer("SCREEN_H"),
        config.get_real("FPS"),
        config.get_real("MAX_FRAME_TIME"),
        config.get_real("FRAME_REST")
    };

    KulkiContext kulki_ctx(config);

    platform.realtime_loop(kulki_ctx);
}

