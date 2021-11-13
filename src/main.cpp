#include <fstream>
#include <iostream>
#include <chrono>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "config.h"
#include "camera.h"
#include "image_lib.h"
#include "structs.h"
#include "collision.h"
#include "file_loader.h"
#include "ray_tracer.h"

int main(int argc, char* argv[]) {
    std::string file_name = std::string(DEBUG_DIR) + '/'; 
    std::string out_name("dragon.png");
    if(argc < 2) {
        file_name += "dragon.dae";
    }
    else if (argc < 3) {
        file_name += argv[1];
    }
    else {
        file_name += argv[1];
        out_name = argv[2];
    }
    std::cout << "Opening " << file_name << std::endl;
    FileLoader file_loader;
    std::shared_ptr<Scene> my_scene = file_loader.LoadScene(file_name);
    int width(1280), height(720);
    int half_width = width >> 1;
    int half_height = height >> 1;
    my_scene->camera_.SetDist(half_width / std::tanf(.5f * my_scene->camera_.GetHalfFOV()));
    RayTracer ray_tracer(my_scene);
    auto s_time = std::chrono::high_resolution_clock::now();
    shared_ptr<Image> img = ray_tracer.Render(width, height);
    auto e_time = std::chrono::high_resolution_clock::now();
    auto min = std::chrono::duration_cast<std::chrono::minutes>(e_time - s_time);
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(e_time - s_time);
    auto mil = std::chrono::duration_cast<std::chrono::milliseconds>(e_time - s_time);
    std::cout << "\nRender Time\n" 
              << "\tMinutes: " << min.count() << "\n"
              << "\tSeconds: " << sec.count() - 60 * min.count() << "\n"
              << "\tMilliseconds: " << mil.count() - (60000 * min.count()) - (1000 * sec.count())  <<"\n";
    std::cout << "Change output file name (y/n)?" << std::endl;
    char new_file_name;
    std::cin >> new_file_name;
    if (new_file_name == 'y' || new_file_name == 'Y') {
        std::cout << "Enter new file name: ";
        std::cin >> out_name;
    }
    img->write(out_name.c_str());
    std::cout << "Wrote result to " << out_name << std::endl;
    return 0;
}
