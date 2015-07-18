#ifndef GDALJOINCLUSTER_H
#define GDALJOINCLUSTER_H

#include <dmmodule.h>

class GDALJoinCluster : public DM::Module
{
		DM_DECLARE_NODE(GDALJoinCluster)

		private:
			double buffer;
			DM::ViewContainer network;
			DM::ViewContainer junctions;
public:
	GDALJoinCluster();
	void run();
};

#endif // GDALJOINCLUSTER_H
