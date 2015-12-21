#ifndef R64FX_WIDGET_FLAGS_HPP
#define R64FX_WIDGET_FLAGS_HPP

/* Values to be used with Widget m_flags. */

#define R64FX_WIDGET_IS_WINDOW               1

#define R64FX_WIDGET_IS_VISIBLE              2

#define R64FX_WIDGET_WANTS_UPDATE            4
#define R64FX_CHILD_WANTS_UPDATE             8
#define R64FX_WIDGET_UPDATE_FLAGS            (R64FX_WIDGET_WANTS_UPDATE | R64FX_CHILD_WANTS_UPDATE)

#define R64FX_WIDGET_IS_EDITABLE             16
#define R64FX_WIDGET_OWNS_TEXT               32
#define R64FX_WIDGET_OWNS_FONT               64

#define R64FX_WIDGET_CLICK_FOCUS             128

#endif//R64FX_WIDGET_FLAGS_HPP