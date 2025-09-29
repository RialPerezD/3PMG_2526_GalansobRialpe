#pragma once

int main();


class Window {
public:
    Window(int width, int height, const char* windowName);
    ~Window();

    bool shouldWindowClose();

    void pollEvents();

private:
    struct Data;
    Data* data;
};