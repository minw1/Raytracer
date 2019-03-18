
//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resources, use the helper
// function `resourcePath()` from ResourcePath.hpp
//

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "SceneTrace.hpp"

#include <iostream>
#include <stdio.h>
// Here is a small helper for you! Have a look.
#include "ResourcePath.hpp"


int main(int, char const**)
{
    int framecount = 0;
    for(float a = 0; a < 100;a++){
        auto ptr = image(a);
        ptr->copyToImage().saveToFile("/Users/williammin/Desktop/Animation/" + pad(framecount,3) + ".png");
        delete ptr;
        framecount++;
        std::cout<<framecount<<" percent complete"<<std::endl;
    }
    return EXIT_SUCCESS;
}
