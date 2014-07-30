#ifndef __x_form_h__
#define __x_form_h__

#pragma once

#include <assert.h>
#include <malloc.h>

#include "xDef.h"
#include "common/xNewDecl.h"

#include "common/xHeap.h"
#include "common/xString.h"
#include "common/xBitArray.h"

#include "containers/xArray.h"
#include "containers/xHashTable.h"
#include "containers/xLinkList.h"

#include "math/xMath.h"
#include "math/xAngles.h"
#include "math/xCurve.h"
#include "math/xMatrix.h"
#include "math/xPlane.h"
#include "math/xQuat.h"
#include "math/xRandom.h"
#include "math/xRotation.h"
#include "math/xSimd.h"
#include "math/xSimd_3DNow.h"
#include "math/xSimd_Generic.h"
#include "math/xSimd_MMX.h"
#include "math/xSimd_SSE.h"
#include "math/xSimd_SSE2.h"
#include "math/xSimd_SSE3.h"
#include "math/xVector.h"
#include "math/xPluecker.h"

#include "geom/xDrawVert.h"
#include "geom/xSphere.h"
#include "geom/xBounds.h"
#include "geom/xBox.h"
#include "geom/xFrustum.h"
#include "geom/xTerrainVerts.h"

#include "xMegaTexture.h"

/*
#ifdef DEBUG_APP_HEAP
#undef new
#endif
*/

#include <D3DX9.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DUtil.h"
#include "D3DFile.h"
#include "D3DFont.h"
#include "DSUtil.h"

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>

#ifdef DEBUG_APP_HEAP
#include "common/xNewDecl.h"
#endif

enum PrintType_t
{
  PT_Message,
  PT_DevMessage,
  PT_Warning,
  PT_DevWarning,
  PT_Error,
  PT_DevError,
  PT_FatalError,
};

class xForm: public CD3DApplication
{
  friend class xSound;

	LPDIRECTINPUT8 dinput;
	LPDIRECTINPUTDEVICE8 dkeyboard;
	byte keyboardData[256];

  CSoundManager * soundManager;
  LPDIRECTSOUND3DLISTENER listener;
  DS3DLISTENER listenerParams;

  xHashTable<xHashTableIndexKey, DWORD> renderStates;

protected:

  void InitInput();
  void ShutdownInput();

  void InitSound();
  void ShutdownSound();

  CSound * CreateCSound(const xString& filename, int flags = 0, GUID guid3DAlgorithm = GUID_NULL, int bufferCount = 1);

public:

  static D3DXVECTOR3 dxVec(const xVec3& src){ return D3DXVECTOR3(-src.y, src.z, -src.x); }
  static D3DXMATRIX dxMat(const xMat4& src);

  static xForm * x;

  xForm();
  ~xForm();

  void ReadInput();
  bool IsKeyDown(int key) const;

  virtual void Print(PrintType_t pt, const xString& fmt, va_list);
  void Print(PrintType_t pt, const xString& fmt, ...);

  DWORD RenderState(D3DRENDERSTATETYPE);
  float FloatRenderState(D3DRENDERSTATETYPE);

  DWORD SetRenderState(D3DRENDERSTATETYPE, DWORD value);
  float SetFloatRenderState(D3DRENDERSTATETYPE, float value);

  void SetViewport(int x, int y, int w, int h);
  void SetPerspectiveByFovxZ(float fovx, float aspect, float zNear, float zFar);
  void SetPerspectiveByWidthZ(float width, float height, float zNear, float zFar);
  void SetPerspectiveByFovxAspectWidth(float fovx, float aspect, float width, float zFar);
  void SetPerspectiveByFovxWidth(float fovx, float width, float height, float zFar);
  void SetProjectionOrtho(float left, float top, float right, float bottom, float zNear, float zFar);
  void SetCameraPosition(const xVec3& origin, const xMat3& axis);
  void SetCameraPosition(const xVec3& origin, const xAngles& angles);
  void SetFrustum(const xFrustum& f);

  void SetSoundListener(const xVec3& origin, const xAngles& angles, const xVec3& speed, float dopplerFactor, float rolloffFactor, float distanceFactor = 1.0f);
};


#endif