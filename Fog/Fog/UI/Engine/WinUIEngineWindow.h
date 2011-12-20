// [Fog-UI]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_ENGINE_WINUIENGINEWINDOW_H
#define _FOG_UI_ENGINE_WINUIENGINEWINDOW_H

// [Dependencies]
#include <Fog/Core/Kernel/Object.h>
#include <Fog/Core/Kernel/Timer.h>
#include <Fog/Core/Math/Constants.h>
#include <Fog/Core/OS/Library.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Time.h>
#include <Fog/G2d/Geometry/Point.h>
#include <Fog/G2d/Geometry/Rect.h>
#include <Fog/G2d/Geometry/Size.h>
#include <Fog/G2d/Imaging/Image.h>
#include <Fog/G2d/Imaging/ImageBits.h>
#include <Fog/G2d/Source/Color.h>
#include <Fog/G2d/Tools/Region.h>
#include <Fog/UI/Engine/UIEngineSecondaryFB.h>

namespace Fog {

//! @addtogroup Fog_UI_Engine
//! @{

// ============================================================================
// [Fog::WinUIEngineWindowImpl]
// ============================================================================

//! @brief Frame-buffer window data (abstract)
struct FOG_API WinUIEngineWindowImpl : public UIEngineWindowImpl
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  WinUIEngineWindowImpl(UIEngine* engine, UIEngineWindow* window);
  virtual ~WinUIEngineWindowImpl();

  // --------------------------------------------------------------------------
  // [Create / Destroy]
  // --------------------------------------------------------------------------

  virtual err_t create(uint32_t hints);
  virtual err_t destroy();

  // --------------------------------------------------------------------------
  // [Enabled / Disabled]
  // --------------------------------------------------------------------------

  virtual err_t setEnabled(bool enabled);

  // --------------------------------------------------------------------------
  // [Focus]
  // --------------------------------------------------------------------------

  virtual err_t focus();

  // --------------------------------------------------------------------------
  // [Window State]
  // --------------------------------------------------------------------------

  virtual err_t setState(uint32_t state);

  // --------------------------------------------------------------------------
  // [Window Geometry]
  // --------------------------------------------------------------------------

  virtual err_t setWindowPosition(const PointI& pos);
  virtual err_t setWindowSize(const SizeI& size);

  // --------------------------------------------------------------------------
  // [Window Stack]
  // --------------------------------------------------------------------------

  virtual err_t moveToTop(void* targetHandle);
  virtual err_t moveToBottom(void* targetHandle);

  // --------------------------------------------------------------------------
  // [Window Coordinates]
  // --------------------------------------------------------------------------

  virtual err_t worldToClient(PointI& pt) const;
  virtual err_t clientToWorld(PointI& pt) const;

  // --------------------------------------------------------------------------
  // [Window Opacity]
  // --------------------------------------------------------------------------

  virtual err_t setOpacity(float opacity);

  // --------------------------------------------------------------------------
  // [Window Title]
  // --------------------------------------------------------------------------

  virtual err_t setWindowTitle(const StringW& title);

  // --------------------------------------------------------------------------
  // [Window Double-Buffer]
  // --------------------------------------------------------------------------

  virtual err_t allocDoubleBuffer(const SizeI& size);
  virtual err_t freeDoubleBuffer();

  // --------------------------------------------------------------------------
  // [OnWinMsg]
  // --------------------------------------------------------------------------

  LRESULT onWinMsg(UINT msg, WPARAM wParam, LPARAM lParam);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Image _bufferImage;

private:
  _FOG_NO_COPY(WinUIEngineWindowImpl)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_UI_ENGINE_WINUIENGINEWINDOW_H
