#ifndef _TEMPLATE_HPP
#define _TEMPLATE_HPP

#include <inttypes.h>
#include <string>

struct TemplateData //
{
};

class Template
{
public:
  Template(uint8_t* data, uint32_t length, const std::string& name);
  TemplateData template;

private:
  std::string name;
};

#endif // _TEMPLATE_HPP