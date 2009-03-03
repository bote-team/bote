#pragma once
#include <math.h>

#define PI	3.14159265

template<class T>
class vec3
{
public:
    T x, y, z;

    vec3 (void) 
      {x = y = z = 0;};
    vec3 (const T X, const T Y, const T Z) 
      { x=X; y=Y; z=Z; };
    vec3 (const vec3& v) 
      { x=v.x; y=v.y; z=v.z; };
    vec3 (T v[3])
      { x=v[0]; y=v[1]; z=v[2]; };
    void Set (const T X, const T Y, const T Z)
      { x=X; y=Y; z=Z; }
    void Set (T v[3])
      { x=v[0]; y=v[1]; z=v[2]; };

    operator T*()                             // T * CONVERSION
      { return (T *)&x; }
    operator const T*() const                 // CONST T * CONVERSION
      { return &x; }

    int operator == (const vec3& A)              // COMPARISON (==)
      { return (x==A.x && y==A.y && z==A.z); }

	int operator != (const vec3& A)              // COMPARISON (!=)
      { return (x!=A.x || y!=A.y || z!=A.z); }

    vec3& operator = (const vec3& A)            // ASSIGNMENT (=)
      { x=A.x; y=A.y; z=A.z; 
        return(*this);  };
    vec3 operator + (const vec3& A) const       // ADDITION (+)
      { vec3 Sum(x+A.x, y+A.y, z+A.z); 
        return(Sum); };
    vec3 operator - (const vec3& A) const       // SUBTRACTION (-)
      { vec3 Diff(x-A.x, y-A.y, z-A.z);
        return(Diff); };
    T operator * (const vec3& A) const       // DOT-PRODUCT (*)
      { T DotProd = x*A.x+y*A.y+z*A.z; 
        return(DotProd); };
    vec3 operator / (const vec3& A) const       // CROSS-PRODUCT (/)
      { vec3 CrossProd(y*A.z-z*A.y, z*A.x-x*A.z, x*A.y-y*A.x);
        return(CrossProd); };
    vec3 operator * (const T s) const        // MULTIPLY BY SCALAR V*s (*)
      { vec3 Scaled(x*s, y*s, z*s); 
        return(Scaled); };
    vec3 operator / (const T s) const        // DIVIDE BY SCALAR (/)
      { vec3 Scaled(x/s, y/s, z/s);
        return(Scaled); };
	vec3 operator + (const T s) const        // ADDITION WITH SCALAR V+s (+)
      { vec3 Add(x+s, y+s, z+s); 
        return(Add); };
    vec3 operator - (const T s) const        // SUBTRACTION WITH SCALAR V-s (-)
      { vec3 Sub(x-s, y-s, z-s);
        return(Sub); };

    friend inline vec3 operator *(T s, const vec3& v)  // SCALAR MULT s*V
      { return vec3(v.x*s, v.y*s, v.z*s); }

    void operator += (const vec3 A)              // ACCUMULATED VECTOR ADDITION (+=)
      { x+=A.x; y+=A.y; z+=A.z; };
    void operator -= (const vec3 A)              // ACCUMULATED VECTOR SUBTRACTION (+=)
      { x-=A.x; y-=A.y; z-=A.z; };
    void operator *= (const T s)              // ACCUMULATED SCALAR MULT (*=)
      { x*=s; y*=s; z*=s; };
    void operator /= (const T s)              // ACCUMULATED SCALAR DIV (/=)
      { x/=s; y/=s; z/=s; };
    vec3 operator - (void) const                 // NEGATION (-)
      { vec3 Negated(-x, -y, -z);
        return(Negated); };

    const T& operator [] (const int i) const         // ALLOWS VECTOR ACCESS AS AN ARRAY.
      { return( (i==0)?x:((i==1)?y:z) ); };
    T & operator [] (const int i)
      { return( (i==0)?x:((i==1)?y:z) ); };

    T Length (void) const                     // LENGTH OF VECTOR
      { return ((T)sqrt((double)(x*x+y*y+z*z))); };
    T LengthSqr (void) const                  // LENGTH OF VECTOR (SQUARED)
      { return (x*x+y*y+z*z); };
    void Normalize (void)                     // NORMALIZE VECTOR
      { T L = Length();                       // CALCULATE LENGTH
        if (L>0) { x/=L; y/=L; z/=L; } };     // DIV COMPONENTS BY LENGTH

    void UpdateMinMax(vec3 &Min, vec3 &Max)
    {
      if (x<Min.x) Min.x=x; else if (x>Max.x) Max.x=x;
      if (y<Min.y) Min.y=y; else if (y>Max.y) Max.y=y;
      if (z<Min.z) Min.z=z; else if (z>Max.z) Max.z=z;
    }

	T Distance(const vec3 &B)
	{
		vec3<T> C(x-B.x, y-B.y, z-B.z);
		return C.Length();
	}

	bool IsNull()
	{
		return (x == 0 && y == 0 && z == 0);
	}

	//T ScalarProduct(const vec3 &B)				// SCALARPRODUCT
	//{
	//	return x * B.x + y * B.y + z * B.z;		
	//}
};

typedef vec3<int>           vec3i;
typedef vec3<float>         vec3f;
typedef vec3<double>        vec3d;