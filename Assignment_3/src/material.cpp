#include "world.h"
#include "material.h"
#include "iostream"

Color Material::shade(const Ray& incident, const bool isSolid) const
{
//    Vector3D lightPos = Vector3D(0, 1, -10);
//    Vector3D shadowDir = Vector3D(0, 1, -10); - incident.getPosition();

    if( incident.didHit())
    {

        Vector3D point  = incident.getPosition();
        Vector3D lightPos = Vector3D(-10, 3, 2);
        Vector3D n = incident.getPosition() - Vector3D(3, 0, -10);
        n.normalize();
        Vector3D v = incident.getDirection();
        v.normalize();
        v=-v;
        Vector3D l = (lightPos-point);
        l.normalize();
        Vector3D h = l+v;
        h.normalize();
        double val=  0.3*0.1 + 0.4*(0.5 * std::max(0.0,dotProduct(n,l))) + 0.8*(0.7* pow(std::max(0.0,dotProduct(n,h)), 256));

//        double val = 0.5 * std::max(0.0, dotProduct(n, l));

        Color color = Color(val *color.R(), val *color.G(), val *color.B());

        return color;
    }
    return color;

}
