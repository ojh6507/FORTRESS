#pragma once

struct FVector3
{
	float x, y, z;
	FVector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
	FVector3 operator*(float scalar) const {
		return FVector3(x * scalar, y * scalar, z * scalar);
	}
	FVector3 operator*(const FVector3& rhs) const {
		return FVector3(x * rhs.x, y * rhs.y, z * rhs.z);
	}
	FVector3 operator-(const FVector3& rhs) const {
		return FVector3(x - rhs.x, y - rhs.y, z - rhs.z);
	}
	FVector3 operator+(const FVector3& rhs) const {
		return FVector3(x + rhs.x, y + rhs.y, z + rhs.z);
	}
	FVector3 operator/(float scalar) const {
		if (scalar == 0) return FVector3();
		return FVector3(x / scalar, y / scalar, z / scalar);
	}
	FVector3& operator*=(float scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
	FVector3& operator+=(const FVector3& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}
	FVector3& operator-=(const FVector3& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	float Magnitude() const
	{
		return std::sqrt(x * x + y * y + z * z);
	}
	FVector3 Normalized() const
	{
		float length = Magnitude();
		if (length != 0)
			return *this / length;

		return FVector3();
	}
};