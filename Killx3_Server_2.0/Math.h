#pragma once

#include "protocol.h"

typedef struct _tagVector2 :
	public XMFLOAT2
{

	_tagVector2()
	{
		x = y = 0.f;
	}

	_tagVector2(float _x, float _y)
	{
		x = _x;
		y = _y;
	}

	_tagVector2 operator +(const _tagVector2& v)	const
	{
		_tagVector2	v1;
		v1.x = x + v.x;
		v1.y = y + v.y;
		return v1;
	}

	_tagVector2 operator +(float f)	const
	{
		_tagVector2	v;
		v.x = x + f;
		v.y = y + f;
		return v;
	}

	void operator +=(const _tagVector2& v)
	{
		x += v.x;
		y += v.y;
	}

	void operator +=(float f)
	{
		x += f;
		y += f;
	}

	_tagVector2 operator -(const _tagVector2& v)	const
	{
		_tagVector2	v1;
		v1.x = x - v.x;
		v1.y = y - v.y;
		return v1;
	}

	_tagVector2 operator -(float f)	const
	{
		_tagVector2	v;
		v.x = x - f;
		v.y = y - f;
		return v;
	}

	void operator -=(const _tagVector2& v)
	{
		x -= v.x;
		y -= v.y;
	}

	void operator -=(float f)
	{
		x -= f;
		y -= f;
	}

	_tagVector2 operator *(const _tagVector2& v)	const
	{
		_tagVector2	v1;
		v1.x = x * v.x;
		v1.y = y * v.y;
		return v1;
	}

	_tagVector2 operator *(float f)		const
	{
		_tagVector2	v;
		v.x = x * f;
		v.y = y * f;
		return v;
	}

	void operator *=(const _tagVector2& v)
	{
		x *= v.x;
		y *= v.y;
	}

	//½ºÄ®¶ó °ö
	void operator *=(float f)
	{
		x *= f;
		y *= f;
	}


	_tagVector2 operator /(const _tagVector2& v)	const
	{
		_tagVector2	v1;
		v1.x = x / v.x;
		v1.y = y / v.y;
		return v1;
	}

	_tagVector2 operator /(float f)	const
	{
		_tagVector2	v;
		v.x = x / f;
		v.y = y / f;
		return v;
	}

	void operator /=(const _tagVector2& v)
	{
		x /= v.x;
		y /= v.y;
	}

	void operator /=(float f)
	{
		x /= f;
		y /= f;
	}

	void operator =(const POINT& pt)
	{
		x = pt.x;
		y = pt.y;
	}

	bool operator ==(const _tagVector2& v) const
	{
		return x == v.x && y == v.y;
	}

	bool operator !=(const _tagVector2& v)const
	{
		return x != v.x || y != v.y;
	}

	float Length()
	{
		return sqrtf(x * x + y * y);
	}

	_tagVector2 Normalize()
	{
		_tagVector2 v;
		float f = Length();
		v.x = x / f;
		v.y = y / f;
		return v;
	}

	float Dot(const _tagVector2& v)
	{
		return x * v.x + y * v.y;
	}

}Vector2, *PVector2;


typedef struct _tagVector3 :
	public XMFLOAT3
{
	_tagVector3()
	{
		x = y = z = 0.f;
	}

	_tagVector3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	_tagVector3 operator +(const _tagVector3& v)	const
	{
		_tagVector3	v1;
		v1.x = x + v.x;
		v1.y = y + v.y;
		v1.z = z + v.z;
		return v1;
	}

	_tagVector3 operator +(float f)	const
	{
		_tagVector3	v;
		v.x = x + f;
		v.y = y + f;
		v.z = z + f;
		return v;
	}

	void operator +=(const _tagVector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}

	void operator +=(float f)
	{
		x += f;
		y += f;
		z += f;
	}

	_tagVector3 operator -(const _tagVector3& v)	const
	{
		_tagVector3	v1;
		v1.x = x - v.x;
		v1.y = y - v.y;
		v1.z = z - v.z;
		return v1;
	}

	_tagVector3 operator -(float f[3]) const
	{
		_tagVector3	v;
		v.x = x - f[0];
		v.y = y - f[1];
		v.z = z - f[2];
		return v;
	}

	_tagVector3 operator -(float f)	const
	{
		_tagVector3	v;
		v.x = x - f;
		v.y = y - f;
		v.z = z - f;
		return v;
	}

	void operator -=(const _tagVector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	void operator -=(float f)
	{
		x -= f;
		y -= f;
		z -= f;
	}

	_tagVector3 operator *(const _tagVector3& v)	const
	{
		_tagVector3	v1;
		v1.x = x * v.x;
		v1.y = y * v.y;
		v1.z = z * v.z;
		return v1;
	}

	_tagVector3 operator *(float f)	const
	{
		_tagVector3	v;
		v.x = x * f;
		v.y = y * f;
		v.z = z * f;
		return v;
	}

	void operator *=(const _tagVector3& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
	}

	//½ºÄ®¶ó °ö
	void operator *=(float f)
	{
		x *= f;
		y *= f;
		z *= f;
	}


	_tagVector3 operator /(const _tagVector3& v)	const
	{
		_tagVector3	v1;
		v1.x = x / v.x;
		v1.y = y / v.y;
		v1.z = z / v.z;
		return v1;
	}

	_tagVector3 operator /(float f)	const
	{
		_tagVector3	v;
		v.x = x / f;
		v.y = y / f;
		v.z = z / f;
		return v;
	}

	float operator [](UINT idx) const
	{
		if (idx == 0)
			return x;

		else if (idx == 1)
			return y;

		else if (idx == 2)
			return z;

		assert(false);
		return 0;
	}

	void operator /=(const _tagVector3& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
	}

	void operator /=(float f)
	{
		x /= f;
		y /= f;
		z /= f;
	}

	bool operator ==(const _tagVector3& v)const
	{
		return x == v.x && y == v.y && z == v.z;
	}

	bool operator !=(const _tagVector3& v)const
	{
		return x != v.x || y != v.y || z != v.z;
	}

	float Length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	_tagVector3 Normalize()	const
	{
		_tagVector3 v;
		float f = Length();
		v.x = x / f;
		v.y = y / f;
		v.z = z / f;
		return v;
	}

	float Dot(const _tagVector3& v)	const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	_tagVector3 Cross(const _tagVector3& v)	const
	{
		//_tagVector3 vCross;
		//vCross.Convert(XMVector3Cross(Convert(), v.Convert()));
		//return vCross;
		return _tagVector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	float GetAngle(_tagVector3 v)	const
	{
		v = v.Normalize();
		//ÇöÀçº¤ÅÍ Á¤±ÔÈ­
		_tagVector3 v1 = Normalize();
		//³»Àû°ª
		float fDot = v1.Dot(v);

		return acosf(fDot);
	}

	float Distance(const _tagVector3& v) const
	{
		_tagVector3 _v = *this - v;
		return _v.Length();
	}

	XMVECTOR Convert() const
	{
		return XMLoadFloat3(this);
	}

	void Convert(const XMVECTOR& v)
	{
		XMStoreFloat3(this, v);
	}

	void Max(const _tagVector3 v)
	{
		x = x < v.x ? v.x : x;
		y = y < v.y ? v.y : y;
		z = z < v.z ? v.z : z;
	}

	void Min(const _tagVector3 v)
	{
		x = x > v.x ? v.x : x;
		y = y > v.y ? v.y : y;
		z = z > v.z ? v.z : z;
	}

}Vector3, *PVector3;

typedef struct _tagVector4 :
	public XMFLOAT4
{
	_tagVector4()
	{
		x = y = z = w = 0.f;
	}

	_tagVector4(float _x, float _y, float _z, float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	_tagVector4 operator +(const _tagVector4& v)	const
	{
		_tagVector4	v1;
		v1.x = x + v.x;
		v1.y = y + v.y;
		v1.z = z + v.z;
		v1.w = w + v.w;
		return v1;
	}

	_tagVector4 operator +(float f)	const
	{
		_tagVector4	v;
		v.x = x + f;
		v.y = y + f;
		v.z = z + f;
		v.w = w + f;
		return v;
	}

	void operator +=(const _tagVector4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
	}

	void operator +=(float f)
	{
		x += f;
		y += f;
		z += f;
		w += f;
	}

	_tagVector4 operator -(const _tagVector4& v)	const
	{
		_tagVector4	v1;
		v1.x = x - v.x;
		v1.y = y - v.y;
		v1.z = z - v.z;
		v1.w = w - v.w;
		return v1;
	}

	_tagVector4 operator -(float f)	const
	{
		_tagVector4	v;
		v.x = x - f;
		v.y = y - f;
		v.z = z - f;
		v.w = w - f;
		return v;
	}

	void operator -=(const _tagVector4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
	}

	void operator -=(float f)
	{
		x -= f;
		y -= f;
		z -= f;
		w -= f;
	}

	_tagVector4 operator *(const _tagVector4& v)	const
	{
		_tagVector4	v1;
		v1.x = x * v.x;
		v1.y = y * v.y;
		v1.z = z * v.z;
		v1.w = w * v.w;
		return v1;
	}

	_tagVector4 operator *(float f)	const
	{
		_tagVector4	v;
		v.x = x * f;
		v.y = y * f;
		v.z = z * f;
		v.w = w * f;
		return v;
	}

	void operator *=(const _tagVector4& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
	}

	//½ºÄ®¶ó °ö
	void operator *=(float f)
	{
		x *= f;
		y *= f;
		z *= f;
		w *= f;
	}


	_tagVector4 operator /(const _tagVector4& v)	const
	{
		_tagVector4	v1;
		v1.x = x / v.x;
		v1.y = y / v.y;
		v1.z = z / v.z;
		v1.w = w / v.w;
		return v1;
	}

	_tagVector4 operator /(float f)	const
	{
		_tagVector4	v;
		v.x = x / f;
		v.y = y / f;
		v.z = z / f;
		v.w = w / f;
		return v;
	}

	void operator /=(const _tagVector4& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
	}

	void operator /=(float f)
	{
		x /= f;
		y /= f;
		z /= f;
		w /= f;
	}

	void operator =(const _tagVector3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	bool operator ==(const _tagVector4& v)const
	{
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}

	bool operator !=(const _tagVector4& v)const
	{
		return x != v.x || y != v.y || z != v.z || w != v.w;
	}

	float Length()
	{
		return sqrtf(x * x + y * y + z * z + w * w);
	}

	void Normalize()
	{
		x /= Length();
		y /= Length();
		z /= Length();
		w /= Length();
	}

	XMVECTOR Convert() const
	{
		return XMLoadFloat4(this);
	}

	void Convert(const XMVECTOR& v)
	{
		XMStoreFloat4(this, v);
	}

}Vector4, *PVector4;


typedef Vector2 Vec2;
typedef Vector3 Vec3;
typedef Vector4 Vec4;
typedef XMVECTOR Vector;
typedef Vector4 Quaternion;
typedef XMMATRIX Matrix;