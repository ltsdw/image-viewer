#ifndef IMAGE_VIEWER
#define IMAGE_VIEWER


#include <assert.h>
#include <glibmm/main.h>
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


#define VERSION "0.0.1"


class FileHandler
{
    public:
        FileHandler(int argc, char** argv);

        std::string next();
        std::string previous();

        /*!
         * removeFile
         *
         * @param delete_file: if true will actually delete the file
         * otherwise will only remove the filename from the vector.
         * Defaults to false.
         *
         * Removes the filename and delete it if delete_file is true.
        */
        void removeFile(const bool delete_file=false);

        /*!
         * getCurrentIdx
         *
         * @return: Return the current index.
        */
        int getCurrentIdx();

        /*!
         * getTotalSize
         *
         * @return: Return the total of files.
        */
        int getTotalSize();

    private:
        std::vector<std::string> m_files;

        int m_idx;
};

class DrawingArea : public Gtk::DrawingArea
{
    public:
        DrawingArea(int argc, char** argv);

        void setImage(const std::string& filename);

        /*!
         * hasParentWindow
         *
         * Certifies that DrawingArea is inside a container, and select an image
         * to be displayed.
         * should be called after creating the DrawingArea object.
        */
        void hasParentWindow();

    private:
        Gtk::Window* m_parent_window;

        Glib::RefPtr<Gdk::Pixbuf> m_image;

        sigc::connection m_conn;

        double m_scale;
        double m_img_focus_x, m_img_focus_y;
        double m_last_x_mouse, m_last_y_mouse;
        bool m_move_flag;
        bool m_reset_flag;
        bool m_is_click_released;
        bool m_is_click_holded;

        FileHandler m_filehandler;

        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
        bool on_scroll_event(GdkEventScroll* ev) override;
        bool on_button_press_event(GdkEventButton* ev) override;
        bool on_button_release_event(GdkEventButton* ev) override;
        bool on_key_press_event(GdkEventKey* ev) override;
        bool on_motion_notify_event(GdkEventMotion* ev) override;

        /* onTimeoutSetClickHold
         * 
         * @param x_mouse: X mouse position
         * @param y_mouse: Y mouse position
         * @return: True if the callback should be called again, false otherwise.
         *
         * Determine whereas the Button 1 of the mouse is being hold.
        */
        bool onTimeoutSetClickHold(const double last_x_mouse, const double last_y_mouse);

        /*!
         * setTitleForParentWindow
         *
         * @param filename: Filename to be used as part of the title.
         *
         * Sets the title for the parent window.
        */
        void setTitleForParentWindow(const std::string& filename);

        void fitImage(const int w_width, const int w_height, const int i_width, const int i_height);
        void fit();
        void reset();
};

class ImageViewer : public Gtk::Window {
    public:
        ImageViewer();

        Gtk::Box m_box;

        void addToBox(int argc, char** argv);
};

/*!
 * helper
 *
 * Displays a helper message.
 *
 */
void helper();

#endif
