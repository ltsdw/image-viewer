#include "image-viewer.hpp"

ImageViewer::ImageViewer() : m_box(Gtk::Orientation::ORIENTATION_VERTICAL)
{
    auto m_image = Gtk::manage(new DrawingArea("/tmp/example.png"));

    set_title("Image Viewer");
    set_default_size(500, 500);

    m_box.set_margin_start(10);
    m_box.set_margin_end(10);
    m_box.set_margin_top(10);
    m_box.set_margin_bottom(10);
    
    m_box.set_spacing(10);

    m_box.set_hexpand(true);
    m_box.set_vexpand(true);

    set_resizable();

    m_box.pack_start(*m_image, Gtk::PACK_SHRINK);

    add(m_box);

    //m_frame.set_halign(Gtk::Align::ALIGN_CENTER);
    //m_frame.set_valign(Gtk::Align::ALIGN_CENTER);

    //m_box.pack_start(m_frame);

    //m_frame.add(m_image);
}

DrawingArea::DrawingArea(const std::string& filename)
{
    printf("alowwww!\n");

    try
    {
        m_image = Gdk::Pixbuf::create_from_file(filename);
        printf("sim lixo %s\n", filename.c_str());
    } catch (...)
    {
        printf("lixo\n");
        std::cerr << "can't load file " << filename << std::endl;
    }

    if (m_image)
        set_size_request(m_image->get_width(), m_image->get_height());

    set_halign(Gtk::Align::ALIGN_CENTER);
    set_valign(Gtk::Align::ALIGN_CENTER);

}

bool DrawingArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    printf("calling here\n");

    Gtk::Allocation allocation = get_allocation();
    const int height = allocation.get_height();
    const int width = allocation.get_width();
//
    printf("calling here\n");
    Gdk::Cairo::set_source_pixbuf(cr, m_image, width - m_image->get_width(), height - m_image->get_height());
//
    cr->paint();

    return true;
}

void ImageViewer::setImage(const std::string& filename)
{
    //DrawingArea m_image(filename);

    ;;
    //m_box.pack_start(m_image, Gtk::PACK_SHRINK);
}
