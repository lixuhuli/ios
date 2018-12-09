#ifndef __C3DTRANSFORM_H__
#define __C3DTRANSFORM_H__

#include "matrix4.h"
#include "vector3d.h"
#include "vector2d.h"

using namespace irr::core;
class CViewPort
{
public:
	CViewPort(int x, int y, int width, int height);
	~CViewPort();
	vector2d<f32>  transform(vector2d<f32>& p);
	int m_x;
	int m_y;
	int m_width;
	int m_height;
};

class C3DTransform
{
public:
	C3DTransform(matrix4<f32> & mat, CViewPort& port);
	~C3DTransform();
	vector2d transform(vector3d<f32> & p);

	matrix4<f32>  m_matrix;
	CViewPort m_viewPort;
};


#endif//end __C3DTRANSFORM_H__