// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGUSEELEMENT_P_H
#define _FOG_SVG_DOM_SVGUSEELEMENT_P_H

// [Dependencies]
#include <Fog/Svg/Dom/SvgStyledElement_p.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgUseElement]
// ============================================================================

struct FOG_NO_EXPORT SvgUseElement : public SvgStyledElement
{
  typedef SvgStyledElement base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgUseElement();
  virtual ~SvgUseElement();

  // --------------------------------------------------------------------------
  // [SVG Rendering]
  // --------------------------------------------------------------------------

  virtual err_t onRenderShape(SvgRenderContext* context) const;
  virtual err_t onCalcBoundingBox(RectF* box) const;

  // --------------------------------------------------------------------------
  // [SVG Embedded Attributes]
  // --------------------------------------------------------------------------

  // NONE

private:
  FOG_DISABLE_COPY(SvgUseElement)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGUSEELEMENT_P_H