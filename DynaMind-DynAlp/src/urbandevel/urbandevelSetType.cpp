#include <urbandevelSetType.h>
#include <dm.h>
#include <tbvectordata.h>
#include <dmgeometry.h>
#include <dahelper.h>
#include <algorithm>
#include <iterator>

DM_DECLARE_NODE_NAME(urbandevelSetType, DynAlp)

urbandevelSetType::urbandevelSetType()
{
    numbernearest = 9;
    from_SB = 0;
    from_nearest = 1;
    minperctype = 10;
    this->addParameter("no superblocks considered", DM::INT, &this->numbernearest);
    this->addParameter("type from SB", DM::BOOL, &this->from_SB);
    this->addParameter("type from neighbors", DM::BOOL, &this->from_nearest);
    this->addParameter("min percentage of each type", DM::INT, &this->minperctype);
    typevec.push_back("res");
    typevec.push_back("com");
    typevec.push_back("ind");
}

urbandevelSetType::~urbandevelSetType()
{
}

void urbandevelSetType::init()
{
    city = DM::View("CITY", DM::NODE, DM::READ);
    sb = DM::View("SUPERBLOCK", DM::FACE, DM::MODIFY);
    sb_cent = DM::View("SUPERBLOCK_CENTROIDS", DM::NODE, DM::READ);

    sb.addAttribute("type", DM::Attribute::DOUBLE, DM::WRITE);

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(city);
    views.push_back(sb);
    views.push_back(sb_cent);
    this->addData("data", views);
}

void urbandevelSetType::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> superblocks = sys->getAllComponentsInView(sb);
    std::vector<DM::Component *> superblocks_centroids = sys->getAllComponentsInView(sb_cent);

    std::map<std::string,int> sbtypecount;
    std::vector<std::string> sbtypevec;
    std::vector<double> sbdistvec;

    for (int i = 0; i < typevec.size(); ++i) {
        sbtypecount[typevec[i]] = 0;
    }

    for (int active = 0; active < superblocks.size(); active++)
    {
        std::map<double,std::string> disttype;

        std::string actualtype = superblocks[active]->getAttribute("type")->getString();

        if ( find(typevec.begin(), typevec.end(), actualtype) == typevec.end() )
        {
            DM::Logger(DM::Debug) << "type" << actualtype << " is not defined ... redefining";
            actualtype = "";
        }

        if ( !actualtype.empty() )
        {
            sbtypecount[actualtype]++;
            sbtypevec.push_back(actualtype);
            sbdistvec.push_back(99999);
            continue;
        }
        std::vector<DM::Component*> sblinkactive = superblocks[active]->getAttribute("SUPERBLOCK_CENTROIDS")->getLinkedComponents();

        if( sblinkactive.size() < 1 )
        {
            DM::Logger(DM::Error) << "no area - centroid link";
            return;
        }

        DM::Node * sbcentroidactive = dynamic_cast<DM::Node*>(sblinkactive[0]);

        for (int compare = 0; compare < superblocks.size(); compare++)
        {
            std::string comparetype = superblocks[compare]->getAttribute("type")->getString();

            if ( active == compare || comparetype.empty() )
                continue;

            std::vector<DM::Component*> sblinkcompare = superblocks[compare]->getAttribute("SUPERBLOCK_CENTROIDS")->getLinkedComponents();

            if( sblinkcompare.size() < 1 )
            {
                DM::Logger(DM::Error) << "no area - centroid link";
                return;
            }

            DM::Node * sbcentroidcompare = dynamic_cast<DM::Node*>(sblinkcompare[0]);

            double distance = TBVectorData::calculateDistance((DM::Node*)sbcentroidactive,(DM::Node*)sbcentroidcompare)/1000;
            std::string type =static_cast<string>( superblocks[compare]->getAttribute("type")->getString() );

            disttype[distance] = type;
            //DM::Logger(DM::Warning) << "type: " << type << "distance: " << distance;
        }

        int max = numbernearest;
        if (disttype.size() < numbernearest)
            max = disttype.size();

        map<std::string, pair<double,int> > rnktype;

        std::string settype = "";
        double setdist = 0;

        for (map<double,std::string>::iterator it = disttype.begin(); it != disttype.end(); it++)
        {
            double distance = it->first;
            std::string type = it->second;

            map<std::string, pair<double,int> >::iterator rnkit = rnktype.find(type);

            if ( rnkit == rnktype.end() )   // initialize if first element with type x
            {
                rnktype[type].first = 1/distance;
                rnktype[type].second = 1;
            }
            else                            // if type already exists increase invert distance and count
            {
                rnktype[type].first += 1/distance;
                rnktype[type].second++;
            }

            // set largest accumulated invert distance as typedistance

            if ( rnktype[type].first > setdist) {
                setdist = rnktype[type].first;
                settype = type;
            }

            if ( rnktype[type].second > max )
            {
                break;
            }
            //DM::Logger(DM::Warning) << "type = " << type << " num = " << rnktype[type].second << " dist = " << rnktype[type].first;
        }
        superblocks[active]->changeAttribute("type", settype);
        sbtypevec.push_back(settype);
        sbdistvec.push_back(setdist);
        sbtypecount[settype]++;
    }

    // correction for missing types below

    std::vector<int> changevec;

    for (int i = 0; i < typevec.size(); ++i)
    {
        double minnum = superblocks.size() * minperctype;
        minnum = ceil(minnum/100);
        std::string type = typevec[i];

        if (sbtypecount[type] < minnum)
        {
            DM::Logger(DM::Debug) << "typecount: " << type << " = " << sbtypecount[type] << " < minnum " <<minnum;
            std::string inctype = type;
            std::string dectype;

            for (int j = 0; j < typevec.size(); ++j) {
                if (sbtypecount[typevec[j]] > superblocks.size()/3 ) dectype = typevec[j];
            }

            double maxdist = 10^99;
            int where;

            for (int j = 0; j < sbtypevec.size(); ++j)
            {
                if ( sbtypevec[j] == dectype && sbdistvec[j] < maxdist && find(changevec.begin(),changevec.end(),j) == changevec.end())
                {
                    DM::Logger(DM::Debug) << "maxdist loop" << maxdist;
                    maxdist = sbdistvec[j];
                    where = j;
                }
            }
            sbtypecount[dectype]--;
            sbtypecount[inctype]++;
            changevec.push_back(where);
            superblocks[where]->changeAttribute("type", inctype);
            DM::Logger(DM::Debug) << "change: " <<  dectype << " to: " << inctype;
        }
    }
}
