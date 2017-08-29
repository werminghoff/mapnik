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

#ifndef MAPNIK_JSON_GRAMMAR_CONFIG_HPP
#define MAPNIK_JSON_GRAMMAR_CONFIG_HPP

#include <mapnik/feature.hpp>
#include <mapnik/unicode.hpp>
#pragma GCC diagnostic push
#include <mapnik/warning_ignore.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/assign/list_of.hpp>
#pragma GCC diagnostic pop

namespace mapnik { namespace json {

enum well_known_names
{
    id = 1,
    type,
    features,
    geometry,
    coordinates,
    properties
};

constexpr char const* wkn_to_string(well_known_names val)
{
    switch(val)
    {
    case id: return "id";
    case type: return "type";
    case features: return "features";
    case geometry: return "geometry";
    case coordinates: return "coordinates";
    case properties: return "properties";
    default: return "unknown";
    }
}


using keys_map = boost::bimap<boost::bimaps::unordered_set_of<std::string>,
                              boost::bimaps::set_of<int>>;

inline keys_map get_keys()
{
    keys_map keys = boost::assign::list_of<keys_map::relation>
        ("type", well_known_names::type)
        ("features", well_known_names::features)
        ("geometry", well_known_names::geometry)
        ("coordinates", well_known_names::coordinates)
        ("properties", well_known_names::properties)
        ("id", well_known_names::id)
        ;
    return keys;
}

namespace grammar {

struct keys_tag;
struct transcoder_tag;
struct feature_tag;

namespace x3 = boost::spirit::x3;
using space_type = x3::standard::space_type;
using iterator_type = char const*;

using phrase_parse_context_type = x3::phrase_parse_context<space_type>::type;
using context_type = x3::with_context<keys_tag,
                                      std::reference_wrapper<keys_map> const,
                                      phrase_parse_context_type>::type;

using feature_context_type = x3::with_context<transcoder_tag,
                                              std::reference_wrapper<mapnik::transcoder> const,
                                              x3::with_context<feature_tag,
                                                               std::reference_wrapper<mapnik::feature_impl> const,
                                                               phrase_parse_context_type>::type>::type;

// our spirit x3 grammars needs this one with changed order of feature_impl and transcoder (??)
using feature_context_const_type = x3::with_context<feature_tag,
                                                    std::reference_wrapper<mapnik::feature_impl> const,
                                                    x3::with_context<transcoder_tag,
                                                                     std::reference_wrapper<mapnik::transcoder const> const,
                                                                     phrase_parse_context_type>::type>::type;

// helper macro
#define BOOST_SPIRIT_INSTANTIATE_UNUSED(rule_type, Iterator, Context)   \
    template bool parse_rule<Iterator, Context, boost::spirit::x3::unused_type const>( \
        rule_type rule_                                                 \
        , Iterator& first, Iterator const& last                         \
        , Context const& context, boost::spirit::x3::unused_type const& ); \
    /***/

}}}

#endif // MAPNIK_JSON_GRAMMAR_CONFIG_HPP
