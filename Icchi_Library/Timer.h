#pragma once

#include <Windows.h>
#include <exception>
#include <cstdint>

namespace ICL
{
	//シュミレーションタイミングを補助するクラス
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

			//デルタを1/10秒に初期化
			MaxDelta = Frequnency.QuadPart / 10;
		}

		//意図的なタイミングの不連続性の後
		//これを呼び出して、固定タイムステップ理論が一通り
		//追いついて更新を止める処理
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

		//タイマーを更新してUpdate関数を適切な回数を呼び出す処理
		template<template Times>
		void Loop(const Times& update)
		{
			//現在の時間を合わせる
			LARGE_INTEGER currentTime;

			if (!QueryPerformanceCounter(&currentTime))
			{
				throw std::exception("QueryPerformanceCounter");
			}

			uint64_t timeDelta = currentTime.QuadPart - LastTime.QuadPart;

			LastTime = currentTime;
			SecondCounter += timeDelta;

			//大きな時間をクランプ（デッバカーで一時停止した後）
			if (timeDelta > MaxDelta)
			{
				timeDelta = MaxDelta;
			}

			//QPC単位を正規に変換。前回のクランプによるオーバフローにはならない
			timeDelta *= TicksPerSeconde;
			timeDelta /= Frequnency.QuadPart;

			uint32_t lastFrameCount = FrameCount;

			if (isFixedTimeStep)
			{
				//固定タイム順番ロジック

				//アプリが目標経過時間が非常に近い状態だった時
				//目標値と正確に一致するように合わせる
				//これによって時間経過によって発生する無関係なエラーの蓄積を防ぐ
				//これが無ければNTSCディスプレイでvsyncを有効して実行される
				//60個のfpsfixedアップデートを要求するゲームは最終的に
				//フレームを落とすほどのエラーを蓄積した
				//スムーズにさせたいのならば編さをゼロに近づけるようにした方がいい

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
				//変数timeStepの更新
				ElapasedTicks = timeDelta;
				TotalTicks += timeDelta;
				LeftOverTicks = 0;
				FrameCount++;

				update();
			}

			//現在のフレームレートを追跡する
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
		
		//1フレーム前に呼び出したupdateの経過時間を取得
		uint64_t GetElapsedTicks() const { return ElapasedTicks; }
		double GetElapseSeconds() const { return TicksToSecon(ElapasedTicks); }

		//プログラムの開始の合計時間の取得
		uint64_t GetTotalTicks() const { return TotalTicks; }
		double GetYotalSeconds() const { return TicksToSecon(TotalTicks); }

		//プログラムの開始以降の更新総数を取得
		uint32_t GetFrameCount() const { return FrameCount; }

		//現在のフレームレートを取得
		uint32_t GetFramePreSecond() const { return FramePerSecond; }

		//固定又は可変タイムステップモードを使用するかの設定
		void SetFixedTimeStep(bool _FixedTimesStep) { isFixedTimeStep = _FixedTimesStep; }

		//固定モードの時に更新を呼び出す頻度の設定
		void SetTargetElapsedTicks(uint64_t targetElapsed) { TargeElapsedTicks = targetElapsed; }
		void SetTargetElapsedSeconds(double targetElapsed) { TargeElapsedTicks = targetElapsed; }

		//整数形式　10000000ティックを使用する時間
		static const uint64_t TicksPerSeconde = 10000000;

		static double TicksToSecon(uint64_t ticks) { return static_cast<double>(ticks) / TicksPerSeconde; }
		static uint64_t SecondsToTicks(double seconds) { return static_cast<uint64_t>(seconds * TicksPerSeconde); }
	private:
		//ソースタイミングデータにはQPCを使用する
		LARGE_INTEGER Frequnency;
		LARGE_INTEGER LastTime;
		uint64_t MaxDelta;

		//導出したタイミングデータは標準的な形式を使用する
		uint64_t ElapasedTicks;
		uint64_t TotalTicks;
		uint64_t LeftOverTicks;

		//フレームレートを追跡する変数
		uint32_t FrameCount;
		uint32_t FramePerSecond;
		uint32_t FrameThisSecond;
		uint64_t SecondCounter;

		//固定ステプモードを設定する変数
		bool isFixedTimeStep;
		uint16_t TargeElapsedTicks;
	};
}