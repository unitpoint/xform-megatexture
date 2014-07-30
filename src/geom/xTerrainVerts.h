#ifndef __X_TERRAIN_VERTS_H__
#define __X_TERRAIN_VERTS_H__

#pragma once

#define TERRAIN_EPSILON_VERT  0.2f

class xTerrainVerts
{
  xArray<float> heights;

  struct {
    int x,y;
  } steps;

  xVec2 size, gridSize;
  float minHeight, maxHeight;

  xVec2 corner;


  // void ExportPhys(xArray<xPhysConvex*>& convexList, const xArray<xDrawVert>& drawVerts, const xArray<int>& indexes, bool clearList = true);
  int CreateBrushPlanes(xPlane * out, xArray<xDrawVert>& drawVerts, int * vertMap, int vertCount, float downPlane);
  // int CreateBrushPlanes(xBrushMap::Brush::Side::Desc * out, xArray<xDrawVert>& drawVerts, int * vertMap, int vertCount, float downPlane);

  // int CorrectAccuracy();

public:

  xTerrainVerts();
  xTerrainVerts(const xVec2& size, const xVec2& gridSize);

  int XPointsNumber() const { return steps.x; }
  int YPointsNumber() const { return steps.y; }

  float MaxHeight() const { return maxHeight; }

  int MapIndex(int x, int y) const;
  // xVec2 MapVert(float xScale, float yScale);

  float Height(int x, int y) const;
  float& Height(int x, int y);

  xVec2 Pos(int x, int y) const;
  xVec3 Vert(int x, int y) const;

  float Height(const xVec2& pos);
  void MapPos(const xVec2& pos, int& x, int& y, bool nearest = false, int mip = 0);

  const xVec2& Size() const { return size; }
  const xVec2& GridSize() const { return gridSize; } 

  void Init(const xVec2& size, const xVec2& gridSize);
  void CreateHill(const xVec2& centerPos, float width, float height, bool setHeight);
  void CreateHill(const xVec2& centerPos, float width, float height, float clearRadius, bool setHeight);
  void ClearRadius(const xVec2& centerPos, float radius, float height = 0);
  void Smooth(int count = 1);

  // void Export(xArray<xDrawVert>& drawVerts, xArray<int>& indexes, xBrushMap::Group * groupMap, xArray<xSurface*> * pConvexSurfList, xArray<xPhysConvex*> * pConvexList, int maxMergeCount = -1, float downPlane = xMath::INFINITY, bool quadSplit = false, int clearConvexList = CLEAR_SURF | CLEAR_PHYS, const xBrushMap::Material& material = xBrushMap::Material());
  // void Export(xBrushMap::Group * groupMap, xArray<xSurface*> * pConvexSurfList, xArray<xPhysConvex*> * pConvexList, int maxMergeCount = -1, float downPlane = xMath::INFINITY, bool quadSplit = false, int clearConvexList = CLEAR_SURF | CLEAR_PHYS, const xBrushMap::Material& material = xBrushMap::Material());
};

#endif
