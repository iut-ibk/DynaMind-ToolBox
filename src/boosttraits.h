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
    class SystemGraph
    {
    private:
        DM::System* sys;
        std::vector<std::string> *edges, *nodes;
        std::vector<int> *edgesindex, *nodesindex;

    public:
        SystemGraph(DM::System *sys, std::vector<std::string> *edges, std::vector<std::string> *nodes)
        {
            this->sys=sys;
            this->edges=edges;
            this->nodes=nodes;

            this->edgesindex=new std::vector<int>();
            this->nodesindex=new std::vector<int>();

            for(int index=0; index<edges->size(); index++)
                this->edgesindex->push_back(index);

            for(int index=0; index<nodes->size(); index++)
                this->nodesindex->push_back(index);
        }

        ~SystemGraph()
        {
            delete edgesindex;
            delete nodesindex;
        }

        DM::System* getSystem()
        {
            return this->sys;
        }

        std::vector<std::string>* getEdges()
        {
            return this->edges;
        }

        std::vector<std::string>* getNodes()
        {
            return this->nodes;
        }

        std::vector<int>* getEdgesIndex()
        {
            return this->edgesindex;
        }

        std::vector<int>* getNodesIndex()
        {
            return this->nodesindex;
        }
    };

    //GRAPH TRAITS
    template <>
    struct graph_traits< SystemGraph* >
    {
        struct system_graph_traversal_category : public virtual edge_list_graph_tag, public virtual vertex_list_graph_tag, public incidence_graph_tag
        {
        };

        typedef int vertex_descriptor;
        typedef int edge_descriptor;
        typedef directed_tag directed_category;
        typedef allow_parallel_edge_tag edge_parallel_category;
        typedef system_graph_traversal_category traversal_category;
        typedef int vertices_size_type;
        typedef int edges_size_type;
        typedef int degree_size_type;


        typedef std::vector<edge_descriptor>::iterator edge_iterator;
        typedef std::vector<vertex_descriptor>::iterator vertex_iterator;
        typedef std::vector<edge_descriptor>::iterator out_edge_iterator;
    };

    inline std::pair<typename graph_traits< SystemGraph* >::out_edge_iterator,typename graph_traits< SystemGraph* >::out_edge_iterator > out_edges(typename graph_traits< SystemGraph* >::vertex_descriptor, SystemGraph* g)
    {
        return std::make_pair( g->getEdgesIndex()->begin(), g->getEdgesIndex()->end() );
    }

    inline typename graph_traits< SystemGraph* >::degree_size_type out_degree(typename graph_traits< SystemGraph* >::vertex_descriptor, SystemGraph* g)
    {
        //TODO implement
        return 2;
    }

    inline std::pair<typename graph_traits< SystemGraph* >::edge_iterator,typename graph_traits< SystemGraph* >::edge_iterator > edges(SystemGraph* g)
    {
        return std::make_pair( g->getEdgesIndex()->begin(), g->getEdgesIndex()->end() );
    }

    inline std::pair<typename graph_traits< SystemGraph* >::vertex_iterator,typename graph_traits< SystemGraph* >::vertex_iterator > vertices(SystemGraph* g)
    {
        return std::make_pair( g->getNodesIndex()->begin(), g->getNodesIndex()->end() );
    }

    typename graph_traits< SystemGraph* >::vertex_descriptor source( typename graph_traits< SystemGraph* >::edge_descriptor e,
                                                                     SystemGraph* g)
    {
        //TODO implement
        DM::Edge *edge = g->getSystem()->getEdge(g->getEdges()->at(e));
        return 1;
    }

    typename graph_traits< SystemGraph* >::vertex_descriptor target( typename graph_traits< SystemGraph* >::edge_descriptor e,
                                                                     SystemGraph* g)
    {
        //TODO implement
        DM::Edge *edge = g->getSystem()->getEdge(g->getEdges()->at(e));
        return 1;
    }

    typename graph_traits< SystemGraph* >::edges_size_type num_edges( SystemGraph* g)
    {
        return g->getEdges()->size();
    }

    typename graph_traits< SystemGraph* >::vertices_size_type num_vertices( SystemGraph* g)
    {
        return g->getNodes()->size();
    }

    //NEW PROPERTY TRAITS
    template<class T>
    class SystemPropertyMap
    {
    private:
        SystemGraph *instance;
    public:

        SystemPropertyMap()
        {
        }

        SystemPropertyMap(SystemGraph* sys)
        {
            this->instance=sys;
        }
    };

    /*
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

        double& operator[](int x)
        {
            double i=1;
            return i;
        }

    };
    */

    /*
    template<>
    struct property_traits< double >
    {
        typedef double value_type;
        typedef int key_type;
        typedef readable_property_map_tag category;
    };

    template<>
    struct property_traits< int >
    {
        typedef int value_type;
        typedef int key_type;
        typedef readable_property_map_tag category;
    };
    */

    template<>
    struct property_map<SystemGraph*, vertex_index_t>
    {
        typedef property_traits<int> type;
        typedef const property_traits<int> const_type;
    };

    template<>
    struct property_map<SystemGraph*, edge_weight_t>
    {
        typedef property_traits<double> type;
        typedef const property_traits<double> const_type;
    };

    typename property_map<SystemGraph*, edge_weight_t>::type get(edge_weight_t, SystemGraph* sys)
    {
        //TODO implement
        return sys;
    }


    typename property_traits< typename property_map<SystemGraph*, edge_weight_t>::type >::value_type get(edge_weight_t, SystemGraph* sys, typename property_traits< typename property_map<SystemGraph*, edge_weight_t>::type >::key_type k)
    {
        //TODO implement
        double i = 0;
        return i;
    }

    typename property_map<SystemGraph*, vertex_index_t>::type get(vertex_index_t, SystemGraph* sys)
    {
        //TODO implement
        return sys;
    }

    typename property_traits< typename property_map<SystemGraph*, vertex_index_t>::type >::value_type get(vertex_index_t, SystemGraph* sys,typename property_traits< typename property_map<SystemGraph*, vertex_index_t>::type >::key_type k)
    {
        //TODO implement
        int i = 0;
        return i;
    }

    /*
    void put(property_traits< SystemPropertyMapInt* >& pmap, typename property_traits< SystemPropertyMapInt* >::key_type& k, typename property_traits< SystemPropertyMapInt* >::value_type& v)
    {
        //TODO implement
        return;
    }
    */

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
