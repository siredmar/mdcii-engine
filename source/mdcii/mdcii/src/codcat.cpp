
// This file is part of the MDCII Game Engine.
// Copyright (C) 2018  Benedikt Freisen
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

#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filtering_stream.hpp>
// #include <boost/iostreams/filter/newline.hpp>
#include <iostream>

using namespace std;
using namespace boost::iostreams;

struct bytewise_2s_complement_input_filter
{
  typedef char char_type;
  typedef input_filter_tag category;

  template<typename Source>
  int get(Source& src)
  {
    int c = boost::iostreams::get(src);
    if (c != EOF && c != WOULD_BLOCK)
      c = -c & 0xff;
    return c;
  }
};

int main(int argc, char** argv)
{
  if (argc != 2)
    exit(EXIT_FAILURE);

  filtering_istream in;
  // in.push(newline_filter(newline::dos));
  in.push(bytewise_2s_complement_input_filter());
  in.push(file_source(argv[1]));

  while (in.good())
  {
    char c = in.get();
    cout.put(c);
  }
}
