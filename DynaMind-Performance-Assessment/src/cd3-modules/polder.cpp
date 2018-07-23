#include "polder.h"

CD3_DECLARE_NODE_NAME(Polder)

Polder::Polder()
{
	addState("StorageVolume", &storage_volume);

	addParameter(ADD_PARAMETERS(Qp))
		.setUnit("m^3");
	addParameter(ADD_PARAMETERS(Vmin))
		.setUnit("m^3");

    addInPort(ADD_PARAMETERS(in));

}


int Polder::f(ptime time, int dt) {
        (void) time;
		storage_volume += in[0];
		for (size_t i = 1; i < Qp.size(); i++) {
			loadings[i]+=in[0]*	in[i];
		}


		for (size_t i = 0; i < Qp.size(); i++) {
			double ds = storage_volume - Qp[i]*dt;
			if (Vmin[i] > ds)
				ds = Vmin[i];
			if (storage_volume - ds < 0)
				continue;

			(*outputs[i])[0] = storage_volume - ds;

			for (size_t j = 1; j < in.size(); j++) {
				(*outputs[i])[j] = loadings[j] / storage_volume;
				loadings[j] -= (storage_volume - ds) * loadings[j] / storage_volume;
			}
			storage_volume= ds;
		}

        return dt;
}

bool Polder::init(ptime start, ptime end, int dt) {
        (void) start;
        (void) end;

        storage_volume = 0;

		if (outputs.size() == Qp.size())
			return true;

		for (size_t i = 0; i < Qp.size(); i++) {
			Flow *tmp = new Flow();
			std::ostringstream name;
			name << "out_" << i;
			addOutPort(name.str(), tmp);
			outputs.push_back(tmp);
			output_names.push_back(name.str());
		}
		loadings.clear();

		for (size_t i = 0; i < Qp.size(); i++) {
			(*outputs[i]) = in;
			(*outputs[i]).clear();
			loadings.push_back(0);

		}

//		out_p ;
//		out_p.clear();
//		out_w = in;
//		out_w.clear();
//		volume = in;
//		volume.clear();

        return true;
}
