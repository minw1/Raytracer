#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

sf::Color lightColor(255,255,255,255);
const double EulerConstant = std::exp(1.0);
sf::Vector3<double> lightPosition(5,0,5);
sf::Vector3<double> cameraPosition(-5,0,0);

const sf::Color ambientColor(120,120,120,255);
const sf::Color backgroundColor(136,206,235,255);
const double pi = 3.141592;
const int max_depth = 4;
int width = 200;
int height = 150;
double viewport[4] = {-1,-0.75,1,0.75};
sf::Uint8* pixels = new sf::Uint8[width*height*4];
const double inf = std::numeric_limits<double>::infinity();

double operator*(sf::Vector3<double> A, sf::Vector3<double> B){
    return (A.x*B.x) + (A.y*B.y) + (A.z*B.z);
}

double magnitude(sf::Vector3<double> A){
    return sqrt(pow(A.x,2) + pow(A.y,2) + pow(A.z,2));
}

sf::Vector3<double> normalize(sf::Vector3<double> A){
    return A/magnitude(A);
}

sf::Vector3<double> cross(sf::Vector3<double> A,sf::Vector3<double> B){
    auto x = A.y*B.z - A.z*B.y;
    auto y = A.z*B.x - A.x*B.z;
    auto z = A.x*B.y - A.y*B.x;
    return sf::Vector3<double>(x,y,z);
}

std::string vector_str(sf::Vector3<double> a){
    return ("{" + std::to_string(a.x) + "," + std::to_string(a.y) + "," + std::to_string(a.z) + "}");
}

std::string color_str(sf::Color a){
    return ("{" + std::to_string(a.r) + "," + std::to_string(a.g) + "," + std::to_string(a.b) + "}");
}

std::string pad(int a, int digits){
    std::string f = std::to_string(a);
    while (f.length() < digits) {
        f = "0" + f;
    }
    return f;
}

bool pointInPlane(sf::Vector3<double> point, sf::Vector3<double> Plane, sf::Vector3<double> normal){
    return (fabs((point-Plane)*normal) < 0.00000001);//oh my god i spent so long trying to figure out why this was broken
    //originally, i had forgotten to take the absolute value, so plane intersections from one direction wouldn't register.
}



std::vector<sf::Vector3<double>> getMatrixFromRotation(double xTilt, double yTilt, double zTilt){
    std::vector<sf::Vector3<double>> toReturn;
    return(toReturn);//this will be implemented later
}
