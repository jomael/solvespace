
#ifndef __DSC_H
#define __DSC_H

typedef unsigned long DWORD;
typedef unsigned char BYTE;

class Vector;

class Quaternion {
public:
    // a + (vx)*i + (vy)*j + (vz)*k
    double w, vx, vy, vz;

    static Quaternion MakeFrom(double w, double vx, double vy, double vz);
    static Quaternion MakeFrom(Vector u, Vector v);

    Quaternion Plus(Quaternion b);
    Quaternion Minus(Quaternion b);
    Quaternion ScaledBy(double s);
    double Magnitude(void);
    Quaternion WithMagnitude(double s);
    
    // Call a rotation matrix [ u' v' n' ]'; this returns the first and
    // second rows, where that matrix is generated by this quaternion
    Vector RotationU(void);
    Vector RotationV(void);
    Vector RotationN(void);
};

class Vector {
public:
    double x, y, z;
    
    static Vector MakeFrom(double x, double y, double z);

    bool Equals(Vector v);
    Vector Plus(Vector b);
    Vector Minus(Vector b);
    Vector Negated(void);
    Vector Cross(Vector b);
    double Dot(Vector b);
    Vector Normal(int which);
    Vector RotatedAbout(Vector axis, double theta);
    double Magnitude(void);
    Vector WithMagnitude(double s);
    Vector ScaledBy(double s);
};

class Point2d {
public:
    double x, y;

    Point2d Plus(Point2d b);
    Point2d Minus(Point2d b);
    Point2d ScaledBy(double s);
    double DistanceTo(Point2d p);
    double DistanceToLine(Point2d p0, Point2d dp, bool segment);
    double Magnitude(void);
    Point2d WithMagnitude(double v);
};

template <class T, class H>
class IdList {
public:
    T     *elem;
    int   n;
    int   elemsAllocated;

    H AddAndAssignId(T *t) {
        int i;
        DWORD id = 0;

        for(i = 0; i < n; i++) {
            id = max(id, elem[i].h.v);
        }

        t->h.v = (id + 1);
        Add(t);

        return t->h;
    }

    void Add(T *t) {
        if(n >= elemsAllocated) {
            elemsAllocated = (elemsAllocated + 32)*2;
            elem = (T *)MemRealloc(elem, elemsAllocated*sizeof(elem[0]));
            if(!elem) oops();
        }

        elem[n] = *t;
        n++;
    }
    
    T *FindById(H h) {
        T *t = FindByIdNoOops(h);
        if(!t) {
            dbp("failed to look up item %08x, searched %d items", h.v, n);
            oops();
        }
        return t;
    }

    T *FindByIdNoOops(H h) {
        int i;
        for(i = 0; i < n; i++) {
            if(elem[i].h.v == h.v) {
                return &(elem[i]);
            }
        }
        return NULL;
    }

    void ClearTags(void) {
        int i;
        for(i = 0; i < n; i++) {
            elem[i].tag = 0;
        }
    }

    void Tag(H h, int tag) {
        int i;
        for(i = 0; i < n; i++) {
            if(elem[i].h.v == h.v) {
                elem[i].tag = tag;
            }
        }
    }

    void RemoveTagged(void) {
        int src, dest;
        dest = 0;
        for(src = 0; src < n; src++) {
            if(elem[src].tag) {
                // this item should be deleted
            } else {
                if(src != dest) {
                    elem[dest] = elem[src];
                }
                dest++;
            }
        }
        n = dest;
        // and elemsAllocated is untouched, because we didn't resize
    }

    void MoveSelfInto(IdList<T,H> *l) {
        memcpy(l, this, sizeof(*this));
        elemsAllocated = n = 0;
        elem = NULL;
    }

    void DeepCopyInto(IdList<T,H> *l) {
        l->elem = (T *)MemAlloc(elemsAllocated * sizeof(elem[0]));
        memcpy(l->elem, elem, elemsAllocated * sizeof(elem[0]));
        l->elemsAllocated = elemsAllocated;
        l->n = n;
    }

    void Clear(void) {
        elemsAllocated = n = 0;
        if(elem) MemFree(elem);
        elem = NULL;
    }

};

class NameStr {
public:
    char str[20];

    inline void strcpy(char *in) {
        memcpy(str, in, min(strlen(in)+1, sizeof(str)));
        str[sizeof(str)-1] = '\0';
    }
};

#endif
