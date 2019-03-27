#include <stdio.h>
#include <iostream>
#include "defaults.hpp"
#include <SFML/Graphics.hpp>
#include <stdexcept>
#pragma once


class Material{//a simple container for storing surface information
public:
    double smoothness;
    double specular_intensity;
    double diffuse_intensity;
    sf::Color diffuse_color;
    double reflectivity;
    Material(double smoothness_passed, double specular_intensity_passed,double diffuse_intensity_passed, sf::Color diffuse_color_passed, double reflectivity_passed): smoothness(smoothness_passed), specular_intensity(specular_intensity_passed), diffuse_intensity(diffuse_intensity_passed),diffuse_color(diffuse_color_passed),reflectivity(reflectivity_passed){};
};

class Shape{//abstract class for all the functions a tracable shape needs
public:
    std::string type = "";
    Material material;
    Shape(std::string type_passed,Material m):
    material(m){}
    virtual intersectInfo intersect(sf::Vector3<double> RO, sf::Vector3<double> RD)=0;
};

class Plane: public Shape{
public:
    sf::Vector3<double> PO;
    sf::Vector3<double> PN;
    
    
    Plane(sf::Vector3<double> PO_passed, sf::Vector3<double> PN_passed, Material m): Shape("Plane",m), PO(PO_passed),PN(PN_passed){};
    
    intersectInfo intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        //Checks intersection from a ray to plane. Ray is defined by RO is an arbitrary point on vector, RD is unit vector for direction of ray PO is point on plane PN is unit normal vector for plane.
        //Equation used is
        // d =  (PO-RO) . PN
        //      ------------
        //      RD . PN
        intersectInfo toReturn = intersectInfo();
        
        double denominator = RD * PN;
        if (fabs(denominator) < 0.00001){
            return toReturn;
        }
        double total = ((PO-RO)*PN) / denominator;
        if(total < 0){
            return toReturn; //Negative d means no intersection
        }
        toReturn.push_back(std::make_pair(total, PN));
        toReturn.push_back(std::make_pair(total + .001, -PN));
        
        return toReturn;
    }
    
};


class Disc: public Plane{
public:
    double radSq;
    Disc(sf::Vector3<double> PO_passed, sf::Vector3<double> PN_passed, double radiusSq, Material m)
    : Plane(PN_passed,PO_passed, m), radSq(radiusSq){type="Disc";}
    intersectInfo intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        intersectInfo toReturn = intersectInfo();
        
        double denominator = RD * PN;
        if (fabs(denominator) < 0.00001){
            return toReturn;
        }
        double total = ((PO-RO)*PN) / denominator;
        if(total < 0 || (RD*total + RO - PO)*(RD*total + RO - PO) > radSq){//gotta make sure the POI isn't too far from the PO
            return toReturn;
        }
        toReturn.push_back(std::make_pair(total, PN));
        toReturn.push_back(std::make_pair(total + .001, -PN));
        
        return toReturn;
    }
};



class Sphere: public Shape{
public:
    sf::Vector3<double> SO;
    double SR;
    Sphere(sf::Vector3<double> SO_passed, double SR_passed, Material m)
    : Shape("Sphere", m), SO(SO_passed),SR(SR_passed){};
    intersectInfo intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        //Checks intersection from a ray to a sphere. Ray is defined by RO as an endpoint, RD is unit vector for direction of ray. CO is center of circle CR is a scalar representing radius.
        //Equation  is d = -RD*(RO-SO) plus or minus sqrt(  (RD * (RO-SO)) **2 - mag(RO-SO)**2 + SR**2) )
        intersectInfo toReturn = intersectInfo();
        sf::Vector3<double> away = RO-SO;
        double disc = pow(RD * away,2) - pow(magnitude(away),2) + pow(SR,2);
        double form = -(RD * (RO-SO));
        if(disc < 0){return toReturn;}
        else if (disc == 0){
            return toReturn;//it's safe to ignore, literal edge (tangent to spere) cases?
        }
        double close =std::min(form + sqrt(disc), form - sqrt(disc));
        double far = std::max(form + sqrt(disc), form - sqrt(disc));
        
        if (close<0){return toReturn;} //there is no intersection if the 'intersection' would be behind the ray origin
        
        toReturn.push_back(std::make_pair(close, normalize((cameraPosition+RD*close)-SO)));
        toReturn.push_back(std::make_pair(far, normalize((cameraPosition+RD*far)-SO)));
        
        return toReturn;
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
    
    Rect(sf::Vector3<double> PO_passed, sf::Vector3<double> PN_passed, sf::Vector3<double> yposAxis, double ymax, double xmax, double ymin, double xmin, Material m)
    :Plane(PO_passed, PN_passed, m),yvecMax(ymax),xvecMax(xmax),yvecMin(ymin),xvecMin(xmin),yposAxis(normalize(yposAxis))
    {
        type = "Rect";
        if(abs(PN*yposAxis) > 0.00001){throw std::logic_error("A rect object was improperly initialized, decalred y axis was not perpendicular to the plane normal vector");}
        else{xposAxis = normalize(cross(PN, yposAxis));}
    }
    
    intersectInfo intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        
        //this part is same as plane intersect
        intersectInfo toReturn = intersectInfo();
        
        double denominator = RD * PN;
        if (fabs(denominator) < 0.00001){
            return toReturn;
        }
        double total = ((PO-RO)*PN) / denominator;
        
        //but here, the coordinates of the intersection point are tested for being in bounds
        auto coor = total*RD + RO - PO;
        auto y = coor*yposAxis;
        auto x = coor*xposAxis;
        if (y>yvecMax || y < yvecMin || x>xvecMax || x<xvecMin){
            return toReturn;
        }
        
        toReturn.push_back(std::make_pair(total, PN));
        toReturn.push_back(std::make_pair(total + .001, -PN));
        
        return toReturn;
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
    
    intersectInfo intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        intersectInfo toReturn = intersectInfo();
        
        sf::Vector3<double> closeN;
        double closeVal = inf;
        
        sf::Vector3<double> farN;
        double farVal = -inf;
        
        for(int i = 0; i < walls.size();i++){
            auto inter = walls[i].intersect(RO, RD);
            if(inter.size() > 0){
                if (inter[0].first<closeVal){
                    closeN = inter[0].second;
                    closeVal = inter[0].first;
                }
                if (inter[0].first>farVal){
                    farN = inter[0].second;
                    farVal = inter[0].first;
            }
            }
        }
        if(closeVal == inf || farVal == -inf){return toReturn;}
        toReturn.push_back(std::make_pair(closeVal, closeN));
        toReturn.push_back(std::make_pair(farVal, farN));
        return toReturn;
    }
    
};


class Intersect: public Shape{
public:
    Shape* shape1;
    Shape* shape2;
    
    Intersect(Shape* one, Shape* two, Material m):
    Shape("Intersect",m),shape1(one),shape2(two)
    {}
    intersectInfo intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        intersectInfo firstSect = shape1->intersect(RO, RD);
        intersectInfo secondSect = shape2->intersect(RO, RD);
        return IntersectEdges(firstSect, secondSect);
    }

};
/*
class Union: public Shape{
public:
    Shape* shape1;
    Shape* shape2;
    
    Union(Shape* one, Shape* two, Material m):
    Shape("Union",m),shape1(one),shape2(two)
    {}
    double intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        double firstSect = shape1->intersect(RO, RD);
        double secondSect = shape2->intersect(RO, RD);
        if(secondSect<inf or firstSect<inf){
            return(std::min(firstSect,secondSect));
        }
        return inf;
    }
    sf::Vector3<double> getNormal(sf::Vector3<double> RO, sf::Vector3<double> RD){
        double firstSect = shape1->intersect(RO, RD);
        double secondSect = shape2->intersect(RO, RD);
        if(secondSect<inf or firstSect<inf){
            if(firstSect>secondSect){return(shape2->getNormal(RO, RD));}
            else{return(shape1->getNormal(RO, RD));}
        }
    }
};
*/

struct rayIntersectData{
    Shape* s;
    double l;
    sf::Vector3<double> N;
};
