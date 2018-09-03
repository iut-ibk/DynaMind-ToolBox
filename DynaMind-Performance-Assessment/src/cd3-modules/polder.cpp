#include "polder.h"

CD3_DECLARE_NODE_NAME(Polder)

Polder::Polder()
{
	addState("StorageVolume", &storage_volume);

	addParameter(ADD_PARAMETERS(Qp))
		.setUnit("m^3");
	addParameter(ADD_PARAMETERS(Vmin))
		.setUnit("m^3");

	addState(ADD_PARAMETERS(storage_level));

	addState(ADD_PARAMETERS(total_pollution));

    addInPort(ADD_PARAMETERS(in));
}

int Polder::f(ptime time, int dt) {
        (void) time;
		//std::cout << storage_volume << " "  << in[0] << std::endl;
		storage_volume += in[0];


		// Add inflow loading to total load
		for (size_t i = 1; i < in.size(); i++) {
			loadings[i-1]+=in[0]*in[i];
			//std::cout << in[0] << " " <<  loadings[i-1] << std::endl;
		}


		for (size_t i = 0; i < Qp.size(); i++) {
			double ds = storage_volume - Qp[i]*dt;

			if (Vmin[i] > ds)
				ds = Vmin[i];
			if (storage_volume - ds < 0)
				continue;

			(*outputs[i])[0] = storage_volume - ds;


			for (size_t j = 1; j < in.size(); j++) {
				(*outputs[i])[j] = loadings[j-1] / storage_volume; // current concentraion

				//std::cout << (storage_volume - ds) << std::endl;

				loadings[j-1] -= (storage_volume - ds) * loadings[j-1]/ storage_volume; //reduce total load by extracted water

			}

			storage_volume= ds;

		}
		storage_level.push_back(storage_volume);
		total_pollution.push_back(loadings[0]);
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
		}

		for (size_t i = 1; i < in.size(); i++)
			loadings.push_back(0);

//		out_p ;
//		out_p.clear();
//		out_w = in;
//		out_w.clear();
//		volume = in;
//		volume.clear();

        return true;
}
