#pragma once
// ����ϴ� ���� ������ /////////////////////////////////////////////////////////////////
//
// LOCALE_US		: ���ͳ׼ų�
// LOCALE_JAPAN		: �Ϻ�
// LOCALE_KOREA		: �ѱ�
// LOCALE_BRAZIL	: �����
// LOCALE_INDIA		: �ε�
//
/////////////////////////////////////////////////////////////////////////////////////////
// ������ ���� define ///////////////////////////////////////////////////////////////////
#define LOCALE_US
#if (!defined(LOCALE_US)) && (!defined(LOCALE_JAPAN)) && (!defined(LOCALE_KOREA)) && (!defined(LOCALE_BRAZIL)) && (!defined(LOCALE_INDIA)) && (!defined(LOCALE_NHNUSA))

#define LOCALE_KOREA		// �ƹ� �����ε� ������ �⺻���� �ѱ�

#endif // (!defined(LOCALE_US)) && (!defined(LOCALE_JAPAN)) && (!defined(LOCALE_KOREA))
