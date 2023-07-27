/*
 *    Copyright (C) 2023 by YOUR NAME HERE
 *
 *    This file is part of RoboComp
 *
 *    RoboComp is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    RoboComp is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "specificworker.h"

/**
* \brief Default constructor
*/
SpecificWorker::SpecificWorker(TuplePrx tprx, bool startup_check) : GenericWorker(tprx)
{
	this->startup_check_flag = startup_check;
	// Uncomment if there's too many debug messages
	// but it removes the possibility to see the messages
	// shown in the console with qDebug()
//	QLoggingCategory::setFilterRules("*.debug=false\n");
}

/**
* \brief Default destructor
*/
SpecificWorker::~SpecificWorker()
{
	std::cout << "Destroying SpecificWorker" << std::endl;

    if(robot)
        delete robot;
}

bool SpecificWorker::setParams(RoboCompCommonBehavior::ParameterList params)
{
//	THE FOLLOWING IS JUST AN EXAMPLE
//	To use innerModelPath parameter you should uncomment specificmonitor.cpp readConfig method content
//	try
//	{
//		RoboCompCommonBehavior::Parameter par = params.at("InnerModelPath");
//		std::string innermodel_path = par.value;
//		innerModel = std::make_shared(innermodel_path);
//	}
//	catch(const std::exception &e) { qFatal("Error reading config params"); }

	return true;
}

void SpecificWorker::initialize(int period)
{
	std::cout << "Initialize worker" << std::endl;
	this->Period = period;
	if(this->startup_check_flag)
	{
		this->startup_check();
	}
	else
	{
		timer.start(Period);
	}

	robot = new webots::Robot();

    lidar = robot->getLidar("lidar");
    lidar->enable(64);

    robot->step(100);
}

void SpecificWorker::compute()
{
    receiving_lidarData(lidar);

    robot->step(100);
}

int SpecificWorker::startup_check()
{
	std::cout << "Startup check" << std::endl;
	QTimer::singleShot(200, qApp, SLOT(quit()));
	return 0;
}

void SpecificWorker::receiving_lidarData(webots::Lidar* _lidar){
    if (!_lidar) { std::cout << "No lidar available." << std::endl; return; }

    const float *rangeImage = _lidar->getRangeImage();
    int horizontalResolution = _lidar->getHorizontalResolution();
    int verticalResolution = _lidar->getNumberOfLayers();
    double minRange = _lidar->getMinRange();
    double maxRange = _lidar->getMaxRange();
    double fov = _lidar->getFov();
    double verticalFov = _lidar->getVerticalFov();

    RoboCompLaser::TLaserData newLaserData;
    RoboCompLaser::LaserConfData newLaserConfData;
    RoboCompLidar3D::TData newLidar3dData;

    newLaserConfData.maxDegrees = fov;
    newLaserConfData.maxRange = maxRange;
    newLaserConfData.minRange = minRange;
    newLaserConfData.angleRes = fov / horizontalResolution;

    //std::cout << "horizontal resolution: " << horizontalResolution << " vertical resolution: " << verticalResolution << " fov: " << fov << " vertical fov: " << verticalFov << std::endl;

    if(!rangeImage) { std::cout << "Lidar data empty." << std::endl; return; }


    for (int j = 0; j < verticalResolution; ++j) {
        for (int i = 0; i < horizontalResolution; ++i) {
            int index = j * horizontalResolution + i;

            const float distance = rangeImage[index];

            float horizontalAngle = i * newLaserConfData.angleRes - fov / 2;
            float verticalAngle = j * (verticalFov / verticalResolution) - verticalFov / 2;

            RoboCompLaser::TData data;
            data.angle = horizontalAngle;
            data.dist = distance;

            RoboCompLidar3D::TPoint point;
            point.x = distance * cos(horizontalAngle) * cos(verticalAngle);
            point.y = distance * sin(horizontalAngle) * cos(verticalAngle);
            point.z = distance * sin(verticalAngle);


            //std::cout << "X: " << point.x << " Y: " << point.y << " Z: " << point.z << std::endl;

            newLidar3dData.points.push_back(point);
            newLaserData.push_back(data);
        }
    }

    laserData = newLaserData;
    laserDataConf = newLaserConfData;
    lidar3dData = newLidar3dData;
}

RoboCompCameraRGBDSimple::TRGBD SpecificWorker::CameraRGBDSimple_getAll(std::string camera)
{
//implementCODE

}

RoboCompCameraRGBDSimple::TDepth SpecificWorker::CameraRGBDSimple_getDepth(std::string camera)
{
//implementCODE

}

RoboCompCameraRGBDSimple::TImage SpecificWorker::CameraRGBDSimple_getImage(std::string camera)
{
//implementCODE

}

RoboCompCameraRGBDSimple::TPoints SpecificWorker::CameraRGBDSimple_getPoints(std::string camera)
{
//implementCODE

}


RoboCompLaser::TLaserData SpecificWorker::Laser_getLaserAndBStateData(RoboCompGenericBase::TBaseState &bState)
{
    return laserData;
}

RoboCompLaser::LaserConfData SpecificWorker::Laser_getLaserConfData()
{
    return laserDataConf;
}

RoboCompLaser::TLaserData SpecificWorker::Laser_getLaserData()
{
    return laserData;
}

RoboCompLidar3D::TData SpecificWorker::Lidar3D_getLidarData(std::string name, int start, int len, int decimationfactor)
{
    RoboCompLidar3D::TData filteredData;

    double startRadians = start * M_PI / 180.0; // Convert to radians
    double lenRadians = len * M_PI / 180.0; // Convert to radians
    double endRadians = startRadians + lenRadians; // Precompute end angle

    int counter = 0;
    int decimationCounter = 0; // Use a separate counter for decimation

    // Reserve space for points. If decimation is 1, at max we could have same number of points
    if (decimationfactor == 1)
    {
        filteredData.points.reserve(lidar3dData.points.size());
    }

    for (int i = 0; i < lidar3dData.points.size(); i++)
    {
        double angle = atan2(lidar3dData.points[i].y, lidar3dData.points[i].x) + M_PI; // Calculate angle in radians
        if (angle >= startRadians && angle <= endRadians)
        {
            if (decimationCounter == 0) // Add decimation factor
            {
                filteredData.points.push_back(lidar3dData.points[i]);
            }
            counter++;
            decimationCounter++;
            if (decimationCounter == decimationfactor)
            {
                decimationCounter = 0; // Reset decimation counter
            }
        }
    }

    return filteredData;
}



/**************************************/
// From the RoboCompLaser you can use this types:
// RoboCompLaser::LaserConfData
// RoboCompLaser::TData

/**************************************/
// From the RoboCompLidar3D you can use this types:
// RoboCompLidar3D::TPoint

