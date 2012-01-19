// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/G2d/Svg/SvgRender.h>

namespace Fog {

// ============================================================================
// [Fog::SvgRender - Helpers]
// ============================================================================

static FOG_INLINE bool SvgRender_setupFill(SvgRender* visitor)
{
  Painter* p = visitor->_painter;

  switch (visitor->_fillStyle.type)
  {
    case SVG_SOURCE_NONE:
    case SVG_SOURCE_INVALID:
      break;

    case SVG_SOURCE_COLOR:
      p->setSource(visitor->_fillStyle.color);
      goto _FillUsed;

    case SVG_SOURCE_URI:
      p->setSource(visitor->_fillStyle.pattern);
_FillUsed:
      p->setOpacity(visitor->_fillStyle.opacity * visitor->_opacity);
      p->setFillRule(visitor->_fillRule);
      return true;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return false;
}

static FOG_INLINE bool SvgRender_setupStroke(SvgRender* visitor)
{
  Painter* p = visitor->_painter;

  switch (visitor->_strokeStyle.type)
  {
    case SVG_SOURCE_NONE:
    case SVG_SOURCE_INVALID:
      break;

    case SVG_SOURCE_COLOR:
      p->setSource(visitor->_strokeStyle.color);
      goto _StrokeUsed;

    case SVG_SOURCE_URI:
      p->setSource(visitor->_strokeStyle.pattern);
_StrokeUsed:
      p->setOpacity(visitor->_strokeStyle.opacity * visitor->_opacity);
      p->setStrokeParams(visitor->_strokeParams);
      return true;

    default:
      FOG_ASSERT_NOT_REACHED();
  }

  return false;
}

// ============================================================================
// [Fog::SvgRender - Construction / Destruction]
// ============================================================================

SvgRender::SvgRender(Painter* painter) :
  _painter(painter)
{
  _visitorType = SVG_VISITOR_RENDER;

  _painter->save();
  _painter->setCompositingOperator(COMPOSITE_SRC_OVER);
  _painter->setFillRule(FILL_RULE_EVEN_ODD);
  _painter->resetStrokeParams();
}

SvgRender::~SvgRender()
{
  _painter->restore();
}

// ============================================================================
// [Fog::SvgRender - Interface]
// ============================================================================

err_t SvgRender::onShape(SvgElement* obj, const ShapeF& shape)
{
  _painter->save();
  _painter->transform(_transform);

  if (shape.isClosed() && SvgRender_setupFill(this))
    _painter->fillShape(shape);

  if (SvgRender_setupStroke(this))
    _painter->drawShape(shape);

  _painter->restore();
  return ERR_OK;
}

err_t SvgRender::onImage(SvgElement* obj, const PointF& pt, const Image& image)
{
  _painter->save();
  _painter->transform(_transform);

  _painter->blitImage(pt, image);

  _painter->restore();
  return ERR_OK;
}

} // Fog namespace
