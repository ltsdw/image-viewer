#include "image-viewer.hpp"


int main(int argc, char** argv)
{
    auto app = Gtk::Application::create("org.gtkmm.image_viewer");

    std::string filename = "/tmp/example.png";

    ImageViewer window {};
    
    window.setImage(filename);

    window.show_all_children();

    return app->run(window);
}
