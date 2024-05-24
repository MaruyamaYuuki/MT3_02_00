#define _USE_MATH_DEFINES
#include <math.h>
#include "Vector3.h"

class ExpantionVector3 {
public:
	Vector3 Add(const Vector3& v1, const Vector3& v2);
	Vector3 Subtract(const Vector3& v1, const Vector3& v2);
	Vector3 Multiply(float scalar, const Vector3& v);
	float Dot(const Vector3& v1, const Vector3& v2);
	float Length(const Vector3& v);
	Vector3 Normalize(const Vector3& v);
};