// Copyright (C) 2015 Krzysztof Stachowiak

#include "config.h"
#include "dick.h"
#include "kulki_context.h"

int main()
{
    KulkiConfig config { "config.mn", "config.var.mn" };

    dick::Platform platform { {
        static_cast<double>(config.get_integer("SCREEN_W")),
        static_cast<double>(config.get_integer("SCREEN_H"))
    } };

    KulkiContext kulki_ctx { config };

    platform.real_time_loop(kulki_ctx);
}

