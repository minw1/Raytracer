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
        
        
        if(thisPtr->type == "Sphere"){
            Sphere * castedPtr = static_cast<Sphere *>(thisPtr);
            distance = castedPtr->intersect(RO, RD);
        }
        if(thisPtr->type == "Plane"){
            Plane * castedPtr = static_cast<Plane *>(thisPtr);
            distance = castedPtr->intersect(RO, RD);
        }
        if(thisPtr->type == "Disc"){
            Disc * castedPtr = static_cast<Disc *>(thisPtr);
            distance = castedPtr->intersect(RO, RD);
        }
        if(thisPtr->type == "Rect"){
            Rect * castedPtr = static_cast<Rect *>(thisPtr);
            distance = castedPtr->intersect(RO, RD);
        }
        if(thisPtr->type == "Cube"){
            Cube * castedPtr = static_cast<Cube *>(thisPtr);
            distance = castedPtr->intersect(RO, RD);
        }
        
        
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
    
    if(distance == inf){

        return backgroundColor;
    
    }
    
    
    Shape * objectP = std::get<1>(info);
    
    Shape object = *objectP;
    
    
    
    sf::Vector3<double> POI = Origin + (Direction*distance);//Point of intersection
    
    
    sf::Vector3<double> normal;
    
    if(object.type == "Sphere"){
        Sphere * castedPtr = static_cast<Sphere*>(objectP);
        normal = castedPtr->getNormal(POI);
    }
    if(object.type == "Plane"){
        Plane * castedPtr = static_cast<Plane*>(objectP);
        normal = castedPtr->getNormal(POI);
    }
    
    if(object.type == "Disc"){
        Disc * castedPtr = static_cast<Disc*>(objectP);
        normal = castedPtr->getNormal(POI);
    }
    
    if(object.type == "Rect"){
        Rect * castedPtr = static_cast<Rect*>(objectP);
        normal = castedPtr->getNormal(POI);
    }
    if(object.type == "Cube"){
        Cube * castedPtr = static_cast<Cube*>(objectP);
        normal = castedPtr->getNormal(POI);
        
    }
    
    
    
    double specular_intensity = object.specular_intensity;
    double smoothness = object.smoothness;
    sf::Color diffuse_color = object.diffuse_color;
    double diffuse_intensity = object.diffuse_intensity;
    
    sf::Vector3<double> toLight = normalize(lightPosition - POI);
    sf::Vector3<double> viewVec = normalize(POI-Origin);
    
    
    std::tuple<double,Shape*> lightIntersection = closestIntersection(scene, POI + normal*0.01, toLight,objectP);
    
    if(std::get<0>(lightIntersection) < magnitude(toLight)){return sf::Color(0,0,0,255);}
    //if path to light is blocked returns black
    
    sf::Color rayColor = ambientColor;
    
    //lambert shading
    rayColor = rayColor*diffuse_color;
    rayColor = rayColor + (lightColor * (diffuse_intensity * std::max((double)0,toLight*normal)));
    
    //Gaussian specular shading
    sf::Vector3<double> halfAngle = normalize(toLight - viewVec);//this works because both vectors are already normalized
    double angle = acos(halfAngle*normal);
    double spec = pow(EulerConstant,-1 * pow(angle/smoothness,2));

    rayColor = rayColor + lightColor*spec*specular_intensity;
    
    
    sf::Vector3<double> bounceAngle = normalize((normal*(normal * viewVec))*-2.0 + viewVec);
    sf::Color nextStep = Raytrace(scene, POI+(normal*0.001), bounceAngle,depth+1, objectP);
    if(object.type == "Cube"){
        /*
         //std::cout<<"spec: "<<spec<<std::endl;
        std::cout<<vector_str(bounceAngle)<<std::endl;
        std::cout<<vector_str(POI+(normal*0.001))<<std::endl;
        std::cout<<color_str(nextStep)<<std::endl;
        
        std::cout<<std::get<0>(
                               closestIntersection(scene,
                                                   sf::Vector3<double>(28.698761,-0.999420,1.112961),
                                                   sf::Vector3<double>(-0.707107,0.000000,0.707107)
                                                   )
                               )<<std::endl;
         */
    }
    
    
    rayColor = rayColor * double(1-object.reflectivity) + nextStep*object.reflectivity;
    
    return rayColor;
}
