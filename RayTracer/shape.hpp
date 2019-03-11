#include <stdio.h>
#include <iostream>
#include "defaults.hpp"
#include <SFML/Graphics.hpp>
#include <stdexcept>
#pragma once


class Material{//a simple container dictionary for storing surface information
public:
    double smoothness;
    double specular_intensity;
    double diffuse_intensity;
    sf::Color diffuse_color;
    double reflectivity;
    
    Material(double smoothness_passed, double specular_intensity_passed,double diffuse_intensity_passed, sf::Color diffuse_color_passed, double reflectivity_passed): smoothness(smoothness_passed), specular_intensity(specular_intensity_passed), diffuse_intensity(diffuse_intensity_passed),diffuse_color(diffuse_color_passed),reflectivity(reflectivity_passed){};
};

class Shape{
public:
    std::string type = "";
    Shape(std::string typepassed, double smoothness_passed, double specular_intensity_passed,double diffuse_intensity_passed, sf::Color diffuse_color_passed, double reflectivity_passed):
    diffuse_color(diffuse_color_passed), reflectivity(reflectivity_passed)
    {
        type = typepassed;
        smoothness = smoothness_passed;
        specular_intensity = specular_intensity_passed;
        diffuse_intensity = diffuse_intensity_passed;
    }
    Shape(std::string type_passed,Material m){
        type = type_passed;
        smoothness = m.smoothness;
        specular_intensity=m.specular_intensity;
        diffuse_intensity=m.diffuse_intensity;
        reflectivity = m.reflectivity;
        diffuse_color = m.diffuse_color;
        
    }
    virtual double intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        std::cout<<"error"<<std::endl;
        return inf;//these functions should never be called
    };
    virtual sf::Vector3<double> getNormal(sf::Vector3<double> POI){
        std::cout<<"error 2"<<std::endl;
        return sf::Vector3<double>(inf,inf,inf);//these functions should never be called
    };
    double smoothness; //a number between 0 and 1
    double specular_intensity;
    double diffuse_intensity;
    double reflectivity;
    sf::Color diffuse_color;
};

class Plane: public Shape{
public:
    sf::Vector3<double> PO;
    sf::Vector3<double> PN;
    Plane(sf::Vector3<double> PO_passed, sf::Vector3<double> PN_passed, double smoothness_passed,double specular_intensity_passed,double diffuse_intensity_passed, sf::Color diffuse_color_passed, double reflectivity_passed)
    : Shape("Plane",smoothness_passed,specular_intensity_passed,diffuse_intensity_passed,diffuse_color_passed, reflectivity_passed),PO(PO_passed), PN(PN_passed){}
    
    Plane(sf::Vector3<double> PO_passed, sf::Vector3<double> PN_passed, Material m): Shape("Plane",m), PO(PO_passed),PN(PN_passed){};
    
    double intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        //Checks intersection from a ray to plane. Ray is defined by RO is an arbitrary point on vector, RD is unit vector for direction of ray PO is point on plane PN is unit normal vector for plane.
        //Equation used is
        // d =  (PO-RO) . PN
        //      ------------
        //      RD . PN
        double denominator = RD * PN;
        
        
        if (fabs(denominator) < 0.00001){
            return inf;
        }
         
        double total = ((PO-RO)*PN) / denominator;

        if(total < 0){
            return inf; //Negative d means no intersection
        }
        
        
        return total;
    }
    sf::Vector3<double> getNormal(sf::Vector3<double> POI){
        return PN; //plane's normal vector is always the same vector which is perpendicular to the plane
    }
};


class Disc: public Plane{
    
public:
    double radSq;
    
    Disc(sf::Vector3<double> PO_passed, sf::Vector3<double> PN_passed, double smoothness_passed,double specular_intensity_passed,double diffuse_intensity_passed, sf::Color diffuse_color_passed, double reflectivity_passed, double radiusSq)
    : Plane(PO_passed, PN_passed, smoothness_passed,specular_intensity_passed,diffuse_intensity_passed, diffuse_color_passed, reflectivity_passed), radSq(radiusSq){type = "Disc";}
    Disc(sf::Vector3<double> PO_passed, sf::Vector3<double> PN_passed, double radiusSq, Material m)
    : Plane(PN_passed,PO_passed, m), radSq(radiusSq){type="Disc";}
    
    
    
    double intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        double denominator = RD * PN;
        if (denominator == 0){return inf;}
        double total = ((PO-RO)*PN) / denominator;
        
        if(total < 0 || (RD*total + RO - PO)*(RD*total + RO - PO) > radSq){
            return inf; //Negative d means no intersection
        }
        
        return total;
    }
    
    
    
};



class Sphere: public Shape{
public:
    sf::Vector3<double> SO;
    double SR;
    Sphere(sf::Vector3<double> SO_passed, double SR_passed, double smoothness_passed,double specular_intensity_passed,double diffuse_intensity_passed, sf::Color  diffuse_color_passed, double reflectivity_passed)
    : Shape("Sphere",smoothness_passed,specular_intensity_passed,diffuse_intensity_passed,diffuse_color_passed, reflectivity_passed), SO(SO_passed), SR(SR_passed){}
    Sphere(sf::Vector3<double> SO_passed, double SR_passed, Material m)
    : Shape("Sphere", m), SO(SO_passed),SR(SR_passed){};
    
    
    
    double intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        //Checks intersection from a ray to a sphere. Ray is defined by RO is an arbitrary point on vector, RD is unit vector for direction of ray. CO is center of circle CR is a scalar representing radius.
        //Equation  is d = -RD*(RO-SO) plus or minus sqrt(  (RD * (RO-SO)) **2 - mag(RO-SO)**2 + SR**2) )
        
        sf::Vector3<double> away = RO-SO;
        
        
        double disc = pow(RD * away,2) - pow(magnitude(away),2) + pow(SR,2);
        
        
        double form = -(RD * (RO-SO));
        
        if(disc < 0){return inf;}
        else if (disc == 0){return form;}
        double len = std::min(form + sqrt(disc), form - sqrt(disc));
        return len<0?inf:len;
    }
    
    sf::Vector3<double> getNormal(sf::Vector3<double> POI){
        return normalize(POI - SO);
    }
};


class Rect: public Plane{
public:
    double yvecMax;
    double xvecMax;
    double yvecMin;
    double xvecMin;
    sf::Vector3<double> yposAxis;
    sf::Vector3<double> xposAxis;
    
    Rect(sf::Vector3<double> PO_passed, sf::Vector3<double> PN_passed, double smoothness_passed,double specular_intensity_passed,double diffuse_intensity_passed, sf::Color diffuse_color_passed, double reflectivity_passed, sf::Vector3<double> yposAxis, double ymax, double xmax, double ymin, double xmin)
    : Plane(PO_passed, PN_passed, smoothness_passed,specular_intensity_passed,diffuse_intensity_passed, diffuse_color_passed, reflectivity_passed),
    yvecMax(ymax),xvecMax(xmax),yvecMin(ymin),xvecMin(xmin),yposAxis(normalize(yposAxis))
    {
        type = "Rect";
        if(abs(PN*yposAxis) > 0.00001){
            throw std::logic_error("A rect object was improperly initialized");
        }
        else{
            xposAxis = normalize(cross(yposAxis,PN));
        }
    }
    
    
    Rect(sf::Vector3<double> PO_passed, sf::Vector3<double> PN_passed, sf::Vector3<double> yposAxis, double ymax, double xmax, double ymin, double xmin, Material m)
    :Plane(PO_passed, PN_passed, m),yvecMax(ymax),xvecMax(xmax),yvecMin(ymin),xvecMin(xmin),yposAxis(normalize(yposAxis))
    {
        type = "Rect";
        if(abs(PN*yposAxis) > 0.00001){
            throw std::logic_error("A rect object was improperly initialized");
        }
        else{
            xposAxis = normalize(cross(PN, yposAxis));
        }
    }
    double intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        double denominator = RD * PN;
        if (denominator == 0){return inf;}
        double total = ((PO-RO)*PN) / denominator;
        if(total < 0.0000001){
            return inf; //Negative d means no intersection
        }
        auto coor = total*RD + RO - PO;
        auto y = coor*yposAxis;
        auto x = coor*xposAxis;
        if (y>yvecMax || y < yvecMin || x>xvecMax || x<xvecMin){
            return inf;
        }
        return total;
    }
};

class Cube: public Shape{
public:
    std::vector<Rect> walls;
    Cube (sf::Vector3<double> center, sf::Vector3<double> upward, sf::Vector3<double> rightward,sf::Vector3<double> forward, Material m)
    : Shape("Cube",m){
    Rect rightwall = Rect(center+rightward,normalize(rightward),normalize(upward), magnitude(upward),magnitude(rightward),-magnitude(upward),-magnitude(rightward),m);
    Rect leftwall = Rect(center-rightward,normalize(-rightward),normalize(upward), magnitude(upward),magnitude(rightward),-magnitude(upward),-magnitude(rightward),m);
    Rect topwall = Rect(center+upward,normalize(upward),normalize(forward), magnitude(forward),magnitude(rightward),-magnitude(forward),-magnitude(rightward),m);
    Rect botwall = Rect(center-upward,normalize(-upward),normalize(forward), magnitude(forward),magnitude(rightward),-magnitude(forward),-magnitude(rightward),m);
        
    Rect backwall = Rect(center+forward,normalize(forward),normalize(upward), magnitude(upward),magnitude(rightward),-magnitude(upward),-magnitude(rightward),m);
        
    Rect frontwall= Rect(center-forward,normalize(-forward),normalize(upward), magnitude(upward),magnitude(rightward),-magnitude(upward),-magnitude(rightward),m);
        
    walls.push_back(rightwall);
    walls.push_back(leftwall);
    walls.push_back(topwall);
    walls.push_back(botwall);
    walls.push_back(backwall);
    walls.push_back(frontwall);
    }
    
    double intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        Rect * closest = nullptr;
        double closeVal = inf;
        for(int i = 0; i < walls.size();i++){
            double val = walls[i].intersect(RO, RD);
            if (val<closeVal){
                closest = &walls[i];
                closeVal = val;
            }
        }

        return closeVal;
    }
    
    sf::Vector3<double> getNormal(sf::Vector3<double> POI){
        for(int i=0; i<walls.size();i++){
            if (pointInPlane(POI, walls[i].PO, walls[i].PN)){

                return (walls[i].getNormal(POI));
                
            }
        }

    }
};
