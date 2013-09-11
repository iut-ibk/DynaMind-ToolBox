#include <readcsv.h>
DM_DECLARE_NODE_NAME(readCSV, DynAlp)

readCSV::readCSV() {

    //init values
    this->filestring = "inhabitants.csv";

    //Define model input

    this->addParameter("Filename",DM::FILENAME, &this->filestring);

    data = DM::View("DATA", DM::STRING_MAP, DM::WRITE);

    //QFile file(this->filename);
    //QTextStream in(&file);

    /*while(!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");
//        data.addAttribute(line)= fields;
    }

    file.close();*/
    std::vector<DM::View> view;
    view.push_back(data);

    this->addData("DATA", view);
}

void readCSV::run() {
}

