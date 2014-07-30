#include "app.h"

// =================================================================
// =================================================================
// =================================================================

void xModelFrame::Clear()
{
  verts.Clear();
  segs.Clear();
  bounds.Clear();
}

void xModelFrame::CalcBounds()
{
  bounds.Clear();
  int count = verts.Count();
  for(int i = 0; i < count; i++)
    bounds.Add(verts[i]);
}

xModel * xModelFrame::CreateModel(xWorld * world, const xVec3& size, float radius)
{
  xModel * model = new xModel(world);

  if(bounds.IsCleared())
    CalcBounds();

  xVec3 modelSize = bounds[1] - bounds[0];
  xVec3 needSize(size.x > 0 ? size.x : modelSize.x,
                 size.y > 0 ? size.y : modelSize.y, 
                 size.z > 0 ? size.z : modelSize.z);
  xVec3 scale = needSize.Div(modelSize);

  xPlane planes[6];
  xList<xPhysConvex*> convexList;
  for(int i = 0; i < segs.Count(); i++){
    Segment& seg = segs[i];
    xVec3 v0 = verts[seg.i[0]].Mul(scale);
    xVec3 v1 = verts[seg.i[1]].Mul(scale);
    xBox((v0 + v1) * 0.5f,
      vec3_forward.Fabs() * ((v1 - v0).LengthFast() * 0.5f) +
      vec3_right.Fabs() * (radius * seg.s) +
      vec3_up.Fabs() * (radius * seg.s),
      xAngles((v1-v0).Norm(), vec3_up).ToMat3()).ToPlanes(planes);

    model->AddSurf(new xSurface_Convex(planes, 6));
    convexList.Append(xPhysConvex::Create(planes, 6, 0.0005f));
  }
  model->SetPhysGeom(xPhysGeom::Create(convexList.Ptr(), convexList.Count()));
  
  return model;
}

