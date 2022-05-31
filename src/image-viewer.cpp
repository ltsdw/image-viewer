#include "image-viewer.hpp"

ImageViewer::ImageViewer() : m_box(Gtk::Orientation::ORIENTATION_VERTICAL)
{
    set_title("Image Viewer");
    set_default_size(1366, 768);

    m_box.set_margin_start(10);
    m_box.set_margin_end(10);
    m_box.set_margin_top(10);
    m_box.set_margin_bottom(10);
    
    m_box.set_spacing(10);

    m_box.set_hexpand(true);
    m_box.set_vexpand(true);

    set_resizable();

    //m_box.pack_start(*m_image, Gtk::PACK_SHRINK);

    add(m_box);

    m_frame.set_halign(Gtk::Align::ALIGN_CENTER);
    m_frame.set_valign(Gtk::Align::ALIGN_CENTER);

    m_box.pack_start(m_frame);
}

DrawingArea::DrawingArea(const std::string& filename)
{
    try
    {
        m_image = Gdk::Pixbuf::create_from_file(filename);
    } catch (...)
    {
        std::cerr << "can't load file " << filename << std::endl;
        return;
    }


    m_image = m_image->scale_simple(m_image->get_width() * 0.8, m_image->get_height() * 0.8, Gdk::InterpType::INTERP_BILINEAR);

    scale = 1;

    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::SMOOTH_SCROLL_MASK | Gdk::SMOOTH_SCROLL_MASK);

    if (m_image) set_size_request(m_image->get_width(), m_image->get_height());

    set_halign(Gtk::Align::ALIGN_CENTER);
    set_valign(Gtk::Align::ALIGN_CENTER);

}

bool DrawingArea::on_scroll_event(GdkEventScroll* ev)
{
    scale -= ev->delta_y / 10.0;
 
    if (scale < 0.1) scale = 0.1;

    queue_draw();

    return false;
}

bool DrawingArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    if (m_image)
    {
        Gtk::Allocation allocation = get_allocation();

        const int height = allocation.get_height();
        const int width = allocation.get_width();

        printf("%d - %d\n", height, width);

        cr->scale(scale, scale);

        Gdk::Cairo::set_source_pixbuf(
            cr,
            m_image,
            ((width / 2.0) / scale) - (m_image->get_width() / 2.0),
            ((height / 2.0) / scale) - (m_image->get_height() / 2.0)
        );

        cr->rectangle(0, 0, width / scale, height / scale);

        cr->fill();
    }

    return false;
}

void ImageViewer::setImage(const std::string& filename)
{
    auto m_image = Gtk::manage(new DrawingArea(filename));
    m_frame.add(*m_image);
}
