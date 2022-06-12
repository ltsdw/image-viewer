#include "image-viewer.hpp"


ImageViewer::ImageViewer() : m_box(Gtk::Orientation::ORIENTATION_VERTICAL)
{
    set_title("Image Viewer");
    set_default_size(500, 500);

    m_box.set_margin_start(10);
    m_box.set_margin_end(10);
    m_box.set_margin_top(10);
    m_box.set_margin_bottom(10);
    m_box.set_spacing(10);

    m_box.set_hexpand(true);
    m_box.set_vexpand(true);

    add(m_box);
}

void ImageViewer::addToBox(int argc, char** argv) {
    DrawingArea* drawingarea = Gtk::manage(new DrawingArea(argc, argv));

    m_box.pack_start(*drawingarea);

    drawingarea->hasParentWindow();
}

DrawingArea::DrawingArea(int argc, char** argv) :
    m_move_flag(false),
    m_is_click_released(true),
    m_is_click_held(false),
    m_filehandler(argc, argv)
{
    add_events(
            Gdk::BUTTON_PRESS_MASK |
            Gdk::BUTTON_RELEASE_MASK |
            Gdk::SCROLL_MASK |
            Gdk::SMOOTH_SCROLL_MASK |
            Gdk::KEY_PRESS_MASK |
            Gdk::KEY_RELEASE_MASK |
            Gdk::POINTER_MOTION_MASK
    );

    set_can_focus();
}

bool DrawingArea::on_scroll_event(GdkEventScroll* ev)
{
    double new_scale = m_scale * (1 - ev->delta_y / 10.0);

    if (new_scale < 0.1) new_scale = 0.1;

    const double delta_x = ev->x - get_allocated_width() / 2.0;
    const double delta_y = ev->y - get_allocated_height() / 2.0;

    m_img_focus_x = m_img_focus_x + delta_x / m_scale - delta_x / new_scale;
    m_img_focus_y = m_img_focus_y + delta_y / m_scale - delta_y / new_scale;

    m_scale = new_scale;

    queue_draw();

    return true;
}

bool DrawingArea::on_button_press_event(GdkEventButton* ev)
{
    if (ev->button == 1)
    {
        m_is_click_released = false;

        const sigc::slot<bool ()> slot = sigc::bind(
            sigc::mem_fun(*this, &DrawingArea::onTimeoutSetClickHold),
            ev->x,
            ev->y
        );

        m_conn = Glib::signal_timeout().connect(slot, 180);

        return true;
    }

    return false;
}

bool DrawingArea::on_button_release_event(GdkEventButton* ev)
{
    if (ev->button == 1 and m_move_flag and m_is_click_held)
    {
        m_move_flag = false;
        m_is_click_held = false;
        m_is_click_released = true;

        if (m_conn.connected()) m_conn.disconnect();

        m_img_focus_x -= (ev->x - m_last_x_mouse) / m_scale;
        m_img_focus_y -= (ev->y - m_last_y_mouse) / m_scale;

        queue_draw();

        return true;
    } else if (ev->button == 1) 
    {
        m_is_click_held = false;
        m_is_click_released = true;

        if (m_conn.connected()) m_conn.disconnect();

        setImage(m_filehandler.next());

        return true;
    }

    return false;
}

bool DrawingArea::on_key_press_event(GdkEventKey* ev)
{
    switch(ev->keyval)
    {
        case(GDK_KEY_space):
            DrawingArea::reset();

            break;
        case(GDK_KEY_Left):
            setImage(m_filehandler.previous());
            
            break;
        case(GDK_KEY_Right):
            setImage(m_filehandler.next());

            break;
        case(GDK_KEY_x):
            m_filehandler.removeFile(true);

            setImage(m_filehandler.next());

            // certifies that after the last image is removed
            // it will redraw, otherwise the image would still be displayed
            queue_draw();

            break;
        case(GDK_KEY_Escape):
            m_parent_window->hide();

            break;
        case(GDK_KEY_q):
            m_parent_window->hide();

            break;
    }

    return true;
}

bool DrawingArea::on_motion_notify_event(GdkEventMotion* ev)
{
    if (m_move_flag and m_is_click_held)
    {
        const int x_mouse = ev->x;
        const int y_mouse = ev->y;

        m_img_focus_x -= (x_mouse - m_last_x_mouse) / m_scale;
        m_img_focus_y -= (y_mouse - m_last_y_mouse) / m_scale;

        m_last_x_mouse = x_mouse;
        m_last_y_mouse = y_mouse;

        queue_draw();

        return true;
    }

    return false;
}

bool DrawingArea::onTimeoutSetClickHold(const double x_mouse, const double y_mouse)
{
    if (not m_is_click_released) {
        m_last_x_mouse = x_mouse;
        m_last_y_mouse = y_mouse;

        m_move_flag = true;

        m_is_click_held = true;
    }

    return false;
}

void DrawingArea::reset()
{
    m_scale = 1;

    m_img_focus_x = m_image->get_width() / 2.0;
    m_img_focus_y = m_image->get_height() / 2.0;

    queue_draw();
}

void DrawingArea::fit()
{
    m_reset_flag = true;

    queue_draw();
}

void DrawingArea::fitImage( int w_width,  int w_height, int i_width,  int i_height)
{
    const double w_ratio = (double) w_width / w_height;
    const double i_ratio = (double) i_width / i_height;

    if (i_ratio < w_ratio) m_scale = (double) w_height / i_height;
    else m_scale = (double) w_width / i_width;

    m_img_focus_x = i_width / 2.0;
    m_img_focus_y = i_height / 2.0;

    m_reset_flag = false;
}

bool DrawingArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    if (m_image)
    {
        const int w_width = get_allocated_width();
        const int w_height = get_allocated_height();
        const int i_width = m_image->get_width();
        const int i_height = m_image->get_height();

        const bool has_alpha = m_image->get_has_alpha();

        if (m_reset_flag) fitImage(w_width, w_height, i_width, i_height);

        Glib::RefPtr<Gdk::Pixbuf> display = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, has_alpha, 8, w_width, w_height);

        const double offset_x = (double)w_width / 2.0 - m_img_focus_x * m_scale;
        const double offset_y = (double)w_height / 2.0 - m_img_focus_y * m_scale;

        const double min_x = std::max(0.0, offset_x);
        const double min_y = std::max(0.0, offset_y);
        const double max_x = std::min((double)w_width, (double)w_width / 2 + (i_width - m_img_focus_x) * m_scale);
        const double max_y = std::min((double)w_height, (double)w_height / 2 + (i_height - m_img_focus_y) * m_scale);

        const double width = max_x - min_x;
        const double height = max_y - min_y;

        m_image->scale(
            display,
            min_x,
            min_y,
            width,
            height,
            offset_x,
            offset_y,
            m_scale,
            m_scale,
            Gdk::INTERP_TILES
        );

        Gdk::Cairo::set_source_pixbuf(cr, display, 0, 0);

        cr->rectangle(0, 0, w_width, w_height);

        cr->fill();
    }

    return false;
}

void DrawingArea::setImage(const std::string& filename)
{
    try
    {
        if (m_image) m_image.reset();

        if (not filename.empty()) 
        {
            m_image = Gdk::Pixbuf::create_from_file(filename);

            setTitleForParentWindow(filename);

            fit();
        } else
        {
            setTitleForParentWindow("");
        }

    } catch (const Glib::Error& e)
    {
        std::cerr << e.what() << std::endl;

        m_filehandler.removeFile();

        setImage(m_filehandler.next());

        return;
    }
}

void DrawingArea::setTitleForParentWindow(const std::string& filename)
{
    if (not filename.empty()) {
        const std::string title =
        {
            std::to_string(m_filehandler.getCurrentIdx() + 1)   +
            " of "                                              +
            std::to_string(m_filehandler.getTotalSize())        +
            " - "                                               +
            filename
        };

        if (m_parent_window) m_parent_window->set_title(title);

        return;
    }

    m_parent_window->set_title("Image Viewer");
}

void DrawingArea::hasParentWindow()
{
    m_parent_window = dynamic_cast<Gtk::Window*>(this->get_toplevel());

    assert (m_parent_window && "Null pointer to parent window!");

    setImage(m_filehandler.next());
}

FileHandler::FileHandler(int argc, char** argv) : m_idx{-1}
{
    for (int i = 1; i < argc; i++)
    {
        if (std::filesystem::is_directory(argv[i]))
        {
            for (const std::filesystem::directory_entry& dir_entry : std::filesystem::recursive_directory_iterator(argv[i]))
            {
                if (std::filesystem::is_regular_file(dir_entry)) m_files.push_back(dir_entry.path());
            }
        } else
        {
            m_files.push_back(argv[i]);
        }
    }

    sort(m_files.begin(), m_files.end());

}

std::string FileHandler::next()
{
    if (not m_files.size()) return "";

    if (++m_idx > m_files.size() - 1) m_idx = 0;

    return m_files.at(m_idx);
}

std::string FileHandler::previous()
{
    if (not m_files.size()) return "";

    if (--m_idx < 0) m_idx = m_files.size() - 1;

    return m_files.at(m_idx);
}

void FileHandler::removeFile(const bool delete_file)
{
    if (not m_files.size()) return;

    if (delete_file) 
    {
        try { std::filesystem::remove(m_files.at(m_idx)); }
        catch(const std::filesystem::filesystem_error& e) { std::cerr << e.what() << std::endl; }
    }

    m_files.erase(m_files.begin() + m_idx);

    --m_idx;
}

int FileHandler::getCurrentIdx() { return m_idx; }

int FileHandler::getTotalSize() { return m_files.size(); }

void helper()
{
    std::cerr
    << "image-viwer version: " << VERSION
    << std::endl
    << "Usage: ./image-viwer filenames"
    << std::endl;

    exit(EXIT_FAILURE);
}
