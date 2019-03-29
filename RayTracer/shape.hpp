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
    type(type_passed),material(m){}
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
    Disc(sf::Vector3<double> PO_passed, sf::Vector3<double> PN_passed, double radius, Material m)
    : Plane(PO_passed,PN_passed, m), radSq(pow(radius,2.0)){type="Disc";}
    intersectInfo intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        intersectInfo toReturn = intersectInfo();
        
        double denominator = RD * PN;
        if (fabs(denominator) < 0.00001){
            return toReturn;
        }
        double total = ((PO-RO)*PN) / denominator;
        
        if(total <= 0){//gotta make sure the POI isn't too far from the PO
            return toReturn;
        }
        
        if((RD*total + RO - PO)*(RD*total + RO - PO) > radSq){
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
            return toReturn;//we could get into a philosohpical debate about the color of a ray of light tangent to a sphere.
            //but I would pose an equally philosophical question: If an infinitely small sliver of sphere disappears and there
            //isn't anyone to not see it, does it return a color?
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


class Cylinder: public Shape{
public:
    Disc top;
    Disc bottom;
    sf::Vector3<double> center;
    sf::Vector3<double> upwards;
    double radiusSQ;
    double height;
    
    std::pair<double,double> infIntersect(sf::Vector3<double>RO,sf::Vector3<double>RD){//intersection if the cylinder extened infinitely
        sf::Vector3<double> As = (RD-(RD*upwards)*upwards);
        sf::Vector3<double> dp = RO-center;
        sf::Vector3<double> Cs = dp - (dp*upwards)*upwards;
        double A = As*As;
        double B = 2.0* ((RD-(RD*upwards)*upwards) * (dp-(dp*upwards)*upwards));
        double C = Cs*Cs -radiusSQ;
        double discr =pow(B,2.0)-4*A*C;
        if(discr <= 0){return std::make_pair(inf, inf);}
        double first = (-B + sqrt(discr))/2*A;
        double second =(-B - sqrt(discr))/2*A;
        return std::make_pair(first, second);
        
    }
    
    bool tooFarOut(sf::Vector3<double> POI){
        return (fabs((POI-center) * upwards) > height);
    }
    
    sf::Vector3<double> normal(sf::Vector3<double> POI){
        sf::Vector3<double> crossOrigin = ((POI-center) * upwards)*upwards + center;
        return(normalize(POI-crossOrigin));
    }
    
    Cylinder(sf::Vector3<double> centerPass, sf::Vector3<double> upwardsPass, double radiusPass, double heightPass, Material m)
    //height is a little misleading-- it's more like a height radius, the height from the core towards each cap
    :Shape("Cylinder",m),center(centerPass),upwards(upwardsPass),radiusSQ(pow(radiusPass,2.0)),height(heightPass),
    top(center + upwards*height,upwards,radiusPass,m),bottom(center - upwards*height,-upwards,radiusPass,m){};
    

    intersectInfo intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        intersectInfo toReturn = intersectInfo();
        
        intersectInfo topint = top.intersect(RO,RD);
        intersectInfo bottomint = bottom.intersect(RO,RD);
        
        
        
        if(topint.size()!=0 && bottomint.size()!=0){
            std::cout<<"ok"<<std::endl;
            bool topSmaller = topint[0].first < bottomint[0].first;
            if(topSmaller){
                toReturn.push_back(topint[0]);
                toReturn.push_back(bottomint[0]);
            }
            else{
                toReturn.push_back(bottomint[0]);
                toReturn.push_back(topint[0]);
            }

        }
        else if(topint.size()!=0 || bottomint.size()!=0){
            std::cout<<"ok2"<<std::endl;
            edge realcap = topint.size()!=0? topint[0]: bottomint[0];//the cap that acutally has an intersection
            std::pair<double,double> sideHits = infIntersect(RO, RD);
            assert(sideHits.first!=inf);//if it intersects 1 cap, it must intersect the sides too
            double theInOne;//which of the sidehits is in the side
            if(!tooFarOut(RO+sideHits.first*RD)){theInOne = sideHits.first;}
            else{theInOne = sideHits.second;}
            bool capCloser = realcap.first<theInOne;
            if(capCloser){
                toReturn.push_back(realcap);
                toReturn.push_back(std::make_pair(theInOne, normal(RO+RD*theInOne)));
            }
            else{
                toReturn.push_back(std::make_pair(theInOne, normal(RO+RD*theInOne)));
                toReturn.push_back(realcap);
            }
        }
        else{
            std::pair<double,double> sideHits = infIntersect(RO, RD);
            if (sideHits.first == inf){return toReturn;}
            if (tooFarOut(RO+RD*sideHits.first)){return toReturn;}
            if (tooFarOut(RO+RD*sideHits.second)){return toReturn;}//in theory, only one of these can be tooFarOut
            bool fSmaller = sideHits.first < sideHits.second;
            if (fSmaller){
                toReturn.push_back(std::make_pair(sideHits.first, normal(RO+RD*sideHits.first)));
                toReturn.push_back(std::make_pair(sideHits.second, normal(RO+RD*sideHits.second)));
            }
            else{
                toReturn.push_back(std::make_pair(sideHits.second, normal(RO+RD*sideHits.second)));
                toReturn.push_back(std::make_pair(sideHits.first, normal(RO+RD*sideHits.first)));
            }
            
        }
        
        return toReturn;
        }
    
};



class Cube: public Shape{
public:
    std::vector<Rect> walls;
    Cube (sf::Vector3<double> center, sf::Vector3<double> forward, sf::Vector3<double> rightward,sf::Vector3<double> upward, Material m)
    : Shape("Cube",m){
    Rect rightwall = Rect(center+rightward,normalize(rightward),normalize(upward), magnitude(upward),magnitude(forward),-magnitude(upward),-magnitude(forward),m);
    Rect leftwall = Rect(center-rightward,normalize(-rightward),normalize(upward), magnitude(upward),magnitude(forward),-magnitude(upward),-magnitude(forward),m);
        
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
                if(inter[0].first>0){
                    if (inter[0].first<closeVal){
                        closeN = inter[0].second;
                        closeVal = inter[0].first;}
                    if (inter[0].first>farVal){
                        farN = inter[0].second;
                        farVal = inter[0].first;}
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
    Shape("Intersect",m),shape1(one),shape2(two){};
    intersectInfo intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        intersectInfo firstSect = shape1->intersect(RO, RD);
        intersectInfo secondSect = shape2->intersect(RO, RD);
        return IntersectEdges(firstSect, secondSect);}
};

class Union: public Shape{
public:
    Shape* shape1;
    Shape* shape2;
    
    Union(Shape* one, Shape* two, Material m):
    Shape("Union",m),shape1(one),shape2(two){};
    intersectInfo intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        intersectInfo firstSect = shape1->intersect(RO, RD);
        intersectInfo secondSect = shape2->intersect(RO, RD);
        return UnionEdges(firstSect, secondSect);}
};

class Difference: public Shape{
public:
    Shape* shape1;
    Shape* shape2;
    
    Difference(Shape* one, Shape* two, Material m):
    Shape("Difference",m),shape1(one),shape2(two){};
    intersectInfo intersect(sf::Vector3<double> RO, sf::Vector3<double> RD){
        intersectInfo firstSect = shape1->intersect(RO, RD);
        intersectInfo secondSect = shape2->intersect(RO, RD);
        return DifferenceEdges(firstSect, secondSect);
    }
    
};
