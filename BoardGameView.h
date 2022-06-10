
// BoardGameView.h: CBoardGameView 클래스의 인터페이스
//

#pragma once


class CBoardGameView : public CView
{
protected: // serialization에서만 만들어집니다.
	CBoardGameView() noexcept;
	DECLARE_DYNCREATE(CBoardGameView)

// 특성입니다.
public:
	CBoardGameDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CBoardGameView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

//---------------------------------------------------------------------------------------------------------------------------------------------------
	//기능 함수

	bool DrawFromFile(CPoint point, PCWSTR filePath);
	void DrawSquareBoard(int col, int row, int width, int height);	//보드판 그리기
	CRect PointToRect(CPoint point, int width, int height);			//중심점과 넢이, 높이를 사각형 좌표로 변환

	bool IsObject(CPoint point);		//오브젝트가 클릭되었는지
	bool CanMove(CPoint point);			//활성화된 오브젝트가 이동할 수 있는 곳인지
	bool IsEmptySpace(CPoint point);	//보드의 빈 공간을 클릭하였는지

//---------------------------------------------------------------------------------------------------------------------------------------------------
	//게임 모드

	void TwelveGame(CDC* pDC);

//---------------------------------------------------------------------------------------------------------------------------------------------------
	//게임 모드 기능

	void Twelve_DrawObject(int notDrawObjectIndex);
	void Twelve_SettingExtraSpace();
	void Twelve_FirstObjectSetting();

	bool Twelve_SonMoving(int x, int y);

	void Twelve_MoveObject(int index);

	void Twelve_CatchObject(int index);
	void Twelve_SettingObject(int index);
	void Twelve_ReverseObject(int index);

	void Twelve_CatchAnimation(int index, CPoint depart, CPoint arrive);
	void Twleve_MovingAnimation(int index, CPoint depart, CPoint arrive);
	void Twelve_ReverseAnimation(int index);

//---------------------------------------------------------------------------------------------------------------------------------------------------

	CDC* DC;
	//보드 클래스
	class Space 
	{
	public:
		CPoint point;		//한 칸의 중심점
		CRect rect;		//한 칸을 그릴 좌표
		int isSet = 101;		//자신의 칸에 위치한 오브젝트 인덱스
	} boardSpace[10][10];

	//보드 외의 사용할 위치 좌표
	CPoint place[2][10];

	//말 구현에 필요한 변수
	class Stone
	{
	public:
		CPoint point;	//말의 중심점
		CRect rect;		//말의 좌표
		CString side;	//말의 소속
		CString job;		//말의 직업
		bool isSet;			//말이 보드위에 있는지

		bool diagonal;
		bool straight;
	} object[100];

	CString imgFilePath;	//사용할 이미지 파일 경로

	bool newGame;	//게임 초기화에 사용하는 변수
	
	bool isAnimating;	//애니메이션중인지 확인하는 변수

	bool isFirst;	//한번만 실행하기 위한 변수
	bool isActive;	//오브젝트가 활성화 된 상태

	bool doSet;		//오브젝트 재배치일 때
	bool doCatch;	//오브젝트 잡힐때
	bool doMove;	//오브젝트 움직일 때
	bool readyReverse;
	bool doReverse;	//오브젝트 뒤집기

	int row;		//행 수
	int column;		//열 수

	int totalObject;		//총 오브젝트 개수
	int actObjIndex;		//활성화된 오브젝트 인덱스
	int tempObjIndex;		//임시 오브젝트 인덱스
	int reverseIndex;

	CPoint arrivePoint;		//목적지가 저장되는 변수
	CPoint departPoint;		//출발지가 저장되는 변수

	CPoint departIndex;		//출발지 인덱스
	CPoint arriveIndex;		//목적지 인덱스

	CString turn;

	//윈도우 크기
	int winX, winY;
	
	//이미지 인덱스
	int imgFrame;

	//임시 변수
	CString str;
	CString tempStr;
};

#ifndef _DEBUG  // BoardGameView.cpp의 디버그 버전
inline CBoardGameDoc* CBoardGameView::GetDocument() const
   { return reinterpret_cast<CBoardGameDoc*>(m_pDocument); }
#endif

