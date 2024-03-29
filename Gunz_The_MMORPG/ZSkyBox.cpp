#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
//	Include
//////////////////////////////////////////////////////////////////////////
#include "ZSkybox.h"
#include "CCDebug.h"

//////////////////////////////////////////////////////////////////////////
//	持失切/社瑚切
//////////////////////////////////////////////////////////////////////////
ZSkyBox::ZSkyBox()
{
}

ZSkyBox::~ZSkyBox()
{
	Release();
}

//////////////////////////////////////////////////////////////////////////
//	Create
//////////////////////////////////////////////////////////////////////////
bool ZSkyBox::Create( RVisualMesh* pvMesh_ )
{
	mpVMesh = pvMesh_;
	mpVMesh->m_pMesh->mbSkyBox	= true;
	return true;
}

//////////////////////////////////////////////////////////////////////////
//	Release
//////////////////////////////////////////////////////////////////////////
void ZSkyBox::Release()
{
	SAFE_DELETE( mpVMesh );
}


//////////////////////////////////////////////////////////////////////////
//	Render
//////////////////////////////////////////////////////////////////////////
void ZSkyBox::Render()
{
	rmatrix identity;
	D3DXMatrixIdentity( &identity );
	mpVMesh->SetWorldMatrix(identity);
	mpVMesh->Frame();
	mpVMesh->Render();
}