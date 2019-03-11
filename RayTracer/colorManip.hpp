#include <SFML/Graphics.hpp>
sf::Color operator*(sf::Color A, double B){

    sf::Uint8 r = A.r;
    sf::Uint8 b = A.b;
    sf::Uint8 g = A.g;
    sf::Uint8 a = A.a;
    
    sf::Uint8 rn = double(int(A.r)) * B;
    sf::Uint8 bn = double(int(A.b)) * B;
    sf::Uint8 gn = double(int(A.g)) * B;
    
    if(rn > 255){rn=255;}
    if(rn < 0){rn=0;}
    
    if(bn > 255){bn=255;}
    if(bn < 0){bn=0;}
    
    if(gn > 255){gn=255;}
    if(gn < 0){gn=0;}
    
    return sf::Color(rn,gn,bn,a);
}


sf::Color clamp(sf::Color A){
    sf::Color B(0,0,0,0);
    B.r = A.r > 255 ? 255 : A.r;
    B.g = A.g > 255 ? 255 : A.g;
    B.b = A.b > 255 ? 255 : A.b;
    B.a = A.a > 255 ? 255 : A.a;
    return B;
}
