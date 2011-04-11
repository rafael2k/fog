// [Fog-Xml]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif

// [Dependencies]
#include <Fog/Core/Global/Assert.h>
#include <Fog/Xml/Dom/XmlAttribute.h>
#include <Fog/Xml/Global/Constants.h>

namespace Fog {

// ============================================================================
// [Fog::XmlAttribute]
// ============================================================================

XmlAttribute::XmlAttribute(XmlElement* element, const ManagedString& name, int offset) :
  _element(element),
  _name(name),
  _offset(offset)
{
}

XmlAttribute::~XmlAttribute()
{
}

String XmlAttribute::getValue() const
{
  return _value;
}

err_t XmlAttribute::setValue(const String& value)
{
  return _value.set(value);
}

void XmlAttribute::destroy()
{
  if (_offset == -1)
  {
    fog_delete(this);
  }
  else
  {
    _value.reset();
  }
}

} // Fog namespace
