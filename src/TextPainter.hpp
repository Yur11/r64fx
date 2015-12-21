#ifndef R64FX_TEXT_PAINTER_HPP
#define R64FX_TEXT_PAINTER_HPP

#include "Rect.hpp"
#include <string>

namespace r64fx{

class Image;
class Font;

class TextPainter{
    void*        m        = nullptr;

public:
    Image*        image  = nullptr;
    std::string*  text   = nullptr;
    Font*         font   = nullptr;

    TextPainter();

   ~TextPainter();

    bool isGood() const;

    void putPen(Point<int> p);

    Point<int> penPosition() const;

    void inputUtf8(const char* bytes, int nbytes);

    void deleteBeforeCursor();

    void deleteAfterCursor();

    void clear();

    void homeCursor();

    void endCursor();

    void putCursor(int index);

    void putCursor(Point<int> p);

    int cursorIndex() const;

    void setCursorVisible(bool yes);

    bool cursorVisible();

    void resetRect();

    Rect<int> rectToUpdate();
};

}//namespace

#endif//R64FX_TEXT_PAINTER_HPP