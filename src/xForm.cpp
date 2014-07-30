#include <xForm.h>

#ifndef X_DEV
#define X_DEV 1
#endif

xForm * xForm::x;

// ===================================================

xForm::xForm(): renderStates(256)
{
  // xPhys::Init();
  x = this;
}

xForm::~xForm()
{
  // DeleteEntities();

  ShutdownInput();
  ShutdownSound();
  x = NULL;
}

// ===================================================

DWORD xForm::RenderState(D3DRENDERSTATETYPE state)
{
  DWORD * pValue = renderStates.Get((int)state);
  if(!pValue){
    DWORD value;
    m_pd3dDevice->GetRenderState(state, &value); // not get real value - driver error?!
    m_pd3dDevice->SetRenderState(state, value); // force set get value
    pValue = renderStates.Set((int)state, value);
  }
  return *pValue;
}
float xForm::FloatRenderState(D3DRENDERSTATETYPE state)
{
  DWORD value = RenderState(state);
  return *(float*)&value;
}

DWORD xForm::SetRenderState(D3DRENDERSTATETYPE state, DWORD value)
{
  DWORD oldValue = RenderState(state);
  if(oldValue != value){
    if(m_pd3dDevice->SetRenderState(state, value) == D3D_OK)
      renderStates.Set((int)state, value);
  }
  return oldValue;
}
float xForm::SetFloatRenderState(D3DRENDERSTATETYPE state, float value)
{
  DWORD oldValue = SetRenderState(state, *(DWORD*)&value);
  return *(float*)&oldValue;
}

void xForm::InitInput()
{
  ShutdownInput();

  dinput = NULL;
  dkeyboard = NULL;

  HRESULT hr;
  // Create a DInput object
  if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                       IID_IDirectInput8, (VOID**)&dinput, NULL ) ) )
    return;

  // Obtain an interface to the system keyboard device.
  if( FAILED( hr = dinput->CreateDevice( GUID_SysKeyboard, &dkeyboard, NULL ) ) ){
    ShutdownInput();
    return;
  }

  // Set the data format to "keyboard format" - a predefined data format 
  //
  // A data format specifies which controls on a device we
  // are interested in, and how they should be reported.
  //
  // This tells DirectInput that we will be passing an array
  // of 256 bytes to IDirectInputDevice::GetDeviceState.
  if( FAILED( hr = dkeyboard->SetDataFormat(&c_dfDIKeyboard) ) ){
    ShutdownInput();
    return;
  }
  // Set the cooperativity level to let DirectInput know how
  // this device should interact with the system and with other
  // DirectInput applications.
  if( FAILED( hr = dkeyboard->SetCooperativeLevel( m_hWndFocus, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND ) )){
    ShutdownInput();
    return;
  }

  // Acquire the newly created device
  dkeyboard->Acquire();
}

void xForm::ShutdownInput()
{
  if(dkeyboard)
		dkeyboard->Unacquire();

  // Release any DirectInput objects.
  SAFE_RELEASE( dkeyboard );
  SAFE_RELEASE( dinput );
}

void xForm::ReadInput()
{
  if(!dkeyboard){
    ZeroMemory(keyboardData, sizeof(keyboardData));
    return;
  }

	// get the immediate keyboard data
	HRESULT hr = dkeyboard->GetDeviceState(sizeof(keyboardData), keyboardData);
	
  // attempt to reacquire the device if it has been lost
  if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED){
		dkeyboard->Acquire();
	  hr = dkeyboard->GetDeviceState(sizeof(keyboardData), keyboardData);
  }
  if(FAILED(hr))
	  ZeroMemory(keyboardData, sizeof(keyboardData));
}

bool xForm::IsKeyDown(int key) const
{
  if(key >= 0 && key < sizeof(keyboardData))
    return (keyboardData[key] & 0x80) != 0;

  return false;
}

// ===================================================

void xForm::InitSound()
{
  ShutdownSound();

  soundManager = new CSoundManager();
  if(!soundManager)
    return;

  HRESULT hr;

  if(FAILED(hr = soundManager->Initialize(m_hWndFocus, DSSCL_PRIORITY))){
    return;
  }
  
  if(FAILED(hr = soundManager->SetPrimaryBufferFormat(2, 44100, 32))){ //22050, 16))){
    return;
  }

  if(FAILED(hr = soundManager->Get3DListenerInterface(&listener))){
    return;
  }
  listenerParams.dwSize = sizeof(listenerParams);
  listener->GetAllParameters(&listenerParams);
}
void xForm::ShutdownSound()
{
  if(!soundManager)
    return;

  SAFE_RELEASE(listener);

  delete soundManager;
  soundManager = NULL;
}

void xForm::SetSoundListener(const xVec3& origin, const xAngles& angles, const xVec3& speed, float dopplerFactor, float rolloffFactor, float distanceFactor)
{
  /*
  listenerParams.vPosition = xForm::dxVec(origin);
  listenerParams.vVelocity = xForm::dxVec(speed);
  */
  listenerParams.vPosition.x = -origin.x;
  listenerParams.vPosition.y = origin.y;
  listenerParams.vPosition.z = origin.z;

  listenerParams.vVelocity.x = -speed.x;
  listenerParams.vVelocity.y = speed.y;
  listenerParams.vVelocity.z = speed.z;

  xVec3 forward, up;
  angles.ToVectors(&forward, NULL, &up);
  /*
  listenerParams.vOrientFront = xForm::dxVec(forward);
  listenerParams.vOrientTop = xForm::dxVec(up);
  */
  listenerParams.vOrientFront.x = -forward.x;
  listenerParams.vOrientFront.y = forward.y;
  listenerParams.vOrientFront.z = forward.z;
  
  listenerParams.vOrientTop.x = -up.x;
  listenerParams.vOrientTop.y = up.y;
  listenerParams.vOrientTop.z = up.z;

  listenerParams.flDistanceFactor = distanceFactor;
  listenerParams.flRolloffFactor = rolloffFactor;
  listenerParams.flDopplerFactor = dopplerFactor;

  if(listener)
    listener->SetAllParameters(&listenerParams, DS3D_IMMEDIATE);
}

CSound * xForm::CreateCSound(const xString& filename, int flags, GUID guid3DAlgorithm, int bufferCount)
{
  DWORD creationFlags = (DWORD)flags | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
  // DSBCAPS_CTRLPAN | 

/*
  if( bFocusGlobal )
      dwCreationFlags |= DSBCAPS_GLOBALFOCUS;

  if( bFocusSticky )
      dwCreationFlags |= DSBCAPS_STICKYFOCUS;

  if( bMixHardware )
      dwCreationFlags |= DSBCAPS_LOCHARDWARE;

  if( bMixSoftware )
      dwCreationFlags |= DSBCAPS_LOCSOFTWARE;
*/

  HRESULT hr;
  CSound * csound; 
  if(FAILED(hr = soundManager->Create(&csound, (LPTSTR)filename.ToChar(), creationFlags, guid3DAlgorithm, bufferCount))){
    return NULL;
  }

  return csound;
}

// ===================================================

void xForm::Print(PrintType_t pt, const xString& fmt, va_list va)
{
  switch(pt){
#ifndef X_DEV
  case PT_DevMessage:
  case PT_DevWarning:
  case PT_DevError:
    return;
#endif
  case PT_Message:
  case PT_Warning:
  case PT_Error:
  case PT_FatalError:
    break;
  }

  xString s = xString::Format(fmt, va);

  if(pt == PT_FatalError){
    assert(0);
    exit(1);
  }
}
void xForm::Print(PrintType_t pt, const xString& fmt, ...)
{
  va_list va;
  va_start(va,fmt);
  Print(pt, fmt, va);
  va_end(va);
}

// ===================================================

D3DXMATRIX xForm::dxMat(const xMat4& src)
{
  assert(sizeof(D3DXMATRIX) == sizeof(xMat4));
  return *(D3DXMATRIX*)&src.Transpose();
}

// ===================================================

void xForm::SetViewport(int x, int y, int w, int h)
{
  int windowWidth = m_d3dsdBackBuffer.Width;
  int windowHeight = m_d3dsdBackBuffer.Height;
  int x2 = x + w;
  int tmp = windowHeight - (y + h);
  int y2 = windowHeight - y;
  y = tmp;

  // fudge around because of frac screen scale
  if(x > 0)
	  x--;
  if(x2 < windowWidth)
	  x2++;
  if(y > 0)
	  y--;
  if(y2 < windowHeight)
	  y2++;

  D3DVIEWPORT9 view;
  view.X = x;
  view.Y = y;
  view.Width = x2 - x;
  view.Height = y2 - y;
  view.MinZ = 0;
  view.MaxZ = 1;
  m_pd3dDevice->SetViewport(&view);
}

void xForm::SetPerspectiveByFovxZ(float fovx, float aspect, float zNear, float zFar)
{
  float width = 2 * zNear * xMath::Tan(DEG2RAD(fovx * 0.5f));
  float height = width / aspect;
  SetPerspectiveByWidthZ(width, height, zNear, zFar);
}

void xForm::SetPerspectiveByWidthZ(float width, float height, float zNear, float zFar)
{
  D3DXMATRIX mat;
  D3DXMatrixPerspectiveRH(&mat, width, height, zNear, zFar);
  m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mat);
}

void xForm::SetPerspectiveByFovxAspectWidth(float fovx, float aspect, float width, float zFar)
{
  SetPerspectiveByFovxWidth(fovx, width, width / aspect, zFar);
}

void xForm::SetPerspectiveByFovxWidth(float fovx, float width, float height, float zFar)
{
  float zNear = width / (2 * xMath::Tan(DEG2RAD(fovx * 0.5f)));
  SetPerspectiveByWidthZ(width, height, zNear, zNear + zFar);
}

void xForm::SetProjectionOrtho(float left, float top, float right, float bottom, float zNear, float zFar)
{
  D3DXMATRIX mat;
  D3DXMatrixOrthoOffCenterRH(&mat, left, right, top, bottom, zNear, zFar);
  m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mat);
}

void xForm::SetCameraPosition(const xVec3& origin, const xMat3& axis)
{
  xMat4 m(axis, origin); // camera to world space
  m.InverseSelf(); // world to camera space
  D3DXMATRIX mat = dxMat(m), out, out2;

  D3DXMatrixRotationZ(&out, DEG2RAD(90));
  D3DXMatrixMultiply(&out2, &mat, &out); // result in out2

  D3DXMatrixRotationX(&out, DEG2RAD(-90));
  D3DXMatrixMultiply(&mat, &out2, &out); // result in mat

  m_pd3dDevice->SetTransform(D3DTS_VIEW, &mat);
}

void xForm::SetCameraPosition(const xVec3& origin, const xAngles& angles)
{
  SetCameraPosition(origin, angles.ToMat3());
}

void xForm::SetFrustum(const xFrustum& f)
{
  SetPerspectiveByWidthZ(f.NearLeft()*2, f.NearUp()*2, f.NearDistance(), f.FarDistance());
  SetCameraPosition(f.Origin(), f.Axis());
}

