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
 * \file      Utility_Thread.cpp
 * \brief     Source file for CPARAM_THREADPROC class.
 * \author    Ratnadip Choudhury
 * \copyright Copyright (c) 2011, Robert Bosch Engineering and Business Solutions. All rights reserved.
 *
 * Source file for CPARAM_THREADPROC class.
 */

#include "Utils_stdafx.h"
#include "Utility_Thread.h"

void CPARAM_THREADPROC::vInitialise(void)
{
    m_hActionEvent  = nullptr;
    m_hThread       = nullptr;
    m_pBuffer       = nullptr;
    m_unActionCode  = IDLE;
    m_unPrevActionCode = IDLE;
}

HANDLE CPARAM_THREADPROC::hGetExitNotifyEvent(void)
{
    return m_hNotifyExit;
}

BOOL CPARAM_THREADPROC::bStartThreadEx(LPTHREAD_START_ROUTINE NewThreadProc,
                                       HANDLE hActionEvent, LPVOID pvBuffer)
{
    // First check if the thread exists
    if (nullptr != m_hThread) {
        if (bTerminateThread() == FALSE) {
            return FALSE;
        }
    }

    m_hActionEvent = hActionEvent;
    m_pBuffer = pvBuffer;
    return bStartThread(NewThreadProc);
}

BOOL CPARAM_THREADPROC::bStartThread(LPTHREAD_START_ROUTINE NewThreadProc)
{
    BOOL bResult = FALSE;

    // First check if the thread exists
    if (nullptr == m_hThread) {
        DWORD dwThreadID = 0x0;
        m_hThread = CreateThread(nullptr, 0, NewThreadProc, this, 0, &dwThreadID);
        Sleep(0);
    }

    if (m_hThread != nullptr) {
        bResult = TRUE;
    }
    return bResult;
}

BOOL CPARAM_THREADPROC::bTerminateThread(void)
{
    BOOL bResult = FALSE;

    if ((m_hActionEvent != nullptr) && (m_hThread != nullptr)) {
        m_unActionCode = EXIT_THREAD;
        SetEvent(m_hActionEvent);
        Sleep(0);

        switch (WaitForSingleObject(m_hNotifyExit, THREAD_WAIT_PERIOD)) {
            case WAIT_OBJECT_0:
            {
                bResult = TRUE;
            }
            break;
            case WAIT_TIMEOUT:
            case WAIT_FAILED:
            {
                bResult = TerminateThread(m_hThread, EXIT_CODE_ABNORMAL);
            }
            break;
        }
    }

    if (bResult) {
        vInitialise();
    }

    return bResult;
}

BOOL CPARAM_THREADPROC::bTransitToInaction(void)
{
    BOOL bResult = FALSE;

    /* The intention is to bring the thread into a state of inaction. One way
    of achieving this is to signal the thread to wait for an indefinite period
    of time until the owner decides the retransition of the same. */
    if ((nullptr != m_hThread2Owner) && (nullptr != m_hOwner2Thread)
            && (nullptr != m_hThread)) {
        m_unPrevActionCode = m_unActionCode;
        m_unActionCode = INACTION; // New instruction
        SetEvent(m_hActionEvent);  // The thread should get released from wait
        Sleep(0);
        WaitForSingleObject(m_hThread2Owner, INFINITE); /* Now wait until the
        thread signals that it is going to handle 'INACTION' switch case. */
        bResult = TRUE;
    }

    return bResult;
}
BOOL CPARAM_THREADPROC::bForceTerminateThread()
{
    BOOL bResult = FALSE;
    if (nullptr != m_hThread) {
        TerminateThread(m_hThread, EXIT_CODE_ABNORMAL);
    }
    if (bResult) {
        vInitialise();
    }
    return bResult;
}

BOOL CPARAM_THREADPROC::bTransitToActiveState(void)
{
    BOOL bResult = FALSE;

    /* The intention is to bring the thread back into the state of action. At
    present the thread is waiting for the event m_hOwner2Thread to get
    signalled. */
    if ((nullptr != m_hThread2Owner) && (nullptr != m_hOwner2Thread)
            && (nullptr != m_hThread)) {
        m_unActionCode = m_unPrevActionCode; /* Resume whatever the thread
        was doing prior to transiting into inaction. */
        SetEvent(m_hOwner2Thread);  // The thread should get released from wait
        Sleep(0);
        WaitForSingleObject(m_hThread2Owner, INFINITE); /* Now wait until the
        thread signals that it is going to come out of 'INACTION' switch case.*/
        bResult = TRUE;
    }

    return bResult;
}
