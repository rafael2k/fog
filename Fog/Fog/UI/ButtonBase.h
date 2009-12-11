// [Fog/UI Library - C++ API]
//
// [Licence] 
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_UI_BUTTONBASE_H
#define _FOG_UI_BUTTONBASE_H

// [Dependencies]
#include <Fog/UI/Widget.h>

//! @addtogroup Fog_UI
//! @{

namespace Fog {

// ============================================================================
// [Fog::ButtonBase]
// ============================================================================

struct FOG_API ButtonBase : public Widget
{
  FOG_DECLARE_OBJECT(ButtonBase, Widget)

  // [Construction / Destruction]

  ButtonBase();
  virtual ~ButtonBase();

  // [Text]

  String getText() const;
  void setText(const String& text);

  // [Value]

  //! @brief Get whether widget is checked (see @c CheckedState).
  int getChecked() const;
  //! @brief Set checked state of the widget (see @c CheckedState).
  void setChecked(int value);

  //! @brief Set widget state to checked.
  FOG_INLINE void check() { setChecked(true); }
  //! @brief Set widget state to unchecked.
  FOG_INLINE void uncheck() { setChecked(false); }

  // [Interaction]

  bool isMouseOver() const;
  bool isMouseDown() const;
  bool isSpaceDown() const;
  bool isDown() const;

  // [Event Handlers]

  // Fog::Widget Events.
  virtual void onFocusIn(FocusEvent* e);
  virtual void onFocusOut(FocusEvent* e);
  virtual void onKeyPress(KeyEvent* e);
  virtual void onKeyRelease(KeyEvent* e);
  virtual void onMouseIn(MouseEvent* e);
  virtual void onMouseOut(MouseEvent* e);
  virtual void onMousePress(MouseEvent* e);
  virtual void onMouseRelease(MouseEvent* e);

  // Fog::ButtonBase Events.
  virtual void onCheck(CheckEvent* e);
  virtual void onUncheck(CheckEvent* e);

  // [Event Map]

  FOG_EVENT_BEGIN()
    FOG_EVENT_DEF(EV_CHECK            , onCheck           , CheckEvent     , OVERRIDE)
    FOG_EVENT_DEF(EV_UNCHECK          , onUncheck         , CheckEvent     , OVERRIDE)
  FOG_EVENT_END()

protected:
  String _text;

  int _checked;
  bool _isMouseOver;
  bool _isMouseDown;
  bool _isSpaceDown;
};

} // Fog namespace

//! @}

// [Guard]
#endif // _FOG_UI_BUTTONBASE_H
