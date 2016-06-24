/* To be included in WindowX11.cpp */

void WindowX11::processSomeEvents(WindowEventDispatcherIface* events)
{
    g_events = events;

    while(XPending(g_display))
    {
        XEvent xevent;
        g_incoming_event = &xevent;

        XNextEvent(g_display, &xevent);
        auto xwindow = xevent.xany.window;

        WindowX11* window = getWindowFromXWindow(xwindow);
        if(!window)
        {
            continue;
        }

        if(window->doingTextInput() && XFilterEvent(&xevent, xevent.xany.window))
        {
            continue;
        }

        switch(xevent.type)
        {
            case KeyPress:
            {
                if(window->doingTextInput())
                {
                    Status status;
                    char buff[8];
                    KeySym keysym;
                    int nbytes = Xutf8LookupString(
                        window->inputContext(), &xevent.xkey,
                        buff, 8, &keysym, &status
                    );

                    string str = "";
                    if(nbytes)
                    {
                        str = string(buff, nbytes);
                    }
                    events->textInputEvent(window, str, XLookupKeysym(&xevent.xkey, 0));
                }
                else
                {
                    events->keyPressEvent(window, XLookupKeysym(&xevent.xkey, 0));
                }
                break;
            }

            case KeyRelease:
            {
                events->keyReleaseEvent(window, XLookupKeysym(&xevent.xkey, 0));
                break;
            }

            case ButtonPress:
            {
                unsigned int button = getEventButton(&xevent.xbutton);
                if(button != R64FX_MOUSE_BUTTON_NONE)
                {
                    events->mousePressEvent(window, xevent.xbutton.x, xevent.xbutton.y, button);
                }
                break;
            }

            case ButtonRelease:
            {
                unsigned int button = getEventButton(&xevent.xbutton);
                if(button != R64FX_MOUSE_BUTTON_NONE)
                {
                    if(g_outgoing_drag_object)
                    {
                        window->ungrabMouse();
                        events->dndFinishedEvent();
                        g_outgoing_drag_object = nullptr;
                        g_drag_anchor_x = 0;
                        g_drag_anchor_y = 0;
                    }
                    else
                    {
                        events->mouseReleaseEvent(window, xevent.xbutton.x, xevent.xbutton.y, button);
                    }
                }
                break;
            }

            case MotionNotify:
            {
                if(g_outgoing_drag_object)
                {
                    window->dndMove(xevent.xmotion.x, xevent.xmotion.y);
                }
                else
                {
                    events->mouseMoveEvent(window, xevent.xmotion.x, xevent.xmotion.y);
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
                window->mx = xevent.xconfigure.x;
                window->my = xevent.xconfigure.y;

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
                window->selectionClearEvent();
                break;
            }

            case SelectionRequest:
            {
                window->selectionRequestEvent();
                break;
            }

            case SelectionNotify:
            {
                window->selectionNotifyEvent();
                break;
            }

            case ClientMessage:
            {
                auto &msg = xevent.xclient;

                if(msg.message_type == X11_Atom::XdndPosition)
                {
                    window->xdndPositionEvent();
                }
                else if(msg.message_type == X11_Atom::XdndEnter)
                {
                    window->xdndEnterEvent();
                }
                else if(msg.message_type == X11_Atom::XdndLeave)
                {
                    window->xdndLeaveEvent();
                }
                else if(msg.message_type == X11_Atom::XdndDrop)
                {
                    window->xdndDropEvent();
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
                if(xevent.type == g_mitsm_completion_event)
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
        }//switch

        g_incoming_event = nullptr;
    }//while

    g_events = nullptr;
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
}
