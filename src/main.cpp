#include "image-viewer.hpp"

int main(int argc, char** argv)
{
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("org.gtkmm.image_viewer");

    std::string filename = argv[1];

    ImageViewer window(app);

    window.addToBox(argc, argv);

    window.show_all();

    return app->run(window);
}
