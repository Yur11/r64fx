#include "MainView.hpp"
#include "Painter.hpp"
#include "ImageUtils.hpp"
#include "SystemUtils.hpp"

#include "Widget_Menu.hpp"
#include "Widget_TabBar.hpp"
#include "Widget_ItemBrowser.hpp"
#include "Widget_DirectoryItem.hpp"

#include "ProgramActions.hpp"

#include "IconSupply.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

class TopBar;
class LeftDock;
class RightDock;
class BottomDock;
class MainPart;


IconColors g_colors;

struct MainViewPrivate{
    MainViewEventIface* event  = nullptr;
    
    MainView*    main_view     = nullptr;

    TopBar*      top_bar       = nullptr;
    LeftDock*    left_dock     = nullptr;
    RightDock*   right_dock    = nullptr;
    BottomDock*  bottom_dock   = nullptr;
    MainPart*    main_part     = nullptr;

    Widget_Menu* menu          = nullptr;
    Widget_Menu* menu_session  = nullptr;
    Widget_Menu* menu_project  = nullptr;
    Widget_Menu* menu_edit     = nullptr;
    Widget_Menu* menu_view     = nullptr;
    Widget_Menu* menu_help     = nullptr;
    
    Widget_TabBar* main_tab_bar = nullptr;
    
    int gap = 1;
    
    bool left_dock_expanded    = true;
    bool right_dock_expanded   = true;

    Widget* currently_resized_dock = nullptr;

    Widget* findDockAt(Point<int> p);
    
    Image* icon10 = nullptr;
    Image* icon16 = nullptr;
    Image* icon18 = nullptr;
    Image* icon20 = nullptr;
    Image* icon22 = nullptr;
    Image* icon24 = nullptr;
    Image* icon26 = nullptr;
    Image* icon28 = nullptr;
    Image* icon30 = nullptr;
    Image* icon32 = nullptr;
};

    
class TopBar : public Widget{
    MainViewPrivate* m = nullptr;
    
    
    
public:
    TopBar(MainViewPrivate* m, Widget* parent) : Widget(parent), m(m)
    {
        
    }
    
    virtual ~TopBar()
    {
        
    }
    
protected:
    virtual void paintEvent(WidgetPaintEvent* event)
    {
        auto p = event->painter();
        p->fillRect({0, 0, width(), height()}, Color(127, 127, 127, 0));
        
        Widget::paintEvent(event);
    }

    virtual void resizeEvent(WidgetResizeEvent* event)
    {
        
    }
    
    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        auto window = Widget::rootWindow();
        if(window)
        {
            window->setCursorType(Window::CursorType::Arrow);
        }
    }
};


class LeftDock : public Widget{
    MainViewPrivate* m = nullptr;
    Widget_ItemBrowser* m_browser = nullptr;
    
public:
    LeftDock(MainViewPrivate* m, Widget* parent) : Widget(parent), m(m)
    {
        std::string name = "home";
        auto path = home_dir();
        if(path.empty())
        {
            path = "/";
            name = "/";
        }
        else if(path.back() != '/')
        {
            path.push_back('/');
        }
        
        auto wd = new Widget_DirectoryItem(name, path);
        
        m_browser = new Widget_ItemBrowser(this);
        m_browser->addItem(wd);
    }
    
    virtual ~LeftDock()
    {
        delete m_browser;
    }
    
protected:
    virtual void paintEvent(WidgetPaintEvent* event)
    {
        auto p = event->painter();
        p->fillRect({0, 0, width(), height()}, Color(127, 127, 127, 0));
        
        Widget::paintEvent(event);
    }

    virtual void resizeEvent(WidgetResizeEvent* event)
    {
        m_browser->setSize(event->size());
    }
    
    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        auto window = Widget::rootWindow();
        if(window)
        {
            window->setCursorType(Window::CursorType::Arrow);
        }
    }
};


class RightDock : public Widget{
    MainViewPrivate* m = nullptr;
    
public:
    RightDock(MainViewPrivate* m, Widget* parent) : Widget(parent), m(m)
    {
        
    }
    
protected:
    virtual void paintEvent(WidgetPaintEvent* event)
    {
        auto p = event->painter();
        p->fillRect({0, 0, width(), height()}, Color(127, 127, 127, 0));
        
        p->putImage(m->icon10, {10, 10});
        p->putImage(m->icon16, {10, 24});
        p->putImage(m->icon18, {10, 45});
        p->putImage(m->icon20, {10, 68});
        p->putImage(m->icon22, {10, 93});
        p->putImage(m->icon24, {10, 120});
        p->putImage(m->icon26, {10, 149});
        p->putImage(m->icon28, {10, 180});
        p->putImage(m->icon30, {10, 214});
        p->putImage(m->icon32, {10, 250});
    }

    virtual void resizeEvent(WidgetResizeEvent* event)
    {
        
    }
    
    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        auto window = Widget::rootWindow();
        if(window)
        {
            window->setCursorType(Window::CursorType::Arrow);
        }
    }
};


class BottomDock : public Widget{
    MainViewPrivate* m = nullptr;
    
public:
    BottomDock(MainViewPrivate* m, Widget* parent) : Widget(parent), m(m)
    {
        
    }
    
protected:
    virtual void paintEvent(WidgetPaintEvent* event)
    {
        auto p = event->painter();
        p->fillRect({0, 0, width(), height()}, Color(127, 127, 127, 0));
    }

    virtual void resizeEvent(WidgetResizeEvent* event)
    {
        
    }
    
    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        auto window = Widget::rootWindow();
        if(window)
        {
            window->setCursorType(Window::CursorType::Arrow);
        }
    }
};


class MainPart : public Widget{
    MainViewPrivate* m = nullptr;
    
public:
    MainPart(MainViewPrivate* m, Widget* parent) : Widget(parent), m(m)
    {
        
    }

protected:
    virtual void paintEvent(WidgetPaintEvent* event)
    {
//         auto p = event->painter();
//         p->fillRect({0, 0, width(), height()}, Color(127, 127, 127, 0));
        Widget::paintEvent(event);
    }

    virtual void resizeEvent(WidgetResizeEvent* event)
    {
        for(auto child : *this)
        {
            child->setSize(event->size());
            break;
        }
    }
    
    virtual void mouseMoveEvent(MouseMoveEvent* event)
    {
        auto window = Widget::rootWindow();
        if(window)
        {
            window->setCursorType(Window::CursorType::Arrow);
        }
    }
};

        
MainView::MainView(MainViewEventIface* event_iface, Widget* parent) : Widget(parent)
{
    m = new MainViewPrivate;
    m->event = event_iface;
    m->main_view = this;
    
    m->top_bar      = new TopBar      (m, this);
    m->left_dock    = new LeftDock    (m, this);
    m->right_dock   = new RightDock   (m, this);
    m->bottom_dock  = new BottomDock  (m, this);
    m->main_part    = new MainPart    (m, this);

    m->menu_session = new Widget_Menu(m->menu);
    m->menu_session->setOrientation(Orientation::Vertical);
    m->menu_session->addAction(g_acts->new_session_act);
    m->menu_session->addAction(g_acts->open_session_act);
    m->menu_session->addAction(g_acts->save_session_act);
    m->menu_session->addAction(g_acts->save_session_as_act);
    m->menu_session->addAction(g_acts->quit_act);
    m->menu_session->resizeAndRealign();
    
    m->menu_project = new Widget_Menu(m->menu);
    m->menu_project->setOrientation(Orientation::Vertical);
    m->menu_project->addAction(g_acts->new_project_act);
    m->menu_project->addAction(g_acts->open_project_act);
    m->menu_project->addAction(g_acts->save_project_act);
    m->menu_project->addAction(g_acts->save_project_as_act);
    m->menu_project->addAction(g_acts->create_player_act);
    m->menu_project->addAction(g_acts->close_project_act);
    m->menu_project->resizeAndRealign();
    
    m->menu_edit = new Widget_Menu(m->menu);
    m->menu_edit->setOrientation(Orientation::Vertical);
    m->menu_edit->addAction(g_acts->cut_act);
    m->menu_edit->addAction(g_acts->copy_act);
    m->menu_edit->addAction(g_acts->paste_act);
    m->menu_edit->addAction(g_acts->undo_act);
    m->menu_edit->addAction(g_acts->redo_act);
    m->menu_edit->resizeAndRealign();
    
    m->menu_view = new Widget_Menu(m->menu);
    m->menu_view->setOrientation(Orientation::Vertical);
    m->menu_view->addAction(g_acts->no_view_act);
    m->menu_view->resizeAndRealign();
    
    m->menu_help = new Widget_Menu(m->menu);
    m->menu_help->setOrientation(Orientation::Vertical);
    m->menu_help->addAction(g_acts->no_help_act);
    m->menu_help->resizeAndRealign();
    
    m->menu = new Widget_Menu(m->top_bar);
    m->menu->setOrientation(Orientation::Horizontal);
    m->menu->addSubMenu(m->menu_session,  "Session");
    m->menu->addSubMenu(m->menu_project,  "Project");
    m->menu->addSubMenu(m->menu_edit,     "Edit");
    m->menu->addSubMenu(m->menu_view,     "View");
    m->menu->addSubMenu(m->menu_help,     "Help");
    m->menu->resizeAndRealign();
    m->menu->setPosition({0, 0});

    m->main_tab_bar = new Widget_TabBar(m->top_bar);
    m->main_tab_bar->resizeAndRealign();
    m->main_tab_bar->setY(0);
    m->main_tab_bar->onTabSelected([](TabHandle* handle, void* payload, void* arg){
        auto m = (MainViewPrivate*)arg;
        m->event->mainPartOptionSelected(payload);
    }, m);
    
    m->top_bar->setHeight(m->menu->height());
    
    m->left_dock->setWidth(300);
    m->right_dock->setWidth(100);
    m->bottom_dock->setHeight(256);
    
    g_colors.stroke1 = Color(31, 31, 31, 0);
    g_colors.fill1   = Color(127, 127, 127, 0);
    g_colors.stroke2 = Color(63, 63, 63, 0);
    g_colors.fill2   = Color(223, 223, 223, 0);
    
    auto name = IconName::DoublePage;
    m->icon10 = get_icon(name, 10, &g_colors);
    m->icon16 = get_icon(name, 16, &g_colors);
    m->icon18 = get_icon(name, 18, &g_colors);
    m->icon20 = get_icon(name, 20, &g_colors);
    m->icon22 = get_icon(name, 22, &g_colors);
    m->icon24 = get_icon(name, 24, &g_colors);
    m->icon26 = get_icon(name, 26, &g_colors);
    m->icon28 = get_icon(name, 28, &g_colors);
    m->icon30 = get_icon(name, 30, &g_colors);
    m->icon32 = get_icon(name, 32, &g_colors);
    
    setSize({800, 600});
}


MainView::~MainView()
{
    free_icon(m->icon10);
    free_icon(m->icon16);
    free_icon(m->icon18);
    free_icon(m->icon20);
    free_icon(m->icon22);
    free_icon(m->icon24);
    free_icon(m->icon28);
    free_icon(m->icon30);
    free_icon(m->icon32);
    
    m->main_tab_bar->setParent(nullptr);
    delete m->main_tab_bar;
    
    m->menu_session->setParent(nullptr);
    delete m->menu_session;
    
    m->menu_project->setParent(nullptr);
    delete m->menu_project;
    
    m->menu_edit->setParent(nullptr);
    delete m->menu_edit;
    
    m->menu_view->setParent(nullptr);
    delete m->menu_view;
    
    m->menu_help->setParent(nullptr);
    delete m->menu_help;
}


void MainView::addMainPartOption(void* option, const std::string &name)
{
    m->main_tab_bar->addTab(option, name, IconName::Page);
    m->main_tab_bar->resizeAndRealign();
    setSize(size());
}


void MainView::setMainPartWidget(Widget* widget)
{
    widget->setParent(m->main_part);
}


void MainView::paintEvent(WidgetPaintEvent* event)
{
    Widget::paintEvent(event);
    
    auto p = event->painter();
    
    p->fillRect(
        {0, m->top_bar->height(), width(), m->gap}, 
        Color(0, 0, 0, 0)
    );
    
    if(m->left_dock->parent() == this)
    {
        p->fillRect(
            {m->left_dock->width(), m->left_dock->y(), m->gap, m->left_dock->height()},
            Color(0, 0, 0, 0)
        );
    }

    if(m->right_dock->parent() == this)
    {
        p->fillRect(
            {m->right_dock->x() - m->gap, m->right_dock->y(), m->gap, m->right_dock->height()},
            Color(0, 0, 0, 0)
        );
    }
    
    if(m->bottom_dock->parent() == this)
    {
        p->fillRect(
            {m->bottom_dock->x(), m->bottom_dock->y() - m->gap, m->bottom_dock->width(), m->gap},
            Color(0, 0, 0, 0)
        );
    }
    
    {
        int x = m->main_tab_bar->x() + m->main_tab_bar->currentTabX();
        int w = m->main_tab_bar->currentTabWidth();
        
        p->fillRect(
            {x, m->top_bar->height(), w, m->gap},
            Color(191, 191, 191, 0)
        );
    }
}


void MainView::resizeEvent(WidgetResizeEvent* event)
{
    m->top_bar->setWidth(event->width());
    m->top_bar->setPosition({0, 0});
    
    m->main_part->setY(m->top_bar->height() + m->gap);
    
    if(m->bottom_dock->parent() == this)
    {
        m->bottom_dock->setY(event->height() - m->bottom_dock->height());
        
        if(m->left_dock->parent() == this && m->left_dock_expanded)
        {
            m->bottom_dock->setX(m->left_dock->width() + m->gap);
        }
        else
        {
            m->bottom_dock->setX(0);
        }
        
        if(m->right_dock->parent() == this && m->right_dock_expanded)
        {
            m->bottom_dock->setWidth(event->width() - m->bottom_dock->x() - m->right_dock->width() - m->gap);
        }
        else
        {
            m->bottom_dock->setWidth(event->width() - m->bottom_dock->x());
        }
        
        m->main_part->setHeight(
            event->height() - m->top_bar->height() - m->bottom_dock->height() - m->gap - m->gap
        );
    }
    else
    {
        m->main_part->setHeight(
            event->height() - m->top_bar->height() - m->gap
        );
    }
    
    if(m->left_dock->parent() == this)
    {
        m->left_dock->setX(0);
        m->left_dock->setY(m->top_bar->height() + m->gap);
        
        if(m->bottom_dock->parent() == this && !m->left_dock_expanded)
        {
            m->left_dock->setHeight(event->height() - m->left_dock->y() - m->bottom_dock->height());
        }
        else
        {
            m->left_dock->setHeight(event->height() - m->left_dock->y());
        }
        
        m->main_part->setX(m->left_dock->width() + m->gap);
    }
    else
    {
        m->main_part->setX(0);
    }
    
    if(m->right_dock->parent() == this)
    {
        m->right_dock->setX(event->width() - m->right_dock->width());
        m->right_dock->setY(m->top_bar->height() + m->gap);
        
        if(m->bottom_dock->parent() == this && !m->right_dock_expanded)
        {
            m->right_dock->setHeight(event->height() - m->right_dock->y() - m->bottom_dock->height());
        }
        else
        {
            m->right_dock->setHeight(event->height() - m->right_dock->y());
        }
        
        if(m->left_dock->parent() == this)
        {
            m->main_part->setWidth(
                event->width() - m->left_dock->width() - m->right_dock->width() - m->gap - m->gap
            );
        }
        else
        {
            m->main_part->setWidth(
                event->width() - m->right_dock->width() - m->gap
            );
        }
    }
    else
    {
        if(m->left_dock->parent() == this)
        {
            m->main_part->setWidth(
                event->width() - m->left_dock->width() - m->gap
            );
        }
        else
        {
            m->main_part->setWidth(event->width());
        }
    }
    
    m->main_tab_bar->setX(
        (m->main_part->x() + m->main_part->width()) - m->main_tab_bar->width() + 1
    );
}


void MainView::mousePressEvent(MousePressEvent* event)
{
    if(!childrenMousePressEvent(event))
    {
        if(event->button() == MouseButton::Left())
        {
            auto dock = m->findDockAt(event->position());
            if(dock)
            {
                m->currently_resized_dock = dock;
                grabMouseFocus();
            }
        }
    }
}


void MainView::mouseReleaseEvent(MouseReleaseEvent* event)
{
    if(isMouseFocusOwner())
    {
        releaseMouseFocus();
        m->currently_resized_dock = nullptr;
    }
    else
    {
        childrenMouseReleaseEvent(event);
    }
}


void MainView::mouseMoveEvent(MouseMoveEvent* event)
{
    if(m->currently_resized_dock && event->button() == MouseButton::Left())
    {
        if(m->currently_resized_dock == m->left_dock)
        {
            int new_width = event->x();
            int new_main_part_width = width() - (new_width + m->gap);
            if(m->right_dock->parent() == this)
            {
                new_main_part_width -= (m->right_dock->width() + m->gap);
            }

            if(new_main_part_width >= 10)
            {
                m->left_dock->setWidth(new_width);
                setSize(size());
                repaint();
            }
        }
        else if(m->currently_resized_dock == m->bottom_dock)
        {
            int new_height = height() - event->y();
            int new_main_part_height = height() - new_height - m->top_bar->height() - m->gap - m->gap;
            if(new_main_part_height >= 10)
            {
                m->bottom_dock->setHeight(new_height);
                m->bottom_dock->setY(event->y());
                setSize(size());
                repaint();
            }
        }
        else if(m->currently_resized_dock == m->right_dock)
        {
            int new_width = width() - event->x();
            int new_main_part_width = width() - (new_width + m->gap);
            if(m->left_dock->parent() == this)
            {
                new_main_part_width -= (m->left_dock->width() + m->gap);
            }
            
            if(new_main_part_width >= 10)
            {
                m->right_dock->setWidth(new_width);
                setSize(size());
                repaint();
            }
        }

        return;
    }

    auto window = Widget::rootWindow();
    if(!window)
    {
        return;
    }

    auto dock = m->findDockAt(event->position());
    if(dock)
    {
        window->setCursorType(
            (dock == m->bottom_dock) ? Window::CursorType::ResizeNS : Window::CursorType::ResizeWE
        );
    }
}


Widget* MainViewPrivate::findDockAt(Point<int> p)
{
    if(p.y() <= (top_bar->height() + gap))
    {
        return nullptr;
    }

    int left = (
        (left_dock->parent() == main_view && left_dock_expanded) ? left_dock->width() + gap : 0
    );

    int right = (
        (right_dock->parent() == main_view && right_dock_expanded) ? main_part->x() + main_part->width() : main_view->width()
    );

    if(p.x() < left)
    {
        return left_dock;
    }
    else if(p.x() < right)
    {
        if(bottom_dock->parent() == main_view && p.y() < bottom_dock->y())
        {
            return bottom_dock;
        }
    }
    else
    {
        return right_dock;
    }

    return nullptr;
}


void MainView::closeEvent()
{
    g_acts->quit_act->exec();
}
    
}//namespace r64fx
