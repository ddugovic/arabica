#ifndef ARABICA_XSLT_STYLESHEETPARSER_HPP
#define ARABICA_XSLT_STYLESHEETPARSER_HPP

#include <SAX/filter/NamespaceTracker.hpp>
#include <SAX/filter/XMLBaseTracker.hpp>
#include <SAX/filter/TextCoalescer.hpp>

#include <XPath/impl/xpath_namespace_context.hpp>

namespace Arabica
{
namespace XSLT
{

class StylesheetParser : public XPath::NamespaceContext<std::string, Arabica::default_string_adaptor<std::string> >
{
public:
  StylesheetParser() { }

  void setContentHandler(SAX::ContentHandler& handler)
  {
    namespace_tracker_.setContentHandler(handler);
  } // setContentHandler
 
  void parse(SAX::InputSource& source)
  {
    SAX::XMLReader<std::string> base_parser;
    text_coalescer_.setParent(base_parser);
    xmlbase_tracker_.setParent(text_coalescer_);
    namespace_tracker_.setParent(xmlbase_tracker_);

    namespace_tracker_.parse(source);
  } // parse

  virtual std::string namespaceURI(const std::string& prefix) const
  {
    return namespace_tracker_.getURI(prefix);
  } // namespaceURI

  std::pair<std::string, std::string> processQName(const std::string& qName) const
  {
    return namespace_tracker_.process(qName);
  } // processQName

  std::map<std::string, std::string> inScopeNamespaces() const
  {
    return namespace_tracker_.inScopeNamespaces();
  } // inScopeNamespaces

  std::string makeAbsolute(const std::string& href) 
  {
    return xmlbase_tracker_.makeAbsolute(href);
  } // makeAbsolute

private:
  SAX::TextCoalescer<std::string> text_coalescer_;
  SAX::XMLBaseTracker<std::string> xmlbase_tracker_;
  SAX::NamespaceTracker<std::string> namespace_tracker_;

  StylesheetParser(const StylesheetParser&);
  StylesheetParser operator=(const StylesheetParser&);
  bool operator==(const StylesheetParser&);
}; // class StylesheetParser 

} // namespace XSLT
} // namespace Arabica

#endif // ARABICA_XSLT_STYLESHEETPARSER_HPP

