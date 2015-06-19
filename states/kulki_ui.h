// Copyright (C) 2015 Krzysztof Stachowiak

#ifndef KULKI_UI_H
#define KULKI_UI_H

class KulkiUi {
    int *m_score;
public:
    KulkiUi(int *score);
    void draw();
};

#endif
