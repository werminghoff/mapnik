/*****************************************************************************
 *
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2017 Artem Pavlenko
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

#ifndef MAPNIK_TOPOJSON_GRAMMAR_X3_HPP
#define MAPNIK_TOPOJSON_GRAMMAR_X3_HPP

// mapnik
#include <mapnik/json/topology.hpp>

#pragma GCC diagnostic push
#include <mapnik/warning_ignore.hpp>
#include <boost/spirit/home/x3.hpp>
#pragma GCC diagnostic pop

namespace mapnik { namespace json { namespace grammar {

namespace x3 = boost::spirit::x3;

using topojson_grammar_type = x3::rule<class topojson_rule_tag, topojson::topology>;

BOOST_SPIRIT_DECLARE(topojson_grammar_type);

}

grammar::topojson_grammar_type const& topojson_grammar();

}}

#endif //MAPNIK_TOPOJSON_GRAMMAR_X3_HPP
