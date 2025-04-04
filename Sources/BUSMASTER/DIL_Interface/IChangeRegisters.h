/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BUSMASTER_DIL_INTERFACE_DIL_ICHANGEREGISTERS_H_
#define BUSMASTER_DIL_INTERFACE_DIL_ICHANGEREGISTERS_H_

#include"../../Kernel/BusmasterDriverInterface/Include/BaseDIL_CAN.h"

class IChangeRegisters
{
public:
	virtual int InvokeAdavancedSettings(PSCONTROLLER_DETAILS pControllerDetails, UINT nCount, UINT ) = 0;
	virtual double vValidateBaudRate(double ,int ,UINT =16) = 0;
	
};

#endif // BUSMASTER_DIL_INTERFACE_DIL_ICHANGEREGISTERS_H_