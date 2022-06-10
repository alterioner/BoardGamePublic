
// BoardGameView.cpp: CBoardGameView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "BoardGame.h"
#endif

#include "BoardGameDoc.h"
#include "BoardGameView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define EMPTY 101

// CBoardGameView

IMPLEMENT_DYNCREATE(CBoardGameView, CView)

BEGIN_MESSAGE_MAP(CBoardGameView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CBoardGameView 생성/소멸

CBoardGameView::CBoardGameView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.
	isFirst = true;
	newGame = true;
	tempStr = L"ㅇ";
}

CBoardGameView::~CBoardGameView()
{
}

BOOL CBoardGameView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CBoardGameView 그리기

void CBoardGameView::OnDraw(CDC* pDC)
{
	CBoardGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	pDC->Rectangle(0, 0, winX, winY);		//화면 덮어쓰기
	TwelveGame(pDC);

	//DC->TextOutW(0, 50, str);

	//for (int c = 0; c < column; c++)
	//{
	//	for (int r = 0; r < row; r++)
	//	{
	//		str.Format(L"%d", boardSpace[c][r].isSet);
	//		pDC->DrawText(str, boardSpace[c][r].rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	//	}
	//}

	//str.Format(L"%d활성, %d잡힘", actObjIndex, tempObjIndex);
	//if (isActive || isAnimating)
	//	pDC->TextOutW(10, 100, str);

	//pDC->TextOutW(600, 600, tempStr);
}


// CBoardGameView 인쇄

BOOL CBoardGameView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CBoardGameView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CBoardGameView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CBoardGameView 진단

#ifdef _DEBUG
void CBoardGameView::AssertValid() const
{
	CView::AssertValid();
}

void CBoardGameView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBoardGameDoc* CBoardGameView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBoardGameDoc)));
	return (CBoardGameDoc*)m_pDocument;
}
#endif //_DEBUG


// CBoardGameView 메시지 처리기

void CBoardGameView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	winX = cx;
	winY = cy;
}

void CBoardGameView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	switch (nIDEvent)
	{
	case 0:
		imgFrame++;
		break;
	}

	Invalidate(0);
	CView::OnTimer(nIDEvent);
}

void CBoardGameView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (!isAnimating)
	{
		if (isActive)
		{
			if (object[actObjIndex].isSet && CanMove(point))
			{
				if (turn == L"Green")
					turn = L"Red";
				else if (turn == L"Red")
					turn = L"Green";

				doMove = true;
				isFirst = true;
			}
			else if (!object[actObjIndex].isSet && IsEmptySpace(point))
			{
				if (turn == L"Green")
					turn = L"Red";
				else if (turn == L"Red")
					turn = L"Green";

				doSet = true;
				isFirst = true;
			}

			isActive = false;
		}
		else if (IsObject(point))
		{
			isActive = true;
		}
	}

	Invalidate(0);
	CView::OnLButtonUp(nFlags, point);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
//기능 함수

bool CBoardGameView::DrawFromFile(CPoint point, PCWSTR filePath)
{
	CImage image;
	if (FAILED(image.Load(filePath)))
	{
		return FALSE;
	}
	int width = image.GetWidth();		//이미지의 넓이 저장
	int height = image.GetHeight();		//이미지의 높이 저장

	image.Draw(DC->m_hDC, point.x - (width / 2), point.y - (height / 2));	//이미지가 입력된 위치의 가운데에 그려지게 조정
	image.Detach();
	return TRUE;
}

//중심점 좌표값을 받아 사각형좌표로 변환
//(중심점, 넓이, 높이)
CRect CBoardGameView::PointToRect(CPoint point, int width, int height)
{
	CRect rect = CRect(point.x - (width / 2), point.y - (height / 2), point.x + (width / 2), point.y + (height / 2));

	return rect;
}

//보드 값 초기화 및 그리기 (최대 10 * 10)
//(열 개수, 행 개수, 한 칸의 넓이, 한 칸의 높이)
void CBoardGameView::DrawSquareBoard(int col, int row, int width, int height)
{
	int totalWidth = width * col;		//그려질 보드의 넓이
	int totalheight = height * row;		//그려질 보드의 높이

	int startX = (winX / 2) - (totalWidth / 2);		//보드가 그려지기 시작할 위치(위쪽 픽셀)
	int startY = (winY / 2) - (totalheight / 2);	//보드가 그려지기 시작할 위치(왼쪽 픽셀)

	for (int c = 0; c < col; c++)
	{
		for (int r = 0; r < row; r++)
		{
			boardSpace[c][r].point = CPoint(startX + (width * c) + (width / 2), startY + (height * r) + (height / 2));		//한 칸의 중심점 좌표 저장
			boardSpace[c][r].rect = PointToRect(boardSpace[c][r].point, width, height);								//한 칸의 사각형이 그려지는 좌표 저장
			if (newGame)
				boardSpace[c][r].isSet = EMPTY;		//처음 한번만 칸이 비어있는 상태로 저장

			DC->Rectangle(boardSpace[c][r].rect);	//사각형을 그림
		}
	}
}

//클릭한 칸이 비어있는가
//해당 칸의 인덱스 저장
bool CBoardGameView::IsEmptySpace(CPoint point)
{
	for (int c = 0; c < column; c++)
	{
		for (int r = 0; r < row; r++)
		{
			if (boardSpace[c][r].rect.PtInRect(point) && boardSpace[c][r].isSet == EMPTY)
			{
				if (object[actObjIndex].side == "Green" && r >= 1)
				{
					arriveIndex = CPoint(c, r);

					return true;
				}
				else if (object[actObjIndex].side == "Red" && r <= 2)
				{
					arriveIndex = CPoint(c, r);

					return true;
				}
			}
		}
	}



	return false;
}

bool CBoardGameView::IsObject(CPoint point)
{
	for (int i = 0; i < totalObject; i++)
	{
		if (object[i].rect.PtInRect(point) && object[i].side == turn)		//오브젝트가 눌렸는지 확인
		{
			actObjIndex = i;
			departPoint = object[i].point;

			for (int c = 0; c < column; c++)
			{
				for (int r = 0; r < row; r++)
				{
					if (boardSpace[c][r].isSet == i)
					{
						departIndex = CPoint(c, r);
						return true;
					}
				}
			}
			
			return true;
		}
	}

	return false;
}

bool CBoardGameView::CanMove(CPoint point)
{
	int c, r;
	bool find = false;		//보드를 클릭했는지
	bool isEmpty = true;	//클릭한 보드에 오브젝트가 없는지

	for (c = 0; c < column; c++)
	{
		for (r = 0; r < row; r++)
		{
			if (boardSpace[c][r].rect.PtInRect(point))
			{
				find = true;
				if (boardSpace[c][r].isSet != EMPTY)
					isEmpty = false;

				break;
			}
		}
		if (find) break;
	}
	if (!find) return false;		//보드를 클릭하지 않았다면 false반환


	if (!isEmpty)
	{
		if (object[actObjIndex].side == object[boardSpace[c][r].isSet].side)	//이동하려는 곳에 아군이 있다면
			return false;			//false반환
	}

	if (object[actObjIndex].diagonal || object[actObjIndex].straight)
	{

		if (object[actObjIndex].diagonal)
		{
			if (abs(c - departIndex.x) == 1 && abs(r - departIndex.y) == 1)
			{

				if (object[actObjIndex].job == L"Lord")
				{
					if (object[actObjIndex].side == L"Green" && r - departIndex.y < 0)
					{
						arrivePoint = boardSpace[c][r].point;
						arriveIndex = CPoint(c, r);
						return true;
					}
					else if (object[actObjIndex].side == L"Red" && r - departIndex.y > 0)
					{
						arrivePoint = boardSpace[c][r].point;
						arriveIndex = CPoint(c, r);
						return true;
					}
				}
				else
				{
					arrivePoint = boardSpace[c][r].point;
					arriveIndex = CPoint(c, r);
					return true;
				}
			}
		}
		
		if (object[actObjIndex].straight)
		{
			if (c == departIndex.x && abs(r - departIndex.y) == 1)
			{
				arrivePoint = boardSpace[c][r].point;
				arriveIndex = CPoint(c, r);
				return true;
			}
			else if (abs(c - departIndex.x) == 1 && r == departIndex.y)
			{
				arrivePoint = boardSpace[c][r].point;
				arriveIndex = CPoint(c, r);
				return true;
			}
		}
	}
	else
	{
		if (Twelve_SonMoving(c, r))
		{
			arrivePoint = boardSpace[c][r].point;
			arriveIndex = CPoint(c, r);
			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// 게임 모드

void CBoardGameView::TwelveGame(CDC* pDC)
{
	DC = pDC;

	if (newGame)
	{
		column = 3;	//보드 행, 열 개수 초기화
		row = 4;

		arrivePoint = (0, 0);
		
		tempObjIndex = EMPTY;

		isAnimating = false;
		imgFrame = 0;

		doCatch = false;
		doSet = false;
		doMove = false;
		readyReverse = false;
		doReverse = false;

		isActive = false;

		Twelve_SettingExtraSpace();
		turn = L"Green";
	}

	DrawSquareBoard(column, row, 100, 100);

	if (newGame)
	{
		Twelve_FirstObjectSetting();	//오브젝트 초기화
		totalObject = 8;	//총 오브젝트 개수
		newGame = false;
	}


	if (isAnimating)
		Twelve_DrawObject(actObjIndex);
	else
		Twelve_DrawObject(EMPTY);


	if (doMove)
	{
		if (isFirst)
		{
			Twelve_MoveObject(actObjIndex);
		}
		Twleve_MovingAnimation(actObjIndex, departPoint, arrivePoint);
	}

	if (doSet)		//오브젝트가 재배치되는 상황일 때
	{
		if (isFirst)		//처음 한번만 실행
			Twelve_SettingObject(actObjIndex);
		Twleve_MovingAnimation(actObjIndex, departPoint, arrivePoint);				//말을 재배치 할 때의 애니메이션 구현
	}

	if (doCatch)		//오브젝트가 잡히는 상황일 때
	{
		if (isFirst)
			Twelve_DrawObject(actObjIndex);
		Twelve_CatchAnimation(actObjIndex, departPoint, arrivePoint);	//말이 잡혔을 때의 애니메이션 구현
	}

	if (doReverse)
	{
		if (isFirst)
			Twelve_ReverseObject(reverseIndex);
		Twelve_ReverseAnimation(reverseIndex);
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
//게임 모드 기능 

//처음 시작 상태
void CBoardGameView::Twelve_FirstObjectSetting()
{
	object[0].side = L"Green";
	object[0].job = L"Son";
	object[0].point = boardSpace[1][2].point;
	object[0].rect = PointToRect(object[0].point, 90, 90);
	object[0].isSet = true;

	object[0].diagonal = false;
	object[0].straight = false;
	boardSpace[1][2].isSet = 0;

	object[1].side = L"Red";
	object[1].job = L"Son";
	object[1].point = boardSpace[1][1].point;
	object[1].rect = PointToRect(object[1].point, 90, 90);
	object[1].isSet = true;

	object[1].diagonal = false;
	object[1].straight = false;
	boardSpace[1][1].isSet = 1;

	object[2].side = L"Green";
	object[2].job = L"Minister";
	object[2].point = boardSpace[0][3].point;
	object[2].rect = PointToRect(object[2].point, 90, 90);
	object[2].isSet = true;

	object[2].diagonal = true;
	object[2].straight = false;
	boardSpace[0][3].isSet = 2;

	object[3].side = L"Red";
	object[3].job = L"Minister";
	object[3].point = boardSpace[2][0].point;
	object[3].rect = PointToRect(object[3].point, 90, 90);
	object[3].isSet = true;

	object[3].diagonal = true;
	object[3].straight = false;
	boardSpace[2][0].isSet = 3;

	object[4].side = L"Green";
	object[4].job = L"General";
	object[4].point = boardSpace[2][3].point;
	object[4].rect = PointToRect(object[4].point, 90, 90);
	object[4].isSet = true;

	object[4].diagonal = false;
	object[4].straight = true;
	boardSpace[2][3].isSet = 4;

	object[5].side = L"Red";
	object[5].job = L"General";
	object[5].point = boardSpace[0][0].point;
	object[5].rect = PointToRect(object[5].point, 90, 90);
	object[5].isSet = true;

	object[5].diagonal = false;
	object[5].straight = true;
	boardSpace[0][0].isSet = 5;

	object[6].side = L"Green";
	object[6].job = L"King";
	object[6].point = boardSpace[1][3].point;
	object[6].rect = PointToRect(object[6].point, 90, 90);
	object[6].isSet = true;

	object[6].diagonal = true;
	object[6].straight = true;
	boardSpace[1][3].isSet = 6;

	object[7].side = L"Red";
	object[7].job = L"King";
	object[7].point = boardSpace[1][0].point;
	object[7].rect = PointToRect(object[7].point, 90, 90);
	object[7].isSet = true;

	object[7].diagonal = true;
	object[7].straight = true;
	boardSpace[1][0].isSet = 7;
}

//입력된 값의 인덱스 오브젝트는 그리지 않음
void CBoardGameView::Twelve_DrawObject(int nDrawObjIndex)
{
	for (int i = 0; i < totalObject; i++)
	{
		imgFilePath.Format(L"res/%s/%s.png", object[i].side, object[i].job);
		if (object[i].isSet == false)	//잡힌 상태라면
		{
			imgFilePath.Format(L"res/%s/%s_Taken.png", object[i].side, object[i].job);	//_Taken추가
		}

		//DC->TextOutW(200, 50 * i, imgFilePath);

		if (i != nDrawObjIndex)
		{
			DrawFromFile(object[i].point, imgFilePath);
		}
	}
}

//죽은 말이 위치할 곳
void CBoardGameView::Twelve_SettingExtraSpace()
{
	place[0][0] = CPoint(350, 100);
	place[0][1] = CPoint(350, 200);
	place[0][2] = CPoint(250, 100);
	place[0][3] = CPoint(250, 200);
	place[0][4] = CPoint(150, 100);
	place[0][5] = CPoint(150, 200);

	place[1][0] = CPoint(850, 600);
	place[1][1] = CPoint(850, 500);
	place[1][2] = CPoint(950, 600);
	place[1][3] = CPoint(950, 500);
	place[1][4] = CPoint(1050, 600);
	place[1][5] = CPoint(1050, 500);
}

bool CBoardGameView::Twelve_SonMoving(int x, int y)
{
	if (object[actObjIndex].side == L"Green")
	{
		if (x == departIndex.x && y == departIndex.y - 1)
		{
			if (y == 0)
			{
				reverseIndex = actObjIndex;
				readyReverse = true;
			}
			return true;
		}
	}
	else if (object[actObjIndex].side == L"Red")
	{
		if (x == departIndex.x && y == departIndex.y + 1)
		{
			if (y == 3)
			{
				reverseIndex = actObjIndex;
				readyReverse = true;
			}
			return true;
		}
	}

	return false;
}

void CBoardGameView::Twelve_MoveObject(int index)
{
	imgFrame = 0;		//애니메이션에 필요한 변수 초기화
	isAnimating = true;

	if (boardSpace[arriveIndex.x][arriveIndex.y].isSet != EMPTY)			//도착칸에 오브젝트가 있다면
		tempObjIndex = boardSpace[arriveIndex.x][arriveIndex.y].isSet;

	boardSpace[departIndex.x][departIndex.y].isSet = EMPTY;					//보드판의 오브젝트 인덱스 정보 이동
	boardSpace[arriveIndex.x][arriveIndex.y].isSet = index;

	object[index].point = arrivePoint;										//오브젝트의 최종 상태 저장
	object[index].rect = PointToRect(arrivePoint, 90, 90);

	isFirst = false;
}


//말이 잡혔을 때의 처리
void CBoardGameView::Twelve_CatchObject(int index)
{
		imgFrame = 0;
		isAnimating = true;
		
		if (object[index].job == L"Lord")
		{
			object[index].job = L"Son";
			object[index].diagonal = false;
			object[index].straight = false;
		}

		if (object[index].side == L"Green")	//잡힌 말이 녹색말일 때
		{
			object[index].side = L"Red";		//적색말로 변경

			departPoint = object[index].point;	//출발지 저장
			object[index].point = place[0][index];	//오브젝트 위치를 적색측 대기열로 변경
		}
		else if (object[index].side == L"Red")	//잡힌 말이 적색말일 때
		{
			object[index].side = L"Green";		//녹색말로 변경

			departPoint = object[index].point;	//출발지 저장
			object[index].point = place[1][index];	//오브젝트 위치를 녹색측 대기열로 변경
		}

		if (object[index].job == L"King")
		{
			if (object[index].side == L"Green")
			{
				MessageBox(L"녹색이 이겼습니다!!!!!");
			}
			else if (object[index].side == L"Red")
			{
				MessageBox(L"적색이 이겼습니다!!!!!");
			}
			newGame = true;
		}

		arrivePoint = object[index].point;					//도착지 저장

		boardSpace[departIndex.x][departIndex.y].isSet = EMPTY;		//출발지의 칸에 오브젝트가 없다고 저장
		object[index].isSet = false;
		isFirst = false;

		object[index].rect = PointToRect(object[index].point, 60, 60);
}

//말을 재배치할 때의 처리
void CBoardGameView::Twelve_SettingObject(int index)
{
	imgFrame = 0;
	isAnimating = true;

	departPoint = object[index].point;
	object[index].point = boardSpace[arriveIndex.x][arriveIndex.y].point; // 말을 보드 위에 올림
	arrivePoint = object[index].point;
	object[index].isSet = true;
	isFirst = false;	
	boardSpace[arriveIndex.x][arriveIndex.y].isSet = index;		//목적지칸에 오브젝트 인덱스 저장
	object[index].rect = PointToRect(object[index].point, 90, 90);
}


void CBoardGameView::Twelve_ReverseObject(int index)
{
	imgFrame = 0;

	object[index].job = L"Lord";
	object[index].diagonal = true;
	object[index].straight = true;
	actObjIndex = index;
	isFirst = false;
}

//말이 잡혔을 때의 애니메이션
//(오브젝트 인덱스, 출발위치, 도착위치)
void CBoardGameView::Twelve_CatchAnimation(int index, CPoint depart, CPoint arrive)
{
	imgFilePath.Format(L"res/%s/%s_Catch%d.png", object[index].side, object[index].job, imgFrame);

	SetTimer(0, 100, NULL);			//0.1초마다 움직임

	if (imgFrame >= 6)				//6프레임 이상 넘어갔을 시
	{
		KillTimer(0);				//애니메이션 종료
		doCatch = false;
		if (readyReverse)
		{
			doReverse = true;
			readyReverse = false;
			isFirst = true;
		}
		else
			isAnimating = false;
	}

	CPoint movingPoint;							//말이 그려질 위치
	CPoint move = arrive - depart;	//출발지와 목적지의 차이
												//말이 그려질 위치를 계산하는데 쓰임

	if (imgFrame == 0)										//0프레임
	{
		movingPoint.x = depart.x + (move.x * 0.4);		//출발위치와 도착위치의 2/5에 위치 
		movingPoint.y = depart.y + (move.y * 0.4);
	}
	else if (imgFrame == 1)									//1프레임
	{
		movingPoint.x = depart.x + (move.x * 0.8);		//출발위치와 도착위치의 4/5에 위치
		movingPoint.y = depart.y + (move.y * 0.8);
	}
	else if (imgFrame == 2)									//2프레임
	{
		movingPoint.x = depart.x + (move.x * 0.9);		//출발위치와 도착위치의 9/10에 위치
		movingPoint.y = depart.y + (move.y * 0.9);
	}
	else if (imgFrame == 3)									//3프레임
	{
		if (object[index].side == L"Green")			//죽은 말이 녹색측으로 갔다면
		{
			movingPoint.x = arrive.x - 10;					//도착위치에서 살짝 떨어진 곳에 위치
			movingPoint.y = arrive.y - 10;
		}
		else if (object[index].side == L"Red")			//죽은 말이 적색측으로 갔다면
		{
			movingPoint.x = arrive.x + 10;					//도착위치에서 살짝 떨어진 곳에 위치
			movingPoint.y = arrive.y + 10;
		}
	}
	else
		movingPoint = arrive;							///도착위치에 위치

	DrawFromFile(movingPoint, imgFilePath);
}

//말을 재배치 할 때의 애니메이션
//(출발위치, 도착위치)
void CBoardGameView::Twleve_MovingAnimation(int index, CPoint depart, CPoint arrive)
{
	imgFilePath.Format(L"res/%s/%s_Setting%d.png", object[index].side, object[index].job, imgFrame);	//파일 이름 저장

	SetTimer(0, 100, NULL);			//0.1초마다 움직임

	if (imgFrame >= 2)				//2프레임 이상 넘어갔을 시
	{
		KillTimer(0);				//0.1초 주기를 없앰
		SetTimer(0, 50, NULL);		//0.05초마다 움직임

		if (imgFrame >= 7)			//7프레임 이상 넘어갔을 시
		{
			KillTimer(0);			//애니메이션 종료
			doSet = false;
			doMove = false;
			if (tempObjIndex != EMPTY)
			{
				actObjIndex = tempObjIndex;
				tempObjIndex = EMPTY;

				doCatch = true;
				Twelve_CatchObject(actObjIndex);

				isFirst = true;
			}
			else if (readyReverse)
			{
				doReverse = true;
				readyReverse = false;
				isFirst = true;
			}
			else
				isAnimating = false;
		}
	}

	CPoint movingPoint;							//말이 그려질 위치
	CPoint move = arrive - depart;

	 

	if (imgFrame == 0)										//0프레임
	{
		movingPoint.x = depart.x + (move.x * 0.5);		//출발위치와 도착위치의 중간에 위치
		movingPoint.y = depart.y + (move.y * 0.5);
	}
	else if (imgFrame == 1)									//1프레임
	{
		movingPoint.x = depart.x + (move.x * 0.8);		//출발위치와 도착위치의 4/5에 위치
		movingPoint.y = depart.y + (move.y * 0.8);
	}
	else
		movingPoint = arrive;							//도착위치에 위치

	DrawFromFile(movingPoint, imgFilePath);
}

void CBoardGameView::Twelve_ReverseAnimation(int index)
{
	imgFilePath.Format(L"res/%s/Reverse%d.png", object[index].side, imgFrame);	//파일 이름 저장

	SetTimer(0, 100, NULL);			//0.1초마다 움직임

	if (imgFrame >= 15)				//2프레임 이상 넘어갔을 시
	{
		KillTimer(0);				//0.1초 주기를 없앰
		doReverse = false;
		isAnimating = false;
	}

	DrawFromFile(object[index].point, imgFilePath);
}