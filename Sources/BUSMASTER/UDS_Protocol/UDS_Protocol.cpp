/**
 * \file      UDS_Protocol.cpp
 * \brief     Definition file for CUDS_Protocol class
 * \author    Sanchez Marin Maria Alejandra
 *  Manager of the UDS_Protocol Porject
 */

// clang-format off
#include "stdafx.h"
// clang-format on

#define USAGE_EXPORT
#include "UDS_Protocol.h"

#include "CANDriverDefines.h"
#include "UDSMainWnd.h"
#include "UDSSettingsWnd.h"
#include "UDS_Extern.h"
#include "UDS_NegRespMng.h"

CUDSMainWnd* omMainWnd = NULL;
CUDS_NegRespMng* NegRespManager = NULL;
CUDSSettingsWnd* omSettingsWnd = NULL;
CUDS_Protocol* omManagerPtr = NULL;

/** Variable used to control if a message received should be shown */
bool FDontShow = FALSE;  // if it's TRUE the message should not be shown
// This variable is used to check if the response corresponds to my request

/** It's the number of bytes received in a long response  */
float Length_Received = 0;

/**  It contains the number of Data bytes that can be sent in the current
 * message */
int numberOfTaken;

int initialByte;
/** It's used to control the index i according to if it's working on extended or
 * normal addressing  */
int aux_Finterface;
unsigned char abByteArr[64];

/** Corresponds to the STMin received from the flowControlMessage */
int STMin;
// BSize depends of the value put in the settingsWnd.
int BSize;

//; Corresponds to the BSize received from the flowControlMessage
int BSizE;
int SizeFC;
// Time to wait to able the send button again
int P2_Time = 250;

int P2_Time_Extended = 2000;

int Current_Channel;

/** This variable is used to know if a message has been sent from the UDSMainWnd
 */
bool FSending = FALSE;

CString Bytes_to_Show = ("\r\n   1-> ");

/** This variable is used to determinate how many continuos frames will be
 * received */
float TotalFrames;

/** it corresponds to the exact ID of the message received    */
int respID;

/** It Corresponds to the value put in the settingWnd if it's extended   */
int RespMsgID;

/** If TRUE the msg should have 8 bytes always  */
bool fMsgSize = FALSE;

/** If TRUE the msg should have 8 bytes always   */
bool FCRespReq = FALSE;

int BytesShown_Line = 1;
int LengthLastFrame;
CString CurrentService;
UINT_PTR m_nTimer;

BEGIN_MESSAGE_MAP(CUDS_Protocol, CWinApp)
END_MESSAGE_MAP()

/**
 * CUDS_Protocol Construction
 */
CUDS_Protocol::CUDS_Protocol() {
  omManagerPtr = this;
  SourceAddress = 0;
  TargetAddress = 0;
  fInterface = INTERFACE_NORMAL_11;
  MsgID = 0x700;
  numberOfBytes = 1;
  Data_Recibida = "";
  Current_Channel = 1;
}

// The one and only CUDS_Protocol object
CUDS_Protocol theApp;

/**
 * CUDS_Protocol initialization
 */

static HINSTANCE ghLangInst = NULL;

BOOL CUDS_Protocol::InitInstance() {
  CWinApp::InitInstance();
  return TRUE;
}

/**********************************************************************************************************
 Function Name  :   DIL_UDS_ShowWnd

 Description    :   It's called from the MainFrm to show the MainWnd
 Member of      :   CUDS_Protocol

 Author(s)      :   Sanchez Marin Maria Alejandra
 Date Created   :   01.03.2013
**********************************************************************************************************/
USAGEMODE HRESULT DIL_UDS_ShowWnd(HWND hParent, int TotalChannels) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());
  INT_PTR nRet = -1;
  CWnd objParent;
  objParent.Attach(hParent);

  if (omMainWnd == NULL) {
    omMainWnd = new CUDSMainWnd(
        omManagerPtr->SourceAddress, omManagerPtr->TargetAddress,
        omManagerPtr->fInterface, omManagerPtr->MsgID, &objParent);
    omMainWnd->TotalChannel = TotalChannels;
    omMainWnd->Create(IDM_UDS);
    NegRespManager = new CUDS_NegRespMng(omManagerPtr);
  }

  omMainWnd->ShowWindow(SW_SHOW);

  objParent.Detach();
  return 0;
}

/**********************************************************************************************************
 Function Name  :   UpdateChannelUDS

 Description    :   Called from MainFrm to update the channel count
 Member of      :   CUDS_Protocol

 Author(s)      :   Sanchez Marin Maria Alejandra
 Date Created   :   01.06.2013
**********************************************************************************************************/
USAGEMODE HRESULT UpdateChannelUDS(HWND hParent) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  if (omMainWnd != NULL) {
    omMainWnd->vUpdateChannelIDInfo();
  }

  return 0;
}

/**********************************************************************************************************
 Function Name  :   Show_ResponseData

 Input(s)       :   psMsg[] bytes to be shown,
                    Datalen is the position inside of psMsg[]
                    Contains the last byte to shown,
                    posFirstByte is the location inside of psMsg[]
                    where this function should start to read.
 Output         :   -
 Description    :   Call by EvaluateMessage to show in the Response Data section
                    the bytes received in the bus as response of a request sent
                    from the UDS tool.
 Member of      :   CUDS_Protocol

 Author(s)      :   Sanchez Marin Maria Alejandra
 Date Created   :   19.06.2013
/**********************************************************************************************************/

void CUDS_Protocol::Show_ResponseData(unsigned char psMsg[],
                                      unsigned char Datalen, int posFirstByte) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());
  int loc = Datalen;
  char hex[9];
  if (omMainWnd != NULL) {
    int intResp;
    for (int ByteCounter = posFirstByte + 1; ByteCounter < loc; ByteCounter++) {
      UCHAR TempByte = (psMsg[ByteCounter]);
      int Temp_char2Int = TempByte;
      intResp = sprintf(hex, "%02X ", Temp_char2Int);
      Bytes_to_Show = Bytes_to_Show + hex;
      BytesShown_Line++;
      numberOfBytes++;
      Data_Recibida = Data_Recibida + hex;

      // It enters here when it has been shown a complete line with
      // NUM_BYTES_SHOWN_RESP+1
      if (BytesShown_Line == NUM_BYTES_SHOWN_RESP + 1) {
        CString CstringNumberOfBytes;  // elements  in the response data section
        CstringNumberOfBytes.Format("%d", numberOfBytes);
        int i = 4;  // Prepares the number of Bytes to have all the data aligned
                    // in the Response
        // data section del Main Window
        while (i - CstringNumberOfBytes.GetLength() > 0) {
          CstringNumberOfBytes = " " + CstringNumberOfBytes;
        }
        BytesShown_Line = 1;
        Bytes_to_Show =
            Bytes_to_Show + ("\r\n") + CstringNumberOfBytes + " -> ";
      }
    }
    omMainWnd->m_abDatas = Bytes_to_Show + '\0';

    omMainWnd->UpdateData(false);
  }
}

/**********************************************************************************************************
 Function Name  :   EvaluateMessage

 Description    :   Evaluate any message received from the ECU -Filter the
message received and call initialize it the variables to show it Member of :

 Author(s)      :   Sanchez Marin Maria Alejandra

 Date Created   :   19.06.2013
**********************************************************************************************************/

USAGEMODE HRESULT EvaluateMessage(STCAN_MSG Mensaje) {
  unsigned char psMsg[64];
  memcpy(psMsg, Mensaje.m_ucData, 64);
  unsigned char Datalen = Mensaje.m_ucDataLen;
  UINT Check_SourceAddress;
  UINT Check_TargetAddress;
  UINT MessageID = Mensaje.m_unMsgID;
  UINT Channel = Mensaje.m_ucChannel;

  UINT Check_MSG_received;

  // -------for checking also if the SA and TA are inserted in Main window and
  // settings window holds only the base address (0x18DA0000)- Variable used is
  // "Check_MSG_received" - Casian
  if (omMainWnd != NULL) {
    Check_SourceAddress = omMainWnd->m_omSourceAddress.lGetValue() << 8;
    Check_TargetAddress = omMainWnd->m_omTargetAddress.lGetValue();

    Check_MSG_received = respID + (Check_TargetAddress) + Check_SourceAddress;
  }
  // Datalen is the DLC of the message received
  AFX_MANAGE_STATE(AfxGetStaticModuleState());
  // -------  // Evaluates the received message
  // when: 1.MainWnd created - 2. Response to
  // my Msg - 3.I've sent something
  if (omMainWnd != NULL && FSending &&
      (respID == MessageID || Check_MSG_received == MessageID) &&
      Current_Channel == Channel) {
    // This is a general function because it uses the
    // variable initialByte
    TYPE_OF_FRAME TypeofFrame = omManagerPtr->getFrameType(psMsg[initialByte]);

    // Am I waiting for a flow control?
    if (FWaitFlow == TRUE && TypeofFrame == FLOW_CONTROL) {
      switch (omManagerPtr->TypeofControl) {
        case 0: {
          BSizE = psMsg[initialByte + 1];
          // Evaluate all the possible Values for STmin
          if (psMsg[initialByte + 2] <= 0x7F) {
            STMin = psMsg[initialByte + 2];
          } else if ((0xF1 <= psMsg[initialByte + 2]) &&
                     (psMsg[initialByte + 2] <= 0xF9)) {
            STMin = (psMsg[initialByte + 2] & 0x0F) * 0.1;
          } else {
            STMin = 127;  // According to the ISO-TP
          }
          FWaitFlow = FALSE;  // Don't wait for the flowcontrol anymore

          omMainWnd->SendContinuosFrames(
              omMainWnd->psTxCanMsgUds->m_psTxMsg->m_ucData,
              omMainWnd->psTxCanMsgUds, omMainWnd->fInterface);

        } break;
        case 1: {
          // Wait,ReStart Timer -> do not able the
          // SEND button yet
          omMainWnd->KillTimer(ID_TIMER_SEND_BUTTON);
          omMainWnd->SetTimer(ID_TIMER_SEND_BUTTON, P2_Time, NULL);
        } break;
        default: {
          // If I've received any reserved message or  an Overflow
          // message.
          FWaitFlow = FALSE;  // To indicate that I should not receive a new
                              // flow control
        }                     // If it's -1 then it's not a flow control message
                              // do nothing
      }

    } else if (TypeofFrame == LONG_RESPONSE)  // Is it a long response??
    {
      int TempValue = strtol(CurrentService, NULL, 16);
      // Evaluate it only if it corresponds to my request
      if (psMsg[initialByte + 2] == TempValue + 0x40) {
        // The length of the receivedMessage
        omManagerPtr->numberOfBytes = 1;
        Length_Received = Length_Received + psMsg[initialByte + 1];
        omMainWnd->PrepareFlowControl();
        omManagerPtr->Show_ResponseData(psMsg, Datalen, initialByte + 1);
        // This variable is used to determinate how much continuos frames will
        // be received
        TotalFrames =
            (Length_Received - 6 + aux_Finterface) / (7 - aux_Finterface);

        LengthLastFrame =
            ((int)Length_Received - 6 + aux_Finterface) % (7 - aux_Finterface);
        omMainWnd->m_omDiagService =
            NegRespManager->evaluateResp(psMsg, initialByte + 1);
        omMainWnd->m_omSendButton.EnableWindow(FALSE);
        /*TIME_UNABLE_SEND_BUTTON*/
        m_nTimer = omMainWnd->SetTimer(ID_TIMER_SEND_BUTTON, P2_Time, NULL);
      }

    } else if (TypeofFrame == CONSECUTIVE_FRAME) {
      // Is it a consecutive Frame?
      Counter_BSize--;
      TotalFrames--;
      omMainWnd->KillTimer(ID_TIMER_SEND_BUTTON);
      omMainWnd->SetTimer(ID_TIMER_SEND_BUTTON, P2_Time, NULL);  // ReStart
                                                                 // Timer
      // It enters here when the last frame has exactly 7/6 bytes of data
      if (TotalFrames == 0) {
        FSending = FALSE;  // Stops showing the bytes in the mainWnd
        omMainWnd->m_omSendButton.EnableWindow(TRUE);
        omMainWnd->KillTimer(ID_TIMER_SEND_BUTTON);

      } else if (TotalFrames < 0) {
        // It enters here when the last frame has less than 7/6 bytes of data
        Datalen = LengthLastFrame + 1 + aux_Finterface;
        FSending = FALSE;  // Stops showing the bytes in the mainWnd
        omMainWnd->KillTimer(ID_TIMER_SEND_BUTTON);
        omMainWnd->m_omSendButton.EnableWindow(TRUE);
      }
      // now show the response
      omManagerPtr->Show_ResponseData(psMsg, Datalen, initialByte);
      if (Counter_BSize == 0) {
        omMainWnd->PrepareFlowControl();  // If I've received BSize frames->
                                          // Send the flow control
      }

    } else {
      // This case corresponds to a simple response
      omManagerPtr->numberOfBytes = 1;
      FSending = FALSE;  // Show this message and then Stop showing the bytes
      // Show the description of the Response
      omMainWnd->m_omDiagService =
          NegRespManager->evaluateResp(psMsg, initialByte);
      if (!FDontShow) {
        // If it's a simple response the legth will be in psMsg[initialByte]
        Length_Received,
            omManagerPtr->Show_ResponseData(
                psMsg, psMsg[initialByte] + initialByte + 1, initialByte);
      }
      // The nextFramme can be shown
      FDontShow = FALSE;
    }
    omMainWnd->m_omBytes.vSetValue(Length_Received);
  }
  return 0;
}

/**********************************************************************************************************
 Function Name  :   getFrameType

 Description    :   It returns the type of frame that was received
 Member of      :   CUDS_Protocol

 Author(s)      :   Sanchez Marin Maria Alejandra
 Date Created   :   19.06.2013
**********************************************************************************************************/
TYPE_OF_FRAME CUDS_Protocol::getFrameType(BYTE FirstByte) {
  BYTE Mask_FB = FirstByte & 0xF0;
  int TempByte = FirstByte + (0x30);

  // it supports all cases of Flow Control
  if (0x60 <= TempByte && TempByte < 0x62) {
    TypeofControl = FirstByte & 0x0F;
    return FLOW_CONTROL;
  } else if (Mask_FB == 0x10) {
    Length_Received = (FirstByte & 0x0F) << 8;  // long response
    return LONG_RESPONSE;
  } else if (Mask_FB == 0x20) {
    return CONSECUTIVE_FRAME;  // If it's a consecutive frame the
                               // Length_Received doesn't change.
  }
  Length_Received = FirstByte;
  return SIMPLE_RESPONSE;
}

/**********************************************************************************************************
 Function Name  :   DIL_UDS_ShowSettingWnd

 Description    :   It's called from the MainFrm to show the settings Wnd
 Member of      :   CUDS_Protocol

 Author(s)      :   Sanchez Marin Maria Alejandra
 Date Created   :   01.03.2013
**********************************************************************************************************/
USAGEMODE HRESULT DIL_UDS_ShowSettingWnd(HWND hParent) {
  AFX_MANAGE_STATE(AfxGetStaticModuleState());
  INT_PTR nRet = -1;
  CWnd objParent;
  objParent.Attach(hParent);

  if (omSettingsWnd == NULL) {
    omSettingsWnd = new CUDSSettingsWnd(&objParent, omManagerPtr);
    omSettingsWnd->Create(IDD_SETTINGS_UDS);
  }
  omSettingsWnd->ShowWindow(SW_SHOW);
  objParent.Detach();
  return 0;
}

USAGEMODE HRESULT TX_vSetDILInterfacePtrUDS(void* ptrDILIntrf) {
  CUDSMainWnd::s_podGetUdsMsgManager()->vSetDILInterfacePtr(ptrDILIntrf);
  return S_OK;
}

/**********************************************************************************************************
 Function Name  :   UpdateFields
 Description    :   Call from the SettingsWnd to save values that are set
                    in settings window. Also set values in the MainWnd such as
                    SA, TA,CANId
 Member of      :   CUDS_Protocol
 Author(s)      :   Sanchez Marin Maria Alejandra
 Date Created   :   01.03.2013
**********************************************************************************************************/
void CUDS_Protocol::UpdateFields() {
  if (omMainWnd != NULL) {
    omMainWnd->fInterface = fInterface;
    switch (fInterface) {
      case INTERFACE_NORMAL_11: {
        omMainWnd->CanID = MsgID;
        omMainWnd->m_omTargetAddress.vSetValue(TargetAddress);
        omMainWnd->m_omSourceAddress.vSetValue(SourceAddress);
        omMainWnd->m_omSourceAddress.SetReadOnly(TRUE);
        omMainWnd->m_omTargetAddress.SetReadOnly(TRUE);
      } break;
      case INTERFACE_EXTENDED_11: {
        omMainWnd->CanID = MsgID;
        omMainWnd->m_omSourceAddress.SetReadOnly(FALSE);
        omMainWnd->m_omTargetAddress.SetReadOnly(FALSE);
        omMainWnd->m_omSourceAddress.SetLimitText(2);
        omMainWnd->m_omTargetAddress.SetLimitText(2);
        if (MsgID == 0) {
          omMainWnd->m_omSourceAddress.vSetValue(0);
          omMainWnd->m_omTargetAddress.vSetValue(0);
        }
        SourceAddress = omMainWnd->m_omSourceAddress.lGetValue();
        TargetAddress = omMainWnd->m_omTargetAddress.lGetValue();

        // This is necessary to set the proper CAN
        // ID when the user has recently select the
        int MsgID2 = SourceAddress + MsgID;
        // the option Extended addressing in the Interface Combo
        // Box.
        omMainWnd->m_omCanID.vSetValue(MsgID2);
        respID = TargetAddress + RespMsgID;
      } break;
      case INTERFACE_NORMAL_ISO_29: {
        omMainWnd->CanID = MsgID;
        omMainWnd->m_omSourceAddress.SetLimitText(3);
        omMainWnd->m_omTargetAddress.SetLimitText(3);
        omMainWnd->m_omSourceAddress.vSetValue(SourceAddress);
        omMainWnd->m_omTargetAddress.vSetValue(TargetAddress);
        omMainWnd->m_omSourceAddress.SetReadOnly(false);
        omMainWnd->m_omTargetAddress.SetReadOnly(false);
      } break;
      case INTERFACE_NORMAL_J1939_29: {
        omMainWnd->CanID = MsgID;
        omMainWnd->m_omSourceAddress.SetLimitText(2);
        omMainWnd->m_omTargetAddress.SetLimitText(2);
        omMainWnd->m_omSourceAddress.vSetValue(SourceAddress);
        omMainWnd->m_omTargetAddress.vSetValue(TargetAddress);
        omMainWnd->m_omSourceAddress.SetReadOnly(false);
        omMainWnd->m_omTargetAddress.SetReadOnly(false);
      } break;
    }
  }
}

/**********************************************************************************************************
 Function Name  :   StartTimer_Disable_Send
 Description    :   Call from CUDS_NegRespMng to disable the send button
                    and start again the default timer
 Member of      :   CUDS_Protocol
 Author(s)      :   Sanchez Marin Maria Alejandra
 Date Created   :   11.06.2013
**********************************************************************************************************/
void CUDS_Protocol::StartTimer_Disable_Send() {
  omMainWnd->m_omSendButton.EnableWindow(FALSE);
  // Time to wait to able the send button again
  omMainWnd->SetTimer(ID_TIMER_SEND_BUTTON, P2_Time, NULL);
}

/**********************************************************************************************************
 Function Name  :   StartTimer_Disable_Send
 Description    :   Call from the CUDS_NegRespMng to able send bt
                    kill the timer Member
 CUDS_Protocol Author(s) : Sanchez Marin Maria Alejandra
 Date Created            :   11.06.2013
**********************************************************************************************************/
void CUDS_Protocol::KillTimer_Able_Send() {
  // restart the timer that Enables the SendButton
  omMainWnd->KillTimer(ID_TIMER_SEND_BUTTON);
  omMainWnd->m_omSendButton.EnableWindow(TRUE);
}