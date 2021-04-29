/*
  ������� ����������� ������������ �����-�
*/
#include <includes.h>
#include "uart0.h"
#include "fiscal.h"
#include "fr.h"

CPU_INT08U  fisc_buf[256];


int FiscPollExt(void)
{
  CPU_INT08U ack;
  int byte;
  FiscUartSendByte(FISC_ENQ);
  byte = FiscUartRecieveByte(&ack, FISC_TIMEOUT);
  if (!byte) return FISC_UNDEF;;
  if (ack == FISC_NACK) return FISC_READY;
  else if (ack == FISC_ACK) return FISC_BUSY;
  return FISC_UNDEF;
}


// �������� ������� ��
int FiscPoll(void)
{
  CPU_INT08U ack;
  FiscUartSendByte(FISC_ENQ);
  if (!FiscUartRecieveByte(&ack, FISC_TIMEOUT)) return FISC_UNDEF;;
  if (ack == FISC_NACK) return FISC_READY;
  else if (ack == FISC_ACK) return FISC_BUSY;
  return FISC_UNDEF;
}

// �������� �������
int FiscSendCommand(CPU_INT08U cmd, CPU_INT08U *dat, CPU_INT08U len)
{
  CPU_INT08U lrc,ack;
  
  FiscPurgeRx();
    
  FiscUartSendByte(FISC_STX);
  FiscUartSendByte(len+1);
  
  lrc = len+1;
  FiscUartSendByte(cmd);
  lrc ^= cmd;
  while(len--) 
    {
      FiscUartSendByte(*dat);
      lrc ^= *dat++;
    }
  FiscUartSendByte(lrc);
  
  if (!FiscUartRecieveByte(&ack, FISC_TIMEOUT)) return FISC_ERR;
  
  if (ack == FISC_NACK)
    { // ���� 5 ������, ��������
      for (int i=0; i<100; i++) 
        {
          FiscSleep(FISC_TIMEOUT);
          int poll = FiscPoll();
          if (poll == FISC_READY) return FISC_OK;
          else if (poll == FISC_BUSY) continue;
          return FISC_ERR;
        }
    }
  else if (ack == FISC_ACK) return FISC_OK;
  
  return FISC_ERR;
}


// ����� ������
// ���������� ���������, ��������� �� ������ � ����� ���� ������
int FiscReceiveAnswer(CPU_INT08U **dat, CPU_INT08U *len, CPU_INT32U timeout)
{
  CPU_INT08U byte, lrc, num;

  *dat = NULL;
  *len = 0;

  // ��������� ����
  if (!FiscUartRecieveByte(&byte, timeout)) return FISC_ERR;
  if (byte != FISC_STX) return FISC_ERR;
  
  // ����� ���������
  if (!FiscUartRecieveByte(&byte, FISC_TIMEOUT)) return FISC_ERR;
  if (!byte) return FISC_ERR;
  num = byte;
  
  // ������ ��������� ����� ���� = byte
  if (!FiscUartRecieve(fisc_buf, num, FISC_BYTE_TIMEOUT)) return FISC_ERR;

  // ��������� ����������� �����
  if (!FiscUartRecieveByte(&byte, FISC_BYTE_TIMEOUT)) return FISC_ERR;

  // ������� ����������� �����
  lrc = num;
  for (CPU_INT08U i=0; i<num; i++) lrc ^= fisc_buf[i];
  
  // ��������� ����������� �����
  if (lrc != byte) return FISC_ERR;
    
  // �� ��
  FiscUartSendByte(FISC_ACK);
  *dat = &fisc_buf[0];
  *len = num;
  
  return FISC_OK;
}

// ������ ���������� ������ ��
// !!! � ����� ������ �� �� �������������� !!!
int FiscGetFactoryNumber(CPU_INT32U pass, TFiscFactoryNumber* fnum, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  if (FiscSendCommand(FISC_GET_FACTORY_NUMBER, (CPU_INT08U*)&pass, 4) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if ((sizeof(TFiscFactoryNumber)+2) != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_GET_FACTORY_NUMBER) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  memcpy(fnum, &rxdat[2], sizeof(TFiscFactoryNumber));

  return FISC_OK;
}

// �������� ������ ��������� ��
int FiscGetShortStatus(CPU_INT32U pass, TFiscShortStatus* stat, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  *err = 0;
    
  if (FiscSendCommand(FISC_GET_SHORT_STATUS, (CPU_INT08U*)&pass, 4) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if ((sizeof(TFiscShortStatus)+2) != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_GET_SHORT_STATUS) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  memcpy(stat, &rxdat[2], sizeof(TFiscShortStatus));
  
  return FISC_OK;
}

// ������ ������ ��������� ��
int FiscGetFullStatus(CPU_INT32U pass, TFiscFullStatus* stat, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  *err = 0;
    
  if (FiscSendCommand(FISC_GET_FULL_STATUS, (CPU_INT08U*)&pass, 4) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if ((sizeof(TFiscFullStatus)+2) != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_GET_FULL_STATUS) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  memcpy(stat, &rxdat[2], sizeof(TFiscFullStatus));
  
  return FISC_OK;
}

// �������� 20 �������� �������������
// flags: ��� 0 - ������ �� ����������� �����, ��� 1 - ������ �� ������� �����
int FiscPrintBoldString(CPU_INT32U pass, CPU_INT08U flags, CPU_INT08U* str, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  fisc_buf[4] = flags;
  memset(&fisc_buf[5], 0, 20);
  len = strlen((char const*)str);
  if (len > 20) len = 20;
  memcpy(&fisc_buf[5], str, len);
  
  if (FiscSendCommand(FISC_PRINT_BOLD_STRING, fisc_buf, 25) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (len != 3) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_PRINT_BOLD_STRING) || (rxdat[1] != 0)) {return FISC_ERR;}
    
  return FISC_OK;
}

// �����
int FiscBeep(CPU_INT32U pass, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;
  
  if (FiscSendCommand(FISC_BEEP, (CPU_INT08U*)&pass, 4) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
  
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_BEEP) || (rxdat[1] != 0)) {return FISC_ERR;}

  return FISC_OK;
}


// ������ ������ (����. 40 ��������)
// flags: ��� 0 - ������ �� ����������� �����, ��� 1 - ������ �� ������� �����
int FiscPrintString(CPU_INT32U pass, CPU_INT08U flags, CPU_INT08U* str, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  fisc_buf[4] = flags;
  memset(&fisc_buf[5], 0, 40);
  len = strlen((char const*)str);
  if (len > 40) len = 40;
  memcpy(&fisc_buf[5], str, len);
  
  if (FiscSendCommand(FISC_PRINT_STRING, fisc_buf, 45) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (len != 3) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_PRINT_STRING) || (rxdat[1] != 0)) {return FISC_ERR;}
    
  return FISC_OK;
}


// ������ ��������� ���������
int FiscPrintDocHeader(CPU_INT32U pass, CPU_INT08U* str, CPU_INT16U number, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;
  
  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  memset(&fisc_buf[4], 0, 30);
  len = strlen((char const*)str);
  if (len > 30) len = 30;
  memcpy(&fisc_buf[4], str, len);
  memcpy(&fisc_buf[34], &number, 2);
  
  if (FiscSendCommand(FISC_PRINT_DOC_HEADER, fisc_buf, 36) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (len != 5) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_PRINT_DOC_HEADER) || (rxdat[1] != 0)) {return FISC_ERR;}
    
  return FISC_OK;
}

// ������ ��������� �������� �� ������
// � �������� 6 ���� - ���������� � long long
int FiscGetMoneyReg(CPU_INT32U pass, CPU_INT08U reg, CPU_INT64U* value, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  fisc_buf[4] = reg;
  
  if (FiscSendCommand(FISC_GET_MONEY_REG, fisc_buf, 5) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (9 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_GET_MONEY_REG) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  *value = 0;
  memcpy(value, &rxdat[3], 6);
  
  return FISC_OK;
}


// ������ ������������� �������� �� ������
// � �������� 2 �����
int FiscGetOperReg(CPU_INT32U pass, CPU_INT08U reg, CPU_INT16U* value, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  fisc_buf[4] = reg;
  
  if (FiscSendCommand(FISC_GET_OPER_REG, fisc_buf, 5) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (5 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_GET_OPER_REG) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  memcpy(value, &rxdat[3], 2);
  
  return FISC_OK;
}

// ������� ����
int FiscBillCut(CPU_INT32U pass, CPU_INT08U type, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  fisc_buf[4] = type;
  
  if (FiscSendCommand(FISC_BILL_CUT, fisc_buf, 5) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_BILL_CUT) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  return FISC_OK;
}

// ��������
int FiscPullOutTape(CPU_INT32U pass, CPU_INT08U flags, CPU_INT08U strnum, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  fisc_buf[4] = flags;
  fisc_buf[5] = strnum;
  
  if (FiscSendCommand(FISC_PULL_OUT_TAPE, fisc_buf, 6) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_PULL_OUT_TAPE) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  return FISC_OK;
}


// ������ ����������� ���������
// �� ��������������
int FiscEjectUnderlayDoc(CPU_INT32U pass, CPU_INT08U dir, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  fisc_buf[4] = dir;
  
  if (FiscSendCommand(FISC_EJECT_UNDERLAY_DOC, fisc_buf, 5) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_EJECT_UNDERLAY_DOC) || (rxdat[1] != 0)) {return FISC_ERR;}

  return FISC_OK;
}


// ������ ������ �������� ������� (����. 40 ��������)
// flags: ��� 0 - ������ �� ����������� �����, ��� 1 - ������ �� ������� �����
int FiscPrintStringByFont(CPU_INT32U pass, CPU_INT08U flags, CPU_INT08U font, CPU_INT08U* str, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  fisc_buf[4] = flags;
  fisc_buf[5] = flags;
  memset(&fisc_buf[6], 0, 40);
  len = strlen((char const*)str);
  if (len > 40) len = 40;
  memcpy(&fisc_buf[6], str, len);
  
  if (FiscSendCommand(FISC_PRINT_STRING_BY_FONT, fisc_buf, 46) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (len != 3) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_PRINT_STRING_BY_FONT) || (rxdat[1] != 0)) {return FISC_ERR;}

  return FISC_OK;
}


// �������� ����� ��� �������
int FiscPrintDayReportNoClear(CPU_INT32U admpass, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  if (FiscSendCommand(FISC_PRINT_DAY_REPORT_NO_CLEAR, (CPU_INT08U*)&admpass, 4) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_PRINT_DAY_REPORT_NO_CLEAR) || (rxdat[1] != 0)) {return FISC_ERR;}

  OSTimeDly(5000);
  
  return FISC_OK;
}


// �������� ����� c ��������
int FiscPrintDayReportClear(CPU_INT32U admpass, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  if (FiscSendCommand(FISC_PRINT_DAY_REPORT_CLEAR, (CPU_INT08U*)&admpass, 4) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_PRINT_DAY_REPORT_CLEAR) || (rxdat[1] != 0)) {return FISC_ERR;}

  OSTimeDly(5000);
  
  return FISC_OK;
}

// �������� ����� c �������� � �����
int FiscPrintDayReportToBuf(CPU_INT32U admpass, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  if (FiscSendCommand(FISC_PRINT_DAY_REPORT_TO_BUF, (CPU_INT08U*)&admpass, 4) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_PRINT_DAY_REPORT_TO_BUF) || (rxdat[1] != 0)) {return FISC_ERR;}

  OSTimeDly(100);
  
  return FISC_OK;
}

// ������� �����
int FiscOpenDay(CPU_INT32U pass, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  *err = 0;
    
  if (FiscSendCommand(FISC_OPEN_DAY, (CPU_INT08U*)&pass, 4) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
  
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_OPEN_DAY) || (rxdat[1] != 0)) {return FISC_ERR;}

  return FISC_OK;
}


// �������� ����� c �������� � �����
int FiscPrintDayReportsFromBuf(CPU_INT32U admpass, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  if (FiscSendCommand(FISC_PRINT_DAY_REPORT_FROM_BUF, (CPU_INT08U*)&admpass, 4) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_PRINT_DAY_REPORT_FROM_BUF) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  OSTimeDly(5000);

  return FISC_OK;
}


// ����� �� �������
int FiscPrintSectionReport(CPU_INT32U admpass, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  if (FiscSendCommand(FISC_PRINT_SECTION_REPORT, (CPU_INT08U*)&admpass, 4) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_PRINT_SECTION_REPORT) || (rxdat[1] != 0)) {return FISC_ERR;}

  return FISC_OK;
}

// ����� �� �������
int FiscPrintTaxesReport(CPU_INT32U admpass, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  if (FiscSendCommand(FISC_PRINT_TAXES_REPORT, (CPU_INT08U*)&admpass, 4) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_PRINT_TAXES_REPORT) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  return FISC_OK;
}

// �������� �����
// ����� - � ��������, doc - �������� ����� ���������
int FiscMakeDeposit(CPU_INT32U pass, CPU_INT32U sum, CPU_INT16U* doc, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  memcpy(&fisc_buf[4], (CPU_INT08U*)&sum, 4);
  fisc_buf[8] = 0;
    
  if (FiscSendCommand(FISC_MAKE_DEPOSIT, fisc_buf, 9) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (5 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_MAKE_DEPOSIT) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  memcpy(doc, (CPU_INT08U*)&rxdat[3], 2);
  
  return FISC_OK;
}

// �������
int FiscMakePayout(CPU_INT32U pass, CPU_INT32U sum, CPU_INT16U* doc, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  memcpy(&fisc_buf[4], (CPU_INT08U*)&sum, 4);
  fisc_buf[8] = 0;
    
  if (FiscSendCommand(FISC_MAKE_PAYOUT, fisc_buf, 9) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (5 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_MAKE_PAYOUT) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  memcpy(doc, (CPU_INT08U*)&rxdat[3], 2);

  return FISC_OK;
}

// ������ �����
// �� ��������������
int FiscPrintCliche(CPU_INT32U pass, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  if (FiscSendCommand(FISC_PRINT_CLICHE, (CPU_INT08U*)&pass, 4) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_PRINT_CLICHE) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  return FISC_OK;
}


// ����� ���������
int FiscEndDoc(CPU_INT32U pass, CPU_INT08U param, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  fisc_buf[4] = param;
  
  if (FiscSendCommand(FISC_END_DOC, fisc_buf, 5) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_END_DOC) || (rxdat[1] != 0)) {return FISC_ERR;}

  return FISC_OK;
}

// ������ ���������� ������
int FiscPrintAdvText(CPU_INT32U pass, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  if (FiscSendCommand(FISC_PRINT_ADV_TEXT, (CPU_INT08U*)&pass, 4) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_PRINT_ADV_TEXT) || (rxdat[1] != 0)) {return FISC_ERR;}

  return FISC_OK;
}

// ��������� ���������� �� ����������
int FiscGetDeviceType(TFiscDevType* dev, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  if (FiscSendCommand(FISC_GET_DEVICE_TYPE, NULL, 0) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if ((sizeof(TFiscDevType)+2) < len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_GET_DEVICE_TYPE) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  memcpy(dev, &rxdat[2], sizeof(TFiscDevType));

  return FISC_OK;
}

// �������� ����
int FiscOpenBill(CPU_INT32U pass, CPU_INT08U type, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  fisc_buf[4] = type;
  
  if (FiscSendCommand(FISC_OPEN_BILL, fisc_buf, 5) != FISC_OK)
  {
    return FISC_ERR;
  }
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK)
  {
    return FISC_ERR;
  }

  *err = rxdat[1];
    
  if (3 != len) 
  {
    return FISC_ERR;
  }
  
  if ((rxdat[0] != FISC_OPEN_BILL) || (rxdat[1] != 0))
  {
    return FISC_ERR;
  }
  
  return FISC_OK;
}

// ���������������� �������
int FiscMakeSell(CPU_INT32U pass, 
                 CPU_INT64U *count, CPU_INT64U *price, CPU_INT08U department,
                 CPU_INT08U* tax, char* text, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  memcpy(&fisc_buf[4], (CPU_INT08U*)count, 5);
  memcpy(&fisc_buf[9], (CPU_INT08U*)price, 5);
  fisc_buf[14] = department;
  memcpy(&fisc_buf[15], (CPU_INT08U*)tax, 4);
  memset(&fisc_buf[19], 0, 40);
  strcpy((char*)&fisc_buf[19], text);
  
  if (FiscSendCommand(FISC_MAKE_SELL, fisc_buf, 59) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_MAKE_SELL) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  return FISC_OK;
}

// ���������������� �������
int FiscMakeSellV2(CPU_INT32U pass, 
                 CPU_INT64U *count, CPU_INT64U *price, CPU_INT08U department,
                 CPU_INT08U* tax, CPU_INT08U subj, char* text, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;
  static const CPU_INT08U subj_tbl[3] = {1, 3, 4};
  
  fisc_buf[0] = 0x46; // ������� 0xFF46
  memcpy(&fisc_buf[1], (CPU_INT08U*)&pass, 4);
  fisc_buf[5] = 0x01; // ��� �������� 1 - ������
  memcpy(&fisc_buf[6], (CPU_INT08U*)count, 6);  // ���������� 6 ����, 6 ������ ����� �������
  memcpy(&fisc_buf[12], (CPU_INT08U*)price, 5); // ����
  fisc_buf[17] = 0xFF; // ����� �������������� ������
  fisc_buf[18] = 0xFF;
  fisc_buf[19] = 0xFF;
  fisc_buf[20] = 0xFF;
  fisc_buf[21] = 0xFF;
  fisc_buf[22] = 0xFF; // ����� �� ������ (�������������� ������)
  fisc_buf[23] = 0xFF;
  fisc_buf[24] = 0xFF;
  fisc_buf[25] = 0xFF;
  fisc_buf[26] = 0xFF;
  if (tax[0]) fisc_buf[27] = 1 << (tax[0] - 1);
  else fisc_buf[27] = 0;
  fisc_buf[28] = department;
  fisc_buf[29] = 0x04;  // ������� ������� ������� 4 - "������ ������"
  if (subj > 2) subj = 0;
  fisc_buf[30] = subj_tbl[subj];       // ������� �������� �������
  memset(&fisc_buf[31], 0, 40);         // ������������
  strcpy((char*)&fisc_buf[31], text);
  
  if (FiscSendCommand(FISC_EXTENDED_CMD, fisc_buf, 71) != FISC_OK) {return FISC_ERR;}

  memset(fisc_buf, 0, 256);
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[2];
  
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_EXTENDED_CMD) || (rxdat[1] != 0x46) || (rxdat[2] != 0)) {return FISC_ERR;}
  
  return FISC_OK;
}

// �������� ����
int FiscCloseBill(CPU_INT32U pass, CPU_INT64U *cash, CPU_INT08U* tax, char* text, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  memcpy(&fisc_buf[4], cash, 5);
  memset(&fisc_buf[9], 0, 15+2);

  memcpy(&fisc_buf[26], tax, 4);  
  memset(&fisc_buf[30], 0, 40);
  strcpy((char*)&fisc_buf[30], text);
  
  if (FiscSendCommand(FISC_CLOSE_BILL, fisc_buf, 70) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (8 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_CLOSE_BILL) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  return FISC_OK;
}

// �������� ����
int FiscCloseBillV2(CPU_INT32U pass, CPU_INT64U *cash, CPU_INT08U taxsys, char* text, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  fisc_buf[0] = 0x45;
  memcpy(&fisc_buf[1], (CPU_INT08U*)&pass, 4);
  memcpy(&fisc_buf[5], cash, 5);
  memset(&fisc_buf[10], 0, 75);

  fisc_buf[85] = 0x00; // ���������� �� ����� � ��������
  memset(&fisc_buf[86], 0, 5); // ����� 1
  memset(&fisc_buf[91], 0, 5); // ����� 2
  memset(&fisc_buf[96], 0, 5); // ����� 3
  memset(&fisc_buf[101], 0, 5); // ����� 4
  memset(&fisc_buf[106], 0, 5); // ����� 5
  memset(&fisc_buf[111], 0, 5); // ����� 6
  fisc_buf[116] = (1 << taxsys);
  memset(&fisc_buf[117], 0, 40);
  strcpy((char*)&fisc_buf[117], text);
  
  if (FiscSendCommand(FISC_EXTENDED_CMD, fisc_buf, 157) != FISC_OK) {return FISC_ERR;}
  
  memset(fisc_buf, 0, 256);
      
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[2];
    
  //if (8 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_EXTENDED_CMD) || (rxdat[1] != 0x45) || (rxdat[2] != 0)) {return FISC_ERR;}
  
  return FISC_OK;
}

int FiscCloseBillV2Online(CPU_INT32U pass, CPU_INT64U *cash, CPU_INT08U taxsys, char* text, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  fisc_buf[0] = 0x45;
  memcpy(&fisc_buf[1], (CPU_INT08U*)&pass, 4);
  
  memset(&fisc_buf[5], 0, 5);
  
  memcpy(&fisc_buf[10], cash, 5); // online ������
  
  memset(&fisc_buf[15], 0, 70);
  fisc_buf[85] = 0x00; // ���������� �� ����� � ��������
  memset(&fisc_buf[86], 0, 5); // ����� 1
  memset(&fisc_buf[91], 0, 5); // ����� 2
  memset(&fisc_buf[96], 0, 5); // ����� 3
  memset(&fisc_buf[101], 0, 5); // ����� 4
  memset(&fisc_buf[106], 0, 5); // ����� 5
  memset(&fisc_buf[111], 0, 5); // ����� 6
  fisc_buf[116] = (1 << taxsys);
  memset(&fisc_buf[117], 0, 40);
  strcpy((char*)&fisc_buf[117], text);
  
  if (FiscSendCommand(FISC_EXTENDED_CMD, fisc_buf, 157) != FISC_OK) {return FISC_ERR;}
  
  memset(fisc_buf, 0, 256);
      
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[2];
    
  //if (8 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_EXTENDED_CMD) || (rxdat[1] != 0x45) || (rxdat[2] != 0)) {return FISC_ERR;}
  
  return FISC_OK;
}

// ����������� ������ (����� ���������� ������)
int FiscPrintContinue(CPU_INT32U pass, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  *err = 0;
    
  if (FiscSendCommand(FISC_PRINT_CONTINUE, (CPU_INT08U*)&pass, 4) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
  
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_PRINT_CONTINUE) || (rxdat[1] != 0)) {return FISC_ERR;}

  return FISC_OK;
}


/// ������ �������� �� �������
int FiscReadTableData(CPU_INT32U pass, CPU_INT08U table, CPU_INT16U row, CPU_INT08U field, CPU_INT08U field_len, CPU_INT08U* data, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;
  CPU_INT08U req[8];

  *err = 0;
    
  memcpy(&req[0], &pass, 4);
  memcpy(&req[4], &table, 1);
  memcpy(&req[5], &row, 2);
  memcpy(&req[7], &field, 1);
      
  if (FiscSendCommand(FISC_READ_TABLE, (CPU_INT08U*)&req, 8) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if ((field_len + 2) != len) {return FISC_ERR;}
      
  if ((rxdat[0] != FISC_READ_TABLE) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  memcpy(data, &rxdat[2], field_len);
  
  return FISC_OK;
}

/// ������ �������� � �������
int FiscWriteTableData(CPU_INT32U pass, CPU_INT08U table, CPU_INT16U row, CPU_INT08U field, CPU_INT08U field_len, CPU_INT08U* data, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  memcpy(&fisc_buf[4], &table, 1);
  memcpy(&fisc_buf[5], &row, 2);
  memcpy(&fisc_buf[7], &field, 1);
  memcpy(&fisc_buf[8], &data, field_len);
  
  if (FiscSendCommand(FISC_WRITE_TABLE, fisc_buf, 8 + field_len) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (3 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_WRITE_TABLE) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  return FISC_OK;
}

// �����.������ ����
int FiscCanselBill(CPU_INT32U pass, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  
  if (FiscSendCommand(FISC_CANSEL_BILL, fisc_buf, 4) != FISC_OK)
  {
    return FISC_ERR;
  }
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK)
  {
    return FISC_ERR;
  }

  *err = rxdat[1];
    
  if (3 != len) 
  {
    return FISC_ERR;
  }
  
  if ((rxdat[0] != FISC_CANSEL_BILL) || (rxdat[1] != 0))
  {
    return FISC_ERR;
  }
  
  return FISC_OK;
}

