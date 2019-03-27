
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
//#include "SceneTrace.hpp"
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
     
    
    /*
    sf::Vector3<double> fakeVector(0,0,0);
    
    intersectInfo a = intersectInfo();
    intersectInfo b = intersectInfo();
    
    auto c = std::make_pair(0, fakeVector);
    auto d =std::make_pair(100, fakeVector);
    auto e = std::make_pair(70, fakeVector);
    auto f =std::make_pair(155, fakeVector);
    a.push_back(c);
    a.push_back(d);
    b.push_back(e);
    b.push_back(f);
    
    auto l = IntersectEdges(a, b);
    */
    
    return EXIT_SUCCESS;
}
