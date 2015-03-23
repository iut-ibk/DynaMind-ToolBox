#include "gdaljoincluster.h"

#include <ogr_api.h>
#include <ogrsf_frmts.h>
//#include <geos/operation/linemerge/LineMerger.h>
#include <geos_c.h>

#include <dm.h>
DM_DECLARE_NODE_NAME(GDALJoinCluster, GDALModules)


GDALJoinCluster::GDALJoinCluster()
{
	GDALModule = true;
	buffer = 0.5;
	this->addParameter("buffer", DM::DOUBLE, &buffer);
	network = DM::ViewContainer("network", DM::EDGE, DM::MODIFY);
	network.addAttribute("cluster_id",  DM::Attribute::INT, DM::READ);
	network.addAttribute("start_id",  DM::Attribute::INT, DM::MODIFY);
	network.addAttribute("end_id",  DM::Attribute::INT, DM::MODIFY);
	network.addAttribute("intersect_id",  DM::Attribute::INT, DM::WRITE);
	network.addAttribute("new",  DM::Attribute::INT, DM::WRITE);

	junctions = DM::ViewContainer("node", DM::NODE, DM::READ);
	junctions.addAttribute("node_id",  DM::Attribute::INT, DM::READ);
	junctions.addAttribute("possible_endpoint",  DM::Attribute::INT, DM::READ);
	junctions.addAttribute("intersects",  DM::Attribute::INT, DM::WRITE);
	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&network);
	data_stream.push_back(&junctions);

	this->registerViewContainers(data_stream);
}

void GDALJoinCluster::run()
{
	junctions.resetReading();
	network.resetReading();
	OGRFeature * f;

	//Cluster intersections sorted
	std::set< std::pair<long, long> > cluster_intersections;

	typedef std::pair<long, double> segment;
	std::map<long, std::vector< segment > > segments;
	//GEOSContextHandle_t gh = OGRGeometry::createGEOSContext();

	//Sort all intersection and choose smallest
	struct intersection {
		double distance;
		int line_id;
		segment s;
	};
	//Endnode cluster table
	OGRFeature * f_n;
	std::map<long, long> node_to_cluster;
	while(f_n = network.getNextFeature()) {
		int start_node = f_n->GetFieldAsInteger("start_id");
		int end_node = f_n->GetFieldAsInteger("end_id");
		int current_cluster = f_n->GetFieldAsInteger("cluster_id");

		node_to_cluster[start_node] = current_cluster;
		node_to_cluster[end_node] = current_cluster;

	}

	while (f = junctions.getNextFeature()) {
		OGRPoint * p = (OGRPoint *)f->GetGeometryRef();
		if (!p)
			continue;
		OGRGeometry *p_buffer = p->Buffer(this->buffer);
		network.resetReading();
		network.setSpatialFilter(p_buffer);

		int node_id = f->GetFieldAsInteger("node_id");
		int current_cluster = node_to_cluster[node_id];

		//Identify closest cluster intersections
		while (f_n = network.getNextFeature()) {
			int start_node = f_n->GetFieldAsInteger("start_id");
			int end_node = f_n->GetFieldAsInteger("end_id");
			//avoid self intersections
			if (end_node == node_id || start_node == node_id)
				continue;


			OGRLineString * n = (OGRLineString *)f_n->GetGeometryRef();
			if (!n)
				continue;
//Problem with GDAL 1.10
//			if (n->Intersects(p_buffer)) {
//				int intersected_cluster = f_n->GetFieldAsInteger("cluster_id");
//				//avoid intersections within cluster
//				if (intersected_cluster == current_cluster) {
//					//DM::Logger(DM::Error) << "Self intersect";
//					continue;
//				}

//				//Calculate intersections
//				GEOSGeometry* geos_p = p->exportToGEOS(gh);
//				GEOSGeometry* line = n->exportToGEOS(gh);
//				double p_l = GEOSProject_r(gh, line, geos_p);
//				if (p_l == 0) {
//					continue;
//				}
//				if (segments.count(f_n->GetFID()) == 0) {
//					std::vector<segment> seg_vec;
//					seg_vec.push_back(segment(end_node, 0));
//					seg_vec.push_back(segment(start_node, n->get_Length()));
//					segments[f_n->GetFID()] = seg_vec;
//				}
//				std::pair<long, long> intersection_pair;
//				if ( current_cluster < intersected_cluster ) {
//					intersection_pair = std::pair<long, long>(current_cluster, intersected_cluster);
//				} else {
//					intersection_pair = std::pair<long, long>(intersected_cluster, current_cluster);
//				}
//				/*if (cluster_intersections.find(intersection_pair) != cluster_intersections.end()) {
//					DM::Logger(DM::Error) << "Cluster have existing intersection";
//				}*/

//				cluster_intersections.insert(intersection_pair);
//				std::vector<segment> & seg_vec = segments[f_n->GetFID()];
//				seg_vec.push_back(segment(node_id, p_l));

//				f_n->SetField("intersect_id", f->GetFieldAsInteger("node_id"));
//				f->SetField("intersects", (int) f_n->GetFID());
//			}
		}
	}

	this->network.syncReadFeatures();
	this->network.syncAlteredFeatures();
	this->junctions.syncReadFeatures();
	this->junctions.syncAlteredFeatures();

	//Actually do the intersection
	for (std::map<long, std::vector< segment > >::const_iterator it = segments.begin(); it != segments.end(); ++it) {
		std::vector< segment > segements_vec = it->second;
		for(int i = 0; i < segements_vec.size(); i++) {
			segment s_i = segements_vec[i];
			//find min element
			int lowest_element = i;
			double current_low = s_i.second;
			for(int j = i; j < segements_vec.size(); j++) {
				segment s_j = segements_vec[j];
				if (s_j.second < current_low) {
					lowest_element = j;
					current_low = s_j.second;
				}
			}
			//switch pos
			segment s_tmp = segements_vec[lowest_element];
			segements_vec[i] = s_tmp;
			segements_vec[lowest_element] = s_i;
		}
		//Create Elements;
		long edge_id = it->first;
		OGRFeature * e = network.getFeature(edge_id);
		OGRLineString * n = (OGRLineString *)e->GetGeometryRef();
//Build problem with 1.10
//		for(int i = 1; i < segements_vec.size(); i++ ) {
//			if (segements_vec[i-1].second == segements_vec[i].second)
//				continue;
//			OGRLineString * s = n->getSubLine(segements_vec[i-1].second, segements_vec[i].second, false);
//			if (!s) {
//				DM::Logger(DM::Error) << segements_vec[i-1].second << " " << segements_vec[i].second;
//				continue;
//			}
//			OGRFeature * s_f = this->network.createFeature();
//			s_f->SetGeometry(s);
//			s_f->SetField("new", 1);
//			s_f->SetField("start_id", (int)segements_vec[i-1].first);
//			s_f->SetField("end_id", (int)segements_vec[i].first);
//		}
	}
}
