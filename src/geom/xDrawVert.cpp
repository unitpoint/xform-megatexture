#include <xForm.h>

/*
=============
xDrawVert::Normalize
=============
*/
void xDrawVert::Normalize() {
	normal.Normalize();
	tangents[1].Cross(normal, tangents[0]);
	tangents[1].Normalize();
	tangents[0].Cross(tangents[1], normal);
	tangents[0].Normalize();
}
