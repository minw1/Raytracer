#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>
typedef std::pair<double, sf::Vector3<double>> edge;
typedef std::vector<edge> intersectInfo;//list of edges going, in, out, in ,out, etc. Must be even sized to make sense.

intersectInfo IntersectEdges(intersectInfo& A, intersectInfo& B){
    intersectInfo toReturn;
    int a = 0;
    int b = 0;
    int d = 0;
    assert(A.size()%2 == 0 && B.size()%2 ==0);
    while(a<A.size() || b< B.size()){
        bool aHasNext;//a is available with the closest edge
        bool in; //current edge enters a shape
        edge nextEdge;
        if(b>=B.size()){aHasNext = true;nextEdge=A[a];}
        else if(a>=A.size()){aHasNext = false;nextEdge=B[b];}
        else if(A[a].first<B[b].first){aHasNext = true;nextEdge=A[a];}
        else{aHasNext = false;nextEdge=B[b];}
        if(aHasNext){in = (a%2 == 0);}
        else{in = (b%2 == 0);}
        
        if(in){
            assert(d!=2);
            if(d==1){toReturn.push_back(nextEdge);}
            d++;
        }
        else{
            assert(d!=0);
            if(d==2){toReturn.push_back(nextEdge);}
            d-=1;
        }
        
        if(aHasNext){a++;}
        else{b++;}
    }
    return toReturn;
}

intersectInfo UnionEdges(intersectInfo& A, intersectInfo& B){
    intersectInfo toReturn;
    int a = 0;
    int b = 0;
    int d = 0;
    assert(A.size()%2 == 0 && B.size()%2 ==0);
    while(a<A.size() || b< B.size()){
        bool aHasNext;//a is available with the closest edge
        bool in; //current edge enters a shape
        edge nextEdge;
        
        if(b>=B.size()){aHasNext = true;nextEdge=A[a];}
        else if(a>=A.size()){aHasNext = false;nextEdge=B[b];}
        else if(A[a].first<B[b].first){aHasNext = true;nextEdge=A[a];}
        else{aHasNext = false;nextEdge=B[b];}
        if(aHasNext){in = (a%2 == 0);}
        else{in = (b%2 == 0);}
        
        if(in){
            assert(d!=2);
            if(d==0){toReturn.push_back(nextEdge);}
            d++;
        }
        else{
            assert(d!=0);
            if(d==1){toReturn.push_back(nextEdge);}
            d-=1;
        }
        
        if(aHasNext){a++;}
        else{b++;}
    }
    return toReturn;
    
}

intersectInfo DifferenceEdges(intersectInfo& A, intersectInfo& B){
    intersectInfo toReturn;
    int a = 0;
    int b = 0;
    
    bool inA = false;
    bool inB = false;
    assert(A.size()%2 == 0 && B.size()%2 ==0);
    while(a<A.size() || b< B.size()){
        bool aHasNext;//a is available with the closest edge
        bool in; //current edge enters a shape
        edge nextEdge;
        
        if(b>=B.size()){aHasNext = true;nextEdge=A[a];}
        else if(a>=A.size()){aHasNext = false;nextEdge=B[b];}
        else if(A[a].first<B[b].first){aHasNext = true;nextEdge=A[a];}
        else{aHasNext = false;nextEdge=B[b];}
        if(aHasNext){in = (a%2 == 0);}
        else{in = (b%2 == 0);}
        
        if(aHasNext){
            if(!inB){toReturn.push_back(nextEdge);}
            inA = !inA;
        }
        else{
            if(inA){nextEdge.second = -nextEdge.second;toReturn.push_back(nextEdge);}
            inB = !inB;
        }
        if(aHasNext){a++;}
        else{b++;}
    }
    
    return toReturn;
    
}
