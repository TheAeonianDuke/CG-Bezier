//triangle.cpp

#include "triangle.h"
#include "iostream"
constexpr float kEpsilon = 0.000001;

bool Triangle::intersect(Ray& r) const
{
    Vector3D e1,e2,h,s,q;
    double a,f,u,v;
    Vector3D N;

    e1 = p2 - p1;
    e2 = p3 - p1;
    Vector3D P = crossProduct(r.getDirection(), e2);
    double det = dotProduct(e1, P);

//    N = crossProduct(e1,e2);
//    float NdotRayDirection = dotProduct(N,r.getDirection());

//    std::cout<< a <<  "here0" << std::endl;
    if (det > -kEpsilon && det < kEpsilon){
        std::cout<< det <<  "here1" << std::endl;
        return(false);
    }

//    float d = dotProduct(N, p1);
//    float t = -((dotProduct(N,r.getOrigin()) + d) / NdotRayDirection);



    double inv_det = 1.f / det;
    Vector3D T = r.getOrigin() - p1;
    u = dotProduct(T,P) * inv_det;
    if(u<0.f || u>1.f){
        return false;
    }

    Vector3D Q = crossProduct(T, e1);
    v = dotProduct(r.getDirection(), Q) * inv_det;
    if(v<0.f || u+v>1.f){
        return false;
    }

    double t = dotProduct(e2,Q)*inv_det;

//    std::cout<< r.getOrigin().X() << r.getOrigin().Y() << r.getOrigin().Z() << " " <<  p1.X() << p1.Y() << p1.Z() << std::endl;
//
//    std::cout<<t << " "<< t2 << std::endl;std::cout<< s.X() << s.Y() << s.Z() << " " <<  q.X() << q.Y() << q.Z() << std::endl;

    if(t>kEpsilon) {
        Vector3D intersect = r.getOrigin()+t*r.getDirection();

//        intersect.normalize();

        r.setParameter(t, this);
//        intersect.normalize();
//        std::cout<<r.getPosition().X()<<" "<<intersect.X()<<std::endl;
        return true;
    }

}
