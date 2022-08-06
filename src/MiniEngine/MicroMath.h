#pragma once

// TODO:
// http://www.realtimecollisiondetection.net/pubs/Tolerances/

//-----------------------------------------------------------------------------
// Vector2
//-----------------------------------------------------------------------------
struct Vector2
{
	Vector2() = default;
	Vector2(float f) : x(f), y(f) {}
	Vector2(float nx, float ny) : x(nx), y(ny) {}
	Vector2(const Vector2&) = default;
	Vector2(Vector2&&) = default;

	Vector2& operator=(const Vector2&) = default;
	Vector2& operator=(Vector2&&) = default;

	const Vector2 operator*(float f) const { return { x * f, y * f }; }
	const Vector2 operator*(const Vector2& rhs) const { return { x * rhs.x, y * rhs.y }; }
	const Vector2 operator/(float f) const { return { x / f, y / f }; }
	const Vector2 operator/(const Vector2& rhs) const { return { x / rhs.x, y / rhs.y }; }
	const Vector2 operator+(float f) const { return { x + f, y + f }; }
	const Vector2 operator+(const Vector2& v) const { return { x + v.x, y + v.y }; }
	const Vector2 operator-() const { return { -x, -y }; }
	const Vector2 operator-(float f) const { return { x - f, y - f }; }
	const Vector2 operator-(const Vector2& v) const { return { x - v.x, y - v.y }; }

	float* operator&() { return (float*)this; };
	operator float*() { return &x; }
	operator const float*() const { return &x; }

	float operator[](size_t i) { return (&x)[i]; }
	const float operator[](size_t i) const { return (&x)[i]; }

	void Set(float nx, float ny) { x = nx; y = ny; }

	[[nodiscard]] float DotProduct(const Vector2& v) const { return x * v.x + y * v.y; }
	[[nodiscard]] float LengthSquared() const { return x * x + y * y; }
	[[nodiscard]] float Length() const { return sqrt(x * x + y * y); }
	[[nodiscard]] float Distance(const Vector2& v) const { return (*this - v).Length(); }
	[[nodiscard]] Vector2 Normalized() const { return *this / Length(); }
	void Normalize() { const float invLen = 1.0f / Length(); x *= invLen; y *= invLen; }

	float x = 0.0f;
	float y = 0.0f;
};

inline Vector2 operator*(float lhs, const Vector2& rhs) { return rhs * lhs; }
inline Vector2 operator+(float lhs, const Vector2& rhs) { return rhs + lhs; }

//-----------------------------------------------------------------------------
// Vector3
//-----------------------------------------------------------------------------
struct Vector3
{
	Vector3() = default;
	Vector3(float nx, float ny, float nz) : x(nx), y(ny), z(nz) {}
	Vector3(float f) : x(f), y(f), z(f) {}
	Vector3(const Vector3&) = default;
	Vector3(Vector3&&) = default;

	Vector3& operator=(const Vector3&) = default;
	Vector3& operator=(Vector3&&) = default;

	const Vector3 operator*(float f) const { return { x * f, y * f, z * f }; }
	const Vector3 operator*(const Vector3& rhs) const { return { x * rhs.x, y * rhs.y, z * rhs.z }; }
	const Vector3 operator/(float f) const { return { x / f, y / f, z / f }; }
	const Vector3 operator/(const Vector3& rhs) const { return { x / rhs.x, y / rhs.y, z / rhs.z }; }
	const Vector3 operator+(float f) const { return { x + f, y + f, z + f }; }
	const Vector3 operator+(const Vector3& v) const { return { x + v.x, y + v.y, z + v.z }; }
	const Vector3 operator-() const { return { -x, -y, -z }; }
	const Vector3 operator-(float f) const { return { x - f, y - f, z - f }; }
	const Vector3 operator-(const Vector3& v) const { return { x - v.x, y - v.y, z - v.z }; }

	void Set(float nx, float ny, float nz) { x = nx; y = ny; z = nz; }

	float* operator&() { return (float*)this; }
	operator float* () { return &x; }
	operator const float* () const { return &x; }

	float operator[](size_t i) { return (&x)[i]; }
	const float operator[](size_t i) const { return (&x)[i]; }

	[[nodiscard]] float DotProduct(const Vector3& v) const { return x * v.x + y * v.y + z * v.z; }
	[[nodiscard]] Vector3 CrossProduct(const Vector3& v) const { return { y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x }; }

	[[nodiscard]] float LengthSquared() const { return x * x + y * y + z * z; }
	[[nodiscard]] float Length() const { return sqrt(x * x + y * y + z * z); }
	[[nodiscard]] float Distance(const Vector3& v) const { return (*this - v).Length(); }
	[[nodiscard]] Vector3 Normalized() const { return *this / Length(); }
	void Normalize() { const float invLen = 1.0f / Length(); x *= invLen; y *= invLen; z *= invLen; }

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

inline Vector3 operator+(float lhs, const Vector3& rhs) { return rhs + lhs; }
inline Vector3 operator*(float lhs, const Vector3& rhs) { return rhs * lhs; }

//-----------------------------------------------------------------------------
// Math func
//-----------------------------------------------------------------------------
inline float DotProduct(const Vector2& v1, const Vector2& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

inline float DotProduct(const Vector3& v1, const Vector3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vector3 CrossProduct(const Vector3& v1, const Vector3& v2)
{
	return {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x };
}

inline float Length(const Vector2& v)
{
	return v.Length();
}

inline float Length(const Vector3& v)
{
	return v.Length();
}

inline float Distance(const Vector2& v1, const Vector2& v2)
{
	return Length(v1 - v2);
}

inline float Distance(const Vector3& v1, const Vector3& v2)
{
	return Length(v1 - v2);
}

inline Vector2 Normalized(const Vector2& v)
{
	return v / Length(v);
}

inline Vector3 Normalized(const Vector3& v)
{
	return v / Length(v);
}

inline Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t)
{
	return v1 * (1.0f - t) + v2 * t;
}

inline Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t)
{
	return v1 * (1.0f - t) + v2 * t;
}