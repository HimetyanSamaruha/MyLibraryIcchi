#pragma once

#include <Windows.h>
#include <exception>
#include <cstdint>

namespace ICL
{
	//�V���~���[�V�����^�C�~���O��⏕����N���X
	class Timer
	{
	public:
		Timer() :
			ElapasedTicks(0),
			TotalTicks(0),
			LeftOverTicks(0),
			FrameCount(0),
			FramePerSecond(0),
			FrameThisSecond(0),
			SecondCounter(0),
			isFixedTimeStep(false),
			TargeElapsedTicks(TicksPerSeconde / 60)

		{
			if (!QueryPerformanceFrequency(&Frequnency))
			{
				throw std::exception("QueryPerformanceFrequency");
			}

			if (!QueryPerformanceFrequency(&LastTime))
			{
				throw std::exception("QueryPerformanceLastTime");
			}

			//�f���^��1/10�b�ɏ�����
			MaxDelta = Frequnency.QuadPart / 10;
		}

		//�Ӑ}�I�ȃ^�C�~���O�̕s�A�����̌�
		//������Ăяo���āA�Œ�^�C���X�e�b�v���_����ʂ�
		//�ǂ����čX�V���~�߂鏈��
		void ResetElapasedTimes()
		{
			if (!QueryPerformanceFrequency(&LastTime))
			{
				throw std::exception("QueryPerformanceLastTime");
			}

			LeftOverTicks = 0;
			FramePerSecond = 0;
			FrameThisSecond = 0;
			SecondCounter = 0;
		}

		//�^�C�}�[���X�V����Update�֐���K�؂ȉ񐔂��Ăяo������
		template<template Times>
		void Loop(const Times& update)
		{
			//���݂̎��Ԃ����킹��
			LARGE_INTEGER currentTime;

			if (!QueryPerformanceCounter(&currentTime))
			{
				throw std::exception("QueryPerformanceCounter");
			}

			uint64_t timeDelta = currentTime.QuadPart - LastTime.QuadPart;

			LastTime = currentTime;
			SecondCounter += timeDelta;

			//�傫�Ȏ��Ԃ��N�����v�i�f�b�o�J�[�ňꎞ��~������j
			if (timeDelta > MaxDelta)
			{
				timeDelta = MaxDelta;
			}

			//QPC�P�ʂ𐳋K�ɕϊ��B�O��̃N�����v�ɂ��I�[�o�t���[�ɂ͂Ȃ�Ȃ�
			timeDelta *= TicksPerSeconde;
			timeDelta /= Frequnency.QuadPart;

			uint32_t lastFrameCount = FrameCount;

			if (isFixedTimeStep)
			{
				//�Œ�^�C�����ԃ��W�b�N

				//�A�v�����ڕW�o�ߎ��Ԃ����ɋ߂���Ԃ�������
				//�ڕW�l�Ɛ��m�Ɉ�v����悤�ɍ��킹��
				//����ɂ���Ď��Ԍo�߂ɂ���Ĕ������閳�֌W�ȃG���[�̒~�ς�h��
				//���ꂪ�������NTSC�f�B�X�v���C��vsync��L�����Ď��s�����
				//60��fpsfixed�A�b�v�f�[�g��v������Q�[���͍ŏI�I��
				//�t���[���𗎂Ƃ��قǂ̃G���[��~�ς���
				//�X���[�Y�ɂ��������̂Ȃ�Ε҂����[���ɋ߂Â���悤�ɂ�����������

				if(abs(static_cast<int64_t>(timeDelta - TargeElapsedTicks)) < TicksPerSeconde / 4000))
				{
					timeDelta = TargeElapsedTicks;
				}

				LeftOverTicks += timeDelta;

				while (LeftOverTicks >= TargeElapsedTicks)
				{
					ElapasedTicks = TargeElapsedTicks;
					TotalTicks += TargeElapsedTicks;
					LeftOverTicks -= TargeElapsedTicks;

					update();
				}
			}
			else
			{
				//�ϐ�timeStep�̍X�V
				ElapasedTicks = timeDelta;
				TotalTicks += timeDelta;
				LeftOverTicks = 0;
				FrameCount++;

				update();
			}

			//���݂̃t���[�����[�g��ǐՂ���
			if (FrameCount != lastFrameCount)
			{
				FrameThisSecond++;
			}

			if (SecondCounter >= static_cast<uint64_t>(Frequnency.QuadPart))
			{
				FramePerSecond = FrameThisSecond;
				FrameThisSecond = 0;
				SecondCounter %= Frequnency.QuadPart;
			}
		}
		
		//1�t���[���O�ɌĂяo����update�̌o�ߎ��Ԃ��擾
		uint64_t GetElapsedTicks() const { return ElapasedTicks; }
		double GetElapseSeconds() const { return TicksToSecon(ElapasedTicks); }

		//�v���O�����̊J�n�̍��v���Ԃ̎擾
		uint64_t GetTotalTicks() const { return TotalTicks; }
		double GetYotalSeconds() const { return TicksToSecon(TotalTicks); }

		//�v���O�����̊J�n�ȍ~�̍X�V�������擾
		uint32_t GetFrameCount() const { return FrameCount; }

		//���݂̃t���[�����[�g���擾
		uint32_t GetFramePreSecond() const { return FramePerSecond; }

		//�Œ薔�͉σ^�C���X�e�b�v���[�h���g�p���邩�̐ݒ�
		void SetFixedTimeStep(bool _FixedTimesStep) { isFixedTimeStep = _FixedTimesStep; }

		//�Œ胂�[�h�̎��ɍX�V���Ăяo���p�x�̐ݒ�
		void SetTargetElapsedTicks(uint64_t targetElapsed) { TargeElapsedTicks = targetElapsed; }
		void SetTargetElapsedSeconds(double targetElapsed) { TargeElapsedTicks = targetElapsed; }

		//�����`���@10000000�e�B�b�N���g�p���鎞��
		static const uint64_t TicksPerSeconde = 10000000;

		static double TicksToSecon(uint64_t ticks) { return static_cast<double>(ticks) / TicksPerSeconde; }
		static uint64_t SecondsToTicks(double seconds) { return static_cast<uint64_t>(seconds * TicksPerSeconde); }
	private:
		//�\�[�X�^�C�~���O�f�[�^�ɂ�QPC���g�p����
		LARGE_INTEGER Frequnency;
		LARGE_INTEGER LastTime;
		uint64_t MaxDelta;

		//���o�����^�C�~���O�f�[�^�͕W���I�Ȍ`�����g�p����
		uint64_t ElapasedTicks;
		uint64_t TotalTicks;
		uint64_t LeftOverTicks;

		//�t���[�����[�g��ǐՂ���ϐ�
		uint32_t FrameCount;
		uint32_t FramePerSecond;
		uint32_t FrameThisSecond;
		uint64_t SecondCounter;

		//�Œ�X�e�v���[�h��ݒ肷��ϐ�
		bool isFixedTimeStep;
		uint16_t TargeElapsedTicks;
	};
}