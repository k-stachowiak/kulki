// Copyright (C) 2015 Krzysztof Stachowiak

#include "config.h"
#include "allegro.h"
#include "kulki.h"

// TODO:
// - port highscore module and add proper highscore input state and menu entry.
// - port menu module and enable basic configuration.
// - FX for: ball arrival, ball removal; improve score increase indicator
// - GFX: add a shade to balls
// - UI: define key colors and make them themeable.

int main()
{
    Allegro al;
    (Kulki {}).run(al);
}

