#ifndef FACEATTRIBUTETORASTER_H
#define FACEATTRIBUTETORASTER_H

#include <dmmodule.h>

class DM_HELPER_DLL_EXPORT FaceAttributeToRaster : public DM::Module
{
	DM_DECLARE_NODE(FaceAttributeToRaster)
private:
		std::string faceName;
		std::string rasterDataName;
		std::string attributeName;
		DM::View inputFaceView;
		DM::View rasterDataView;
public:
	FaceAttributeToRaster();

	void init();
	void run();
	std::string getHelpUrl();
};

#endif // FACEATTRIBUTETORASTER_H
