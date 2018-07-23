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
        for (size_t i = 0; i < Qp.size(); i++) {
            double ds = storage_volume - Qp[i]*dt;
            if (Vmin[i] > ds)
                ds = Vmin[i];
//            std::cout << "storage" << ds << std::endl;
            if (storage_volume - ds < 0)
                continue;

            (*outputs[i])[0] = storage_volume - ds;
            storage_volume = ds;
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
        return true;
}
