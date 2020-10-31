#include "world.h"
#include "iostream"
using namespace std;

float World::firstIntersection(Ray& ray)
{
	for(int i=0; i<objectList.size(); i++)
		objectList[i]->intersect(ray);
	return ray.getParameter();
}
int setlevel =0;
Color World::shade_ray(Ray& ray)
{
    Vector3D lightDir;
	firstIntersection(ray);
	if(ray.didHit()) {
        lightDir = Vector3D(-10, 3, 2) - ray.getPosition();
        Vector3D n = ray.getPosition() - Vector3D(3, 0, -10);
        n.normalize();
        Vector3D reflectDir = ray.getDirection() - 2*(dotProduct(n,ray.getDirection()))*n;
        Vector3D reflectPos = ray.getPosition();

        Ray shadowRay(ray.getPosition(), lightDir);
        Ray reflectedRay(reflectPos, reflectDir, setlevel);
//        std::cout<<reflectedRay.getLevel()<<std::endl;
        for(int i=0; i<objectList.size(); i++){
            objectList[i]->intersect(shadowRay);
//            objectList[i]->intersect(reflectedRay);
        }
//        std::cout<<reflectedRay.getLevel()<<std::endl;
        if(shadowRay.didHit())
        {
            Color shadow = Color(0.1, 0.1, 0.1);
            return shadow;
        }
        else{
            if (reflectedRay.getLevel()<3) {
                setlevel++;
            }

            return (ray.intersected())->shade(ray) + shade_ray(reflectedRay);


        }

    }
	return background;
}
