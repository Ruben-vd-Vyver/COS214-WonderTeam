#ifndef TIVIDY_TUI_WINDOW_H
#define TIVIDY_TUI_WINDOW_H

#include <ncurses.h>

// Small RAII wrapper around a curses WINDOW. TuiApp owns the layout policy.
class TuiWindow
{
public:
    TuiWindow();
    ~TuiWindow();

    TuiWindow(const TuiWindow &) = delete;
    TuiWindow &operator=(const TuiWindow &) = delete;

    void resize(int height, int width, int y, int x);
    void destroy();
    WINDOW *get() const;

private:
    WINDOW *window_;
    int height_;
    int width_;
    int y_;
    int x_;
};

#endif // TIVIDY_TUI_WINDOW_H
