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

/**
 * \file      NodeSimEx_Extern.h
 * \brief     Declaration file for the exported function used to get the
 * \author    Pradeep Kadoor
 * \copyright Copyright (c) 2011, Robert Bosch Engineering and Business Solutions. All rights reserved.
 *
 * Declaration file for the exported function used to get the
 */

#ifndef BUSMASTER_NODESIMEX_NODESIMEX_EXTERN_H_
#define BUSMASTER_NODESIMEX_NODESIMEX_EXTERN_H_

#if defined USAGEMODE
#undef USAGEMODE
#endif

#if defined USAGE_EXPORT
#define USAGEMODE   __declspec(dllexport)
#else
#define USAGEMODE   __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {  // only need to export C interface if used by C++ source code
#endif

    /*  Exported function list */
    USAGEMODE HRESULT NS_GetInterface(ETYPE_BUS eBus, void** ppvInterface);

#ifdef __cplusplus
}
#endif

#endif // BUSMASTER_NODESIMEX_NODESIMEX_EXTERN_H_