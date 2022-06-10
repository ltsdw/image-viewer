#include "image-viewer.hpp"

ImageViewer::ImageViewer(Glib::RefPtr<Gtk::Application> app) : m_box(Gtk::Orientation::ORIENTATION_VERTICAL), m_app{app}
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
    DrawingArea* drawingarea = Gtk::manage(new DrawingArea(argc, argv, m_app));

    m_box.pack_start(*drawingarea);
}

DrawingArea::DrawingArea(int argc, char** argv, Glib::RefPtr<Gtk::Application> app) : m_app{app}, filehandler{argc, argv}
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

    setImage(filehandler.next());

    set_can_focus();

    fit();

    move_flag = false;
}

bool DrawingArea::on_scroll_event(GdkEventScroll* ev)
{
    double new_scale = scale * (1 - ev->delta_y / 10.0);

    if (new_scale < 0.1) new_scale = 0.1;

    const double delta_x = ev->x - get_allocated_width() / 2.0;
    const double delta_y = ev->y - get_allocated_height() / 2.0;

    img_focus_x = img_focus_x + delta_x / scale - delta_x / new_scale;
    img_focus_y = img_focus_y + delta_y / scale - delta_y / new_scale;

    scale = new_scale;

    queue_draw();

    return true;
}

bool DrawingArea::on_button_press_event(GdkEventButton* ev)
{
    if (ev->button == 1)
    {
        last_x_mouse = ev->x;
        last_y_mouse = ev->y;

        move_flag = true;

        return true;
    }

    return false;
}

bool DrawingArea::on_button_release_event(GdkEventButton* ev)
{
    if (ev->button == 1 && move_flag)
    {
        move_flag = false;

        img_focus_x -= (ev->x - last_x_mouse) / scale;
        img_focus_y -= (ev->y - last_y_mouse) / scale;

        queue_draw();

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
            setImage(filehandler.previous());
            
            break;
        case(GDK_KEY_Right):
            setImage(filehandler.next());

            break;
        case(GDK_KEY_x):
            filehandler.removeFile();

            setImage(filehandler.next());

            break;
        case(GDK_KEY_Escape):
            m_app->get_active_window()->hide();
    
            break;
        case(GDK_KEY_q):
            m_app->get_active_window()->hide();

            break;
    }

    return true;
}

bool DrawingArea::on_motion_notify_event(GdkEventMotion* ev)
{
    if (move_flag)
    {
        const int x_mouse = ev->x;
        const int y_mouse = ev->y;

        img_focus_x -= (x_mouse - last_x_mouse) / scale;
        img_focus_y -= (y_mouse - last_y_mouse) / scale;

        last_x_mouse = x_mouse;
        last_y_mouse = y_mouse;

        queue_draw();

        return true;
    }

    return false;
}

void DrawingArea::reset()
{
    scale = 1;

    img_focus_x = m_image->get_width() / 2.0;
    img_focus_y = m_image->get_height() / 2.0;

    queue_draw();
}

void DrawingArea::fit()
{
    reset_flag = true;

    queue_draw();
}

void DrawingArea::fitImage( int w_width,  int w_height, int i_width,  int i_height)
{
    const double w_ratio = (double) w_width / w_height;
    const double i_ratio = (double) i_width / i_height;

    if (i_ratio < w_ratio) scale = (double) w_height / i_height;
    else scale = (double) w_width / i_width;

    img_focus_x = i_width / 2.0;
    img_focus_y = i_height / 2.0;

    reset_flag = false;
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

        if (reset_flag) fitImage(w_width, w_height, i_width, i_height);

        Glib::RefPtr<Gdk::Pixbuf> display = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, has_alpha, 8, w_width, w_height);

        const double offset_x = (double)w_width / 2.0 - img_focus_x * scale;
        const double offset_y = (double)w_height / 2.0 - img_focus_y * scale;

        const double min_x = std::max(0.0, offset_x);
        const double min_y = std::max(0.0, offset_y);
        const double max_x = std::min((double)w_width, (double)w_width / 2 + (i_width - img_focus_x) * scale);
        const double max_y = std::min((double)w_height, (double)w_height / 2 + (i_height - img_focus_y) * scale);

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
            scale,
            scale,
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

            queue_draw();
        }

    } catch (const Glib::Error& e)
    {
        std::cerr << e.what() << std::endl;

        filehandler.removeFile(false);

        setImage(filehandler.next());

        return;
    }
}

FileHandler::FileHandler(int argc, char** argv) : idx{-1}
{
    for (int i = 1; i < argc; i++)
    {
        if (std::filesystem::is_directory(argv[i]))
        {
            for (const std::filesystem::directory_entry& dir_entry : std::filesystem::recursive_directory_iterator(argv[i]))
            {
                if (std::filesystem::is_regular_file(dir_entry)) files.push_back(dir_entry.path());
            }
        } else
        {
            files.push_back(argv[i]);
        }
    }

    sort(files.begin(), files.end());

}

std::string FileHandler::next()
{
    if (not files.size()) return "";

    if (++idx > files.size() - 1) idx = 0;

    return files.at(idx);
}

std::string FileHandler::previous()
{
    if (not files.size()) return "";

    if (--idx < 0) idx = files.size() - 1;

    return files.at(idx);
}

void FileHandler::removeFile(const bool delete_file)
{
    if (not files.size()) return;

    const int tmp_idx = idx;

    if (delete_file) {
        try { std::filesystem::remove(files.at(idx)); }
        catch(const std::filesystem::filesystem_error& e) { std::cerr << e.what() << std::endl; }
    }

    --idx;

    files.erase(files.begin() + tmp_idx);
}

void helper()
{
    std::cerr
    << "image-viwer version: " << VERSION
    << std::endl
    << "Usage: ./image-viwer filenames"
    << std::endl;

    exit(EXIT_FAILURE);
}
