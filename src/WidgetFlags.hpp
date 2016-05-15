#ifndef R64FX_WIDGET_FLAGS_HPP
#define R64FX_WIDGET_FLAGS_HPP

/* Values to be used with Widget m_flags. */

#define R64FX_WIDGET_IS_WINDOW                 1

#define R64FX_WIDGET_IS_VISIBLE               (1 << 0x01)

#define R64FX_WIDGET_IS_PARTIALLY_VISIBLE     (1 << 0x02)

#define R64FX_WIDGET_CHILDREN_CONTINUOUS      (1 << 0x03)

/* The clip() method has been called on this widget. */
#define R64FX_WIDGET_WANTS_CLIPPING           (1 << 0x04)

/* The clip() method has been called on one of the children.*/
#define R64FX_CHILD_WANTS_CLIPPING            (1 << 0x05)

#define R64FX_WIDGET_CLIP_FLAGS               (R64FX_WIDGET_WANTS_CLIPPING | R64FX_CHILD_WANTS_CLIPPING)

/* The repaint() method has been called on this widget. */
#define R64FX_WIDGET_WANTS_REPAINT            (1 << 0x06)

/* The repaint() method has been called on one of the children. */
#define R64FX_CHILD_WANTS_REPAINT             (1 << 0x07)

#define R64FX_WIDGET_REPAINT_FLAGS            (R64FX_WIDGET_WANTS_REPAINT | R64FX_CHILD_WANTS_REPAINT)

/* Widget recieves focus upon mouse click. */
#define R64FX_WIDGET_CLICK_FOCUS              (1 << 0x08)

#define R64FX_WIDGET_GRABS_MOUSE_ON_CLICK     (1 << 0x09)

#define R64FX_WIDGET_UNGRABS_MOUSE_ON_RELEASE (1 << 0x0A)

#define R64FX_WIDGET_IS_EDITABLE              (1 << 0x0B)
#define R64FX_WIDGET_OWNS_TEXT                (1 << 0x0C)
#define R64FX_WIDGET_OWNS_FONT                (1 << 0x0D)

#define R64FX_WIDGET_IS_VERTICAL              (1 << 0x0E)

/* Pinned widget are not scrollable in Widget_ScrollArea. */
#define R64FX_WIDGET_IS_PINNED                (1 << 0x0F)

#define R64FX_WIDGET_OWNS_DATA                (1 << 0x10)

#define R64FX_WIDGET_IS_HOVERED               (1 << 0x11)

#define R64FX_WIDGET_TREE_IS_COLLAPSED        (1 << 0x12)

#endif//R64FX_WIDGET_FLAGS_HPP