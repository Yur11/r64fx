/* To be included in WindowX11.cpp */

void WindowX11::processSomeEvents(WindowEventDispatcherIface* events)
{
    while(XPending(g_display))
    {
        g_incoming_event = XEvent();
        XNextEvent(g_display, &g_incoming_event);
        auto xwindow = g_incoming_event.xany.window;

        WindowX11* window = getWindowFromXWindow(xwindow);
        if(!window)
        {
            continue;
        }

        if(window->doingTextInput() && XFilterEvent(&g_incoming_event, g_incoming_event.xany.window))
        {
            continue;
        }

        switch(g_incoming_event.type)
        {
            case KeyPress:
            {
                if(window->doingTextInput())
                {
                    Status status;
                    char buff[8];
                    KeySym keysym;
                    int nbytes = Xutf8LookupString(
                        window->inputContext(), &g_incoming_event.xkey,
                        buff, 8, &keysym, &status
                    );

                    string str = "";
                    if(nbytes)
                    {
                        str = string(buff, nbytes);
                    }
                    events->textInputEvent(window, str, XLookupKeysym(&g_incoming_event.xkey, 0));
                }
                else
                {
                    events->keyPressEvent(window, XLookupKeysym(&g_incoming_event.xkey, 0));
                }
                break;
            }

            case KeyRelease:
            {
                events->keyReleaseEvent(window, XLookupKeysym(&g_incoming_event.xkey, 0));
                break;
            }

            case ButtonPress:
            {
                unsigned int button = getEventButton(&g_incoming_event.xbutton);
                if(button != R64FX_MOUSE_BUTTON_NONE)
                {
                    events->mousePressEvent(window, g_incoming_event.xbutton.x, g_incoming_event.xbutton.y, button);
                }
                break;
            }

            case ButtonRelease:
            {
                unsigned int button = getEventButton(&g_incoming_event.xbutton);
                if(button != R64FX_MOUSE_BUTTON_NONE)
                {
                    if(g_outgoing_drag_object)
                    {
                        window->ungrabMouse();
                        events->dndFinished();
                        g_outgoing_drag_object = nullptr;
                        g_drag_anchor_x = 0;
                        g_drag_anchor_y = 0;
                    }
                    else
                    {
                        events->mouseReleaseEvent(window, g_incoming_event.xbutton.x, g_incoming_event.xbutton.y, button);
                    }
                }
                break;
            }

            case MotionNotify:
            {
                if(g_outgoing_drag_object)
                {
                    int dnd_obj_x = g_incoming_event.xmotion.x + window->x() - g_drag_anchor_x;
                    int dnd_obj_y = g_incoming_event.xmotion.y + window->y() - g_drag_anchor_y;
                    g_outgoing_drag_object->setPosition({dnd_obj_x, dnd_obj_y});
                }
                else
                {
                    events->mouseMoveEvent(window, g_incoming_event.xmotion.x, g_incoming_event.xmotion.y);
                }
                break;
            }

            case EnterNotify:
            {
                events->mouseEnterEvent(window);
                break;
            }

            case LeaveNotify:
            {
                events->mouseLeaveEvent(window);
                break;
            }

            case ConfigureNotify:
            {
                window->mx = g_incoming_event.xconfigure.x;
                window->my = g_incoming_event.xconfigure.y;

                int old_w = window->width();
                int old_h = window->height();
                window->updateAttrs();
                int new_w = window->width();
                int new_h = window->height();

                if(old_w != new_w || old_h != new_h)
                {
                    if(window->type() == Window::Type::Image)
                    {
                        auto windowximage = (WindowXImage*) window;
                        windowximage->resizeImage();
                    }
                    events->resizeEvent(window, new_w, new_h);
                }
                break;
            }

            case SelectionClear:
            {
                window->clearSelection(g_incoming_event.xselectionclear);
                break;
            }

            case SelectionRequest:
            {
                window->sendSelection(events);
                break;
            }

            case SelectionNotify:
            {
                window->recieveSelection(g_incoming_event.xselection, events);
                break;
            }

            case ClientMessage:
            {
                auto &msg = g_incoming_event.xclient;

                if(msg.message_type == X11_Atom::XdndPosition)
                {
                    int x, y;
                    get_dnd_position(msg.data.l, x, y);
                    if(!g_incoming_drag)
                    {
                        g_incoming_drag = true;
                        window->updateAttrs();
                        request_all_dnd_positions(xwindow, dnd_source(msg.data.l));
                        events->dndEnterEvent(window, x - window->x(), y - window->y());
                    }
                    else
                    {
                        events->dndMoveEvent(window, x - window->x(), y - window->y());
                    }
                }
                else if(msg.message_type == X11_Atom::XdndEnter)
                {
                    vector<Atom> types;
                    get_dnd_type_list(msg.data.l, types);
                }
                else if(msg.message_type == X11_Atom::XdndLeave)
                {
                    g_incoming_drag = false;
                    events->dndLeaveEvent(window);
                }
                else if(msg.message_type == X11_Atom::XdndDrop)
                {
                    send_dnd_finished(xwindow, dnd_source(msg.data.l), false);
                    g_incoming_drag = false;
                    events->dndDropEvent(window);
                }
                else if(msg.message_type == X11_Atom::XdndFinished)
                {
                    cout << "XdndFinished\n";
                }
                else if(msg.message_type == X11_Atom::WM_PROTOCOLS)
                {
                    events->closeEvent(window);
                }
                break;
            }

            default:
            {
#ifdef R64FX_USE_MITSHM
                if(g_incoming_event.type == g_mitsm_completion_event)
                {
                    cout << "MitShm Completion Event!\n";
                }
                else
#endif//R64FX_USE_MITSHM
                {
//                     cout << "Unknown Event!\n";
                }
                break;
            }
        }
    }
}


void WindowX11::setupEvents()
{
    XSetWMProtocols(g_display, m_xwindow, &X11_Atom::WM_DELETE_WINDOW, 1);

    XSelectInput(
        g_display, m_xwindow,
        KeyPressMask | KeyReleaseMask |
        ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
        EnterWindowMask | LeaveWindowMask |
        StructureNotifyMask
    );

    unsigned int dnd_version = 5;
    XChangeProperty(
        g_display,
        m_xwindow,
        X11_Atom::XdndAware,
        XA_ATOM,
        32,
        PropModeReplace,
        (unsigned char*)&dnd_version,
        1
    );
}
