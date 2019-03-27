#include "shape.hpp"
#include "colorManip.hpp"
#include <tuple>


rayIntersectData closestIntersection(std::vector<Shape*>& scene, sf::Vector3<double> RO, sf::Vector3<double> RD, Shape * toExclude = nullptr){
    
    
    rayIntersectData closest = rayIntersectData();
    closest.l = inf;
    
    for(int a = 0; a < scene.size(); a++){
        Shape * thisPtr = scene[a];
        if(toExclude == thisPtr){//some problems can occur without this statement, we don't want the tops of spheres blocking the bottom
            continue;
        }
        
        auto hit =thisPtr->intersect(RO,RD);
        if(hit.size()>0){
            if(hit[0].first < closest.l){
                closest.l = hit[0].first;
                closest.s = scene[a];
                closest.N = hit[0].second;
            }
        }
    }
    return closest;
}

sf::Color Raytrace(std::vector<Shape*> scene, sf::Vector3<double> Origin, sf::Vector3<double> Direction, int depth = 0, Shape * lastShape = nullptr){
    if(depth == max_depth){return sf::Color(128,128,128,255);}
    auto info = closestIntersection(scene, Origin, Direction, lastShape);
    
    
    double distance = info.l;
    if(distance == inf){return backgroundColor;}
    Shape * objectP = info.s;
    
    sf::Vector3<double> POI = Origin + (Direction*distance);//Point of intersection
    sf::Vector3<double> normal = info.N;
    Material material = objectP->material;
    sf::Vector3<double> toLight = normalize(lightPosition - POI);//normal vector to light
    sf::Vector3<double> viewVec = normalize(POI-Origin);//normal vector from camera to POI
    
    auto lightIntersection = closestIntersection(scene, POI + normal*0.01, toLight,objectP);
    if(lightIntersection.l < magnitude(lightPosition - POI + normal*0.01)){return sf::Color(0,0,0,255);}//if path to light is blocked returns black
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
