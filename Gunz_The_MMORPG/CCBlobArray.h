// Ư�� �޸� ���� �迭�� �� ������ �ϳ��� �޸� ���� �ֱ� ���� �Լ���
#pragma once
/// nOneBlobSize��ŭ nBlobCount������ŭ �迭�� �� �����
void* CCMakeBlobArray(int nOneBlobSize, int nBlobCount);
/// �� �����
void CCEraseBlobArray(void* pBlob);
/// ������ �� ���� ����
void* CCGetBlobArrayElement(void* pBlob, int i);
/// ���� ���� ����
int CCGetBlobArrayCount(void* pBlob);
/// ��� ��ü ������ ����
int CCGetBlobArraySize(void* pBlob);
/// Array Block���� ���� �� ó�� ������ ����
void* CCGetBlobArrayPointer(void* pBlob);
