/*!
\mainpage PlatyVG Libraries
\section information
    - 필압/색상/투명도/수분조절까지 가능한 고차원 벡터그래픽스
	- 크로스플랫폼(WIN/MAC/IPHONE/ANDROID...)기반 라이브러리(dll/so/a)
	- OpenVG 비의존적 벡터그래픽스(OpenGLES 2.0이상)
\section advanced
	- 입출력
		-# 터치스크린/타블렛/전자펜을 동시-드로잉할 수 있는 입력시스템
		-# 스크린좌표계/캔버스좌표계의 선택 및 민감도조절이 가능
		-# 간략보기가 가능한 PNG포맷으로 벡터정보를 저장 및 SVG저장
	- 작업환경
		-# 포토샾과 같이 전문가용 드로잉작업을 위한 다중레이어시스템
		-# 여러 캔버스를 함께 열고 복사작업등 할 수 있는 다중캔버스시스템
	    -# 편집모드를 위한 3가지 선택도구(드래그라인/사각영역/자율영역)
	- 성능
	    -# 0.01%~100000%(1천만배차)까지 자유로운 줌시스템
	    -# 줌/스크롤시 화면깜빡임현상(캐시버퍼-스케일링)이 전혀 없음
	    -# 프로세싱분리로 줌/스크롤 속도저하가 기존 벡터엔진의 1/10 이하
	    -# 공유자원시스템으로 메모리사용량이 기존 벡터엔진의 1/3 이하
\section developer
	- Platy House, Inc
*/
#pragma once
#include "../px/px.hpp"
#include "../px/array.hpp"
#include "../px/buffer.hpp"
#include "../px/file.hpp"
#include "../px/math.hpp"
#include "../px/memory.hpp"
#include "../px/parser.hpp"
#include "../px/share.hpp"
#include "../px/string.hpp"

namespace PlatyVGApi {}
namespace PlatyVGCore {}
namespace PlatyVGElement {}
using namespace PlatyVGApi;
using namespace PlatyVGCore;
using namespace PlatyVGElement;

namespace PlatyVGApi
{
    enum DeviceBy:api_enum {DeviceByTouch, DeviceByTablet, DeviceByPen, LengthOfDevice};
    enum StrokeBy:api_enum {StrokeBySpline, StrokeByTube, StrokeByAqua,
		StrokeByErase, StrokeBySelect, StrokeByHand, StrokeBySpoid, LengthOfStroke};
    enum SelectBy:api_enum {SelectByRect, SelectByDrag, SelectByArea, LengthOfSelect};
    enum MergeBy:api_enum {MergeByPlus, MergeByMinus, MergeByReverse, LengthOfMerge};
    enum BoundBy:api_enum {BoundByWidth, BoundByHeight, BoundByInner, BoundByOuter, LengthOfBound};
    enum ChangeColorBy:api_enum {ChangeColorByRGB, ChangeColorByAlpha, ChangeColorByAqua,
		ChangeColorByRGBAlpha, ChangeColorByRGBAqua, ChangeColorByAlphaAqua,
		ChangeColorByRGBAlphaAqua, LengthOfChangeColor};
}
