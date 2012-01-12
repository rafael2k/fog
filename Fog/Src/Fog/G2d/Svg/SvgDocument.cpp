// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Tools/ManagedString.h>
#include <Fog/G2d/Svg/SvgDocument.h>
#include <Fog/G2d/Svg/SvgDom_p.h>
#include <Fog/G2d/Svg/SvgElement.h>
#include <Fog/G2d/Svg/SvgHitTest.h>
#include <Fog/G2d/Svg/SvgRender.h>
#include <Fog/G2d/Svg/SvgVisitor.h>

namespace Fog {

// ============================================================================
// [Fog::SvgDocument]
// ============================================================================

SvgDocument::SvgDocument() :
  _dpi(72.0f)
{
  _type |= SVG_ELEMENT_MASK;
}

SvgDocument::~SvgDocument()
{
}

XmlElement* SvgDocument::clone() const
{
  SvgDocument* doc = fog_new SvgDocument();
  if (!doc) return NULL;

  for (XmlElement* ch = getFirstChild(); ch; ch = ch->getNextSibling())
  {
    XmlElement* e = ch->clone();
    if (e && doc->appendChild(e) != ERR_OK) { fog_delete(e); goto _Fail; }
  }

  return doc;

_Fail:
  fog_delete(doc);
  return NULL;
}

XmlElement* SvgDocument::createElement(const ManagedStringW& tagName)
{
  return createElementStatic(tagName);
}

XmlElement* SvgDocument::createElementStatic(const ManagedStringW& tagName)
{
  if (tagName == FOG_STR_(SVG_ELEMENT_a             )) return fog_new SvgAElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_circle        )) return fog_new SvgCircleElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_defs          )) return fog_new SvgDefsElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_ellipse       )) return fog_new SvgEllipseElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_g             )) return fog_new SvgGElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_image         )) return fog_new SvgImageElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_line          )) return fog_new SvgLineElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_linearGradient)) return fog_new SvgLinearGradientElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_path          )) return fog_new SvgPathElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_pattern       )) return fog_new SvgPatternElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_polygon       )) return fog_new SvgPolygonElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_polyline      )) return fog_new SvgPolylineElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_radialGradient)) return fog_new SvgRadialGradientElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_rect          )) return fog_new SvgRectElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_solidColor    )) return fog_new SvgSolidColorElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_stop          )) return fog_new SvgStopElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_svg           )) return fog_new SvgRootElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_symbol        )) return fog_new SvgSymbolElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_text          )) return fog_new SvgTextElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_tspan         )) return fog_new SvgTSpanElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_use           )) return fog_new SvgUseElement();
  if (tagName == FOG_STR_(SVG_ELEMENT_view          )) return fog_new SvgViewElement();

  // If element is not SVG, use the base class to create a default element
  // for the given tagName. But remember, this element won't be processed.
  return XmlDocument::createElementStatic(tagName);
}

err_t SvgDocument::setDpi(float dpi)
{
  return _dpi.setDpi(dpi);
}

SizeF SvgDocument::getDocumentSize() const
{
  XmlElement* root = getDocumentRoot();
  SizeF size(0.0f, 0.0f);

  if (root == NULL || !root->isSvg())
    return size;

  if (reinterpret_cast<SvgElement*>(root)->getSvgType() == SVG_ELEMENT_SVG)
    size = reinterpret_cast<SvgRootElement*>(root)->getRootSize();

  return size;
}

err_t SvgDocument::onProcess(SvgVisitor* visitor) const
{
  XmlElement* root = getDocumentRoot();

  if (root == NULL || !root->isSvgElement())
    return ERR_OK;

  return visitor->onVisit(reinterpret_cast<SvgElement*>(root));
}

err_t SvgDocument::render(Painter* painter) const
{
  SvgRender ctx(painter);
  return onProcess(&ctx);
}

List<SvgElement*> SvgDocument::hitTest(const PointF& pt, const TransformF* tr) const
{
  SvgHitTest ctx(pt);

  if (tr != NULL)
    ctx.setTransform(*tr);

  onProcess(&ctx);
  return ctx._result;
}

} // Fog namespace