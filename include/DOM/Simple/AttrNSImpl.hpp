#ifndef JEZUK_SimpleDOM_ATTRNSIMPL_H
#define JEZUK_SimpleDOM_ATTRNSIMPL_H

#include <DOM/Simple/AttrImpl.hpp>
#include <DOM/Simple/Helpers.hpp>

namespace Arabica
{
namespace SimpleDOM
{

template<class stringT, class string_adaptorT>
class AttrNSImpl : public AttrImpl<stringT, string_adaptorT>
{
    typedef typename string_adaptorT::size_type size_type;
    typedef AttrImpl<stringT, string_adaptorT> AttrT;
	
  public:
    AttrNSImpl(DocumentImpl<stringT, string_adaptorT>* ownerDoc,
               const stringT& namespaceURI,
               bool hasNamespaceURI,
               const stringT& qualifiedName) : 
        AttrImpl<stringT, string_adaptorT>(ownerDoc, qualifiedName),
        prefix_(&ownerDoc->empty_string()),
        hasNamespaceURI_(false)
    { 
      bool hasPrefix = false;
      stringT const* prefix_for_checking = (&ownerDoc->empty_string());
      size_type index = string_adaptorT::find(qualifiedName, string_adaptorT::construct_from_utf8(":"));

      if(index == string_adaptorT::npos()) 
      {	//qualifiedName contains no ':'
        localName_ = AttrT::ownerDoc_->stringPool(qualifiedName);
        if(*localName_ == string_adaptorT::construct_from_utf8("xmlns"))
        {
          hasPrefix = true;
          prefix_for_checking = localName_;
        } // if ...
      } 
      else 
      {	
        hasPrefix = true;
        prefix_ = AttrT::ownerDoc_->stringPool(string_adaptorT::substr(qualifiedName, 0, index));
        localName_ = AttrT::ownerDoc_->stringPool(string_adaptorT::substr(qualifiedName, index+1));
        prefix_for_checking = prefix_;
      } // if(index == string_adaptorT::npos) 

      std::pair<bool, stringT> mappedURI = 
        checkPrefixAndNamespace<stringT, string_adaptorT>(hasPrefix, *prefix_for_checking, hasNamespaceURI, namespaceURI, DOM::Node<stringT>::ATTRIBUTE_NODE);

      hasNamespaceURI_ = mappedURI.first;
      namespaceURI_ = AttrT::ownerDoc_->stringPool(mappedURI.second);
    } // AttrImpl

    virtual ~AttrNSImpl() { }

    ///////////////////////////////////////////////////////
    // DOM::Node methods
    virtual DOM::Node_impl<stringT>* cloneNode(bool deep) const
    {
      AttrNSImpl* clone = dynamic_cast<AttrNSImpl*>(AttrT::ownerDoc_->createAttributeNS(*namespaceURI_, *AttrT::name_));
      AttrT::cloneChildren(clone);
      clone->setSpecified(AttrT::getSpecified());
      return clone;
    } // cloneNode

    virtual const stringT& getNamespaceURI() const 
    { 
      return *namespaceURI_;
    } // getNamespaceURI

    virtual const stringT& getPrefix() const 
    { 
      return *prefix_;
    } // getPrefix
    
    virtual void setPrefix(const stringT& prefix) 
    {
      if(hasNamespaceURI_ == false) 
        throw DOM::DOMException(DOM::DOMException::NAMESPACE_ERR);

      if(string_adaptorT::empty(prefix)) 
      {
        AttrT::name_ = localName_;
        prefix_ = &AttrT::ownerDoc_->empty_string();
        return;
      } // empty prefix

      checkPrefixAndNamespace<stringT, string_adaptorT>(true, prefix, true, *namespaceURI_, DOM::Node<stringT>::ATTRIBUTE_NODE);

      stringT newName = prefix;
      string_adaptorT::append(newName, string_adaptorT::construct_from_utf8(":"));
      string_adaptorT::append(newName, *localName_);

      prefix_ = AttrT::ownerDoc_->stringPool(prefix);
      AttrT::name_ = AttrT::ownerDoc_->stringPool(newName);
    } // setPrefix

    virtual const stringT& getLocalName() const 
    { 
      return *localName_;
    } // getLocalName

    // additional three methods - since C++ std::string (and by implication
    // stringT) don't differenciate between a null string and an empty string,
    // but the DOM recommendation does, I have to introduce these three methods 
    // to disambiguate.  If they return false, the corresponding attribute should be
    // considered null.  If they return true, the attribute has been set EVEN IF 
    // it has been set to the empty string
    virtual bool hasNamespaceURI() const 
    { 
      return hasNamespaceURI_;
    } // hasNamespaceURI

    virtual bool hasPrefix() const 
    { 
      return !(*prefix_ == AttrT::ownerDoc_->empty_string());
    } // hasPrefix

  private:
    stringT const* namespaceURI_;
    stringT const* prefix_;
    stringT const* localName_;
    bool hasNamespaceURI_;
}; // class AttrNSImpl

} // namespace SAX2DOM
} // namespace Arabica

#endif

