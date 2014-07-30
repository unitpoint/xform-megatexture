#include "app.h"

xFormApp::xFormApp(const xString& cmdLine)
{
  m_strWindowTitle = _T("xForm2 megatexture demo");
  m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
  m_d3dEnumeration.AppMinDepthBits = 16;
  m_d3dEnumeration.AppMinStencilBits = 4;
  m_bShowCursorWhenFullscreen = TRUE;
#ifndef DEBUG
  m_dwCreationWidth = 1000;
  m_dwCreationHeight = 800;
#else
  m_dwCreationWidth = 1000;
  m_dwCreationHeight = 800;
#endif

  consoleFont = new CD3DFont(_T("Arial"), 10);
  subFont = new CD3DFont(_T("Arial"), 8);

  isWire = false;
  isWireKeyLastPressed = false;

  isShowMips = false;
  isShowMipsKeyLastPressed = false;

  ParseCmdLine(cmdLine);
}

xFormApp::~xFormApp()
{
  delete consoleFont;
  delete subFont;
}

void xFormApp::ParseCmdLine(const xString& cmdLine)
{
  this->cmdLine = cmdLine;
  cmdLineSubs.Clear();

  int i = 0, len = cmdLine.Len();
  while(i < len){
    for(; cmdLine.Char(i) && i < len; i++)
      if(!_istspace(cmdLine.Char(i)))
        break;

    int start = i;
    for(; cmdLine.Char(i) && i < len; i++)
      if(_istspace(cmdLine.Char(i)))
        break;

    int subLen = i - start;
    if(subLen)
      cmdLineSubs.Append(cmdLine.SubString(start, subLen));
  }
}
int xFormApp::FindCmdLine(const TCHAR * cmd)
{
  for(int i = 0; i < cmdLineSubs.Count(); i++)
    if(cmdLineSubs[i] == cmd)
      return i;

  return -1;
}
int xFormApp::FindCmdLine(const xString& cmd)
{
  return FindCmdLine(cmd.ToChar());
}

xString xFormApp::CmdLine(int i)
{
  return i >= 0 && i < cmdLineSubs.Count() ? cmdLineSubs[i] : xString();
}

static xString ChangeFilenameExt(const xString& filename, const xString& ext)
{
  const TCHAR * str = filename.ToChar();
  for(int i = filename.Len()-1; i >= 0; i--)
    if(_tcschr(_T("./\\:"), str[i])){
      if(str[i] == _T(".")[0])
        return ext == str+i ? filename : filename.SubString(0, i) + ext;
      break;
    }

  return filename + ext;
}

void xFormApp::Print(PrintType_t pt, const xString& fmt, va_list va)
{
  D3DCOLOR color;
  const TCHAR * t = _T("");
  switch(pt){
#ifndef X_DEV
  case PT_DevMessage:
  case PT_DevWarning:
  case PT_DevError:
    return;
#else
  case PT_DevMessage:
    {
      color = D3DCOLOR_ARGB(255,155,155,255);
      t = _T("DevMessage: ");
      xString s = xString::Format(fmt, va);
      consoleTextList.Add(t + s, color, -10);
    }
    break;
  case PT_DevWarning:
    color = D3DCOLOR_ARGB(255,155,155,255);
    t = _T("DevWarning: ");
    break;
  case PT_DevError:
    color = D3DCOLOR_ARGB(255,255,155,155);
    t = _T("DevError: ");
    break;
#endif

  case PT_Message:
    color = D3DCOLOR_ARGB(255,155,155,255);
    t = _T("Message: ");
    break;
  case PT_Warning:
    color = D3DCOLOR_ARGB(255,155,155,255);
    t = _T("Warning: ");
    break;
  case PT_Error:
    color = D3DCOLOR_ARGB(255,255,155,155);
    t = _T("Error: ");
    break;
  case PT_FatalError:
    color = D3DCOLOR_ARGB(255,255,0,0);
    t = _T("Fatal: ");
    break;
  }

  // xString s = xString::Format(fmt, va);
  // consoleTextList.Add(t + s, color);

  if(pt == PT_FatalError){
    assert(0);
    exit(1);
  }
}

HRESULT xFormApp::ConfirmDevice(D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat)
{
  // Make sure device supports point lights
  if((dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING) ||
     (dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING) )
  {
    if(0 == (pCaps->VertexProcessingCaps & D3DVTXPCAPS_POSITIONALLIGHTS))
      return E_FAIL;
  }

  /* if(//!(pCaps->RasterCaps & D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS) ||
     !(pCaps->RasterCaps & D3DPRASTERCAPS_DEPTHBIAS))
  {
    return E_FAIL;
  } */

  /*
  if(pCaps->VertexShaderVersion < D3DVS_VERSION(2,0))
    return E_FAIL;

  if(pCaps->PixelShaderVersion < D3DPS_VERSION(2,0))
    return E_FAIL;
  */

  // Need to support post-pixel processing (for fog and stencil)
  if(FAILED(m_pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,
        adapterFormat, D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
        D3DRTYPE_SURFACE, backBufferFormat)))
  {
    return E_FAIL;
  }

  return S_OK;
}

HRESULT xFormApp::OneTimeSceneInit()
{
  consoleTextList.Add(_T("OneTimeSceneInit"), D3DCOLOR_ARGB(255,255,200,200));
  InitInput();
  InitSound();

  xString megaFilename;

  int i = FindCmdLine(_T("-make"));
  if(i < 0)
  {
    FILE * f;
    errno_t err = _tfopen_s(&f, _T("../textures/mega-8192/mega-6-128-0x0.tga"), _T("rb"));
    if(err != 0)
    {
      cmdLineSubs.Clear();
      cmdLineSubs.Append(_T("-make"));
      cmdLineSubs.Append(_T("../textures/mega-8192.tga"));
      i = 0;
    }
    else
    {
      fclose(f);
    }
  }
  if(i >= 0)
  {
    xString srcFilename = CmdLine(i+1);
    megaFilename = ChangeFilenameExt(srcFilename, _T(""));
    CreateDirectory(megaFilename, NULL);
    megaFilename = megaFilename + _T("/mega");
    xMegaTexture::Make(srcFilename, megaFilename);
  }

  if(megaFilename.IsEmpty())
  {
    if((i = FindCmdLine(_T("-mega"))) >= 0)
      megaFilename = CmdLine(i+1);
    else
    {
      megaFilename = _T("../textures/mega-8192/mega");
    }
  }

  if(!megaFilename.IsEmpty())
  {
    megaTexture.Init(megaFilename, 7, 128, -1); // (int)(TERRAIN_MIP0_RADIUS / TERRAIN_GRID));
    // megaTexture.UpdateLayers(7, 3, 4, 4);
  }

  lightVec = xAngles(40.0f, 40.0f, 0.0f).ToForward();

  return S_OK;
}

HRESULT xFormApp::InitDeviceObjects()
{
  consoleTextList.Add(_T("InitDeviceObjects"), D3DCOLOR_ARGB(255,255,200,200));
  consoleFont->InitDeviceObjects(m_pd3dDevice);
  subFont->InitDeviceObjects(m_pd3dDevice);

  terrainVerts.Init(xVec2(TERRAIN_SIZE, TERRAIN_SIZE), xVec2(TERRAIN_GRID, TERRAIN_GRID));
  terrainVerts.CreateHill(xVec2(0,0), TERRAIN_SIZE * 0.2f, TERRAIN_MAX_HEIGHT * 0.7f, TERRAIN_SIZE * 0.32f, true);
  
  xRandom r = xRandom(12345);
  for(int i = 0; i < 6; i++)
  {
    terrainVerts.CreateHill(
      xVec2(TERRAIN_SIZE * 0.4f * r.CRandomFloat(), TERRAIN_SIZE * 0.4f * r.CRandomFloat()), 
      TERRAIN_SIZE * 0.4f * (0.5f + r.RandomFloat() * 0.5f), 
      TERRAIN_MAX_HEIGHT * (0.2f + r.RandomFloat() * 0.8f), 
      true);
  }

  terrainVerts.Smooth(1);

  return S_OK;
}

xVec3 xFormApp::TerrainNormal(int i, int j)
{
  int xPointsNumber = terrainVerts.XPointsNumber();
  int yPointsNumber = terrainVerts.YPointsNumber();

  xVec3 p       = terrainVerts.Vert(i, j);
  xVec3 pLeft   = terrainVerts.Vert(Max(i-1, 0), j);
  xVec3 pRight  = terrainVerts.Vert(Min(i+1, xPointsNumber-1), j);
  xVec3 pTop    = terrainVerts.Vert(i, Max(j-1, 0));
  xVec3 pBottom = terrainVerts.Vert(i, Min(j+1, yPointsNumber-1));

  xVec3 norm0 = (p - pTop   ).Cross(p - pLeft   ).Norm();
  xVec3 norm1 = (p - pRight ).Cross(p - pTop    ).Norm();
  xVec3 norm2 = (p - pBottom).Cross(p - pRight  ).Norm();
  xVec3 norm3 = (p - pLeft  ).Cross(p - pBottom ).Norm();

  xVec3 normal = (norm0 + norm1 + norm2 + norm3).Norm();
  return normal;
}

struct VertKey
{
  int x, y;

  VertKey(){}
  VertKey(int ax, int ay){ x = ax; y = ay; }
  VertKey(const VertKey& b){ x = b.x; y = b.y; }
  int Cmp(const VertKey& b)
  {
    return MEMCMP(this, &b, sizeof(b));
  }
  int Hash() const { return (x<<16) ^ y; }
};

void xFormApp::CreateMesh(xMesh& out, int colorNum, int x, int y, int dx, int dy,
                          int clipX, int clipY, int clipSizeX, int clipSizeY)
{
  if(!dx || !dy)
  {
    out.Clear();
    return;
  }

  dx++;
  dy++;

  static xVec3 colors[] = {
    xVec3(1.0f, 1.0f, 1.0f),
    xVec3(1.0f, 0.0f, 0.0f),
    xVec3(0.0f, 1.0f, 0.0f),
    xVec3(0.0f, 0.0f, 1.0f),
    xVec3(1.0f, 0.5f, 0.0f),
    xVec3(0.0f, 1.0f, 0.5f),
    xVec3(0.5f, 0.0f, 1.0f),
    xVec3(1.0f, 1.0f, 0.0f),
  };
  static int colorsNumber = sizeof(colors) / sizeof(colors[0]);
  xVec3 color = colors[colorNum % colorsNumber];

  xHashTable<VertKey, int> map;
  xArray<VertKey> srcKeys;
  xArray<word> indices;

  srcKeys.Resize(dx * dy);
  indices.Resize((dx-1)*(dy-1)*2*3);

  for(int j = 0; j < dy-1; j++)
  {
    for(int i = 0; i < dx-1; i++)
    {
      if(x+i >= clipX && x+i < clipX + clipSizeX &&
         y+j >= clipY && y+j < clipY + clipSizeY)
        continue;

      VertKey keys[] =
      {
        VertKey(i+x, j+y),
        VertKey(i+x+1, j+y),
        VertKey(i+x+1, j+y+1),
        VertKey(i+x, j+y+1)
      };

      word indexes[4];
      for(int k = 0; k < 4; k++)
      {
        int * p = map.Get(keys[k]);
        if(!p)
        {
          p = map.Set(keys[k], srcKeys.Append(keys[k]));
          ASSERT(p);
        }
        indexes[k] = (word)*p;
      }

      indices.Append(indexes[0]);
      indices.Append(indexes[1]);
      indices.Append(indexes[3]);

      indices.Append(indexes[3]);
      indices.Append(indexes[1]);
      indices.Append(indexes[2]);

      // out.indicesNumber += 6;
    }
  }

  if(!out.CreateVertsBuf(m_pd3dDevice, srcKeys.Count()) || 
     !out.CreateIndicesBuf(m_pd3dDevice, indices.Count()))
  {
    out.Clear();
    return;
  }

  word * indicesPtr = out.LockIndices();
  ASSERT(indicesPtr);
  MEMCPY(indicesPtr, indices.Ptr(), sizeof(word) * indices.Count());
  out.UnlockIndices();

  xMesh::Vert * vert = out.LockVerts();
  ASSERT(vert);

  float overSizeX = 1.0f / (float)(dx-1);
  float overSizeY = 1.0f / (float)(dy-1);

  for(int i = 0; i < srcKeys.Count(); i++, vert++)
  {
    const VertKey& k = srcKeys[i];
    vert->pos = terrainVerts.Vert(k.x, k.y);
    vert->normal = vec3_up; // TerrainNormal(i, j);

    vert->SetColor(color);
    vert->st[0] = xVec2((float)(k.x - x) * overSizeX, (float)(k.y - y) * overSizeY);
  }
  out.UnlockVerts();
}

LPDIRECT3DTEXTURE9 xFormApp::Texture(const xString& name, bool generateMipMaps)
{
  LPDIRECT3DTEXTURE9 * pTexture = textures.Get(name);
  if(pTexture)
    return *pTexture;

  LPDIRECT3DTEXTURE9 texture = NULL;
  
  xString filename = name;
  if(FAILED(D3DUtil_CreateTexture(m_pd3dDevice, filename.ToChar(), &texture))){
    filename = _T("../textures/") + filename;
    if(FAILED(D3DUtil_CreateTexture(m_pd3dDevice, filename.ToChar(), &texture)))
      texture = NULL;
  }
  if(texture && generateMipMaps)
    texture->GenerateMipSubLevels();

  textures.Set(name, texture);  
  return texture;
}

static int DeleteTexture(const xString& k, LPDIRECT3DTEXTURE9& t, void * params)
{
  SAFE_RELEASE(t);
  return ITERATE_CONTINUE; //ITERATE_DELETE_ITEM;
}

HRESULT xFormApp::DeleteDeviceObjects()
{
  textures.ForEach(DeleteTexture, this);
  textures.Clear();

  consoleTextList.Add(_T("DeleteDeviceObjects"), D3DCOLOR_ARGB(255,255,200,200));
  consoleFont->DeleteDeviceObjects();
  subFont->DeleteDeviceObjects();

  return S_OK;
}

HRESULT xFormApp::FinalCleanup()
{
  // delete carSound;

  // ShutdownSound();
  // ShutdownInput();
  SAFE_DELETE(consoleFont);
  SAFE_DELETE(subFont);
  return S_OK;
}

HRESULT xFormApp::RestoreDeviceObjects()
{
  consoleTextList.Add(_T("RestoreDeviceObjects"), D3DCOLOR_ARGB(255,255,200,200));

  consoleFont->RestoreDeviceObjects();
  subFont->RestoreDeviceObjects();

  ResetRenderStates();
  
  //frustum.SetSize(1, 1000, 50, 50); // m_d3dsdBackBuffer.Width/2, m_d3dsdBackBuffer.Height/2);
  SetViewport(0, 0, m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height);

  float aspect = (float)m_d3dsdBackBuffer.Width / (float)m_d3dsdBackBuffer.Height;
  frustum.SetPerspectiveByFovxAspectWidth(110, aspect, 0.05f, 3000);

  for(int i = 0; i < TERRAIN_MIPS_NUMBER; i++)
  {
    mipCaches[i].terrainCornerX = -999999999;
  }

  return S_OK;
}

HRESULT xFormApp::InvalidateDeviceObjects()
{
  // terrainMesh.Clear();
  
  for(int i = 0; i < TERRAIN_MIPS_NUMBER; i++)
  {
    mipCaches[i].mesh.Clear();
    SAFE_RELEASE(mipCaches[i].texture);
  }

  consoleTextList.Add(_T("InvalidateDeviceObjects"), D3DCOLOR_ARGB(255,255,200,200));
  consoleFont->InvalidateDeviceObjects();
  subFont->InvalidateDeviceObjects();

  return S_OK;
}

void xFormApp::ResetRenderStates()
{
  // Create and set up the shine materials w/ textures
  D3DMATERIAL9 mtrl;
  D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
  m_pd3dDevice->SetMaterial( &mtrl );

  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
  m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
  m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
  m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
  m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

  m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
  m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

  {
    // Set default values for all dx render states.
    SetRenderState( D3DRS_ZENABLE, TRUE );
    // SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
    SetRenderState(D3DRS_FILLMODE, IsWire() ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
    SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
    SetRenderState( D3DRS_LASTPIXEL, TRUE );
    SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    SetRenderState( D3DRS_DITHERENABLE, TRUE );
    SetRenderState( D3DRS_FOGENABLE, FALSE );
    SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    SetRenderState( D3DRS_FOGCOLOR, 0 );
    SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
    SetFloatRenderState( D3DRS_FOGSTART, 0.0f );
    SetFloatRenderState( D3DRS_FOGEND, 1.0f );
    SetFloatRenderState( D3DRS_FOGDENSITY, 0.0f );
    SetRenderState( D3DRS_RANGEFOGENABLE, FALSE );
    SetRenderState( D3DRS_STENCILENABLE, FALSE);
    SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
    SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
    SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
    SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
    SetRenderState( D3DRS_STENCILREF, 0 );
    SetRenderState( D3DRS_STENCILMASK, 0xFFFFFFFF );
    SetRenderState( D3DRS_STENCILWRITEMASK, 0xFFFFFFFF );
    SetRenderState( D3DRS_TEXTUREFACTOR, 0xFFFFFFFF );
    SetRenderState( D3DRS_WRAP0, 0 ); //D3DWRAP_U | D3DWRAP_V );
    SetRenderState( D3DRS_WRAP1, 0 );
    SetRenderState( D3DRS_WRAP2, 0 );
    SetRenderState( D3DRS_WRAP3, 0 );
    SetRenderState( D3DRS_WRAP4, 0 );
    SetRenderState( D3DRS_WRAP5, 0 );
    SetRenderState( D3DRS_WRAP6, 0 );
    SetRenderState( D3DRS_WRAP7, 0 );
    SetRenderState( D3DRS_CLIPPING, TRUE );
    SetRenderState( D3DRS_LIGHTING, TRUE );
    SetRenderState( D3DRS_AMBIENT, 0 );
    SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
    SetRenderState( D3DRS_COLORVERTEX, TRUE );
    SetRenderState( D3DRS_LOCALVIEWER, TRUE );
    SetRenderState( D3DRS_NORMALIZENORMALS, FALSE );
    SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
    SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2 );
    SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
    SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL );
    SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );
    SetRenderState( D3DRS_CLIPPLANEENABLE, 0 );
    SetFloatRenderState( D3DRS_POINTSIZE, 1.0f );
    SetFloatRenderState( D3DRS_POINTSIZE_MIN, 1.0f );
    SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
    SetRenderState( D3DRS_POINTSCALEENABLE, FALSE );
    SetFloatRenderState( D3DRS_POINTSCALE_A, 1.0f );
    SetFloatRenderState( D3DRS_POINTSCALE_B, 0.0f );
    SetFloatRenderState( D3DRS_POINTSCALE_C, 0.0f );
    SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, TRUE );
    SetRenderState( D3DRS_MULTISAMPLEMASK, 0xFFFFFFFF );
    SetRenderState( D3DRS_PATCHEDGESTYLE, D3DPATCHEDGE_DISCRETE );
    SetRenderState( D3DRS_DEBUGMONITORTOKEN, D3DDMT_ENABLE );

    SetFloatRenderState( D3DRS_POINTSIZE_MAX, 64.0f );
    SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
    SetFloatRenderState( D3DRS_TWEENFACTOR, 0.0f );
    SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
    SetRenderState( D3DRS_POSITIONDEGREE, D3DDEGREE_CUBIC );
    SetRenderState( D3DRS_NORMALDEGREE, D3DDEGREE_LINEAR );
    SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE);
    SetFloatRenderState( D3DRS_SLOPESCALEDEPTHBIAS, 0.0f );
    SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, FALSE );
    SetFloatRenderState( D3DRS_MINTESSELLATIONLEVEL, 1.0f );
    SetFloatRenderState( D3DRS_MAXTESSELLATIONLEVEL, 1.0f );
    SetFloatRenderState( D3DRS_ADAPTIVETESS_X, 0.0f );
    SetFloatRenderState( D3DRS_ADAPTIVETESS_Y, 0.0f );
    SetFloatRenderState( D3DRS_ADAPTIVETESS_Z, 1.0f );
    SetFloatRenderState( D3DRS_ADAPTIVETESS_W, 0.0f );
    SetRenderState( D3DRS_ENABLEADAPTIVETESSELLATION, FALSE );
    SetRenderState( D3DRS_TWOSIDEDSTENCILMODE, FALSE );
    SetRenderState( D3DRS_CCW_STENCILFAIL, 0x00000001);
    SetRenderState( D3DRS_CCW_STENCILZFAIL, 0x00000001 );
    SetRenderState( D3DRS_CCW_STENCILPASS, 0x00000001 );
    SetRenderState( D3DRS_CCW_STENCILFUNC, 0x00000008 );
    SetRenderState( D3DRS_COLORWRITEENABLE1, 0x0000000f );
    SetRenderState( D3DRS_COLORWRITEENABLE2, 0x0000000f );
    SetRenderState( D3DRS_COLORWRITEENABLE3, 0x0000000f);
    SetRenderState( D3DRS_BLENDFACTOR, 0xffffffff );
    SetRenderState( D3DRS_SRGBWRITEENABLE, 0);
    SetFloatRenderState( D3DRS_DEPTHBIAS, 0.0f );
    SetRenderState( D3DRS_WRAP8, 0 );
    SetRenderState( D3DRS_WRAP9, 0 );
    SetRenderState( D3DRS_WRAP10, 0 );
    SetRenderState( D3DRS_WRAP11, 0 );
    SetRenderState( D3DRS_WRAP12, 0 );
    SetRenderState( D3DRS_WRAP13, 0 );
    SetRenderState( D3DRS_WRAP14, 0 );
    SetRenderState( D3DRS_WRAP15, 0 );
    SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, FALSE );
    SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_ONE );
    SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO );
    SetRenderState( D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD );
  }
}

void xFormApp::RenderMesh(const xMesh& mesh)
{
  // SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
  
  m_pd3dDevice->SetFVF(xMesh::Vert::FVF);
  
  m_pd3dDevice->SetStreamSource(0, mesh.vertsBuf, 0, sizeof(xMesh::Vert));
  m_pd3dDevice->SetIndices(mesh.indicesBuf);

  m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 
    0, 0, mesh.vertsNumber, 0, mesh.indicesNumber / 3);
}

HRESULT xFormApp::Render()
{
  m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                       D3DCOLOR_ARGB(255,120,120,120), 1.0f, 0L );

  ResetRenderStates();

  // Begin the scene
  if(FAILED(m_pd3dDevice->BeginScene()))
    return S_OK;

  SetRenderState( D3DRS_LIGHTING, FALSE );
  // m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
  // m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

  // if(curCarEnt)
  //   frustum.SetPosition(curCarEnt->Follower()->Origin(), curCarEnt->Follower()->Angles());

  SetFrustum(frustum);

  // m_pd3dDevice->SetTexture(0, NULL); // Texture(_T("Zemla_2.jpg")));
  // RenderMesh(terrainMesh);

  for(int i = 0; i < TERRAIN_MIPS_NUMBER; i++)
  {
    m_pd3dDevice->SetTexture(0, mipCaches[i].texture);
    RenderMesh(mipCaches[i].mesh);
  }

  consoleTextList.Render(consoleFont, 5, 5);

  m_pd3dDevice->EndScene();
  return S_OK;
}

void xFormApp::UpdateTerrainMipCache(int mip, float idealRadius)
{
  MipCache& mipCache = mipCaches[mip];

  xVec2 cameraPosVec2 = cameraPosition.origin.ToVec2();
  float height = terrainVerts.Height(cameraPosVec2);
  height = Max(cameraPosition.origin.z - TERRAIN_CAMERA_HEIGHT, height)  - height;
  float radius = Max(0.0f, idealRadius * (float)(1 << mip) - height);
  
  int mipSize = (int)((radius + TERRAIN_GRID * 0.5f) / TERRAIN_GRID) >> mip;

  if(!mipSize)
  {
    for(int i = mip+1; i < TERRAIN_MIPS_NUMBER; i++)
    {
      mipCaches[i].terrainCornerX = -999999999;
    }

    mipCache.mipSize = 0;
    // mipCache.x = -999999;
    // mipCache.y = -999999;

    mipCache.terrainCornerX = -999999;
    mipCache.terrainCornerY = -999999;
    mipCache.terrainSizeX = 0;
    mipCache.terrainSizeY = 0;
    
    mipCache.mesh.Clear();
    SAFE_RELEASE(mipCache.texture);
    
    return;
  }

  int x, y;
  terrainVerts.MapPos(cameraPosVec2, x, y, true, mip);

  int ax = x - mipSize/2;
  int ay = y - mipSize/2;
  int dx = mipSize;
  int dy = mipSize;
  int terrainWidth = (terrainVerts.XPointsNumber()-1)>>mip;
  int terrainHeight = (terrainVerts.YPointsNumber()-1)>>mip;

  if(ax + dx > terrainWidth)
  {
    ax = terrainWidth - dx;
  }
  if(ay + dy > terrainHeight)
  {
    ay = terrainHeight - dy;
  }
  ax = Max(0, ax);
  ay = Max(0, ay);
  if(ax + dx > terrainWidth)
  {
    dx = terrainWidth - ax;
  }
  if(ay + dy > terrainHeight)
  {
    dy = terrainHeight - ay;
  }

  int terrainCornerX = ax << mip;
  int terrainCornerY = ay << mip;
  int terrainSizeX = dx << mip;
  int terrainSizeY = dy << mip;

  if(mip > 0 && mipCaches[mip-1].mipSize > 0)
  {
    if(terrainCornerX == mipCaches[mip-1].terrainCornerX)
    {
      if(ax > 0)
      {
        terrainCornerX -= 1 << mip;
        ax--;
      }
    }
    if(terrainCornerY == mipCaches[mip-1].terrainCornerY)
    {
      if(ay > 0)
      {
        terrainCornerY -= 1 << mip;
        ay--;
      }
    }
    if(terrainCornerX + terrainSizeX == mipCaches[mip-1].terrainCornerX + mipCaches[mip-1].terrainSizeX)
    {
      if(ax + dx < terrainWidth)
      {
        terrainSizeX += 1 << mip;
        dx++;
      }
    }
    if(terrainCornerY + terrainSizeY == mipCaches[mip-1].terrainCornerY + mipCaches[mip-1].terrainSizeY)
    {
      if(ay + dy < terrainHeight)
      {
        terrainSizeY += 1 << mip;
        dy++;
      }
    }
  }

  if(mipSize != mipCache.mipSize
      // || x != mipCache.x || y != mipCache.y
      || terrainCornerX != mipCache.terrainCornerX
      || terrainCornerY != mipCache.terrainCornerY
      || terrainSizeX != mipCache.terrainSizeX
      || terrainSizeY != mipCache.terrainSizeY
      )
  {
    for(int i = mip+1; i < TERRAIN_MIPS_NUMBER; i++)
    {
      mipCaches[i].terrainCornerX = -999999999;
    }

    mipCache.mipSize = mipSize;
    // mipCache.x = x;
    // mipCache.y = y;

    mipCache.terrainCornerX = terrainCornerX;
    mipCache.terrainCornerY = terrainCornerY;
    mipCache.terrainSizeX = terrainSizeX;
    mipCache.terrainSizeY = terrainSizeY;

    int clipX = -1, clipY = -1, clipSizeX = 0, clipSizeY = 0;
    if(mip > 0)
    {
      clipX = mipCaches[mip-1].terrainCornerX;
      clipY = mipCaches[mip-1].terrainCornerY;
      clipSizeX = mipCaches[mip-1].terrainSizeX;
      clipSizeY = mipCaches[mip-1].terrainSizeY;

      if(clipX == terrainCornerX && clipY == terrainCornerY
          && clipSizeX == terrainSizeX && clipSizeY == terrainSizeY)
      {
        mipCache.mesh.Clear();
        SAFE_RELEASE(mipCache.texture);
        return;
      }
    }

    CreateMesh(mipCache.mesh, isShowMips ? mip+1 : 0,
      mipCache.terrainCornerX, mipCache.terrainCornerY, 
      mipCache.terrainSizeX, mipCache.terrainSizeY,
      clipX, clipY, clipSizeX, clipSizeY);

    HRESULT hr;

    int textureWidth = megaTexture.ClusterSize() * dx;
    int textureHeight = megaTexture.ClusterSize() * dy;
    if(!mipCache.texture 
        || textureWidth != mipCache.textureWidth
        || textureHeight != mipCache.textureHeight)
    {
      SAFE_RELEASE(mipCache.texture);
      hr = m_pd3dDevice->CreateTexture(
          mipCache.textureWidth = textureWidth,
          mipCache.textureHeight = textureHeight,
          1, // levels
          0, // D3DUSAGE_DYNAMIC, // usage
          D3DFMT_A8R8G8B8, // format
          D3DPOOL_MANAGED,
          &mipCache.texture,
          NULL
        );
      ASSERT(!FAILED(hr));
      ASSERT(mipCache.texture);
    }
    else
    {
      // volatile int i = 0;
    }

    megaTexture.UpdateLayer(mip, ax, ay, dx, dy);

    D3DLOCKED_RECT rect;
    hr = mipCache.texture->LockRect(0, &rect, NULL, D3DLOCK_DISCARD);
    ASSERT(!FAILED(hr));

    megaTexture.GetTexture(mip, ax, ay, dx, dy, (byte*)rect.pBits, rect.Pitch, 
      textureWidth,
      textureHeight,
      32);

    mipCache.texture->UnlockRect(0);
    // mipCache.texture->GenerateMipSubLevels();
  }
}

HRESULT xFormApp::FrameMove()
{
  consoleTextList.RemoveOld();
  ReadInput();

  static float oldTime = DXUtil_Timer(TIMER_GETAPPTIME)-0.1f;
  float curTime = DXUtil_Timer(TIMER_GETAPPTIME);
  float dt = curTime - oldTime;
  if(dt > 0.1f)
    dt = 0.1f;
  //world->Simulate(0.02f);
  // world->Simulate(curTime-oldTime);
  oldTime = curTime;

  static float lastTimeVis = -100;
  if(DXUtil_Timer(TIMER_GETAPPTIME) - lastTimeVis > 0.2f){
    lastTimeVis = DXUtil_Timer(TIMER_GETAPPTIME);

    xHeap::SummaryStats total;
    xHeap::Instance()->GetStats(total);

    consoleTextList.Add(xString::Format(_T("xForm2 megatexture demo. Evgeny Golovin (c) craft@softvariant.ru\nUse arrows keys to control the camera, LSHIFT - up, CTRL - down, A - look up, Z - look down, M - show mipmaps")), D3DCOLOR_ARGB(255,255,255,0),
      1);

    consoleTextList.Add(xString::Format(_T("time: %.1f s, allocated: %.2f Mb (%d blocks)")
      //_T("\n  all allocs: %.2f Mb (%d blocks), all frees: %.2f Mb (%d blocks)")
      , 
        DXUtil_Timer(TIMER_GETAPPTIME),
        total.allocSize / (1024.0 * 1024.0), total.allocCount - total.freeCount
      ), D3DCOLOR_ARGB(255,255,255,255),
      10);
  }

  if(IsKeyDown(DIK_X))
  {
    if(!isWireKeyLastPressed)
    {
      isWire = !isWire;
      isWireKeyLastPressed = true;
    }
  }
  else
  {
    isWireKeyLastPressed = false;
  }

  if(IsKeyDown(DIK_M))
  {
    if(!isShowMipsKeyLastPressed)
    {
      isShowMips = !isShowMips;
      isShowMipsKeyLastPressed = true;

      for(int i = 0; i < TERRAIN_MIPS_NUMBER; i++)
      {
        mipCaches[i].terrainCornerX = -999999999;
      }
    }
  }
  else
  {
    isShowMipsKeyLastPressed = false;
  }

  float forward = 0.0f;
  float right = 0.0f;
  float up = 0.0f;
  float yawRotate = 0.0f;
  float pitchRotate = 0.0f;

  if(IsKeyDown(DIK_E) || IsKeyDown(DIK_UP))
  {
    forward += 1.0f;
  }
  if(IsKeyDown(DIK_D) || IsKeyDown(DIK_DOWN))
  {
    forward -= 1.0f;
  }
  if(IsKeyDown(DIK_F))
  {
    right += 1.0f;
  }
  if(IsKeyDown(DIK_S))
  {
    right -= 1.0f;
  }
  if(IsKeyDown(DIK_W) || IsKeyDown(DIK_LSHIFT))
  {
    up += 1.0f;
  }
  if(IsKeyDown(DIK_R) || IsKeyDown(DIK_LCONTROL))
  {
    up -= 1.0f;
  }
  if(IsKeyDown(DIK_LEFT))
  {
    yawRotate += 1.0f;
  }
  if(IsKeyDown(DIK_RIGHT))
  {
    yawRotate -= 1.0f;
  }
  if(IsKeyDown(DIK_A))
  {
    pitchRotate -= 1.0f;
  }
  if(IsKeyDown(DIK_Z))
  {
    pitchRotate += 1.0f;
  }

  cameraPosition.angles.yaw += yawRotate * CAMERA_ROTATE_SPEED * dt;
  cameraPosition.angles.pitch += pitchRotate * CAMERA_ROTATE_SPEED * dt;
  if(cameraPosition.angles.pitch < -85.0f)
    cameraPosition.angles.pitch = -85.0f;
  else if(cameraPosition.angles.pitch > 85.0f)
    cameraPosition.angles.pitch = 85.0f;
  cameraPosition.angles.Normalize180();

  if(forward || right || up)
  {
    xVec3 forwardDir, rightDir;
    xAngles angles(0, 0, 0);
    angles.yaw = cameraPosition.angles.yaw;
    angles.ToVectors(&forwardDir, &rightDir);
    cameraPosition.Accelerate(
      forwardDir * (forward * CAMERA_MOVE_MAX_SPEED / 3.0f)
      + rightDir * (right * CAMERA_MOVE_MAX_SPEED * 0.8f / 3.0f)
      + vec3_up * (up * CAMERA_MOVE_MAX_SPEED * 0.8f / 3.0f)
      ,
      dt
      );
  }

  cameraPosition.Update(dt);

  {
    float terrainHeight = terrainVerts.Height(cameraPosition.origin.ToVec2());
    cameraPosition.origin.z = 
      Max(cameraPosition.origin.z, terrainHeight + TERRAIN_CAMERA_HEIGHT);
  }

  for(int i = 0; i < TERRAIN_MIPS_NUMBER; i++)
  {
    UpdateTerrainMipCache(i, TERRAIN_MIP0_RADIUS);
  }

  frustum.SetPosition(cameraPosition.origin, cameraPosition.angles);

  consoleTextList.Add(xString::Format(_T("org: %.1f %.1f %.1f, angles: %.1f %.1f %.1f, spd: %.1f km/h")
      , 
      cameraPosition.origin.x, cameraPosition.origin.y, cameraPosition.origin.z,
      cameraPosition.angles.yaw, cameraPosition.angles.pitch, cameraPosition.angles.roll,
      SPEED_MS2KMH(cameraPosition.speedVec.LengthFast())
    ), D3DCOLOR_ARGB(255,255,255,255),
    20);

  return S_OK;
}
