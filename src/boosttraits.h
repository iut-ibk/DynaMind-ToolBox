/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Michael Mair
 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef boosttraits_H
#define boosttraits_H

#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/property_maps/constant_property_map.hpp>
#include <boost/config.hpp>
#include <dmsystem.h>
#include <dmnode.h>
#include <dmedge.h>

using namespace std;

namespace boost {
    //GRAPH TRAITS
    template <>
    struct graph_traits< DM::System * >
    {
        struct system_graph_traversal_category : public virtual edge_list_graph_tag, public virtual vertex_list_graph_tag
        {
        };

        typedef std::pair<std::string,DM::Node*> vertex_descriptor;
        typedef std::pair<std::string,DM::Edge*> edge_descriptor;
        typedef directed_tag directed_category;
        typedef allow_parallel_edge_tag edge_parallel_category;
        typedef system_graph_traversal_category traversal_category;
        typedef int vertices_size_type;
        typedef int edges_size_type;

        typedef map<std::string,DM::Edge*>::iterator edge_iterator;
        typedef map<std::string,DM::Node*>::iterator vertex_iterator;
    };

    inline std::pair<typename graph_traits< DM::System *>::edge_iterator,typename graph_traits< DM::System* >::edge_iterator > edges(DM::System* g)
    {
        return std::make_pair( g->getAllEdges().begin(),g->getAllEdges().end() );
    }

    inline std::pair<typename graph_traits< DM::System *>::vertex_iterator,typename graph_traits< DM::System* >::vertex_iterator > vertices(DM::System* g)
    {
        return std::make_pair( g->getAllNodes().begin(),g->getAllNodes().end() );
    }

    typename graph_traits< DM::System * >::vertex_descriptor source( typename graph_traits< DM::System * >::edge_descriptor e,
                                                                     DM::System* g)
    {
        DM::Node *node = g->getNode(e.second->getStartpointName());
        return std::make_pair(node->getUUID(),node);
    }

    typename graph_traits< DM::System * >::vertex_descriptor target( typename graph_traits< DM::System * >::edge_descriptor e,
                                                                     DM::System* g)
    {
        DM::Node *node = g->getNode(e.second->getEndpointName());
        return std::make_pair(node->getUUID(),node);
    }

    typename graph_traits< DM::System * >::edges_size_type num_edges( DM::System* g)
    {
        return g->getAllEdges().size();
    }

    typename graph_traits< DM::System * >::vertices_size_type num_vertices( DM::System* g)
    {
        return g->getAllNodes().size();
    }


    //PROPERTY TRAITS
    struct dmproperty_category : public virtual readable_property_map_tag, public virtual read_write_property_map_tag
    {
    };

    template<>
    struct property_traits< DM::Edge >
    {
        typedef double value_type;
        typedef double& reference;
        typedef graph_traits< DM::System * >::edge_descriptor key_type;
        typedef dmproperty_category category;
    };

    template<>
    struct property_traits< DM::Node >
    {
        typedef double value_type;
        typedef double& reference;
        typedef graph_traits< DM::System * >::vertex_descriptor key_type;
        typedef dmproperty_category category;
    };

/*
    void put( DM::System*  pmap, std::pair<std::string,DM::Edge*> k, const  double& val)
    {
        //TODO implement
        return;
    }

    void put( DM::System*  pmap, std::pair<std::string,DM::Node*> k, const  double& val)
    {
        //TODO implement
        return;
    }
*/

    /*
    typename property_traits< DM::Edge >::reference get(const  DM::System*& pmap, std::pair<std::string,DM::Edge*>& k)
    {
        //TODO implement
        double i = 0;
        return i;
    }

    typename property_traits< DM::Node >::reference get(const  DM::System*& pmap, std::pair<std::string,DM::Node*>& k)
    {
        //TODO implement
        double i = 0;
        return i;
    }
*/
    template<>
    struct property_map< DM::System*, edge_weight_t>
    {
        typedef DM::System* type;
        typedef DM::System* const_type;
    };

    template<>
    struct property_map< DM::System*, vertex_index_t>
    {
        typedef DM::System* type;
        typedef DM::System* const_type;
    };


    template <>
    inline const double& get(const DM::Edge* pa, std::ptrdiff_t k) { return 0; }

    DM::System* get(edge_weight_t a, DM::System *sys)
    {
        return sys;
    }

    DM::System* get(vertex_index_t i, DM::System* sys)
    {
        return sys;
    }
} // namespace boost
#endif // boosttraits_H
