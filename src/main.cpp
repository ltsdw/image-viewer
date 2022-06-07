#include "image-viewer.hpp"


int main(int argc, char** argv)
{
    auto app = Gtk::Application::create("org.gtkmm.image_viewer");

    std::string filename = argv[1];

    ImageViewer window;

    window.resize(600, 600);

    window.addToFrame(filename);

    //window.setImage(filename);

    window.show_all();

    //window.show_all_children();

    return app->run(window);
}
