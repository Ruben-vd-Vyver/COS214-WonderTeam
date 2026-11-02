#include "TuiWindow.h"

#include <cstddef>

TuiWindow::TuiWindow()
    : window_(NULL), height_(0), width_(0), y_(0), x_(0)
{
}

TuiWindow::~TuiWindow()
{
    destroy();
}

void TuiWindow::resize(int height, int width, int y, int x)
{
    if (height <= 0 || width <= 0)
    {
        destroy();
        return;
    }

    if (window_ && height_ == height && width_ == width && y_ == y && x_ == x)
        return;

    destroy();
    window_ = newwin(height, width, y, x);
    height_ = height;
    width_ = width;
    y_ = y;
    x_ = x;
    if (window_)
    {
        keypad(window_, TRUE);
        werase(window_);
        touchwin(window_);
    }

    // A moved/resized pane may leave its former rectangle visible on terminals
    // that optimise partial updates. Force a full-screen repaint on the next
    // ncurses update; TuiApp then redraws every pane in layout order.
    clearok(stdscr, TRUE);
}

void TuiWindow::destroy()
{
    if (window_)
    {
        delwin(window_);
        window_ = NULL;
    }
    height_ = width_ = y_ = x_ = 0;
}

WINDOW *TuiWindow::get() const
{
    return window_;
}
