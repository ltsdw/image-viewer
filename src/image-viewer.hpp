#ifndef IMAGE_VIEWER
#define IMAGE_VIEWER

#include <gdkmm/pixbuf.h>
#include <gdkmm/general.h>
#include <gtkmm/window.h>
#include <gtkmm/application.h>
#include <gtkmm/alignment.h>
#include <gtkmm/box.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/frame.h>
#include <gtkmm/image.h>
#include <iostream>
#include <string>

class DrawingArea : public Gtk::DrawingArea 
{
    public:
        DrawingArea(const std::string& filename);
    protected:
        Glib::RefPtr<Gdk::Pixbuf> m_image;

        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

class ImageViewer : public Gtk::Window {
    Gtk::Box m_box;
    Gtk::Alignment m_alignment;
    Gtk::Frame m_frame;

    public:
        ImageViewer();

        void setImage(const std::string& filename);
};

#endif
