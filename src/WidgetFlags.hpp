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
#define R64FX_WIDGET_WANTS_REPAINT            (1UL << 0x06)

/* The repaint() method has been called on one of the children. */
#define R64FX_CHILD_WANTS_REPAINT             (1UL << 0x07)

#define R64FX_WIDGET_REPAINT_FLAGS            (R64FX_WIDGET_WANTS_REPAINT | R64FX_CHILD_WANTS_REPAINT)

#define R64FX_WIDGET_UNGRABS_MOUSE_ON_RELEASE (1UL << 0x08)

#define R64FX_WIDGET_IS_EDITABLE              (1UL << 0x09)
#define R64FX_WIDGET_OWNS_TEXT                (1UL << 0x0A)
#define R64FX_WIDGET_OWNS_FONT                (1UL << 0x0B)

#define R64FX_WIDGET_IS_VERTICAL              (1UL << 0x0C)

#define R64FX_WIDGET_OWNS_DATA                (1UL << 0x0D)

#define R64FX_WIDGET_IS_HOVERED               (1UL << 0x0F)

#define R64FX_WIDGET_TREE_IS_EXPANDED         (1UL << 0x10)

#define R64FX_WIDGET_IS_EVEN                  (1UL << 0x11)

#define R64FX_WIDGET_LABEL_OWNS_IMAGE         (1UL << 0x12)

#define R64FX_WIDGET_IS_SELECTED              (1UL << 0x13)

#define R64FX_WIDGET_DATA_ITEM_IS_COMPOUND    (1UL << 0x14)

#define R64FX_WIDGET_DATA_ITEM_IS_TREE        (1UL << 0x15)

#define R64FX_WIDGET_ITEM_IS_DIRECTORY        (1UL << 0x16)

#define R64FX_WIDGET_DIRECTORY_IS_POPULATED   (1UL << 0x17)

#define R64FX_WIDGET_WANTS_MULTI_GRABS        (1UL << 0x18)

#define R64FX_WIDGET_SHOWS_TEXT               (1UL << 0x19)

#define R64FX_WIDGET_OWNS_ANIMATION           (1UL << 0x1A)

#define R64FX_SLIDER_BAR_VISIBLE              (1UL << 0x1B)

#define R64FX_WIDGET_IS_FLIPPED               (1UL << 0x1C)

#define R64FX_WIDGET_IS_REVERSED              (1UL << 0x1D)

#define R64FX_WIDGET_WANTS_DND                (1UL << 0x1E)

#define R64FX_PLAYER_VIEW_REPAINT_PLAYHEAD    (1UL << 0x1F)

#define R64FX_TAB_SELECTED                    (1UL << 0x20)

#endif//R64FX_WIDGET_FLAGS_HPP
