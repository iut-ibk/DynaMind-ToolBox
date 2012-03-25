/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Christian Urich

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
#include "dmcompilersettings.h"

#ifndef GENERATESEWERNETWORK_H
#define GENERATESEWERNETWORK_H

#include "dmmodule.h"
#include "dm.h"
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
namespace   ublas = boost::numeric::ublas;

using namespace DM;

/**
   * @ingroup Sewer
   * @brief Generates the layout of a sewer network
   *
   * Tries to connected new Inlets with the existing sewer network.
   * Objective of the agent is to find a junction of the existing network.
   * If he finds the junction the path is marked with a connectivity function.
   * Following agents are attracted by this path. At the end this connectivity
   * field is exported and can be used by the ExtractNetwork module to
   * extract the new sewer layout
   *
   * Following data views are requiered:
   * @subsection City
   * @subsubsection RasterData: Topology
   * @subsubsection Node: INLET
   * - New
   * @subsection sewerGeneration
   * @subsubsection RasterData: ConnectivityField_in
   * @subsubsection RasterData: ForbiddenAreas
   * 0... Allowed
   * 1... Not Allowed
   * @subsubsection RasterData: Goals
   * 1... Goal
   */
class DM_HELPER_DLL_EXPORT GenerateSewerNetwork : public  Module {
    DM_DECLARE_NODE (GenerateSewerNetwork)
    public:
        class Pos {
        public:
        long x;
        long y;
        double z;
        double h;

        Pos(long x, long y) {this->x = x; this->y = y;}
        Pos() {}
    };
public:
    class Agent {

    public:
        Pos StartPos;
        Node * startNode;
        std::vector<Pos> path;
        Pos currentPos;
        virtual void run();
        bool alive;
        bool successful;
        RasterData * Topology;
        RasterData * MarkPath;
        RasterData * ConnectivityField;
        RasterData * Goals;
        RasterData * ForbiddenAreas;
        long steps;
        double AttractionTopology;
        double AttractionConnectivity;
        double MultiplyerCenterCon;
        double MultiplyerCenterTop;
        double StablizierLastDir;
        double Hmin;
        int lastdir;
        Agent(Pos);
    protected:
        ublas::vector<double> neigh;
        ublas::vector<double> decissionVector;
        ublas::vector<double> ProbabilityVector;


    };


private:
    DM::View  Topology;
    DM::View  ConnectivityField;
    DM::View  ConnectivityField_in;

    DM::View  Path;
    DM::View  Goals;
    DM::View  ForbiddenAreas;


    DM::View  Inlets;


    DM::RasterData *  rTopology;
    DM::RasterData *  rConnectivityField;
    DM::RasterData *  rConnectivityField_in;

    DM::RasterData *  rPath;
    DM::RasterData *  rGoals;
    DM::RasterData *  rForbiddenAreas;

    DM::System * city;
    DM::System * sewerGeneration;

    int ConnectivityWidth;
    double AttractionTopology;
    double AttractionConnectivity;
    double MultiplyerCenterCon;
    double MultiplyerCenterTop;
    long steps;
    double Hmin;
    int StablizierLastDir;
    std::string IdentifierStartPoins;

public:
    GenerateSewerNetwork();
    void run();

    void MarkPathWithField(const std::vector<Pos> & path, RasterData * ConnectivityField, int ConnectivityWidth);
    void addRadiusValueADD(int x, int y, RasterData * layer, int rmax, double value);
    void addRadiusValue(int x, int y, RasterData * layer, int rmax, double value, double **);
    static int indexOfMinValue(const ublas::vector<double> & vec);
};

#endif // GENERATESEWERNETWORK_H
