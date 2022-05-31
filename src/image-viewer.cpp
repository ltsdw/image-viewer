#include "image-viewer.hpp"

ImageViewer::ImageViewer() : m_box(Gtk::Orientation::ORIENTATION_VERTICAL, 10)
{
    set_title("Image Viewer");
    set_default_size(500, 500);

    m_box.set_margin_start(10);
    m_box.set_margin_end(10);
    m_box.set_margin_top(10);
    m_box.set_margin_bottom(10);

    add(m_box);

    m_frame.set_halign(Gtk::Align::ALIGN_CENTER);
    m_frame.set_valign(Gtk::Align::ALIGN_CENTER);

    m_box.add(m_frame);

    m_frame.add(m_image);
}

void ImageViewer::setImage(std::string& filename)
{
    m_image.set(filename);
}
