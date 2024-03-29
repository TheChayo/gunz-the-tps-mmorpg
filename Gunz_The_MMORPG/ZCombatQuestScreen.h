#ifndef _ZCOMBAT_QUEST_SCREEN_H
#define _ZCOMBAT_QUEST_SCREEN_H


class CCDrawContext;
class ZActor;
class ZCharacter;

// 퀘스트나 서바이벌 모드 등 퀘스트 관련 모드에서 보여지는 화면
class ZCombatQuestScreen
{
private:
	void DrawPlayer(CCDrawContext* pDC, int index, ZCharacter* pCharacter);
	list<ZCharacter*>		m_SortedCharacterList;
public:
	ZCombatQuestScreen();
	~ZCombatQuestScreen();
	void OnDraw(CCDrawContext* pDC);
};




#endif