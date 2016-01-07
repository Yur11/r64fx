#ifndef R64FX_WIDGET_FLAGS_HPP
#define R64FX_WIDGET_FLAGS_HPP

/* Values to be used with Widget m_flags. */

#define R64FX_WIDGET_IS_WINDOW               1

#define R64FX_WIDGET_IS_VISIBLE              (1 << 1)

/* The update() method has been called on this widget. */
#define R64FX_WIDGET_WANTS_UPDATE            (1 << 2)

/* The update() method has been called on one of the children. */
#define R64FX_CHILD_WANTS_UPDATE             (1 << 3)
#define R64FX_WIDGET_UPDATE_FLAGS            (R64FX_WIDGET_WANTS_UPDATE | R64FX_CHILD_WANTS_UPDATE)

/* Widget recieves focus upon mouse click. */
#define R64FX_WIDGET_CLICK_FOCUS             (1 << 4)

/* For Widget_Text */
#define R64FX_WIDGET_IS_EDITABLE             (1 << 5)
#define R64FX_WIDGET_OWNS_TEXT               (1 << 6)
#define R64FX_WIDGET_OWNS_FONT               (1 << 7)

#endif//R64FX_WIDGET_FLAGS_HPP