/*

  A Star Search
  -------------

  1999/10/8, lee jang ho
  All copyright (c) 1998, MAIET entertainment software, inc

*/
#pragma once

#include "CCPtrList.h"
#include "CCList.h"

class CCNodeModel{
public:
	float		m_fCostFromStart;
	float		m_fCostToGoal;
	CCNodeModel*	m_pParent;
	int			m_nVisitID;
public:
	CCNodeModel(void);
	virtual ~CCNodeModel(void);
	// �̿� Node ���
	virtual int GetSuccessorCount(void) = 0;
	virtual CCNodeModel* GetSuccessor(int i) = 0;

	// �̿� Node���� Cost ���
	float GetSuccessorCost(int i){
		return GetSuccessorCost(GetSuccessor(i));
	}
	virtual float GetSuccessorCost(CCNodeModel* pSuccessor) = 0;

	// ���������� �̿� Node������� Cost ���
	virtual float GetSuccessorCostFromStart(CCNodeModel* pSuccessor){
		return m_fCostFromStart + GetSuccessorCost(pSuccessor);
	}

	// Heuristic Estimation�� ���� pNode������ Cost ���
	virtual float GetHeuristicCost(CCNodeModel* pNode) = 0;

	float GetTotalCost(void){
		return m_fCostFromStart+m_fCostToGoal;
	}
	virtual float GetTotalCostPriority(void){	// �켱������ �����ϱ� ���� Total Cost
		return GetTotalCost();	// ��κ� GetTotalCost()�� ����.
	}
};

struct CCFORCEDNODE{
	CCNodeModel* pNode;
	CCNodeModel* pFromNode;
};

class MAStar{
	CCPtrList<MNodeModel>	m_ShortestPath;
	class CMSortedList : public CCPtrList<CCNodeModel>{
	public:
		int Compare(CCNodeModel *lpRecord1, CCNodeModel *lpRecord2){
			float fCompare = lpRecord1->GetTotalCostPriority()-lpRecord2->GetTotalCostPriority();
			if(fCompare>0.0f) return 1;
			else if(fCompare==0.0f) return 0;
			else return -1;
		}
	} m_OpenStack;

	CCLinkedList<CCFORCEDNODE>	m_ForcedCloseNodes;
	int		m_nVisitID;	/// ��� �˻� ID
	
public:
	int		m_nNodeTraverseCount;		// ��� Ž�� Ƚ��

	//CCPtrList<MNodeModel>	m_CloseStack;
protected:
	void PushOpen(CCNodeModel* pNode);
	CCNodeModel* PopOpen(void);
	CCNodeModel* PopLowestCostOpen(void);
	bool IsOpenEmpty(void);
	int IsOpenNode(CCNodeModel* pNode);
	/*
	void PushClose(MNodeModel* pNode);
	MNodeModel* PopClose(void);
	int IsCloseNode(MNodeModel* pNode);
	void RemoveClose(int i);
	*/
	bool IsForcedClose(CCNodeModel* pNode, CCNodeModel* pFromNode);

public:
	CCAStar(void);

	void AddForcedClose(CCNodeModel* pNode, CCNodeModel* pFromNode);	// ������ Close ����
	void RemoveForcedClose(void);

	bool FindPath(CCNodeModel* pStartNode, CCNodeModel* pGoalNode, int nLimit=-1, float fMinHeuristicValue=-1, bool bIncludeStartNode=false);

	int GetShortestPathCount(void);
	CCNodeModel* GetShortestPath(int i);
	void RemoveShortestPath(int i);

	CCPtrList<MNodeModel>* GetShortestPath(void);

	void Reset(void);
};
