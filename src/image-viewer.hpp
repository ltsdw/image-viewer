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
#include <gtkmm/viewport.h>
#include <filesystem>
#include <vector>
#include <iostream>
#include <string>


class FileHandler
{
    public:
        FileHandler(int argc, char** argv);

        std::string next();
        std::string previous();

        void removeFile();

    private:
        std::vector<std::string> files;

        int idx;
};

class DrawingArea : public Gtk::DrawingArea
{
    public:
        DrawingArea(int argc, char** argv, Glib::RefPtr<Gtk::Application> app);

        void setImage(const std::string& filename);

    private:
        double scale;
        double img_focus_x, img_focus_y;
        double last_x_mouse, last_y_mouse;
        bool move_flag;
        bool reset_flag;

        Glib::RefPtr<Gdk::Pixbuf> m_image;
        Glib::RefPtr<Gtk::Application> m_app;

        FileHandler filehandler;

        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
        bool on_scroll_event(GdkEventScroll* ev) override;
        bool on_button_press_event(GdkEventButton* ev) override;
        bool on_button_release_event(GdkEventButton* ev) override;
        bool on_key_press_event(GdkEventKey* ev) override;
        bool on_motion_notify_event(GdkEventMotion* ev) override;

        void fitImage(const int w_width, const int w_height, const int i_width, const int i_height);
        void fit();
        void reset();
};

class ImageViewer : public Gtk::Window {
    public:
        ImageViewer(Glib::RefPtr<Gtk::Application> app);

        Gtk::Box m_box;

        void addToBox(int argc, char** argv);

    private:
        Glib::RefPtr<Gtk::Application> m_app;
};

#endif
