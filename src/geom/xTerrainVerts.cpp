#include <xForm.h>

// =================================================================
// =================================================================
// =================================================================

xTerrainVerts::xTerrainVerts()
{
}

xTerrainVerts::xTerrainVerts(const xVec2& size, const xVec2& gridSize)
{
  Init(size, gridSize);
}

int xTerrainVerts::MapIndex(int x, int y) const { return y*steps.x + x; }

float xTerrainVerts::Height(int x, int y) const { return heights[MapIndex(x,y)]; }
float& xTerrainVerts::Height(int x, int y){ return heights[MapIndex(x,y)]; }

xVec2 xTerrainVerts::Pos(int x, int y) const
{
  return corner 
    - vec3_forward.ToVec2() * ((float)y * gridSize.y)
    + vec3_right.ToVec2() * ((float)x * gridSize.x);
}

xVec3 xTerrainVerts::Vert(int x, int y) const
{
  xVec2 p = Pos(x, y);
  return xVec3(p.x, p.y, Height(x, y));
}

float xTerrainVerts::Height(const xVec2& pos)
{
  xVec2 offs = pos - corner;
  float f = vec3_forward.ToVec2() * -offs;
  float r = vec3_right.ToVec2() * offs;
  float py = f / gridSize.y;
  float px = r / gridSize.x;
  px = Clamp(px, 0.0f, steps.x-1.0f);
  py = Clamp(py, 0.0f, steps.y-1.0f);
  int ix = (int)px; // xMath::FtoiFast(p.x);
  int iy = (int)py; // xMath::FtoiFast(p.y);
  float dx = px - ix;
  float dy = py - iy;

  int nx = Min(ix+1, steps.x-1);
  int ny = Min(iy+1, steps.y-1);

  float p0 = Height(ix, iy);
  float p1 = Height(nx, iy);
  float p2 = Height(ix, ny);
  float p3 = Height(nx, ny);

  float p01 = p0 * (1 - dx) + p1 * dx;
  float p23 = p2 * (1 - dx) + p3 * dx;

  return p01 * (1 - dy) + p23 * dy;
}

void xTerrainVerts::MapPos(const xVec2& pos, int& x, int& y, bool nearest, int mip)
{
  xVec2 offs = pos - corner;
  float f = vec3_forward.ToVec2() * -offs;
  float r = vec3_right.ToVec2() * offs;
  float scale = (float)(1 << mip);
  float py = (f + (nearest ? gridSize.y * 0.5f * scale : 0)) / (gridSize.y * scale);
  float px = (r + (nearest ? gridSize.x * 0.5f * scale : 0)) / (gridSize.x * scale);
  x = Clamp((int)px << mip, 0, steps.x-1) >> mip;
  y = Clamp((int)py << mip, 0, steps.y-1) >> mip;
}

void xTerrainVerts::Init(const xVec2& p_size, const xVec2& grid)
{
  size = p_size;
  steps.x = Max(2, xMath::FtoiFast(size.x / grid.x) + 1);
  steps.y = Max(2, xMath::FtoiFast(size.y / grid.y) + 1);
  gridSize.x = size.x / (steps.x - 1);
  gridSize.y = size.y / (steps.y - 1);

  corner = vec3_forward.ToVec2() * (size.y * 0.5f)
    - vec3_right.ToVec2() * (size.x * 0.5f);

  minHeight = maxHeight = 0;
  heights.SetCount(steps.x * steps.y, true);
  
  /*
  xVec2 halfSize = size * 0.5f;
  for(int x = 0; x < steps.x; x++)
  {
    for(int y = 0; y < steps.y; y++)
    {
      xVec2 pos = Pos(x, y);
      heights[MapIndex(x, y)]
      xVec3 p = vec3_forward * (halfSize.y - y*gridSize.y)
                  - vec3_right * (halfSize.x - x*gridSize.x);
      p.x = xMath::EpsRound(p.x, TERRAIN_EPSILON_VERT);
      p.y = xMath::EpsRound(p.y, TERRAIN_EPSILON_VERT);
      Vert(x,y) = p;
    }
  }
  */
}

/*
xVec2 xTerrainVerts::MapVert(float xScale, float yScale)
{
  xScale = Clamp(xScale, 0.0f, 1.0f);
  yScale = Clamp(yScale, 0.0f, 1.0f);
  xVec3 p = vec3_forward * (size.y * (0.5f - yScale))
              - vec3_right * (size.x * (0.5f - xScale));
  return p.ToVec2();
}
*/

/*
int xTerrainVerts::CorrectAccuracy()
{
  int count = verts.Count();
  xVec3 * vert = verts.Ptr();
  for(; count > 0; count--, vert++){
    xVec3& v = *vert;
    vert.z = TERRAIN_EPSILON_ROUND(vert.z);
  }
}
*/

void xTerrainVerts::CreateHill(const xVec2& centerPos, float width, float height, bool setHeight)
{
  CreateHill(centerPos, width, height, -width * 0.5f, setHeight);
}
void xTerrainVerts::CreateHill(const xVec2& centerPos, float width, float height, float clearRadius, bool setHeight)
{
  for(int x = 0; x < steps.x; x++)
    for(int y = 0; y < steps.y; y++){
      xVec3 vert = Vert(x, y);
      float len = (xVec2(vert.x, vert.y) - centerPos).LengthFast();
      if(len >= clearRadius && len <= clearRadius + width){
        float inHill = (len - clearRadius) / width;
        float s = xMath::PI*2 * inHill - xMath::PI;
        float h = (xMath::Cos16(s)+1) * 0.5f * height;
        // h = xMath::EpsRound(h, TERRAIN_EPSILON_VERT);
        if(h >= 0 && vert.z < h){
          if(setHeight)
            vert.z = h;
          else
            vert.z += h;
          maxHeight = Max(maxHeight, vert.z);
          Height(x, y) = vert.z;
        }else if(h < 0 && vert.z > h){
          if(setHeight)
            vert.z = h;
          else
            vert.z += h;
          minHeight = Min(minHeight, vert.z);
          Height(x, y) = vert.z;
        }
      }
    }
}
void xTerrainVerts::ClearRadius(const xVec2& centerPos, float radius, float height)
{
  for(int x = 0; x < steps.x; x++)
    for(int y = 0; y < steps.y; y++){
      xVec2 pos = Pos(x,y);
      float len = (pos - centerPos).LengthFast();
      if(len <= radius)
        Height(x, y) = height;
    }
}

void xTerrainVerts::Smooth(int count)
{
  xArray<float> temp = heights;
  float newMinHeight = maxHeight, newMaxHeight = 0;
  int i;
  for(i = 0; i < count; i++){
    for(int x = 0; x < steps.x; x++)
      for(int y = 0; y < steps.y; y++){
        float h = 0;
        int cnt = 0;
        for(int dx = -1; dx <= 1; dx++)
          for(int dy = -1; dy <= 1; dy++, cnt++){
            int curX = Clamp(x + dx, 0, steps.x-1);
            int curY = Clamp(y + dy, 0, steps.y-1);
            h += Height(curX, curY);
          }
        h /= cnt;
        temp[MapIndex(x, y)] = h;
        if(i == count-1)
        {
          newMinHeight = Min(newMinHeight, h);
          newMaxHeight = Max(newMaxHeight, h);
        }
      }
    heights = temp;
  }
  if(newMaxHeight - newMinHeight <= 0)
    return;

  count = heights.Count();
  float midHeight = (minHeight + maxHeight) * 0.5f;
  float newMidHeight = (newMinHeight + newMaxHeight) * 0.5f;
  float scale = (maxHeight - minHeight) / (newMaxHeight - newMinHeight);
  for(i = 0; i < count; i++){
    float h = heights[i];
    // heights[i] = xMath::EpsRound((vert.z - newMidHeight) * scale + midHeight, TERRAIN_EPSILON_VERT);
    heights[i] = (h - newMidHeight) * scale + midHeight;
  }
}

//#define DEF_EPSILON_VERT 0.0001f // 1/10 mm
//#define DEF_EPSILON_PLANE_NORMAL 0.00001f
/*
void xTerrainVerts::Export(xArray<xDrawVert>& drawVerts, xArray<int>& indexes, int maxMergeCount, bool quadSplit)
{
  Export(drawVerts, indexes, (xArray<xPhysConvex*>)NULL, maxMergeCount, quadSplit, false);
}
void xTerrainVerts::Export(xArray<xDrawVert>& drawVerts, xArray<int>& indexes, xArray<xPhysConvex*>& convexList, int maxMergeCount, bool quadSplit, bool clearConvexList)
{
  Export(drawVerts, indexes, &convexList, maxMergeCount, quadSplit, clearConvexList);
}
*/

/*
void xTerrainVerts::Export(xBrushMap::Group * groupMap, xArray<xSurface*> * pConvexSurfList, xArray<xPhysConvex*> * pConvexList, int maxMergeCount, float downPlane, bool quadSplit, int clearConvexList, const xBrushMap::Material& material)
{
  xArray<xDrawVert> drawVerts;
  xArray<int> indexes;
  Export(drawVerts, indexes, groupMap, pConvexSurfList, pConvexList, maxMergeCount, downPlane, quadSplit, clearConvexList, material);
}

void xTerrainVerts::Export(xArray<xDrawVert>& drawVerts, xArray<int>& indexes, xBrushMap::Group * groupMap, xArray<xSurface*> * pConvexSurfList, xArray<xPhysConvex*> * pConvexList, int maxMergeCount, float downPlane, bool quadSplit, int clearConvexList, const xBrushMap::Material& material)
{
  indexes.Clear();
  drawVerts.Clear();
  if(pConvexSurfList && (clearConvexList & CLEAR_SURF))
    pConvexSurfList->Clear();
  if(pConvexList && (clearConvexList & CLEAR_PHYS))
    pConvexList->Clear();
  if(downPlane == xMath::INFINITY)
    downPlane = minHeight - 10;

  xDrawVert vert;
  memset(&vert, 0, sizeof(vert));

  xBitArray visited(verts.Count());
  xHashTable<xHashTableIndexKey,int> vertexMap(xMath::FloorPowerOfTwo(verts.Count()));

  if(maxMergeCount < 0)
    maxMergeCount = Max(steps.x, steps.y);

  int iv[4], map_iv[4];
  xPlaneExact p[3];
  xPlane sides[6];
  xBrushMap::Brush::Side::Desc mapSides[6];
  for(int x = 0; x < steps.x-1; x++)
    for(int y = 0; y < steps.y-1; y++){
      iv[0] = MapIndex(x,y);
      if(visited[iv[0]])
        continue;      

      int dx = 1, dy = 1;
      iv[1] = MapIndex(x+1,y);
      iv[2] = MapIndex(x+1,y+1);
      iv[3] = MapIndex(x,y+1);

      p[0].FromPoints(verts[iv[0]], verts[iv[1]], verts[iv[2]], true);
      p[1].FromPoints(verts[iv[0]], verts[iv[2]], verts[iv[3]], true);

      bool split = quadSplit, quadPlane;
      if(quadPlane = p[0].Compare(p[1], DEF_EPSILON_PLANE_NORMAL, DEF_EPSILON_VERT)){
        split = false;
        bool dxOpen = true, dyOpen = true;
        for(; dxOpen || dyOpen; ){
          if(dxOpen && dyOpen && visited[MapIndex(x+dx,y+dy)])
            break;
          if(dxOpen){
            if(dx < maxMergeCount && x+dx < steps.x-1 && !visited[MapIndex(x+dx,y)]){
              iv[1] = MapIndex(x+dx+1,y);
              iv[2] = MapIndex(x+dx+1,y+dy);
              iv[3] = MapIndex(x,y+dy);

              p[1].FromPoints(verts[iv[0]], verts[iv[1]], verts[iv[2]], true);
              p[2].FromPoints(verts[iv[0]], verts[iv[2]], verts[iv[3]], true);

              if(p[0].Compare(p[1], DEF_EPSILON_PLANE_NORMAL, DEF_EPSILON_VERT)
                  && p[0].Compare(p[2], DEF_EPSILON_PLANE_NORMAL, DEF_EPSILON_VERT))
                dx++;
              else
                dxOpen = false;
            }else
              dxOpen = false;
          }
          if(dyOpen){
            if(dy < maxMergeCount && y+dy < steps.y-1 && !visited[MapIndex(x,y+dy)]){
              iv[1] = MapIndex(x+dx,y);
              iv[2] = MapIndex(x+dx,y+dy+1);
              iv[3] = MapIndex(x,y+dy+1);

              p[1].FromPoints(verts[iv[0]], verts[iv[1]], verts[iv[2]], true);
              p[2].FromPoints(verts[iv[0]], verts[iv[2]], verts[iv[3]], true);

              if(p[0].Compare(p[1], DEF_EPSILON_PLANE_NORMAL, DEF_EPSILON_VERT)
                  && p[0].Compare(p[2], DEF_EPSILON_PLANE_NORMAL, DEF_EPSILON_VERT))
                dy++;
              else
                dyOpen = false;
            }else
              dyOpen = false;
          }
        }
      }
      for(int ax = 0; ax < dx; ax++)
        for(int ay = 0; ay < dy; ay++)
          visited.Set(MapIndex(x+ax, y+ay));

      iv[1] = MapIndex(x+dx,y);
      iv[2] = MapIndex(x+dx,y+dy);
      iv[3] = MapIndex(x,y+dy);

      int k;
      for(k = 0; k < 4; k++){
        int * p = vertexMap.Get(iv[k]);
        if(!p){
          vert.xyz = verts[iv[k]];
          map_iv[k] = drawVerts.Append(vert);
          vertexMap.Set(iv[k], map_iv[k]);
        }else
          map_iv[k] = *p;
      }
      if(!split){
        indexes.Append(map_iv[0]);
        indexes.Append(map_iv[1]);
        indexes.Append(map_iv[2]);

        indexes.Append(map_iv[0]);
        indexes.Append(map_iv[2]);
        indexes.Append(map_iv[3]);

        if(0 && quadPlane){
          if(pConvexSurfList || pConvexList){
            int planeCount = CreateBrushPlanes(sides, drawVerts, map_iv, 4, downPlane);
            if(pConvexSurfList)
              pConvexSurfList->Append(new xSurface_Convex(sides, planeCount));
            if(pConvexList)
              pConvexList->Append(xPhysConvex::Create(sides, planeCount, ON_EPSILON));
          }        
          if(groupMap){
            int sideCount = CreateBrushPlanes(mapSides, drawVerts, map_iv, 4, downPlane);
            groupMap->Map()->NewBrush(mapSides, sideCount, material, groupMap);
          }
        }else{
          for(int k = 0; k < 2; k++){
            int vertMap[3] = {map_iv[0], map_iv[k+1], map_iv[k+2]};
            
            if(pConvexSurfList || pConvexList){            
              int planeCount = CreateBrushPlanes(sides, drawVerts, vertMap, 3, downPlane);
              if(pConvexSurfList)
                pConvexSurfList->Append(new xSurface_Convex(sides, planeCount));
              if(pConvexList)
                pConvexList->Append(xPhysConvex::Create(sides, planeCount, ON_EPSILON));
            }        
            if(groupMap){
              int sideCount = CreateBrushPlanes(mapSides, drawVerts, vertMap, 3, downPlane);
              groupMap->Map()->NewBrush(mapSides, sideCount, material, groupMap);
            }
          }
        }
      }else{
        xVec3 v(0,0,0);
        for(k = 0; k < 4; k++)
          v += verts[iv[k]];
        v *= 1.0f/4;

        vert.xyz = v;
        int new_iv = drawVerts.Append(vert);
        for(k = 0; k < 4; k++){
          int vertMap[3] = {map_iv[k], map_iv[(k+1)%4], new_iv};
          indexes.Append(vertMap[0]);
          indexes.Append(vertMap[1]);
          indexes.Append(vertMap[2]);

          if(pConvexSurfList || pConvexList){            
            int planeCount = CreateBrushPlanes(sides, drawVerts, vertMap, 3, downPlane);
            if(pConvexSurfList)
              pConvexSurfList->Append(new xSurface_Convex(sides, planeCount));
            if(pConvexList)
              pConvexList->Append(xPhysConvex::Create(sides, planeCount, ON_EPSILON));
          }        
          if(groupMap){
            int sideCount = CreateBrushPlanes(mapSides, drawVerts, vertMap, 3, downPlane);
            groupMap->Map()->NewBrush(mapSides, sideCount, material, groupMap);
          }
        }
      }
    }
  //if(pConvexList)
  //  ExportPhys(*pConvexList, drawVerts, indexes, clearConvexList);
}
*/

int xTerrainVerts::CreateBrushPlanes(xPlane * out, xArray<xDrawVert>& drawVerts, int * vertMap, int vertCount, float downPlane)
{
  assert(vertCount >= 3);
  
  int planeNum = 0;
  out[planeNum++].FromPoints(drawVerts[vertMap[0]].xyz, drawVerts[vertMap[1]].xyz, drawVerts[vertMap[2]].xyz, true);

  out[planeNum].SetNormal(-vec3_up);
  out[planeNum++].FitThroughPoint(vec3_up * downPlane);

  for(int i = 0; i < vertCount; i++){
    xVec3& v0 = drawVerts[vertMap[(i+1) % vertCount]].xyz;
    xVec3& v1 = drawVerts[vertMap[i]].xyz;
    xVec3  v2 = v1 + vec3_up * downPlane;
    out[planeNum++].FromPoints(v0, v1, v2, true);
  }
  return planeNum;
}

/*
int xTerrainVerts::CreateBrushPlanes(xBrushMap::Brush::Side::Desc * out, xArray<xDrawVert>& drawVerts, int * vertMap, int vertCount, float downPlane)
{
  assert(vertCount >= 3);
  
  int planeNum = 0;
  //out[planeNum].texInfo.material = material;
  out[planeNum].texMaping = xBrushMap::TexMaping::Desc();
  out[planeNum++].plane.FromPoints(drawVerts[vertMap[0]].xyz.ToVec3d(), drawVerts[vertMap[1]].xyz.ToVec3d(), drawVerts[vertMap[2]].xyz.ToVec3d(), true);

  xVec3d up = vec3_up.ToVec3d();
  //out[planeNum].texInfo.material = noMaterial;
  out[planeNum].texMaping = xBrushMap::TexMaping::Desc();
  out[planeNum].plane.SetNormal(-up);
  out[planeNum++].plane.FitThroughPoint(up * downPlane);

  for(int i = 0; i < vertCount; i++){
    xVec3d v0 = drawVerts[vertMap[(i+1) % vertCount]].xyz.ToVec3d();
    xVec3d v1 = drawVerts[vertMap[i]].xyz.ToVec3d();
    xVec3d v2 = v1 + up * downPlane;
    //out[planeNum].texInfo.material = noMaterial;
    out[planeNum].texMaping = xBrushMap::TexMaping::Desc();
    out[planeNum++].plane.FromPoints(v0, v1, v2, true);
  }
  return planeNum;
}
*/

/*
void xTerrainVerts::ExportPhys(xArray<xPhysConvex*>& convexList, const xArray<xDrawVert>& drawVerts, const xArray<int>& indexes, bool clearList)
{
  if(clearList)
    convexList.Clear();

  xVec3 convexVerts[6];
  int count = indexes.Count();
  for(int i = 0; i < count-3; i+=3){
    for(int j = 0; j < 3; j++){
      //convexVerts[j] = verts[indexes[i+j]];
      convexVerts[j] = drawVerts[indexes[i+j]].xyz;      
      convexVerts[j+3] = convexVerts[j];
      convexVerts[j+3].z = minHeight - 5;
    }
#if 0
    convexList.Append(xPhysConvex::Create(convexVerts, 6));
#else
    static const int map[] = {
      0, 1, 2, // up
      5, 4, 3, // down
      1, 0, 3,
      2, 1, 4,
      0, 2, 5
    };
    const int planeCount = (sizeof(map) / sizeof(map[0])) / 3;
    xPlane planes[planeCount];
    for(int p = 0; p < planeCount; p++){
      planes[p].FromPoints(convexVerts[map[p*3+0]], convexVerts[map[p*3+1]], convexVerts[map[p*3+2]], true);
    }
    convexList.Append(xPhysConvex::Create(planes, planeCount, 0.0005f));
#endif       
  }
}

void xTerrainVerts::CreateDrawVerts(xArray<xDrawVert>& drawVerts)
{
  xDrawVert vert;
  memset(&vert, 0, sizeof(vert));

  int count = verts.Count();
  drawVerts.SetCount(count, true);
  for(int i = 0; i < count; i++){
    vert.xyz = verts[i];
    drawVerts[i] = vert;
  }
}

void xTerrainVerts::CreateIndexes(xArray<int>& indexes, bool quadSplit)
{
  indexes.Clear();

  int iv[4];
  for(int x = 0; x < steps.x-1; x++)
    for(int y = 0; y < steps.y-1; y++){          
      iv[0] = MapIndex(x,y);
      iv[1] = MapIndex(x+1,y);
      iv[2] = MapIndex(x+1,y+1);
      iv[3] = MapIndex(x,y+1);

      if(!quadSplit){
        indexes.Append(iv[0]);
        indexes.Append(iv[1]);
        indexes.Append(iv[2]);

        indexes.Append(iv[0]);
        indexes.Append(iv[2]);
        indexes.Append(iv[3]);
      }else{
        xVec3 v(0,0,0);
        int k;
        for(k = 0; k < 4; k++)
          v += verts[iv[k]];
        v *= 1.0f/4;

        int cur_iv = verts.Count();
        verts.Append(v);

        for(k = 0; k < 4; k++){
          indexes.Append(iv[k]);
          indexes.Append(iv[(k+1)%4]);
          indexes.Append(cur_iv);
        }
      }
    }
}
*/

