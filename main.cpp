// Copyright (C) 2015 Krzysztof Stachowiak

#include "config.h"
#include "platform.h"
#include "kulki_context.h"

int main()
{
    config::load();
    Platform platform;
    KulkiContext kulki_ctx;
    platform.realtime_loop(kulki_ctx);
}

