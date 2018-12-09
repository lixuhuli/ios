#include "C3DTransform.h"

CViewPort::CViewPort( int x, int y, int width, int height )
{
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;
}

CViewPort::~CViewPort()
{
}

irr::core::vector2d<f32>  CViewPort::transform( vector2d<f32> & p )
{
	vector2d pRet;
	pRet.X = (m_x + 1)*m_width/2 + p.X;
	pRet.Y = (m_y + 1)*m_height/2 + p.Y;
	return pRet;
}


C3DTransform::C3DTransform( matrix4& mat, CViewPort& port )
{
	m_matrix = mat;
	m_viewPort = port;
}

C3DTransform::~C3DTransform()
{

}

irr::core::vector2d<f32>  C3DTransform::transform( vector3d<f32> & p )
{
	vector3d<f32> out;
	m_matrix.transformVect(out, p);
	vector2d<f32>  ret;
	ret.X = out.X/out.Z;
}
