/*
 *    Copyright (C) 2010 by RoboLab - University of Extremadura
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
#include "specificmonitor.h"
/**
* \brief Default constructor
*/
SpecificMonitor::SpecificMonitor(GenericWorker *_worker,Ice::CommunicatorPtr _communicator):GenericMonitor(_worker, _communicator)
{
	ready = false;
}
/**
* \brief Default destructor
*/
SpecificMonitor::~SpecificMonitor()
{
	std::cout << "Destroying SpecificMonitor" << std::endl;
}

void SpecificMonitor::run()
{
	initialize();
	ready = true;
	forever
	{
		//rDebug("specific monitor run");
		this->sleep(period);
	}
}

/**
 * \brief Reads components parameters and checks set integrity before signaling the Worker thread to start running
 *   (1) Ice parameters
 *   (2) Local component parameters read at start
 *
 */
void SpecificMonitor::initialize()
{
	rInfo("Starting monitor ...");
	initialTime=QTime::currentTime();
	RoboCompCommonBehavior::ParameterList params;
	readPConfParams(params);
	readConfig(params);
	if(!sendParamsToWorker(params))
	{
		rError("Error reading config parameters. Exiting");
		killYourSelf();
	}
	state = RoboCompCommonBehavior::State::Running;
	emit initializeWorker(period);
}

bool SpecificMonitor::sendParamsToWorker(RoboCompCommonBehavior::ParameterList params)
{
	if(checkParams(params))
	{
		//Set params to worker
		if(worker->setParams(params)) 
			return true;
	}
	else
	{
		rError("Incorrect parameters");
	}
	return false;

}

///Local Component parameters read at start
///Reading parameters from config file or passed in command line, with Ice machinery
///We need to supply a list of accepted values to each call
void SpecificMonitor::readConfig(RoboCompCommonBehavior::ParameterList &params )
{
	RoboCompCommonBehavior::Parameter aux;
	aux.editable = true;
	//Helios Extrinsic 
	configGetString( "","helios_rx", aux.value, "");
	params["helios_rx"] = aux;

	configGetString( "","helios_ry", aux.value, "");
	params["helios_ry"] = aux;

	configGetString( "","helios_rz", aux.value, "");
	params["helios_rz"] = aux;

	configGetString( "","helios_tx", aux.value, "");
	params["helios_tx"] = aux;

	configGetString( "","helios_ty", aux.value, "");
	params["helios_ty"] = aux;

	configGetString( "","helios_tz", aux.value, "");
	params["helios_tz"] = aux;

	//Bpearl Extrinsic
	configGetString( "","bpearl_rx", aux.value, "");
	params["bpearl_rx"] = aux;

	configGetString( "","bpearl_ry", aux.value, "");
	params["bpearl_ry"] = aux;

	configGetString( "","bpearl_rz", aux.value, "");
	params["bpearl_rz"] = aux;

	configGetString( "","bpearl_tx", aux.value, "");
	params["bpearl_tx"] = aux;

	configGetString( "","bpearl_ty", aux.value, "");
	params["bpearl_ty"] = aux;

	configGetString( "","bpearl_tz", aux.value, "");
	params["bpearl_tz"] = aux;

	//boundin box colision / hitbox
	configGetString( "","center_box_x", aux.value, "");
	params["center_box_x"] = aux;

	configGetString( "","center_box_y", aux.value, "");
	params["center_box_y"] = aux;

	configGetString( "","center_box_z", aux.value, "");
	params["center_box_z"] = aux;

	configGetString( "","size_box_x", aux.value, "");
	params["size_box_x"] = aux;

	configGetString( "","size_box_y", aux.value, "");
	params["size_box_y"] = aux;

	configGetString( "","size_box_z", aux.value, "");
	params["size_box_z"] = aux;

	configGetString( "","floor_line", aux.value, "");
	params["floor_line"] = aux;

}

//Check parameters and transform them to worker structure
bool SpecificMonitor::checkParams(RoboCompCommonBehavior::ParameterList l)
{
	bool correct = true;
	return correct;
}

