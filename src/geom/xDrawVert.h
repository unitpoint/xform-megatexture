#ifndef __DRAWVERT_H__
#define __DRAWVERT_H__

/*
===============================================================================

	Draw Vertex.

===============================================================================
*/

class xDrawVert
{
public:
	xVec3			xyz;
	xVec3			normal;
	byte			color[4];
	xVec2			st;
	xVec3			tangents[2];
#if 0 // was MACOS_X see comments concerning DRAWVERT_PADDED in Simd_Altivec.h 
	float			padding;
#endif
	float			operator[](const int index) const;
	float &			operator[](const int index);

	void			Clear();

	void			Lerp(const xDrawVert &a, const xDrawVert &b, float f);
	void			LerpAll(const xDrawVert &a, const xDrawVert &b, float f);

	void			Normalize();

	void			SetColor(dword color);
	dword			GetColor() const;
};

X_INLINE float xDrawVert::operator[](const int index) const {
	assert(index >= 0 && index < 5);
	return ((float *)(&xyz))[index];
}
X_INLINE float	&xDrawVert::operator[](const int index) {
	assert(index >= 0 && index < 5);
	return ((float *)(&xyz))[index];
}

X_INLINE void xDrawVert::Clear() {
	xyz.Zero();
	st.Zero();
	normal.Zero();
	tangents[0].Zero();
	tangents[1].Zero();
	color[0] = color[1] = color[2] = color[3] = 0;
}

X_INLINE void xDrawVert::Lerp(const xDrawVert &a, const xDrawVert &b, float f) {
	xyz = a.xyz + f * (b.xyz - a.xyz);
	st = a.st + f * (b.st - a.st);
}

X_INLINE void xDrawVert::LerpAll(const xDrawVert &a, const xDrawVert &b, float f) {
	xyz = a.xyz + f * (b.xyz - a.xyz);
	st = a.st + f * (b.st - a.st);
	normal = a.normal + f * (b.normal - a.normal);
	tangents[0] = a.tangents[0] + f * (b.tangents[0] - a.tangents[0]);
	tangents[1] = a.tangents[1] + f * (b.tangents[1] - a.tangents[1]);
	color[0] = (byte)(a.color[0] + f * (b.color[0] - a.color[0]));
	color[1] = (byte)(a.color[1] + f * (b.color[1] - a.color[1]));
	color[2] = (byte)(a.color[2] + f * (b.color[2] - a.color[2]));
	color[3] = (byte)(a.color[3] + f * (b.color[3] - a.color[3]));
}

X_INLINE void xDrawVert::SetColor(dword color) {
	*(dword*)this->color = color;
}

X_INLINE dword xDrawVert::GetColor() const {
	return *(const dword*)this->color;
}

#endif /* !__DRAWVERT_H__ */
