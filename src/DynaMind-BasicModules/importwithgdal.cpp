/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Christian Urich
 
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

#include "importwithgdal.h"
#include "tbvectordata.h"
#include "simplecrypt.h"
#include <sstream>
#include <guiimportwithgdal.h>
#include <algorithm>
#include <QHash>

DM_DECLARE_NODE_NAME(ImportwithGDAL, Modules)

ImportwithGDAL::ImportwithGDAL()
{
    driverType = ShapeFile;

    this->FileName = "";
    this->addParameter("Filename", DM::FILENAME, &this->FileName);
    this->epsgcode=31254;
    this->addParameter("Transform to EPSG:", DM::INT, &this->epsgcode);
    this->ViewName = "";
    this->addParameter("ViewName", DM::STRING, &this->ViewName);
    this->tol = 0.01;
    this->addParameter("Tolerance", DM::DOUBLE, &this->tol);
    devider = 100;
    this->append = false;
    this->addParameter("AppendToExisting", DM::BOOL, &this->append);
    this->attributesToImport = std::map<std::string, std::string>();
    this->addParameter("AttributesToImport", DM::STRING_MAP, &this->attributesToImport);
    this->ImportAll = true;
    this->addParameter("ImportAll", DM::BOOL, &this->ImportAll);

    //WFS Input
    this->WFSDataName = "";
    this->addParameter("WFSDataName", DM::STRING, &this->WFSDataName);
    this->WFSServer = "";
    this->addParameter("WFSServer", DM::STRING, &this->WFSServer);
    this->WFSUsername = "";
    this->addParameter("WFSUsername", DM::STRING, &this->WFSUsername);
    this->WFSPassword = "";
    this->addParameter("WFSPassword", DM::STRING, &this->WFSPassword);

    this->flip_wfs = false;
    this->addParameter("flip_wfs", DM::BOOL, &this->flip_wfs);

    fileok=false;


    poCT=NULL;
}
ImportwithGDAL::~ImportwithGDAL()
{
    if(poCT)
        delete poCT;
}

DM::Node * ImportwithGDAL::addNode(DM::System * sys, double x, double y, double z) {
    //CreateKey
    DM::Node n_tmp(x,y,z);
    QString key = this->createHash(x,y);
    std::vector<DM::Node* > * nodes = nodeList[key];
    if (!nodes) {
        nodes = new std::vector<DM::Node* >;
        nodeList[key] = nodes;
    }

    foreach (DM::Node * n, *nodes) {
        if (n->compare2d(&n_tmp, tol))
            return n;
    }

    DM::Node * res_n = sys->addNode(n_tmp);
    nodes->push_back(res_n);
    return res_n;
}

void ImportwithGDAL::appendAttributes(Component *cmp, OGRFeatureDefn *poFDefn, OGRFeature *poFeature)
{

    int iField;


    for( iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
    {
        OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );
        bool exists = this->attributesToImport.find(poFieldDefn->GetNameRef()) != this->attributesToImport.end();
        if (!ImportAll && !exists)
            continue;
        std::string attrName;
        if (exists)
            attrName = this->attributesToImport[poFieldDefn->GetNameRef()];
        else
            attrName = poFieldDefn->GetNameRef();
        Attribute attr(attrName);
        if( poFieldDefn->GetType() == OFTInteger )
            attr.setDouble(poFeature->GetFieldAsInteger( iField ));
        else if( poFieldDefn->GetType() == OFTReal )
            attr.setDouble(poFeature->GetFieldAsDouble( iField ));
        else if( poFieldDefn->GetType() == OFTString )
            attr.setString(poFeature->GetFieldAsString( iField ));
        else
            attr.setString(poFeature->GetFieldAsString( iField ));
        cmp->addAttribute(attr);
    }
}

Component *ImportwithGDAL::loadNode(System *sys, OGRFeature *poFeature)
{
    OGRGeometry *poGeometry;
    poGeometry = poFeature->GetGeometryRef();
    DM::Node * n = 0;
    if( poGeometry != NULL
            && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint )
    {
        OGRPoint *poPoint = (OGRPoint *) poGeometry;

        double x = poPoint->getX();
        double y = poPoint->getY();

        transform(&x,&y);
        n = this->addNode(sys, x, y, 0);
        sys->addComponentToView(n, this->view);
    }
    return n;

}

Component *ImportwithGDAL::loadEdge(System *sys, OGRFeature *poFeature)
{
    OGRGeometry *poGeometry;
    poGeometry = poFeature->GetGeometryRef();
    DM::Node * n = 0;
    if( poGeometry != NULL
            && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiLineString ) {

        OGRMultiLineString *mpoLineString = (OGRMultiLineString *) poGeometry;
        int number_of_linestrings = mpoLineString->getNumGeometries();
        for (int i = 0; i < number_of_linestrings; i++) {
            OGRLineString *poPolyline = (OGRLineString *) mpoLineString->getGeometryRef(i);
            int npoints = poPolyline->getNumPoints();
            if (npoints == 0)
                return 0;
            OGRPoint *poPoint = new OGRPoint();
            std::vector<Node*> nlist;

            for (int i = 0; i < npoints; i++) {
                poPolyline->getPoint(i, poPoint);
                double x = poPoint->getX();
                double y = poPoint->getY();
                transform(&x,&y);
                n = this->addNode(sys, x, y, 0);
                if (find(nlist.begin(), nlist.end(), n) == nlist.end())
                    nlist.push_back(n);

            }
            if (nlist.size() < 2)
                return 0;
            delete poPoint;
            std::vector<DM::Edge *> edges;
            for (unsigned int i = 1; i < nlist.size(); i++) {
                edges.push_back(sys->addEdge(nlist[i-1], nlist[i], this->view));
            }

            if (edges.size() > 0)
                return edges[0];
        }
    }



    if( poGeometry != NULL
            && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString )
    {
        OGRLineString *poPolyline = (OGRLineString *) poGeometry;
        int npoints = poPolyline->getNumPoints();
        if (npoints == 0)
            return 0;
        OGRPoint *poPoint = new OGRPoint();
        std::vector<Node*> nlist;

        for (int i = 0; i < npoints; i++) {
            poPolyline->getPoint(i, poPoint);
            double x = poPoint->getX();
            double y = poPoint->getY();
            transform(&x,&y);
            n = this->addNode(sys, x, y, 0);
            if (find(nlist.begin(), nlist.end(), n) == nlist.end())
                nlist.push_back(n);

        }
        if (nlist.size() < 2)
            return 0;
        delete poPoint;
        std::vector<DM::Edge *> edges;
        for (unsigned int i = 1; i < nlist.size(); i++) {
            edges.push_back(sys->addEdge(nlist[i-1], nlist[i], this->view));
        }

        if (edges.size() > 0)
            return edges[0];
    }
    return 0;
}

Component *ImportwithGDAL::loadFace(System *sys, OGRFeature *poFeature)
{
    OGRGeometry *poGeometry;
    poGeometry = poFeature->GetGeometryRef();
    DM::Node * n = 0;
    if( poGeometry != NULL
            && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon )
    {
        OGRPolygon *poPolygon = (OGRPolygon *) poGeometry;
        OGRLinearRing * ring = poPolygon->getExteriorRing();
        int npoints = ring->getNumPoints();
        if (npoints == 0)
            return 0;
        OGRPoint *poPoint = new OGRPoint();
        std::vector<Node*> nlist;

        for (int i = 0; i < npoints; i++) {
            ring->getPoint(i, poPoint);
            double x = poPoint->getX();
            double y = poPoint->getY();
            transform(&x,&y);
            n = this->addNode(sys, x, y, 0);
            if (find(nlist.begin(), nlist.end(), n) == nlist.end())
                nlist.push_back(n);

        }
        if (nlist.size() < 3)
            return 0;
        nlist.push_back(nlist[0]);
        delete poPoint;
        return sys->addFace(nlist, this->view);
    }

    if( poGeometry != NULL
            && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiPolygon )
    {
        OGRMultiPolygon *mpoPolygon = (OGRMultiPolygon *) poGeometry;
        int number_of_faces = mpoPolygon->getNumGeometries();
        for (int i = 0; i < number_of_faces; i++) {
            OGRPolygon *poPolygon = (OGRPolygon *) mpoPolygon->getGeometryRef(i);
            OGRLinearRing * ring = poPolygon->getExteriorRing();
            int npoints = ring->getNumPoints();
            if (npoints == 0)
                return 0;
            OGRPoint *poPoint = new OGRPoint();
            std::vector<Node*> nlist;

            for (int i = 0; i < npoints; i++) {
                ring->getPoint(i, poPoint);
                double x = poPoint->getX();
                double y = poPoint->getY();
                transform(&x,&y);
                n = this->addNode(sys, x, y, 0);
                if (find(nlist.begin(), nlist.end(), n) == nlist.end())
                    nlist.push_back(n);

            }
            if (nlist.size() < 3)
                return 0;
            nlist.push_back(nlist[0]);
            delete poPoint;
            return sys->addFace(nlist, this->view);
        }
    }

    return 0;

}

void ImportwithGDAL::initPointList(System *sys)
{
    nodeList.clear();
    std::map<std::string, Node*> nodes =  sys->getAllNodes();
    for (std::map<std::string, Node*>::const_iterator it = nodes.begin();
         it != nodes.end();
         ++it) {
        DM::Node * n = it->second;
        QString key = this->createHash(n->getX(), n->getY());
        std::vector<DM::Node* > * nodes = nodeList[key];
        if (!nodes) {
            nodes = new std::vector<DM::Node* >;
            nodeList[key] = nodes;
        }
        nodes->push_back(n);
    }
}

QString ImportwithGDAL::createHash(double x, double y)
{
    int ix = (int) x / devider;
    int iy = (int) y / devider;
    QString key = QString::number(ix) + "|" +  QString::number(iy);

    return key;
}

void ImportwithGDAL::init() {

    //Only update if something has changed
    bool changed = false;
    if (FileName_old != FileName) changed = true; FileName_old = FileName;
    if (ViewName_old != ViewName) changed = true; ViewName_old = ViewName;
    if (WFSDataName_old != WFSDataName) changed = true; WFSDataName_old = WFSDataName;
    if (WFSServer_old != WFSServer) changed = true; WFSServer_old = WFSServer;
    if (WFSUsername_old != WFSUsername) changed = true; WFSUsername_old = WFSUsername;
    if (WFSPassword_old != WFSPassword) changed = true; WFSPassword_old = WFSPassword;
    if (append_old != append) changed = true; append_old = append;



    if (!changed)
        return;



    if (!this->WFSServer.empty()) driverType = WFS;
    else driverType = ShapeFile;



    if (driverType == WFS) {
        //create server name
        std::stringstream servername;
        servername << "WFS:http://";
        servername <<  this->WFSUsername; //Username
        servername <<  ":";
        SimpleCrypt crypto(Q_UINT64_C(0x0c2ad4a4acb9f023));
        servername <<  crypto.decryptToString(QString::fromStdString(this->WFSPassword)).toStdString(); //Password
        servername <<  "@";
        servername << this->WFSServer;
        this->server_full_name = servername.str();
        OGRLayer * poLayer = this->LoadLayer();
        if (!poLayer) {
            fileok=false;
            return;
        }

        fileok=true;
        view = DM::View();
        view.setName(ViewName);
        this->vectorDataInit(poLayer);
        return;
    }

    fileok=true;

    if(FileName.empty())
    {
        DM::Logger(DM::Error) << "No file specified " << FileName;
        return;
    }

    if(ViewName.empty())
    {
        DM::Logger(DM::Error) << "No view specified";
        return;
    }

    view = DM::View();
    view.setName(ViewName);

    OGRRegisterAll();
    GDALAllRegister();	// neccessary for windows!

    OGRDataSource       *poDS;
    OGRSFDriverRegistrar::GetRegistrar()->GetDriverCount();
    poDS = OGRSFDriverRegistrar::Open( FileName.c_str(), FALSE );

    if( poDS == NULL )
    {
        GDALDataset  *poDataset;

        poDataset = (GDALDataset *) GDALOpen( FileName.c_str(), GA_ReadOnly );
        if( poDataset == NULL )
        {
            DM::Logger(DM::Error) << "Open failed.";
            fileok=false;
            return;
        }
        else
        {
            rasterDataInit(poDataset);
        }
    }
    else
    {
        vectorDataInit(LoadLayer());
    }

    return;
}

bool ImportwithGDAL::createInputDialog()
{
    QWidget * w = new GUIImportWithGDAL(this);
    w->show();
    return true;
}

void ImportwithGDAL::vectorDataInit(OGRLayer       *poLayer)
{
    isvectordata = true;
    // OGRLayer  *poLayer;
    //poLayer = poDS->GetLayer(0);
    OGRFeature *poFeature;

    poLayer->ResetReading();
    int wkbtype = -1;
    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {
        OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
        int iField;

        for( iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
        {
            OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );
            bool exists = this->attributesToImport.find(poFieldDefn->GetNameRef()) != this->attributesToImport.end();
            if (!ImportAll && !exists)
                continue;
            std::string attrName;
            if (exists)
                attrName = this->attributesToImport[poFieldDefn->GetNameRef()];
            else
                attrName = poFieldDefn->GetNameRef();
            view.addAttribute(attrName);
        }

        OGRGeometry *poGeometry;


        poGeometry = poFeature->GetGeometryRef();
        wkbtype = wkbFlatten(poGeometry->getGeometryType());
        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint )
        {
            view.setType(DM::NODE);
            view.setAccessType(DM::WRITE);
            DM::Logger(DM::Debug) << "Found: Geometry type wkbPoint";
        }

        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon )
        {
            view.setType(DM::FACE);
            view.setAccessType(DM::WRITE);
            DM::Logger(DM::Debug) << "Found: Geometry type wkbPolygon";
        }

        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiPolygon)
        {
            view.setType(DM::FACE);
            view.setAccessType(DM::WRITE);
            DM::Logger(DM::Debug) << "Found: Geometry type wkbMultiPolygon";
        }
        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString )
        {
            view.setType(DM::EDGE);
            view.setAccessType(DM::WRITE);
            DM::Logger(DM::Debug) << "Found: Geometry type wkbLineString";
        }

        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiPoint )
        {
            DM::Logger(DM::Error) << "Geometry type not implemented: " << "wkbMultiPoint";
            fileok=false;
            return;
        }

        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiLineString )
        {
            view.setType(DM::EDGE);
            view.setAccessType(DM::WRITE);
            DM::Logger(DM::Debug) << "Found: Geometry type wkbMultiLineString";
        }


        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiPoint )
        {
            DM::Logger(DM::Error) << "Geometry type not implemented: " << "wkbMultiPoint";
            fileok=false;
            return;
        }

        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbGeometryCollection )
        {
            DM::Logger(DM::Error) << "Geometry type not implemented: " << "wkbGeometryCollection";
            fileok=false;
            return;
        }

        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbNone )
        {
            DM::Logger(DM::Error) << "Geometry type not implemented: " << "wkbNone";
            fileok=false;
            return;
        }

        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbLinearRing )
        {
            DM::Logger(DM::Error) << "Geometry type not implemented: " << "wkbLinearRing";
            fileok=false;
            return;
        }

        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint25D )
        {
            DM::Logger(DM::Error) << "Geometry type not implemented: " << "wkbPoint25D";
            fileok=false;
            return;
        }

        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString25D )
        {
            DM::Logger(DM::Error) << "Geometry type not implemented: " << "wkbLineString25D";
            fileok=false;
            return;
        }

        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon25D)
        {
            DM::Logger(DM::Error) << "Geometry type not implemented: " << "wkbPolygon25D";
            fileok=false;
            return;
        }

        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiPoint25D)
        {
            DM::Logger(DM::Error) << "Geometry type not implemented: " << "wkbMultiPoint25D";
            fileok=false;
            return;
        }

        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiLineString25D)
        {
            DM::Logger(DM::Error) << "Geometry type not implemented: " << "wkbMultiLineString25D";
            fileok=false;
            return;
        }

        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbMultiPolygon25D)
        {
            DM::Logger(DM::Error) << "Geometry type not implemented: " << "wkbMultiPolygon25D";
            fileok=false;
            return;
        }

        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbGeometryCollection25D)
        {
            DM::Logger(DM::Error) << "Geometry type not implemented: " << "wkbGeometryCollection25D";
            fileok=false;
            return;
        }



        if( poGeometry != NULL
                && wkbFlatten(poGeometry->getGeometryType()) == wkbUnknown )
        {
            DM::Logger(DM::Error) << "Geometry type unknown";
            fileok=false;
            return;
        }

        OGRFeature::DestroyFeature( poFeature );

        break;
    }
    if (!fileok)
        return;
    if (view.getType() == -1) {
        DM::Logger(DM::Error) << "Unknown wbk type " << wkbtype;
        fileok = false;
        return;
    }

    std::vector<DM::View> data;
    if (append) {
        data.push_back( DM::View("dummy", SUBSYSTEM, READ));
    }
    data.push_back(view);

    this->addData("Data", data);
    //OGRDataSource::DestroyDataSource( poDS );

    return;
}

void ImportwithGDAL::rasterDataInit(GDALDataset  *poDataset)
{
    isvectordata=false;
    append=false;

    double        adfGeoTransform[6];
    int           nBlockXSize, nBlockYSize;
    int           bGotMin, bGotMax;
    double        adfMinMax[2];
    GDALRasterBand  *poBand;

    DM::Logger(DM::Debug) << "Driver: " << poDataset->GetDriver()->GetDescription() << "/" << poDataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME );
    DM::Logger(DM::Debug) << "Size is " << poDataset->GetRasterXSize() << " " << poDataset->GetRasterYSize() << " " << poDataset->GetRasterCount();

    if( !std::string(poDataset->GetProjectionRef()).empty() )
    {
        DM::Logger(DM::Debug) << "Projection is " << " " << poDataset->GetProjectionRef();
    }
    else
    {
        DM::Logger(DM::Error) << "No projection found";
        GDALClose(poDataset);
        fileok=false;
        return;
    }

    if( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
    {
        DM::Logger(DM::Debug) << "Origin = " << adfGeoTransform[0] << "," << adfGeoTransform[3];
        DM::Logger(DM::Debug) << "Pixel Size = " << adfGeoTransform[1] << "," << adfGeoTransform[5];
    }

    poBand = poDataset->GetRasterBand( 1 );
    poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
    DM::Logger(DM::Debug) << "Block=" << nBlockXSize << "x" << nBlockYSize <<
                             "Type=" << GDALGetDataTypeName(poBand->GetRasterDataType()) <<
                             ", ColorInterp=" << GDALGetColorInterpretationName(poBand->GetColorInterpretation());

    adfMinMax[0] = poBand->GetMinimum( &bGotMin );
    adfMinMax[1] = poBand->GetMaximum( &bGotMax );
    if( ! (bGotMin && bGotMax) )
        GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);

    DM::Logger(DM::Debug) << "Min=" << adfMinMax[0] << ", Max=" << adfMinMax[1];

    if( poBand->GetOverviewCount() > 0 )
        DM::Logger(DM::Debug) << "Band has" << poBand->GetOverviewCount() << " overviews";

    if( poBand->GetColorTable() != NULL )
        DM::Logger(DM::Debug) << "Band has a color table with " << poBand->GetColorTable()->GetColorEntryCount() << " entries";


    view.setType(DM::RASTERDATA);
    view.setAccessType(DM::WRITE);

    std::vector<DM::View> data;
    data.push_back(view);

    this->addData("Data", data);

    GDALClose(poDataset);
    return;
}

void ImportwithGDAL::run()
{
    if(!fileok)
    {
        DM::Logger(DM::Error) << "Cannot read file";
        return;
    }

    if(isvectordata)
        importVectorData();
    else
        importRasterData();

    return;
}

bool ImportwithGDAL::importVectorData()
{
    DM::System * sys = this->getData("Data");
    this->initPointList(sys);

    if(poCT!=NULL)
        delete poCT;

    OGRSpatialReference *oSourceSRS, *oTargetSRS;

    OGRRegisterAll();

    //OGRDataSource       *poDS;
    //OGRSFDriverRegistrar::GetRegistrar()->GetDriverCount();
    //poDS = OGRSFDriverRegistrar::Open( FileName.c_str(), FALSE );
    /*if( poDS == NULL )
    {
        DM::Logger(DM::Error) << "Open failed.";
        return false;
    }*/

    OGRLayer  *poLayer = this->LoadLayer();
    if (!poLayer) {
        Logger(Error) << "Something went wrong while loading layer in ImportVectorData";
        return false;
    }

    //int layerCount = poDS->GetLayerCount();
    //poLayer = poDS->GetLayer(0);

    OGRFeature *poFeature;

    poLayer->ResetReading();

    oSourceSRS = poLayer->GetSpatialRef();
    oTargetSRS = new OGRSpatialReference();
    oTargetSRS->importFromEPSG(this->epsgcode);
    poCT = OGRCreateCoordinateTransformation( oSourceSRS, oTargetSRS );

    if(poCT == NULL)
    {
        transformok=false;
        DM::Logger(DM::Error) << "Unknown transformation to EPSG:" << this->epsgcode;
    }
    else
    {
        transformok=true;
    }

    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {

        OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
        DM::Component * cmp;
        if (view.getType() == DM::NODE)
            cmp = this->loadNode(sys, poFeature);
        if (view.getType() == DM::EDGE)
            cmp = this->loadEdge(sys, poFeature);
        if (view.getType() == DM::FACE)
            cmp = this->loadFace(sys, poFeature);
        if (cmp)
            this->appendAttributes(cmp, poFDefn, poFeature);
        OGRFeature::DestroyFeature( poFeature );
    }

    //OGRDataSource::DestroyDataSource( poDS );
    return true;
}

bool ImportwithGDAL::importRasterData()
{
    GDALDataset  *poDataset;
    GDALRasterBand  *poBand;
    double adfGeoTransform[6];
    DM::RasterData * r = this->getRasterData("Data", view);

    poDataset = (GDALDataset *) GDALOpen( FileName.c_str(), GA_ReadOnly );
    poBand = poDataset->GetRasterBand( 1 );

    float *pafScanline;
    int nXSize = poBand->GetXSize();
    int nYSize = poBand->GetYSize();
    double xoff=0;
    double yoff=0;
    double xsize=0;
    double ysize=0;

    r->setNoValue(-9999999999);

    if( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
    {
        xsize = fabs(adfGeoTransform[1]);
        ysize = fabs(adfGeoTransform[5]);
        xoff = adfGeoTransform[0];
        yoff = adfGeoTransform[3] - ysize * nYSize;
    }

    r->setSize(nXSize, nYSize, xsize,ysize,xoff,yoff);

    pafScanline = (float *) CPLMalloc(sizeof(float)*nXSize);

    for(int index=0; index < nYSize; index++)
    {
        poBand->RasterIO( GF_Read, 0, nYSize-index-1, nXSize, 1, pafScanline, nXSize, 1, GDT_Float32, 0, 0 );

        for(int x=0; x < nXSize; x++)
        {
            r->setCell(x,index,pafScanline[x]);
        }
    }

    CPLFree(pafScanline);
    GDALClose(poDataset);
    return true;
}

bool ImportwithGDAL::transform(double *x, double *y)
{

    if (this->driverType == WFS && this->flip_wfs) {
        double tmp_x = *x;
        *x = *y;
        *y = tmp_x;
    }

    if(!transformok)
        return false;

    if( poCT == NULL || !poCT->Transform( 1, x, y ) )
    {
        return false;
    }
    else
    {
        return true;
    }

    return false;
}

OGRLayer *ImportwithGDAL::LoadLayer()
{
    if (driverType == WFS) {
        OGRDataSource       *poDS;
        OGRLayer            *poLayer;
        poDS = OGRSFDriverRegistrar::Open( server_full_name.c_str(), FALSE );

        int LayerCount = poDS->GetLayerCount();

        for (int i = 0; i < LayerCount; i++) {
            poLayer = poDS->GetLayer(i);
            std::string currentLayerName =  poLayer->GetName();
            if (currentLayerName == this->WFSDataName) {
                return poLayer;
            }
        }
        return 0;
    }

    OGRDataSource       *poDS;
    OGRSFDriverRegistrar::GetRegistrar()->GetDriverCount();
    poDS = OGRSFDriverRegistrar::Open( FileName.c_str(), FALSE );

    if( poDS == NULL )
    {
        return 0;
    }

    return poDS->GetLayer(0);

}
