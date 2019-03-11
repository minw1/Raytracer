#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "tracer.hpp"
#include "ResourcePath.hpp"

sf::Texture* image(float time){
    sf::Texture* texture = new sf::Texture();
    
    std::vector<Shape*> scene;
    
    Material steel(0.05, 1, .5, sf::Color(100,100,255), .5);
    Material steel2(.05, 1, .5, sf::Color(100,100,255), .2);
    Material blue(0.05, 1, .5, sf::Color(0,0,255), .1);
    Material mirror(0.05, 1, .5, sf::Color(100,0,0), 1);
    Material red(0.06, 1, .5, sf::Color(200,0,0), .1);
    
    double timescaled = time*(2*pi)/100;
    
    Sphere redSphere(sf::Vector3<double>(10,cos(timescaled),sin(timescaled)), .7, mirror);
    
    Sphere greenSphere(sf::Vector3<double>(sin(timescaled)*20 + 25,2.0*sin(timescaled),2.0*cos(timescaled)+ 0.5), .8, red);
    
    Sphere blueSphere(sf::Vector3<double>(9,1,1.2), .4, blue);
    
    Plane greyPlane(sf::Vector3<double>(15,0,-1), sf::Vector3<double>(0,0,1), steel);
    
    Cube redCube(sf::Vector3<double>(30,0,1),
                 2.0 * sf::Vector3<double>(cos(timescaled),0,sin(timescaled)),
                 
                 sf::Vector3<double>(0,1.0000,0),
                 
                 sf::Vector3<double>(-sin(timescaled),0,cos(timescaled)),
                 
                 steel2);
    
    
    Plane redPlane(sf::Vector3<double>(-20,0,0), sf::Vector3<double>(1,0,0),red);
    
    
    scene.push_back(&redSphere);
    scene.push_back(&greenSphere);
    scene.push_back(&blueSphere);
    scene.push_back(&redCube);
    scene.push_back(&greyPlane);
    
    for(int aPIX = 0; aPIX < width; aPIX++){
        for(int bPIX = 0; bPIX < height;bPIX++){

            
            double aPOS = viewport[0] + ( (viewport[2]-viewport[0]) * (double(aPIX)/double(width-1)) );
            double bPOS = viewport[1] + ( (viewport[3]-viewport[1]) * (double(bPIX)/double(height-1)) );
            
            //this is because the positive y direction in the 3d coordinate plane is the negative y direction in the sfml rendering window
            bPOS = -bPOS;
            
            double cPOS = 1;
            
            sf::Vector3<double> focusPoint(cPOS,aPOS,bPOS);
            
            //if we want a linspace from min to max, if we have x samples from 0...x-1
            //then for sample g we should return min + (max-min)(g)/(x-1)
            //x - 1 because if we want three samples between 0 and 1, we go by halves, we are including min and max
            
            sf::Color thisColor = Raytrace(scene, cameraPosition, normalize(focusPoint-cameraPosition));
            thisColor = clamp(thisColor);
            
            pixels[(aPIX+bPIX*width)*4] = thisColor.r;//r
            pixels[(aPIX+bPIX*width)*4 + 1] = thisColor.g;//g
            pixels[(aPIX+bPIX*width)*4 + 2] = thisColor.b;//b
            pixels[(aPIX+bPIX*width)*4 + 3] = thisColor.a;//a
            
        }
        
    }
    
    
    texture->create(width, height);
    texture->update(pixels);

    
    return texture;
}
