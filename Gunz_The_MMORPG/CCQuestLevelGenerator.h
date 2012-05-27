#ifndef _MQUEST_LEVEL_GENERATOR_H
#define _MQUEST_LEVEL_GENERATOR_H

class CCQuestLevel;


/// ����Ʈ ���� ������ִ� Ŭ���� 
/// - ���� �������� �Ǿ��ִ�.
class CCQuestLevelGenerator
{
private:
	CCMATCH_GAMETYPE		m_eGameType;		// ����Ʈ or �����̹�
	int					m_nPlayerQL;
	int					m_nMapsetID;
	int					m_nScenarioID;
	unsigned int		m_nSacriQItemID[MAX_SCENARIO_SACRI_ITEM];

	int MakeScenarioID();
public:
	CCQuestLevelGenerator(CCMATCH_GAMETYPE eGameType);
	~CCQuestLevelGenerator();

	void BuildPlayerQL(int nQL);						///< �÷��̾��� QL�� �Է�
	void BuildMapset(int nMapsetID);					///< ������ �ʼ� �Է�
	void BuildSacriQItem(unsigned int nItemID);			///< ��� ������ ID �Է�

	int ReturnScenarioID();			///< �Էµ� ������� �������� ������� �ó����� ID�� ��ȯ�Ѵ�.
	CCQuestLevel* MakeLevel();		///< �Էµ� ������� �������� ���巹���� �����Ѵ�.
};




#endif