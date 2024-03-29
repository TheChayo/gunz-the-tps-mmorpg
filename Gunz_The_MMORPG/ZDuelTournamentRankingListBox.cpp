#pragma warning(disable : 4244)

#include "stdafx.h"
#include "ZDuelTournamentRankingListBox.h"

// Added R350a
#include "RealSpace2.h"
#include "ZGameInterface.h"

ZDuelTournamentRankingListBox::ZDuelTournamentRankingListBox(const char* szName, CCWidget* pParent, CCListener* pListener) 
: CCWidget(szName, pParent, pListener)
{
	m_pBmpRankingItemBg = NULL;
	m_pBmpArrowUp = NULL;
	m_pBmpArrowDown = NULL;
	m_pBmpArrowBar = NULL;

	m_nMyRankIndex = -1;
}

ZDuelTournamentRankingListBox::~ZDuelTournamentRankingListBox()
{

}

void ZDuelTournamentRankingListBox::ClearAll()
{
	for (int i=0; i<NUM_DISPLAY_DUELTOURNAMENT_RANKING; ++i)
	{
		m_rankingList[i].bEmptyItem = true;
	}
}

void ZDuelTournamentRankingListBox::LoadInterfaceImgs()
{
	if (m_pBmpRankingItemBg == NULL) {
		m_pBmpRankingItemBg = new CCBitmapR2;
		((CCBitmapR2*)m_pBmpRankingItemBg)->Create( "DuelTournamentRankingItemBg.png", RealSpace2::RGetDevice(), "Interface/loadable/DuelTournamentRankingItemBg.png");
	}
	if (m_pBmpArrowUp == NULL) {
		m_pBmpArrowUp = new CCBitmapR2;
		((CCBitmapR2*)m_pBmpArrowUp)->Create( "arrow_up.tga", RealSpace2::RGetDevice(), "Interface/loadable/arrow_up.tga");
	}
	if (m_pBmpArrowDown == NULL) {
		m_pBmpArrowDown = new CCBitmapR2;
		((CCBitmapR2*)m_pBmpArrowDown)->Create( "arrow_down.tga", RealSpace2::RGetDevice(), "Interface/loadable/arrow_down.tga");
	}
	if (m_pBmpArrowBar == NULL) {
		m_pBmpArrowBar = new CCBitmapR2;
		((CCBitmapR2*)m_pBmpArrowBar)->Create( "arrow_bar.tga", RealSpace2::RGetDevice(), "Interface/loadable/arrow_bar.tga");
	}
}

void ZDuelTournamentRankingListBox::UnloadInterfaceImgs()
{
	SAFE_DELETE(m_pBmpRankingItemBg);
	SAFE_DELETE(m_pBmpArrowUp);
	SAFE_DELETE(m_pBmpArrowDown);
	SAFE_DELETE(m_pBmpArrowBar);
}

void ZDuelTournamentRankingListBox::SetRankInfo( unsigned int nIndex, const ZDUELTOURNAMENTRANKINGITEM& rankingItem )
{
	if (nIndex >= NUM_DISPLAY_DUELTOURNAMENT_RANKING) { _ASSERT(0); return; }
	
	m_rankingList[nIndex] = rankingItem;
}

void ZDuelTournamentRankingListBox::OnDraw( CCDrawContext* pDC )
{
	if (!m_pBmpRankingItemBg || !m_pBmpArrowUp || !m_pBmpArrowDown || !m_pBmpArrowBar) { _ASSERT(0); return; }

	const int nWidth = this->GetRect().w;
	const int nHeight = this->GetRect().h;
	const int nY_firstItem = nHeight * 0.17f;	// 리스트 첫째 항목의 y위치
	const int nItemHeight = (int)(nHeight / 7.f);

	char szTemp[128];
	sRect rc;

	for (int i=0; i<NUM_DISPLAY_DUELTOURNAMENT_RANKING; ++i)
	{
		int y = (nY_firstItem + nItemHeight * i) - (nItemHeight * 0.04f);

		// 리스트아이템 배경이미지
		pDC->SetBitmap(m_pBmpRankingItemBg);
		pDC->Draw(0, y, nWidth, nItemHeight);

		// 항목 내용 그리기
		ZDUELTOURNAMENTRANKINGITEM* pRankItem = &m_rankingList[i];

		if (pRankItem->bEmptyItem) continue;

		y = nY_firstItem + nItemHeight * i;

		pDC->SetColor(sColor(0xFFFFFFFF));

		// 순위
		if (pRankItem->nRank == -1)	// 초기화 직후 순위정렬되지 않았을때 -1로 되어 있음
			strcpy(szTemp, "--");
		else
			sprintf(szTemp, "%d", pRankItem->nRank);
		rc.Set((int)(0.01f*nWidth), y, (int)(fabs(0.01f - 0.11f)*nWidth), nItemHeight);
		pDC->Text(rc, szTemp, CCD_RIGHT|CCD_VCENTER);

		// 순위등락 화살표
		if (pRankItem->nFluctuation == 0)
		{
			pDC->SetBitmap(m_pBmpArrowBar);
			pDC->Draw((int)(0.13f*nWidth), y + (nItemHeight - m_pBmpArrowBar->GetHeight()) * 0.5f);
		}
		else
		{
			if (pRankItem->nFluctuation > 0)
				pDC->SetBitmap(m_pBmpArrowUp);
			else if (pRankItem->nFluctuation < 0)
				pDC->SetBitmap(m_pBmpArrowDown);

			pDC->Draw((int)(0.13f*nWidth), y + (nItemHeight - m_pBmpArrowBar->GetHeight()) * 0.5f);
			
			// 순위등락폭
			pDC->SetColor(sColor(0xFFAAFF00));
			sprintf(szTemp, "%d", abs(pRankItem->nFluctuation));
			//pDC->Text((int)(0.04f*nWidth), y, szTemp);
			rc.Set((int)(0.16f*nWidth), y, (int)(fabs(0.16f - 0.26f)*nWidth), nItemHeight);
			pDC->Text(rc, szTemp, CCD_LEFT|CCD_VCENTER);
			pDC->SetColor(sColor(0xFFFFFFFF));
		}

		// 토너먼트 등급 엠블렘
		GetDuelTournamentGradeIconFileName(szTemp, pRankItem->nGrade);
		CCBitmap* pGradeIcon = CCBitmapManager::Get(szTemp);
		if (pGradeIcon) {
			//int height1px = int(600.f/CCGetWorkspaceHeight() + 0.5f);
			
			int margin = (nItemHeight * 0.05f + 0.5f);
			int size = nItemHeight-(margin*4);
			pDC->SetBitmap(pGradeIcon);
			rc.Set((int)(0.279f*nWidth), y+margin, size, size);//(int)(0.055f*nWidth), nItemHeight-3);
			pDC->Draw(rc);
		}


		// 이름
		rc.Set((int)(0.34f*nWidth), y, (int)(fabs(0.34f - 0.55f)*nWidth), nItemHeight);
		pDC->Text(rc, pRankItem->szCharName, CCD_HCENTER|CCD_VCENTER);

		// 승패
		sprintf(szTemp, "%d", pRankItem->nWins);
		rc.Set((int)(0.57f*nWidth), y, (int)(fabs(0.57f - 0.67f)*nWidth), nItemHeight);
		pDC->Text(rc, szTemp, CCD_RIGHT|CCD_VCENTER);
		rc.Set((int)(0.67f*nWidth), y, (int)(fabs(0.67f - 0.68f)*nWidth), nItemHeight);
		pDC->Text(rc, "/", CCD_HCENTER|CCD_VCENTER);
		sprintf(szTemp, "%d", pRankItem->nLosses);
		rc.Set((int)(0.68f*nWidth), y, (int)(fabs(0.68f - 0.78f)*nWidth), nItemHeight);
		pDC->Text(rc, szTemp, CCD_LEFT|CCD_VCENTER);

		// 우승수
		sprintf(szTemp, "%d", pRankItem->nWinners);
		rc.Set((int)(0.8f*nWidth), y, (int)(fabs(0.8f - 0.91f)*nWidth), nItemHeight);
		pDC->Text(rc, szTemp, CCD_RIGHT|CCD_VCENTER);

		// 토너먼트 포인트
		sprintf(szTemp, "%d", pRankItem->nPoint);
		rc.Set((int)(0.92f*nWidth), y, (int)(fabs(0.92f - 0.99f)*nWidth), nItemHeight);
		pDC->Text(rc, szTemp, CCD_RIGHT|CCD_VCENTER);

		// 내 랭킹 항목인 경우 하이라이트 이미지 덮어줌
		if (i == m_nMyRankIndex)
		{
			CCBitmapR2 *pBitmap=(CCBitmapR2*)CCBitmapManager::Get("button_glow.png");
			if(pBitmap) {
				DWORD defaultcolor = 0x333333;
				DWORD opacity=(DWORD)pDC->GetOpacity();
				sRect rt(0, y, nWidth, nItemHeight);
				CCDrawEffect prevEffect = pDC->GetEffect();
				pDC->SetEffect(CCDE_ADD);
				sColor prevColor = pDC->GetBitmapColor();
				pDC->SetBitmapColor(sColor(defaultcolor));
				unsigned char prevOpacity = pDC->GetOpacity();
				pDC->SetOpacity(opacity);
				pDC->SetBitmap(pBitmap);
				pDC->Draw(rt.x,rt.y,rt.w,rt.h,0,0,64,32);
				pDC->SetBitmapColor(prevColor);
				pDC->SetEffect(prevEffect);
				pDC->SetOpacity(prevOpacity);
			}
		}
	}
}


