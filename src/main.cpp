#include "image-viewer.hpp"


int main(int argc, char** argv)
{
    if (argc == 1 or not strcmp(argv[1], "--help")) helper();

    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("org.gtkmm.image_viewer");

    ImageViewer window;

    window.addToBox(argc, argv);

    window.show_all();

    return app->run(window);
}
