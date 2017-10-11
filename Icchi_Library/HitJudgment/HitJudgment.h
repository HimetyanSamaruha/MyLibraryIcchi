#pragma once
//	物体同士のあたり判定処理
#include <d3d11_1.h>

namespace IchhiLibrary
{
	//各形の設定

	//線分
	class Segment
	{
	public:
		
	};

	//球
	class Sphere
	{
	public:

	};

	//箱
	class Box
	{
	public:

	};

	//三角形(平面)
	class Triangle
	{
	public:

	};

	//カプセル
	class Capsule
	{
	public:

	};

	//=========================
	//各形のあたり判定
	//=========================

	//箱と箱(AABB)
	bool BoxToBox(Box box1, Box box2);

	//箱と箱(OBB)
	bool BoxToBox2(Box box1, Box box2);

	//箱と球
	bool BoxToSphere(Box box, Sphere shere);

	//箱とカプセル
	bool BoxToCapsule(Box box, Capsule capule);

	//球と球
	bool SphereToSphere(Sphere sphere1, Sphere sphere2);

	//球とカプセル
	bool SphereToCapsule(Sphere sphere, Capsule capsule);

	//カプセルとカプセル
	bool CapsuleToCapsule(Capsule capsule1, Capsule capsule2);

}