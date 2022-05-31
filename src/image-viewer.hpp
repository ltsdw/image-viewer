#ifndef IMAGE_VIEWER
#define IMAGE_VIEWER

#include <gdkmm/pixbuf.h>
#include <gtkmm/window.h>
#include <gtkmm/application.h>
#include <gtkmm/alignment.h>
#include <gtkmm/box.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/frame.h>
#include <gtkmm/image.h>
#include <iostream>
#include <string>

class ImageViewer : public Gtk::Window {
    Gtk::Box m_box;
    Gtk::Alignment m_alignment;
    Gtk::Frame m_frame;
    Gtk::Image m_image;

    public:
        ImageViewer();

        void setImage(std::string& filename);

    protected:
        Glib::RefPtr<Gdk::Pixbuf> m_pimage;

};

#endif
