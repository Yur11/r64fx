#ifndef R64FX_WIDGET_FLAGS_HPP
#define R64FX_WIDGET_FLAGS_HPP

/* Values to be used with Widget m_flags. */

#define R64FX_WIDGET_IS_WINDOW               1

#define R64FX_WIDGET_IS_VISIBLE              (1 << 0x01)

/* The update() method has been called on this widget. */
#define R64FX_WIDGET_WANTS_UPDATE            (1 << 0x02)

/* The update() method has been called on one of the children. */
#define R64FX_CHILD_WANTS_UPDATE             (1 << 0x03)

#define R64FX_WIDGET_UPDATE_FLAGS            (R64FX_WIDGET_WANTS_UPDATE | R64FX_CHILD_WANTS_UPDATE)

/* Widget recieves focus upon mouse click. */
#define R64FX_WIDGET_CLICK_FOCUS             (1 << 0x04)

#define R64FX_WIDGET_GRABS_MOUSE_ON_CLICK    (1 << 0x05)

#define R64FX_WIDGET_IS_EDITABLE             (1 << 0x06)
#define R64FX_WIDGET_OWNS_TEXT               (1 << 0x07)
#define R64FX_WIDGET_OWNS_FONT               (1 << 0x08)

#define R64FX_WIDGET_IS_VERTICAL             (1 << 0x09)

/* Pinned widget are not scrollable in Widget_ScrollArea. */
#define R64FX_WIDGET_IS_PINNED               (1 << 0x0A)

#define R64FX_WIDGET_OWNS_DATA               (1 << 0x0B)

#define R64FX_WIDGET_IS_HOVERED              (1 << 0x0C)

#endif//R64FX_WIDGET_FLAGS_HPP