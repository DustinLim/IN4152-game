#ifndef VERTEX_H
#define VERTEX_H

#pragma once
#include "Vec3D.h"

/************************************************************
 * Mesh vertex class. This consists of a normal n and vertex v.
 ************************************************************/
class Vertex {
public:
    inline Vertex () {}
    inline Vertex (const Vec3Df & p) : p (p) {}
    inline Vertex (const Vec3Df & p, const Vec3Df & n) : p (p), n (n) {}
    inline Vertex (const Vertex & v) : p (v.p), n (v.n) {}
    inline virtual ~Vertex () {}
    inline Vertex & operator= (const Vertex & v) {
        p = v.p;
        n = v.n;
        return (*this);
    }
    Vec3Df p;
    Vec3Df n;
};


#endif // VERTEX_H
