/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
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

#include "dataLayer.h"


namespace DM{


 /**
  * \brief  constructor dataLayer
  *
  *  initialize data array and bdata array with parameters
  *
  * \param	width_   data layer width
  * \param	height_   data layer height_
  * \param	cellSize_   data layer cellSize_
  *
  */
dataLayer::dataLayer(int width_, int height_, int cellSize_)
{
  cellSize=cellSize_;
  layerswitch=2;
  width=width_;
  height=height_;
  mHeight=0;
  data=new double[width*height];
  bdata=new double[width*height];
  for (int i=0;i<width*height;i++)
  {
	  data[i]=0;
	  bdata[i]=0;
  }

  Area_ = 0;
  rotArea_ = 0;

};


 /**
  * \brief  constructor dataLayer
  *
  *  initialize data array and bdata array with NULL
  *
  *
  */
dataLayer::dataLayer()
{
	width=0;
	height=0;
	mHeight=0;
	cellSize=0;
	layerswitch=0;

	/*for (int i=0;i<width*height;i++)
	{
		data[i]=0;
		bdata[i]=0;
	}*/
	data = 0;
	bdata = 0;

	Area_ = 0;
	rotArea_ = 0;
};


 /**
  * \brief  destructor dataLayer
  *
  *
  */
dataLayer::~dataLayer()
{
	if ( Area_ ) delete Area_;
	Area_ = 0;
	if ( rotArea_ ) delete rotArea_;
	rotArea_ = 0;
	if ( data ) delete [] data;
	//data=NULL;
	if ( bdata ) delete [] bdata;
	//bdata=NULL;
};

 /**
  * \brief  clean dataLayer
  *
  * set Values in DataLAyer to Zero
  */

void dataLayer::clean()
{
  for (int i=0;i<width*height;i++)
  {
          data[i]=0;
          bdata[i]=0;
  }

  Area_ = 0;
  rotArea_ = 0;
};

 /**
  * \brief  get Value from data layer
  *
  * returns value depending on cellular automata step
  * determined by layerswitch
  * layerswitch==0 reads from data, writes to bdata
  * layerswitch==1 reads from bdata, writes to data
  * layerswitch==2 reads from bdata, writes to bdata (for randVal,...)
  *
  * \param x x coordinate
  * \paraj y y coordinate
  */
 double dataLayer::getVal(int x, int y) const
{
  if (x<0 || y<0 || x>=width || y >=height)
    {
		if(x<-width || y<-height || x>2*width || y>2*height)
		{
			//std::cout<<"error dataLayer::getVal()\n";

		}
	  //std::cout<<"error dataLayer::getVal(), x or y out of range\n";
          double val = neighbour(x,y);
           return val;
    }
    //layer==0 reads from data, writes to bdata
	//layer==1 reads from bdata, writes to data
	//layer==2 reads from bdata, writes to bdata (for randVal,...)

	switch (layerswitch)
	{
		case 0:
			return data[y*width+x];
			break;
		case 1:
			return bdata[y*width+x];
			break;
		case 2:
			//std::cout<<"error, layerswitch=2 in dataLayer::getVal()\n";
			return bdata[y*width+x];
			break;
		default:
			std::cout<<"error layerswitch out of range, dataLayer::getVal\n";
			return 0;
			break;
	}


};



 /**
  * \brief  set Value data layer
  *
  * sets value depending on cellular automata step
  * determined by layerswitch
  * layerswitch==0 reads from data, writes to bdata
  * layerswitch==1 reads from bdata, writes to data
  * layerswitch==2 reads from bdata, writes to bdata (for randVal,...)
  *
  * \param x x-coordinate
  * \param y y-coordinate
  * \param val value
  */
void dataLayer::setVal(int x, int y, double val)
{


	//layer==0 reads from data, writes to bdata
	//layer==1 reads from bdata, writes to data
     if (y>=height || x>=width || y<0 || x<0)
	 {
		 //std::cout<<"error,dataLayer::setVal, out of range\n";
	 }
	 else
	 {

		switch (layerswitch)
		{

			case 2:	//dataLayer::randVal always writes in data[i]
				bdata[y*width+x]=val;
				break;
			case 1:
				data[y*width+x]=val;
				break;
			case 0:
				bdata[y*width+x]=val;
				break;
			default:
				std::cout<<"error layerswitch dataLayer::setVal()\n";
				break;
		}
	 }

};



 /**
  * \brief  random value 
  * if rangeLow==0 && rangeHigh==1 && median == 1: 0-1 random
  * 
  * else weighted equal distribution [rangeLow; rangeHigh]
  *
  * \param rangeLow ceiling
  * \param rangeHigh floor
  * \param median median
  */
double dataLayer::randVal(double rangeLow, double rangeHigh, double median)
{
	//initial rand() in vSys.cpp srand();
	double r=0; //random:  value 0 or 1
	if(rangeLow==0 && rangeHigh==1 && median == 1)
	{
		r=0+(rand()%(2));
	}
	else
	{
		r=0+(rand()%(100));
		if (r<50)
		{
			r=(median-rangeLow)*(r/100*2); //!median /=0!;
		}
		else
		{
			r=(rangeHigh-median)*(2*r/100-1)+rangeLow;
		}
	}
	layerswitch=2;
	return r;
};


 /**
  * \brief  add Value data layer
  *
  * adds value depending on cellular automata step to existing value
  * determined by layerswitch
  * layerswitch==0 reads from data, writes to bdata
  * layerswitch==1 reads from bdata, writes to data
  * layerswitch==2 reads from bdata, writes to bdata (for randVal,...)
  *
  * \param x x-coordinate
  * \param y y-coordinate
  * \param addval value to add
  */
void dataLayer::addVal(int x, int y, double addValue)
{


	//layer==0 reads from data, writes to bdata
	//layer==1 reads from bdata, writes to data
     if (y>=height || x>=width || y<0 || x<0)
	 {
		 //std::cout<<"error,dataLayer::addVal, out of range\n";
	 }
	 else
	 {

		switch (layerswitch)
		{

			case 2:	//dataLayer::randVal always writes in data[i]
				bdata[y*width+x]+=addValue;
				break;
			case 1:
				data[y*width+x]+=addValue;
				break;
			case 0:
				bdata[y*width+x]+=addValue;
				break;
			default:
				std::cout<<"error layerswitch dataLayer::addVal()\n";
				break;
		}
	 }

};


 /**
  * \brief  moore neighbour of a cell
  *
  * if neigbour is out of data layer space 
  * kindOfNb=0 return neighbour value of torus universe
  * kindOfNb=1 return neighbour value of mirror universe
  * 
  *
  * \param x_ x-coordinate
  * \param y_ y-coordinate
  * \return	  value dataLayer value
  *
  */
double dataLayer::neighbour(int x_, int y_) const
{
//boarder neighbour definitions
//-------------------------//
	int kindOfNb=0/*torus*/;
	//int kindOfNb=1/*mirror*/;
//-------------------------//
	/*
	kindOfNb = 0 as "torus" universe:
	boarder are connected to another
	xmin-i=xmax xmax+i=xmin
	ymin-1=ymax ymax+i=ymin

	kindOfNb = 1 as "mirror" universe:
	values are mirrored at the boarder
	xmin-i=xmin+i xmax+i=xmax-i
	ymin-i=ymin+i ymax+i=ymax-i
	*/


	//local buffer variables
	int x=0;
	int y=0;

	//torus-universe
	if (kindOfNb==0)
	{

		//if (x_<0) {x=width+x_;}
		//else if (x_>=width) {x=x_-width;}
		//else if (y_<0) {y=height+y_;}
		//else if (y_>=height) {y=y_-height;}
		//else {x=x_;y=y_;}
		if (x_<0) {x=width+x_;}
		else if (x_>=width) {x=x_-width;}
		else if (y_<0) {y=height+y_;}
		else if (y_>=height) {y=y_-height;}
		else {x=x_;y=y_;}
	}

	//mirror boarder
	else if (kindOfNb==1)
	{
		//if (x_<0) {x=-x_;}
		//else if (x_>=width) {x=width-(x_-width);}
		//else if (y_<0) {y=-y_;}
		//else if (y_>=height) {y=height-(y_-height);}
		//else {x=x_;y=y_;}
		if (x_<0) {x=-x_;}
		else if (x_>=width) {x=width-1-(x_-(width-1));}
		else if (y_<0) {y=-y_;}
		else if (y_>=height) {y=height-1-(y_-(height-1));}
		else {x=x_;y=y_;}
	}
	else
	{
		std::cout<<"error neigbour definitions dataLayer::neighbour()\n";
	};

	return (this->getVal(x,y));


};



 /**
  * \brief  createArea
  *
  * creates area on datalayer
  *
  * \return	 Area_ returns a dataLayer
  *
  */
dataLayer* dataLayer::createArea()
{
	//boardering functions
	//left - south an right - north boarders
	//function of different order
	class function
	{
	private:
		double a;
		double d;
		int order;


	public:
		int numberX;
		double ymax;
		double ymin;


		function()
		{
			order=0;
			a=0;
			d=0;
			numberX=10;
			ymin=1000;
			ymax=-1000;
		};
		function(int order_, double a_)
		{

			numberX=((50+(rand()%(75))));
			ymin=1000;
			ymax=-1000;
			a=a_;
			order=order_;
		};




		//evaluates y value
		//determined by order of function
		int valueY(int x)
		{
			double y=0;
			switch (order)
			{
			case 0:
				y=a;
				break;

			case 1:
				y=a*x;
				break;
			case 2:
				y=a*(x-numberX*.5)*
					(x-numberX*.5);
				break;

			case 3:
				y=a*(x-numberX*.5)*
					(x-numberX*.5)*
					(x-numberX*.5);

				break;

			}
			return floor(y);

		}

	};



	//int cellSize=20;

	function *left;
	function *right;
	left=new function(2,(0+(rand()%(10)))*0.01);
	right=new function(2,-(0+(rand()%(10)))*0.01);

	for (int i=0;i<left->numberX;i++)
	{

		if (left->valueY(i)<left->ymin) {left->ymin=left->valueY(i);}
		if (left->valueY(i)>left->ymax) {left->ymax=left->valueY(i);}
		if (right->valueY(i)<right->ymin) {right->ymin=right->valueY(i);}
		if (right->valueY(i)>right->ymax) {right->ymax=right->valueY(i);}

	}

	if ( Area_ ) delete Area_;
	Area_ = new dataLayer((left->numberX),(left->ymax-right->ymin),cellSize);
	Area_->setSwitch(2);





	for (int i=0;i<left->numberX;i++)
	{
		int yCount=0;
		double myVal=0;
		double mxVal=0;

		for (int j=0;j<(left->ymax-right->ymin);j++)
		{

			if (j>left->valueY(i))
			{


				if (j<((left->ymax-right->ymin)+right->valueY(i)))
				{
					mxVal=sin(3.1415*i/left->numberX);
					/*int yRange=(left->ymax-right->ymin)+right->valueY(i)-left->valueY(i);*/
					myVal=mxVal*sin(3.1415*yCount/(left->ymax-left->valueY(i)-right->ymin+right->valueY(i)));
					Area_->setVal(i,j,myVal);
					yCount+=1;
				}

			}
		}
	}
	delete left;
	delete right;

	//Area_=Area;

	//delete Area;

	return Area_;


};


 /**
  * \brief  rotate araea
  *
  * rotates an area on datalayer
  *
  * \return	 Area_ returns a rotated dataLayer
  *
  */
dataLayer* dataLayer::rotatedArea(int alpha_)
{

	createArea();

	double alpha=(alpha_*0.017453);//angle in radian
	double newDimension;

	newDimension=  ((Area_->getheight())*
					(Area_->getheight())+
					(Area_->getwidth())*
					(Area_->getwidth()));

	newDimension=2*sqrt(newDimension);
	int newDim=ceil(newDimension);


	if ( rotArea_ ) delete rotArea_;
	rotArea_=new dataLayer(newDim,
									 newDim,
									 Area_->getcellSize());
	Area_->setSwitch(2);
	rotArea_->setSwitch(2);

	for (int i=0; i<Area_->getwidth(); i++)
	{
		for (int j=0; j<Area_->getheight();j++)
		{
			int newX=0;
			int newY=0;

			newX=ceil(sin(alpha)*j+cos(alpha)*i+0.5*newDimension);
			newY=ceil(-sin(alpha)*i+cos(alpha)*j+0.5*newDimension);

			rotArea_->setVal(newX,newY,Area_->getVal(i,j));
			rotArea_->setVal(newX-1,newY,Area_->getVal(i,j));
			rotArea_->setVal(newX-1,newY-1,Area_->getVal(i,j));
			rotArea_->setVal(newX,newY-1,Area_->getVal(i,j));

		}

	}

	//Area_=NULL;
	//Area_=rotArea;
	//delete rotArea;
	//delete Area;

	return rotArea_;


};


/** @brief loadData
  *
  * load data from file
  *
  * \param *in input text stream 
  */
void dataLayer::loadDataFromFile(QTextStream *in)
{
//    QTextStream in(file);
    QString line = in->readLine();
    if ( line != "Layer" ) std::cout << "Fehler beim lesen der Datei" << std::endl;
    width = in->readLine().toInt();
    height = in->readLine().toInt();
    mHeight = in->readLine().toInt();
    cellSize = in->readLine().toInt();
    layerswitch = in->readLine().toInt();
    for (int i = 0; i<width*height;i++)
        data[i] = in->readLine().toDouble();
    for (int i = 0; i<width*height;i++)
        bdata[i] = in->readLine().toDouble();

}

/** @brief save data
  *
  * save data to file
  *
  * \param *out ouput text stream 
  */
void dataLayer::saveDataToFile(QTextStream *out)
{
    //QTextStream out(file);
    *out << "Layer\n";
    *out << width << "\n";
    *out << height  << "\n";
    *out << mHeight  << "\n";
    *out << cellSize  << "\n";
    *out << layerswitch  << "\n";
    for (int i = 0; i<width*height;i++)
        *out << data[i] << "\n";
    for (int i = 0; i<width*height;i++)
        *out << bdata[i] << "\n";
}


};
