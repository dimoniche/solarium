/*
  Драйвер фискального регистратора ШТРИХ-М
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


// проверка статуса ФР
int FiscPoll(void)
{
  CPU_INT08U ack;
  FiscUartSendByte(FISC_ENQ);
  if (!FiscUartRecieveByte(&ack, FISC_TIMEOUT)) return FISC_UNDEF;;
  if (ack == FISC_NACK) return FISC_READY;
  else if (ack == FISC_ACK) return FISC_BUSY;
  return FISC_UNDEF;
}

// отправка команды
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
    { // ждем 5 секунд, например
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


// прием ответа
// возвращает результат, указатель на данные и длину поля данных
int FiscReceiveAnswer(CPU_INT08U **dat, CPU_INT08U *len, CPU_INT32U timeout)
{
  CPU_INT08U byte, lrc, num;

  *dat = NULL;
  *len = 0;

  // стартовый байт
  if (!FiscUartRecieveByte(&byte, timeout)) return FISC_ERR;
  if (byte != FISC_STX) return FISC_ERR;
  
  // длина сообщения
  if (!FiscUartRecieveByte(&byte, FISC_TIMEOUT)) return FISC_ERR;
  if (!byte) return FISC_ERR;
  num = byte;
  
  // теперь принимаем число байт = byte
  if (!FiscUartRecieve(fisc_buf, num, FISC_BYTE_TIMEOUT)) return FISC_ERR;

  // принимаем контрольную сумму
  if (!FiscUartRecieveByte(&byte, FISC_BYTE_TIMEOUT)) return FISC_ERR;

  // считаем контрольную сумму
  lrc = num;
  for (CPU_INT08U i=0; i<num; i++) lrc ^= fisc_buf[i];
  
  // проверяем контрольную сумму
  if (lrc != byte) return FISC_ERR;
    
  // всё ок
  FiscUartSendByte(FISC_ACK);
  *dat = &fisc_buf[0];
  *len = num;
  
  return FISC_OK;
}

// чтение заводского номера ФР
// !!! в нашей версии ФР не поддерживается !!!
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

// короткий запрос состояния ФР
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

// полный запрос состояния ФР
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

// печатает 20 символов жирнымшрифтом
// flags: бит 0 - печать на контрольной ленте, бит 1 - печать на чековой ленте
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

// гудок
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


// печать строки (макс. 40 символов)
// flags: бит 0 - печать на контрольной ленте, бит 1 - печать на чековой ленте
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


// печать заголовка документа
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

// запрос денежного регистра по номеру
// в регистре 6 байт - записываем в long long
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


// запрос операционного регистра по номеру
// в регистре 2 байта
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

// отрезка чека
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

// протяжка
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


// выброс подкладного документа
// не поддерживается
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


// печать строки заданным шрифтом (макс. 40 символов)
// flags: бит 0 - печать на контрольной ленте, бит 1 - печать на чековой ленте
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


// суточный отчет без гашения
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


// суточный отчет c гашением
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

// суточный отчет c гашением в буфер
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

// суточный отчет c гашением в буфер
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


// отчет по секциям
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

// отчет по налогам
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

// внесение денег
// сумма - в копейках, doc - сквозной номер документа
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

// выплата
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

// печать клише
// не поддерживается
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


// конец документа
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

// печать рекламного текста
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

// получение информации об устройстве
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

// открытие чека
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

// зарегистрировать продажу
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


// закрытие чека
int FiscCloseBill(CPU_INT32U pass, CPU_INT64U *cash, CPU_INT08U* tax, char* text, CPU_INT08U* err)
{
  CPU_INT08U* rxdat;
  CPU_INT08U len;

  memcpy(&fisc_buf[0], (CPU_INT08U*)&pass, 4);
  memcpy(&fisc_buf[4], cash, 5);
  memset(&fisc_buf[9], 0, 15+2);
  memcpy(&fisc_buf[26], tax, 4);  
  memset(&fisc_buf[30], 0, 40);
  strcpy((char*)&fisc_buf[70], text);
  
  if (FiscSendCommand(FISC_CLOSE_BILL, fisc_buf, 70) != FISC_OK) {return FISC_ERR;}
  
  if (FiscReceiveAnswer(&rxdat, &len, FISC_ANSWER_TIMEOUT) != FISC_OK) {return FISC_ERR;}

  *err = rxdat[1];
    
  if (8 != len) {return FISC_ERR;}
  
  if ((rxdat[0] != FISC_CLOSE_BILL) || (rxdat[1] != 0)) {return FISC_ERR;}
  
  return FISC_OK;
}

// продолжение печати (после отсутствия бумаги)
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

