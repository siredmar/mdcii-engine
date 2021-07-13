// This file is part of the MDCII Game Engine.
// Copyright (C) 2020  Armin Schlegel
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef _TEMPLATE_HPP
#define _TEMPLATE_HPP

#include <inttypes.h>
#include <string>
#include <vector>

struct TemplateData //
{
};

class Template
{
public:
    Template()
    {
    }
    explicit Template(uint8_t* data, uint32_t length, const std::string& name);
    std::vector<TemplateData> template;
    TemplateData template;

private:
    std::string name;
};

#endif // _TEMPLATE_HPP