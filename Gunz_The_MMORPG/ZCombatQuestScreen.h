#ifndef _ZCOMBAT_QUEST_SCREEN_H
#define _ZCOMBAT_QUEST_SCREEN_H


class CCDrawContext;
class ZActor;

// ����Ʈ�� �����̹� ��� �� ����Ʈ ���� ��忡�� �������� ȭ��
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