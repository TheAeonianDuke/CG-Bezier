#include "world.h"
#include "material.h"
#include "iostream"

Color Material::shade(const Ray& incident, const bool isSolid) const
{
//    Vector3D lightPos = Vector3D(0, 1, -10);
//    Vector3D shadowDir = Vector3D(0, 1, -10); - incident.getPosition();
//    std::cout<<color.R()<<color.G()<<color.B()<<std::endl;

    if( incident.didHit())
    {
//        std::cout<<color.R()<<color.B()<<color.G()<<std::endl;
        Vector3D point  = incident.getPosition();
        Vector3D lightPos = Vector3D(-10, 3, 2);
        Vector3D n = incident.getPosition() - Vector3D(3, 0, -10);
        n.normalize();
//        n.normalize();
        Vector3D v = incident.getDirection();
        v.normalize();
        v=-v;
        Vector3D l = (lightPos-point);
        l.normalize();
        Vector3D h = l+v;
        h.normalize();
        Vector3D reflectDir = incident.getDirection() - 2*(dotProduct(n,incident.getDirection()))*n;
        double val=  0.6*0.5 + 0.4*(0.5 * std::max(0.0,dotProduct(n,l))) + 0.8*(0.7* pow(std::max(0.0,dotProduct(n,v)), 256));

//        double val = 0.5 * std::max(0.0, dotProduct(n, l));

        Color color1 = Color(val *color.R(), val *color.G(), val *color.B());

        return color1;
    }

}
