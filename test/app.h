#pragma once

#include <xForm.h>

#define CONSOLE_TEXT_TIME 5.0f // in sec
class xConsoleTextList
{
  struct xConsoleText
  {
    float createTime;
    xString text;
    D3DCOLOR color;
    int key;

    xConsoleText(){}
    xConsoleText(const xString& s, D3DCOLOR c, int k = 0): text(s)
    {
      createTime = DXUtil_Timer(TIMER_GETAPPTIME);
      color = c;
      key = k;
    }
  };

  xArray<xConsoleText> list;

public:

  xConsoleTextList(): list(16){}
  ~xConsoleTextList(){}

  void Add(const xString& s, D3DCOLOR color, int key = 0)
  {
    xConsoleText item(s, color, key);
    if(key){
      for(int i = 0; i < list.Count(); i++)
        if(list[i].key == key){
          list[i] = item;
          return;
        }
    }
    while(list.Count() > 10)
      list.RemoveIndex(0);

    list.Append(item);
  }

  void RemoveOld()
  {
    float removeTime = DXUtil_Timer(TIMER_GETAPPTIME) - CONSOLE_TEXT_TIME;
    for(int i = 0; i < list.Count();){
      xConsoleText& item = list[i];
      if(item.createTime <= removeTime)
        list.RemoveIndex(i);
      else
        i++;
    }
  }

  void Render(CD3DFont * font, float x, float y)
  {
    SIZE size;
    int count = list.Count();
    for(int i = 0; i < count; i++){
      xConsoleText& item = list[i];
      font->DrawText(x+2, y+2, D3DCOLOR_ARGB(150,0,0,0), item.text); 
      font->DrawText(x+1, y+1, D3DCOLOR_ARGB(200,0,0,0), item.text); 
      font->DrawText(x, y, item.color, item.text); 
      font->GetTextExtent(item.text, &size);
      y += size.cy + 1;
    }
  }
};

#define TERRAIN_GRID          8
#define TERRAIN_SIZE          (TERRAIN_GRID * 64)
#define TERRAIN_MAX_HEIGHT    (TERRAIN_GRID * 4)
#define TERRAIN_CAMERA_HEIGHT 1.7f

#define TERRAIN_MIPS_NUMBER   7

#define SPEED_MS2KMH(value) ((value) * (3600.0f / 1000.0f))
#define SPEED_KMH2MS(value) ((value) * (1000.0f / 3600.0f))

#define CAMERA_MOVE_MAX_SPEED   SPEED_KMH2MS(200.0f)  
#define CAMERA_ROTATE_SPEED     70.0f

#define TERRAIN_MIP0_RADIUS   (TERRAIN_GRID * 6)
// #define TERRAIN_MIP0_ACCURATY_SIZE  2
// #define TERRAIN_MIP0_CACHE_SIZE     4

struct xMesh
{
  struct Vert
  {
    xVec3 pos;
    xVec3 normal;
    byte  color[4];
    xVec2 st[1];

    enum
    {
      FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2
    };

    void SetColor(float r, float g, float b, float a = 1.0f)
    {
      int c[] =
      {
        (int)(b * 255.0f),
        (int)(g * 255.0f),
        (int)(r * 255.0f),
        (int)(a * 255.0f)
      };
      for(int i = 0; i < 4; i++)
      {
        if(c[i] < 0)
          color[i] = 0;
        else if(c[i] > 255)
          color[i] = 255;
        else
          color[i] = (byte)c[i];
      }

    }

    void SetColor(const xVec3& color, float a = 1.0f)
    {
      SetColor(color.x, color.y, color.z, a);
    }
  };

  LPDIRECT3DVERTEXBUFFER9 vertsBuf;
  LPDIRECT3DINDEXBUFFER9 indicesBuf;

  int vertsNumber;
  int indicesNumber;

  xMesh()
  {
    vertsBuf = NULL;
    vertsNumber = 0;

    indicesBuf = NULL;
    indicesNumber = 0;
  }
  ~xMesh()
  {
    Clear();
  }

  void Clear()
  {
    SAFE_RELEASE(vertsBuf);
    SAFE_RELEASE(indicesBuf);
    vertsNumber = indicesNumber = 0;
  }

  bool CreateIndicesBuf(LPDIRECT3DDEVICE9 m_pd3dDevice, int nums)
  {
    if(indicesBuf && indicesNumber == nums)
    {
      return true;
    }
    SAFE_RELEASE(indicesBuf);
    indicesNumber = nums;
    HRESULT hr = m_pd3dDevice->CreateIndexBuffer( nums * sizeof(word), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indicesBuf, 0 );
    return !FAILED(hr);
  }

  word * LockIndices()
  {
    ASSERT(indicesBuf);
    void * ptr;
    if(FAILED(indicesBuf->Lock(0, 0, &ptr, 0)))
      return NULL;
    return (word*)ptr;
  }
  void UnlockIndices()
  {
    ASSERT(indicesBuf);
    indicesBuf->Unlock();
  }

  bool CreateVertsBuf(LPDIRECT3DDEVICE9 m_pd3dDevice, int nums)
  {
    if(vertsBuf && vertsNumber == nums)
    {
      return true;
    }
    SAFE_RELEASE(vertsBuf);
    vertsNumber = nums;
    HRESULT hr = m_pd3dDevice->CreateVertexBuffer( nums * sizeof(Vert), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &vertsBuf, 0 );
    return !FAILED(hr);
  }

  Vert * LockVerts()
  {
    ASSERT(vertsBuf);
    void * ptr;
    if(FAILED(vertsBuf->Lock(0, 0, &ptr, 0)))
      return NULL;
    return (Vert*)ptr;
  }
  void UnlockVerts()
  {
    ASSERT(vertsBuf);
    vertsBuf->Unlock();
  }
};

class xFormApp: public xForm
{
  CD3DFont * consoleFont;
  xConsoleTextList consoleTextList;

  CD3DFont * subFont;

  xString cmdLine;
  xArray<xString> cmdLineSubs;

  void ParseCmdLine(const xString& cmdLine);
  int FindCmdLine(const TCHAR * cmd);
  int FindCmdLine(const xString& cmd);
  xString CmdLine(int i);

  xFrustum frustum;

  xTerrainVerts terrainVerts;

  struct PositionPhysics
  {
    xVec3 origin;
    xAngles angles;

    xVec3 speedVec;
    // float speed;
    float dump;
    float maxSpeed;

    bool isLastAccelerated;

    PositionPhysics()
    {
      origin = vec3_origin;
      angles = xAngles(0, 0, 0);
      angles.pitch = 15.0f;
      speedVec = vec3_origin;
      // speed = 0;
      dump = 0.5f;
      maxSpeed = CAMERA_MOVE_MAX_SPEED;
      isLastAccelerated = false;
    }

    void Update(float dt, const xVec3& windSpeedVec = vec3_origin)
    {
      float speed = speedVec.LengthFast();
      if(speed > maxSpeed)
      {
        speedVec *= maxSpeed / speed;
      }
      else if(speed < -maxSpeed)
      {
        speedVec *= -maxSpeed / speed;
      }

      origin += (speedVec + windSpeedVec) * dt;

      if(!isLastAccelerated)
      {
        speedVec *= 1.0f - dump * dt;
        speed = speedVec.LengthFast();
        if(xMath::Fabs(speed) < SPEED_KMH2MS(2.0f))
        {
          speedVec = vec3_origin;
          speed = 0;
        }
      }
      isLastAccelerated = false;
    }
    void Accelerate(const xVec3& p_speedVec, float dt)
    {
      speedVec += p_speedVec * dt;
      isLastAccelerated = true;
    }
  } cameraPosition;

  xVec3 lightVec;

  struct MipCache
  {
    // xBounds2 bounds;
    int mipSize;
    // int x, y;
    // int subX, subY;
    // int subSize;

    int terrainCornerX;
    int terrainCornerY;
    int terrainSizeX;
    int terrainSizeY;

    xMesh mesh;

    LPDIRECT3DTEXTURE9 texture;
    int textureWidth;
    int textureHeight;

    MipCache(){ texture = NULL; }
    ~MipCache()
    {
      ASSERT(!texture);
    }

  } mipCaches[TERRAIN_MIPS_NUMBER];

  /*
  int cameraTerrainX;
  int cameraTerrainY;
  int cameraTerrainMip0Size;
  */

  void UpdateTerrainMipCache(int mip, float idealRadius);

  // xMesh terrainMesh;
  // xMesh terrainSubMesh;

  xMegaTexture megaTexture;

  xHashTable<xString, LPDIRECT3DTEXTURE9> textures;
  LPDIRECT3DTEXTURE9 Texture(const xString& name, bool generateMipMaps = true);

  HRESULT ConfirmDevice(D3DCAPS9 * caps, DWORD dwBehavior, D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat);

  void ResetRenderStates();

  bool isWireKeyLastPressed;
  bool isWire;

  bool isShowMipsKeyLastPressed;
  bool isShowMips;

  bool IsWire() const { return isWire; }

  xVec3 TerrainNormal(int x, int y);
  void CreateMesh(xMesh& out, int colorNum,
    int x, int y, int dx, int dy,
    int clipX, int clipY, int clipSizeX, int clipSizeY);

  void RenderMesh(const xMesh& mesh);

public:

  xFormApp(const xString& cmdLine);
  ~xFormApp();

  void Print(PrintType_t pt, const xString& fmt, va_list);

  HRESULT OneTimeSceneInit();
  HRESULT InitDeviceObjects();
  HRESULT RestoreDeviceObjects();
  HRESULT InvalidateDeviceObjects();
  HRESULT DeleteDeviceObjects();
  HRESULT Render();
  HRESULT FrameMove();
  HRESULT FinalCleanup();

};