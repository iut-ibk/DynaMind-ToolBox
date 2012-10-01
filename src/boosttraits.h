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
        struct system_graph_traversal_category : public virtual edge_list_graph_tag, public virtual vertex_list_graph_tag, public incidence_graph_tag
        {
        };

        typedef std::pair<std::string,DM::Node*> vertex_descriptor;
        typedef std::pair<std::string,DM::Edge*> edge_descriptor;
        typedef directed_tag directed_category;
        typedef allow_parallel_edge_tag edge_parallel_category;
        typedef system_graph_traversal_category traversal_category;
        typedef int vertices_size_type;
        typedef int edges_size_type;
        typedef int degree_size_type;


        typedef map<std::string,DM::Edge*>::iterator edge_iterator;
        typedef map<std::string,DM::Node*>::iterator vertex_iterator;
        typedef map<std::string,DM::Edge*>::iterator out_edge_iterator;
    };

    inline std::pair<typename graph_traits< DM::System *>::out_edge_iterator,typename graph_traits< DM::System* >::out_edge_iterator > out_edges(typename graph_traits< DM::System * >::vertex_descriptor, DM::System* g)
    {
        //TODO implement
        return std::make_pair( g->getAllEdges().begin(),g->getAllEdges().end() );
    }

    inline typename graph_traits< DM::System* >::degree_size_type out_degree(typename graph_traits< DM::System * >::vertex_descriptor, DM::System* g)
    {
        //TODO implement
        return 2;
    }

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

    //NEW PROPERTY TRAITS
    class SystemPropertyMap
    {
    private:
        DM::System *instance;
    public:

        SystemPropertyMap()
        {
        }

        SystemPropertyMap(DM::System* sys)
        {
            this->instance=sys;
        }
    };

    class SystemPropertyMapInt
    {
    private:
        DM::System *instance;
    public:

        SystemPropertyMapInt()
        {
        }

        SystemPropertyMapInt(DM::System* sys)
        {
            this->instance=sys;
        }

        int& operator[](std::pair<std::string,DM::Node*> x)
        {
            int i=1;
            return i;
        }

        /*
        double& operator[](int x)
        {
            double i=1;
            return i;
        }
        */
    };

    template<>
    struct property_traits< SystemPropertyMap* >
    {
        typedef double value_type;
        typedef double& reference;
        typedef typename graph_traits< DM::System * >::edge_descriptor key_type;
        typedef readable_property_map_tag category;
    };

    template<>
    struct property_traits< SystemPropertyMapInt* >
    {
        typedef int value_type;
        typedef int& reference;
        typedef typename graph_traits< DM::System * >::vertex_descriptor key_type;
        typedef read_write_property_map_tag category;
    };

    template<>
    struct property_map<DM::System*, vertex_index_t>
    {
        typedef SystemPropertyMapInt* type;
        typedef const SystemPropertyMapInt* const_type;
    };

    /*
    template<>
    struct property_map<SystemPropertyMap*, edge_weight_t>
    {
        typedef SystemPropertyMap* type;
        typedef const SystemPropertyMap* const_type;
        typedef value_type
    };*/

    SystemPropertyMap* get(edge_weight_t, DM::System* sys)
    {
        return new SystemPropertyMap();
    }

    typename property_traits< SystemPropertyMap* >::value_type get(property_traits< SystemPropertyMap* > pmap, typename property_traits< SystemPropertyMap* >::key_type k)
    {
        //TODO implement
        double i = 0;
        return i;
    }

    SystemPropertyMapInt* get(vertex_index_t, DM::System* sys)
    {
        return new SystemPropertyMapInt();
    }

    typename property_traits< SystemPropertyMapInt* >::value_type get(property_traits< SystemPropertyMapInt* > pmap, typename property_traits< SystemPropertyMapInt* >::key_type k)
    {
        //TODO implement
        double i = 0;
        return i;
    }

    void put(property_traits< SystemPropertyMapInt* >& pmap, typename property_traits< SystemPropertyMapInt* >::key_type& k, typename property_traits< SystemPropertyMapInt* >::value_type& v)
    {
        //TODO implement
        return;
    }


    //PROPERTY TRAITS
    /*
    class SystemPropertyMap
    {
    private:
        DM::System *instance;
    public:

        SystemPropertyMap()
        {
        }

        SystemPropertyMap(DM::System* sys)
        {
            this->instance=sys;
        }

        double& operator[](std::pair<std::string,DM::Component*> x)
        {
            double i=1;
            return i;
        }

        double& operator[](int x)
        {
            double i=1;
            return i;
        }
    };

    template<>
    struct property_traits< SystemPropertyMap* >
    {
        typedef double value_type;
        typedef double& reference;
        typedef std::pair<std::string,DM::Component*> key_type;
        typedef read_write_property_map_tag category;
    };

    template<>
    struct property_map<SystemPropertyMap*, vertex_index_t>
    {
        typedef SystemPropertyMap* type;
        typedef const SystemPropertyMap* const_type;
    };

    template<>
    struct property_map<SystemPropertyMap*, vertex_distance_t>
    {
        typedef SystemPropertyMap* type;
        typedef SystemPropertyMap* const_type;
    };

    template<>
    struct property_map<SystemPropertyMap*, edge_weight_t>
    {
        typedef DM::System* type;
        typedef DM::System* const_type;
    };

    void put( SystemPropertyMap*  pmap, typename property_traits< SystemPropertyMap* >::key_type k, const  double& val)
    {
        //TODO implement
        return;
    }

    typename property_traits< SystemPropertyMap* >::reference get(SystemPropertyMap*& pmap, typename property_traits< SystemPropertyMap* >::key_type& k)
    {
        //TODO implement
        double i = 0;
        return i;
    }


    void put(std::pair<std::string,DM::Node*> k,std::pair<std::string,DM::Node*> k1,std::pair<std::string,DM::Node*> k2)
    {
    }

    property_map<SystemPropertyMap*, edge_weight_t> get(edge_weight_t a, DM::System*& sys)
    {
        return 0;
    }

    SystemPropertyMap* get(vertex_index_t i, DM::System*& sys)
    {
        return new SystemPropertyMap();
    }

    SystemPropertyMap* get(vertex_distance_t i, DM::System*& sys)
    {
        return new SystemPropertyMap();
    }
*/
} // namespace boost
#endif // boosttraits_H
