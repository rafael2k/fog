// [Fog-Svg]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_SVG_DOM_SVGPATHATTRIBUTE_P_H
#define _FOG_SVG_DOM_SVGPATHATTRIBUTE_P_H

// [Dependencies]
#include <Fog/G2d/Geometry/Path.h>
#include <Fog/Xml/Dom/XmlAttribute.h>
#include <Fog/Xml/Dom/XmlElement.h>
#include <Fog/Svg/Global/Constants.h>

namespace Fog {

//! @addtogroup Fog_Svg_Dom
//! @{

// ============================================================================
// [Fog::SvgPathAttribute]
// ============================================================================

struct FOG_NO_EXPORT SvgPathAttribute : public XmlAttribute
{
  typedef XmlAttribute base;

  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  SvgPathAttribute(XmlElement* element, const ManagedString& name, int offset);
  virtual ~SvgPathAttribute();

  // --------------------------------------------------------------------------
  // [Methods]
  // --------------------------------------------------------------------------

  virtual err_t setValue(const String& value);

  // --------------------------------------------------------------------------
  // [Coords]
  // --------------------------------------------------------------------------

  FOG_INLINE const PathF& getPath() const { return _path; }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------
protected:

  PathF _path;

private:
  FOG_DISABLE_COPY(SvgPathAttribute)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_SVG_DOM_SVGPATHATTRIBUTE_P_H
