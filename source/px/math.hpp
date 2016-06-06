#pragma once
#include "../px/px.hpp"

namespace Px
{
	//! \brief 수학함수지원
	class Math
	{
	public:
		/*!
		\brief 절대값 구하기
		\param v : 인수
		\return 절대값
		*/
		inline static sint32 Abs(const sint32 v)
		{return (v < 0)? -v : v;}

		/*!
		\brief 최소 구하기
		\param a : 비교A
		\param b : 비교B
		\return 최소
		\see Max
		*/
		inline static sint32 Min(const sint32 a, const sint32 b)
		{return (a < b)? a : b;}

		/*!
		\brief 최대 구하기
		\param a : 비교A
		\param b : 비교B
		\return 최대
		\see Min
		*/
		inline static sint32 Max(const sint32 a, const sint32 b)
		{return (a > b)? a : b;}

		/*!
		\brief 절대값 구하기
		\param v : 인수
		\return 절대값
		*/
		inline static float AbsF(const float v)
		{return (v < 0)? -v : v;}

		/*!
		\brief 최소 구하기(float)
		\param a : 비교A
		\param b : 비교B
		\return 최소
		\see MaxF
		*/
		static float MinF(const float a, const float b);

		/*!
		\brief 최대 구하기(float)
		\param a : 비교A
		\param b : 비교B
		\return 최대
		\see MinF
		*/
		static float MaxF(const float a, const float b);

		/*!
		\brief 제곱근 구하기
		\param v : 인수
		\return 제곱근
		*/
		static float Sqrt(const float v);

		/*!
		\brief 코사인 구하기
		\param v : 인수
		\return 코사인
		*/
		static float Cos(const float v);

		/*!
		\brief 사인 구하기
		\param v : 인수
		\return 사인
		*/
		static float Sin(const float v);

		/*!
		\brief 아크탄젠트 구하기
		\param x : 변화량X
		\param y : 변화량Y
		\return 아크탄젠트
		*/
		static float Atan(const float x, const float y);

		/*!
		\brief 거리 구하기
		\param x1 : 좌표X1
		\param y1 : 좌표Y1
		\param x2 : 좌표X2
		\param y2 : 좌표Y2
		\return 거리
		*/
		static float Distance(const float x1, const float y1, const float x2, const float y2);
	};
}
