#include "shape.hpp"
#include "colorManip.hpp"
#include <tuple>

std::tuple<double,Shape*> closestIntersection(std::vector<Shape*>& scene, sf::Vector3<double> RO, sf::Vector3<double> RD, Shape * toExclude = nullptr){
    double closest = inf;
    Shape *closestShape = nullptr;
    for(int a = 0; a < scene.size(); a++){
        Shape * thisPtr = scene[a];
        if(toExclude == thisPtr){//some problems can occur without this statement, we don't want the tops of spheres blocking the bottom
            continue;
        }
        double distance = inf;
        distance = thisPtr->intersect(RO,RD);

        if(distance < closest){
            closest = distance;
            closestShape = scene[a];
        }
    }
    
    if (closest == inf){return std::make_tuple(inf,nullptr);}
    else{return std::make_tuple(closest,closestShape);}
    
}

sf::Color Raytrace(std::vector<Shape*>& scene, sf::Vector3<double> Origin, sf::Vector3<double> Direction, int depth = 0, Shape * lastShape = nullptr){
    
    if(depth == max_depth){return sf::Color(128,128,128,255);}
    
    std::tuple<double,Shape*> info = closestIntersection(scene, Origin, Direction, lastShape);
    
    double distance = std::get<0>(info);
    
    if(distance == inf){return backgroundColor;}
    
    Shape * objectP = std::get<1>(info);
    
    Shape object = *objectP;
    
    sf::Vector3<double> POI = Origin + (Direction*distance);//Point of intersection
    
    sf::Vector3<double> normal;
    
    normal = objectP->getNormal(POI);
    Material material = objectP->material;
    
    
    sf::Vector3<double> toLight = normalize(lightPosition - POI);
    sf::Vector3<double> viewVec = normalize(POI-Origin);
    
    std::tuple<double,Shape*> lightIntersection = closestIntersection(scene, POI + normal*0.01, toLight,objectP);
    
    if(std::get<0>(lightIntersection) < magnitude(lightPosition - POI + normal*0.01)){return sf::Color(0,0,0,255);}
    //if path to light is blocked returns black
    
    sf::Color rayColor = ambientColor;
    
    //lambert shading
    rayColor = rayColor*material.diffuse_color;
    rayColor = rayColor + (lightColor * (material.diffuse_intensity * std::max((double)0,toLight*normal)));
    
    //Gaussian specular shading
    sf::Vector3<double> halfAngle = normalize(toLight - viewVec);//this works because both vectors are already normalized
    double angle = acos(halfAngle*normal);
    double spec = pow(EulerConstant,-1 * pow(angle/material.smoothness,2));

    rayColor = rayColor + lightColor*spec*material.specular_intensity;
    
    sf::Vector3<double> bounceAngle = normalize((normal*(normal * viewVec))*-2.0 + viewVec);
    sf::Color nextStep = Raytrace(scene, POI+(normal*0.001), bounceAngle,depth+1, objectP);
    
    rayColor = rayColor * double(1-material.reflectivity) + nextStep*material.reflectivity;
    
    return rayColor;
}
