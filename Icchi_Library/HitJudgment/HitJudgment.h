#pragma once
//	���̓��m�̂����蔻�菈��
#include <d3d11_1.h>

namespace IchhiLibrary
{
	//�e�`�̐ݒ�

	//����
	class Segment
	{
	public:
		
	};

	//��
	class Sphere
	{
	public:

	};

	//��
	class Box
	{
	public:

	};

	//�O�p�`(����)
	class Triangle
	{
	public:

	};

	//�J�v�Z��
	class Capsule
	{
	public:

	};

	//=========================
	//�e�`�̂����蔻��
	//=========================

	//���Ɣ�(AABB)
	bool BoxToBox(Box box1, Box box2);

	//���Ɣ�(OBB)
	bool BoxToBox2(Box box1, Box box2);

	//���Ƌ�
	bool BoxToSphere(Box box, Sphere shere);

	//���ƃJ�v�Z��
	bool BoxToCapsule(Box box, Capsule capule);

	//���Ƌ�
	bool SphereToSphere(Sphere sphere1, Sphere sphere2);

	//���ƃJ�v�Z��
	bool SphereToCapsule(Sphere sphere, Capsule capsule);

	//�J�v�Z���ƃJ�v�Z��
	bool CapsuleToCapsule(Capsule capsule1, Capsule capsule2);

}