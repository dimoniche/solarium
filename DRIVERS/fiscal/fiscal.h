#ifndef _FISCAL_H_
#define _FISCAL_H_

extern CPU_INT08U  FiscalState;
#define FISCAL_NOCONN 0
#define FISCAL_CONN   1

// ���� ��������� ��������
#define FISC_ENQ    0x05
#define FISC_STX    0x02
#define FISC_ACK    0x06
#define FISC_NACK   0x15

// �������� �������� �� ���������
#define FISC_SPEED      115200

// ������� ������ �� ���������
#define FISC_TIMEOUT              105
#define FISC_BYTE_TIMEOUT         10
#define FISC_ANSWER_TIMEOUT       5000

// ���� ������ ��
#define FISC_GET_FACTORY_NUMBER   0x0f
#define FISC_GET_SHORT_STATUS     0x10
#define FISC_GET_FULL_STATUS      0x11
#define FISC_PRINT_BOLD_STRING    0x12
        #define FISC_CONTROL_TAPE    0x01
        #define FISC_BILL_TAPE       0x02
        #define FISC_UNDERLAY_DOC    0x04
#define FISC_BEEP                 0x13
#define FISC_PRINT_STRING         0x17
#define FISC_PRINT_DOC_HEADER     0x18
#define FISC_GET_MONEY_REG        0x1A
#define FISC_GET_OPER_REG         0x1B
#define FISC_BILL_CUT             0x25
        #define FISC_FULL_CUT     0
        #define FISC_HALF_CUT     1
#define FISC_PULL_OUT_TAPE        0x29
#define FISC_EJECT_UNDERLAY_DOC   0x2A
        #define FISC_EJECT_DOWN   0
        #define FISC_EJECT_UP     1
#define FISC_PRINT_STRING_BY_FONT 0x2F
#define FISC_PRINT_DAY_REPORT_NO_CLEAR   0x40
#define FISC_PRINT_DAY_REPORT_CLEAR      0x41
#define FISC_PRINT_SECTION_REPORT        0x42
#define FISC_PRINT_TAXES_REPORT          0x43
#define FISC_MAKE_DEPOSIT          0x50
#define FISC_MAKE_PAYOUT           0x51
#define FISC_PRINT_CLICHE          0x52
#define FISC_END_DOC               0x53
        #define FISC_WITHOUT_ADV   0 // � �������� 
        #define FISC_WITH_ADV      1 // ��� �������
#define FISC_PRINT_ADV_TEXT        0x54
#define FISC_GET_DEVICE_TYPE       0xFC

#define FISC_OPEN_BILL           0x8D
  #define FISC_BILL_SELL  0
  #define FISC_BILL_BUY   1

#define FISC_MAKE_SELL           0x80
#define FISC_CLOSE_BILL          0x85
#define FISC_CANSEL_BILL         0x88

#define FISC_EXTENDED_CMD           0xFF

#define FISC_OPEN_DAY            0xE0
#define FISC_PRINT_CONTINUE      0xB0

#define FISC_PRINT_DAY_REPORT_TO_BUF      0xC6
#define FISC_PRINT_DAY_REPORT_FROM_BUF    0xC7

#define FISC_WRITE_TABLE            0x1E
#define FISC_READ_TABLE             0x1F


// ��������������� ������� ��������
#define FiscPurgeRx           Uart0_Flush
#define FiscUartSend          Uart0_Send
#define FiscUartRecieve       Uart0_Receive
#define FiscUartRecieveByte   Uart0_RdByteWithTimeOut
#define FiscUartSendByte      Uart0_WrByte
#define FiscSleep(x)          OSTimeDly(x)

// ����� �� �������� ������� FiscPoll()
#define FISC_READY   0
#define FISC_BUSY   -1
#define FISC_UNDEF  -2

// ���� ����������� �������
#define FISC_OK      0
#define FISC_ERR    -1

// ���� ������ ��
#define FR_ERROR_CODE_1     0x1   //����������� �������
#define FR_ERROR_CODE_2     0x2   //�������� ��������� �� 
#define FR_ERROR_CODE_3     0x3   //������ �� 
#define FR_ERROR_CODE_4     0x4   //������ ��
#define FR_ERROR_CODE_5     0x5   //�������� ���� ������������ ��
#define FR_ERROR_CODE_6     0x6   //����� �� ����������
#define FR_ERROR_CODE_7     0x7   //�������� ���� �/��� ����� 
#define FR_ERROR_CODE_8     0x8   //��� ����������� ������ 
#define FR_ERROR_CODE_9     0x9   //������������ �������� ���������� �������
#define FR_ERROR_CODE_10    0x10  //���������� �������� TLV ������
#define FR_ERROR_CODE_11    0x11  //��� ������������� ����������
#define FR_ERROR_CODE_12    0x12  //�������� ������ ��
#define FR_ERROR_CODE_14    0x14  //�������� ������ ��������
#define FR_ERROR_CODE_15    0x15  //�������� ������ �������� �������� ���������
#define FR_ERROR_CODE_16    0x16  //����������������� ����� ����� 24 �����
#define FR_ERROR_CODE_17    0x17  //�������� ������� �� ������� ����� 2 ��������� 
#define FR_ERROR_CODE_20    0x20  //��������� �� ��� �� ����� ���� �������
#define FR_ERROR_CODE_2F    0x2F  //������� ������ � ��
#define FR_ERROR_CODE_30    0x30  //�� �� ��������
#define FR_ERROR_CODE_33    0x33  //������������ ��������� � �������
#define FR_ERROR_CODE_34    0x34  //��� ������
#define FR_ERROR_CODE_35    0x35  //������������ �������� ��� ������ ���������� 
#define FR_ERROR_CODE_36    0x36  //������������ ��������� � ������� ��� ������ ���������� ���
#define FR_ERROR_CODE_37    0x37  //������� �� �������������� � ������ ���������� ���
#define FR_ERROR_CODE_38    0x38  //������ � ���
#define FR_ERROR_CODE_39    0x39  //���������� ������ �� ���
#define FR_ERROR_CODE_3a    0x3A  //������������ ���������� �� ��������� � �����
#define FR_ERROR_CODE_3c    0x3C  //����� ������� � �������� �� ��������
#define FR_ERROR_CODE_3D    0x3D  //����� �� ������� � �������� �� �������� 
#define FR_ERROR_CODE_3E    0x3E  //������������ ���������� �� ������� � �����
#define FR_ERROR_CODE_3F    0x3F  //������������ ���������� �� ������� � �����
#define FR_ERROR_CODE_40    0x40  //������������ ��������� ������
#define FR_ERROR_CODE_41    0x41  //������������ ��������� ������ ���������
#define FR_ERROR_CODE_42    0x42  //������������ ��������� ������ ����� 2
#define FR_ERROR_CODE_43    0x43  //������������ ��������� ������ ����� 3
#define FR_ERROR_CODE_44    0x44  //������������ ��������� ������ ����� 4
#define FR_ERROR_CODE_45    0x45  //C���� ���� ����� ������ ������ ����� ����
#define FR_ERROR_CODE_46    0x46  //�� ������� ���������� � �����
#define FR_ERROR_CODE_47    0x47  //������������ ���������� �� ������� � �����
#define FR_ERROR_CODE_48    0x48  //������������ ����� ����
#define FR_ERROR_CODE_49    0x49  //�������� ���������� � �������� ���� ������� ����
#define FR_ERROR_CODE_4A    0x4A  //������ ��� - �������� ����������
#define FR_ERROR_CODE_4B    0x4B  //����� ���� ����������
#define FR_ERROR_CODE_4C    0x4C  //������������ ���������� �� ������� ������� � �����
#define FR_ERROR_CODE_4D    0x4D  //�������� ����������� ������� ����� ������ ����� ����
#define FR_ERROR_CODE_4E    0x4E  //����� ��������� 24 ����
#define FR_ERROR_CODE_4F    0x4F  //�������� ������
#define FR_ERROR_CODE_50    0x50  //���� ������ ���������� �������
#define FR_ERROR_CODE_51    0x51  //������������ ���������� ��������� � �����
#define FR_ERROR_CODE_52    0x52  //������������ ���������� �� ���� ������ 2 � �����
#define FR_ERROR_CODE_53    0x53  //������������ ���������� �� ���� ������ 3 � �����
#define FR_ERROR_CODE_54    0x54  //������������ ���������� �� ���� ������ 4 � �����
#define FR_ERROR_CODE_55    0x55  //��� ������ � �������� ����������
#define FR_ERROR_CODE_56    0x56  //��� ��������� ��� �������
#define FR_ERROR_CODE_58    0x58  //�������� ������� ����������� ������
#define FR_ERROR_CODE_59    0x59  //�������� ������ ������ ����������
#define FR_ERROR_CODE_5B    0x5B  //������������ ��������� ��������
#define FR_ERROR_CODE_5C    0x5C  //�������� ���������� 24�
#define FR_ERROR_CODE_5D    0x5D  //������� �� ����������
#define FR_ERROR_CODE_5E    0x5E  //������������ ��������
#define FR_ERROR_CODE_5F    0x5F  //������������� ���� ����
#define FR_ERROR_CODE_60    0x60  //������������ ��� ���������
#define FR_ERROR_CODE_61    0x61  //������������ ��������� ����
#define FR_ERROR_CODE_62    0x62  //������������ ��������� ����������
#define FR_ERROR_CODE_63    0x63  //������������ ��������� ������
#define FR_ERROR_CODE_65    0x65  //�� ������� ����� � ������
#define FR_ERROR_CODE_66    0x66  //������������ ����� � ������
#define FR_ERROR_CODE_68    0x68  //�� ������� ����� �� ������� �������
#define FR_ERROR_CODE_69    0x69  //������������ ����� �� ������� �������
#define FR_ERROR_CODE_6A    0x6A  //������ ������� � ������ ������ �� I2C
#define FR_ERROR_CODE_6B    0x6B  //��� ������� ����� 
#define FR_ERROR_CODE_6C    0x6C  //��� ����������� �����
#define FR_ERROR_CODE_6D    0x6D  //�� ������� ����� �� ������
#define FR_ERROR_CODE_6E    0x6E  //������������ ����� �� ������
#define FR_ERROR_CODE_6F    0x6F  //������������ �� ������� � �����
#define FR_ERROR_CODE_71    0x71  //������ ���������
#define FR_ERROR_CODE_72    0x72  //������� �� �������������� � ������ ���������
#define FR_ERROR_CODE_73    0x73  //������� �� �������������� � ������ ������
#define FR_ERROR_CODE_74    0x74  //������ ���
#define FR_ERROR_CODE_75    0x75  //������ �������
#define FR_ERROR_CODE_77    0x77  //������ ��������: ��� ������� � ��������
#define FR_ERROR_CODE_78    0x78  //������ ��
#define FR_ERROR_CODE_7A    0x7A  //���� �� ������������� 
#define FR_ERROR_CODE_7B    0x7B  //������ ������������
#define FR_ERROR_CODE_7C    0x7C  //�� ��������� ����
#define FR_ERROR_CODE_7D    0x7D  //�������� ������ ����
#define FR_ERROR_CODE_7E    0x7E  //�������� �������� � ���� �����
#define FR_ERROR_CODE_7F    0x7F  //������������ ��������� ����� ����
#define FR_ERROR_CODE_84    0x84  //������������ ����������
#define FR_ERROR_CODE_85    0x85  //������������ �� �������� � �����
#define FR_ERROR_CODE_86    0x86  //������������ �� �������� � �����
#define FR_ERROR_CODE_87    0x87  //������������ �� ��������� ������ � �����
#define FR_ERROR_CODE_88    0x88  //������������ �� ��������� ������� � �����
#define FR_ERROR_CODE_89    0x89  //������������ �� �������� � �����
#define FR_ERROR_CODE_8A    0x8A  //������������ �� ��������� � ����
#define FR_ERROR_CODE_8B    0x8B  //������������ �� ������� � ����
#define FR_ERROR_CODE_8C    0x8C  //������������� ���� �������� � ����
#define FR_ERROR_CODE_8D    0x8D  //������������� ���� ������ � ����
#define FR_ERROR_CODE_8E    0x8E  //������� ���� ����
#define FR_ERROR_CODE_90    0x90  //���� ��������� ������, ������������� � ����������
#define FR_ERROR_CODE_91    0x91  //����� �� ������� ���� ������ ��� ������ ���������� ������ 
#define FR_ERROR_CODE_92    0x92  //��������� ����� 
#define FR_ERROR_CODE_93    0x93  //�������������� ��� ������ ������� 
#define FR_ERROR_CODE_94    0x94  //�������� ����� �������� � ���� 
#define FR_ERROR_CODE_C0    0xC0  //�������� ���� � ������� (����������� ���� � �����) 
#define FR_ERROR_CODE_C2    0xC2  //���������� ���������� � ����� ������� 
#define FR_ERROR_CODE_C4    0xC4  //������������ ������� ���� 
#define FR_ERROR_CODE_C7    0xC7  //���� �� ������������� � ������ ������ 
#define FR_ERROR_CODE_C8    0xC8  //����������� �������� �� ����������� 

#define FR_ERROR_NUMBER   109

// ��������� ���������� ������ � ���
#pragma pack(push, 1)
typedef struct{
  CPU_INT08U FactoryNumber[7];
  CPU_INT08U RNM[7];
}TFiscFactoryNumber;
#pragma pack(pop)

// ��������� ��������� ���������
#pragma pack(push, 1)
typedef struct{
  CPU_INT08U OperatorNumber; // ����� ��������� 1..30
  CPU_INT16U Flags; // ����� ��
  /*
   ����� ��  ������� ���� (���������� ���): 
      0 � ����� ������������� ������� (0 � ���, 1 � ����) 
      1 � ����� ������� ����� (0 � ���, 1 � ����) 
      2 � ������� ������ ����������� ��������� (0 � ���, 1 � ��) 
      3 � ������ ������ ����������� ��������� (0 � ���, 1 � ��) 
      4 � ��������� ���������� ����� (0 � 0 ������, 1 � 2 �����) 
      5 � ���� (0 � ���, 1 � ����) 
      6 � ���������� ������ ������������� ������� (0 � ������ ���, 1 � ������ ����) 
      7 � ���������� ������ ������� ����� (0 � ������ ���, 1 � ������ ����) 
      8 � ����� ������������ ����������� ����� (0 � ������, 1 � ������) 
      9 � ����� ������������ ������� ����� (0 � ������, 1 � ������) 
      10 � ������ ������� �� (0 � �������, 1 � �������) 
      11 � �������� ���� (0 � ������, 1 � �����) 
      12� � ����� ������� ������� �������� (0 � ���, 1 � ��) 
      12� � ������ �� ����� � ��������� (0 � ���, 1 � ��) 
      13� � ����� ������ ������� �������� (0 � ���, 1 � ��) 
      13� � ������ �� ������ �� ���������� (0 � ���, 1 � ��) 
      14 � ���� ����� ��������� (0 � ���, 1 � ��) 
      15� � ����������� �������� ���������� (0 � ���������� ��������, 1 � ����������� 
      ��������) [��� ��� ��� ����]. ��� ��� � ���� (1 � ���������� ��������, 0 � 
      ����������� ��������) 
      15� � ����� �������� ������ (0 � ����, 1 � ������) 
  */
  CPU_INT08U Mode;  // ����� ��
  CPU_INT08U SubMode;  // �������� ��
  CPU_INT08U BillOperationNumber_L; // ���-�� �������� � ����, ��. ����
  CPU_INT08U BatteryVoltage;  // ���������� ��������� �������
  CPU_INT08U PowerSupplyVoltage;  // ���������� ��������� �������
  CPU_INT08U ErrorCodeFP; // ��� ������ ��
  CPU_INT08U ErrorCodeEKLZ; // ��� ������ ����
  CPU_INT08U BillOperationNumber_H; // ���-�� �������� � ����, ��. ����
  CPU_INT08U Reserve[3]; // ������  
}TFiscShortStatus;
#pragma pack(pop)

// ��������� ������� ���������
#pragma pack(push, 1)
typedef struct{
/*   
���������� ����� ��������� (1 ����) 1�30 
������ �� �� (2 �����) 
������ �� �� (2 �����) 
���� �� �� (3 �����) ��-��-�� 
����� � ���� (1 ����) 
�������� ����� �������� ��������� (2 �����) 
����� �� (2 �����) 
����� �� (1 ����) 
�������� �� (1 ����) 
���� �� (1 ����) 
������ �� �� (2 �����) 
������ �� �� (2 �����) 
���� �� �� (3 �����) ��-��-�� 
���� (3 �����) ��-��-�� 
����� (3 �����) ��-��-�� 
����� �� (1 ����) 
������� ���� (���������� ���): 
0 � �� 1 (0 ����, 1 � ����) 
1 � �� 2 (0 ����, 1 � ����) 
2 � �������� (0 � �� �������, 1 � �������) 
3 � ������������ �� (0 � ���, 1 � ����) 
4 � ������� �� (0 � >80%, 1 � <80%) 
5 � ��������� ������ �� (0 � ���������, 1 � ���������) 
6 � ����� � �� (0 � �������, 1 � �������) 
7 � 24 ���� � �� (0 � �� ���������, 1 � ���������) 
��������� ����� (4 �����) 
����� ��������� �������� ����� (2 �����) 
���������� ��������� ������� � �� (2 �����) 
���������� ��������������� (������������) (1 ����) 
���������� ���������� ��������������� (������������) (1 ����) 
  ��� (6 ����) 
*/    
    CPU_INT08U OperatorNumber; // ����� ��������� 1..30
    CPU_INT08U version[4];
    CPU_INT08U version_date[3];
    CPU_INT08U nomer_v_zale;
    CPU_INT16U document_number;
    CPU_INT16U Flags; // ����� ��
    CPU_INT08U Mode;  // ����� ��
    CPU_INT08U SubMode;  // �������� ��    
    CPU_INT08U port;
    CPU_INT08U ver_fp[4];
    CPU_INT08U date_fp[3];
    CPU_INT08U date[3];
    CPU_INT08U time[3];
    CPU_INT08U fp_flags;
    CPU_INT32U FactoryNumber;
    CPU_INT16U LastSmena;
    CPU_INT16U FpFreePlace;
    CPU_INT08U perereg;
    CPU_INT08U perereg_free;
    CPU_INT08U inn[6];

}TFiscFullStatus;
#pragma pack(pop)

// ��������� ���������� �� ����������
#pragma pack(push, 1)
typedef struct{ 
  CPU_INT08U Type;                // ��� ���������� (1 ����) 0�255 
  CPU_INT08U Subtype;             // ������ ���������� (1 ����) 0�255 
  CPU_INT08U ProtocolVersion;     // ������ ��������� ��� ������� ���������� (1 ����) 0�255 
  CPU_INT08U ProtocolSubVersion;  // ��������� ��������� ��� ������� ���������� (1 ����) 0�255 
  CPU_INT08U Model;               // ������ ���������� (1 ����) 0�255 
  CPU_INT08U Language;            // ���� ���������� (1 ����) 0�255 ������� � 0; ���������� � 1; 
      #define FISC_LANG_RUS   0
      #define FISC_LANG_ENG   1
  CPU_INT08U Name[32];            // �������� ���������� � ������ �������� � ��������� WIN1251. ���������� 
                                  // ����, ��������� ��� �������� ����������, ������������ � ������ ���������� 
                                  // ������ �������������� �������������� ���������� (X ����) 
}TFiscDevType;
#pragma pack(pop)
extern TFiscDevType FiscDevInfo;

// �������
extern int FiscPoll(void);
extern int FiscGetFactoryNumber(CPU_INT32U pass, TFiscFactoryNumber* fnum, CPU_INT08U* err);
extern int FiscGetShortStatus(CPU_INT32U pass, TFiscShortStatus* stat, CPU_INT08U* err);
extern int FiscGetFullStatus(CPU_INT32U pass, TFiscFullStatus* stat, CPU_INT08U* err);
extern int FiscPrintBoldString(CPU_INT32U pass, CPU_INT08U flags, CPU_INT08U* str, CPU_INT08U* err);
extern int FiscBeep(CPU_INT32U pass, CPU_INT08U* err);
extern int FiscPrintString(CPU_INT32U pass, CPU_INT08U flags, CPU_INT08U* str, CPU_INT08U* err);
extern int FiscPrintDocHeader(CPU_INT32U pass, CPU_INT08U* str, CPU_INT16U number, CPU_INT08U* err);
extern int FiscGetMoneyReg(CPU_INT32U pass, CPU_INT08U reg, CPU_INT64U* value, CPU_INT08U* err);
extern int FiscGetOperReg(CPU_INT32U pass, CPU_INT08U reg, CPU_INT16U* value, CPU_INT08U* err);
extern int FiscBillCut(CPU_INT32U pass, CPU_INT08U type, CPU_INT08U* err);
extern int FiscPullOutTape(CPU_INT32U pass, CPU_INT08U flags, CPU_INT08U strnum, CPU_INT08U* err);
extern int FiscEjectUnderlayDoc(CPU_INT32U pass, CPU_INT08U dir, CPU_INT08U* err);
extern int FiscPrintStringByFont(CPU_INT32U pass, CPU_INT08U flags, CPU_INT08U font, CPU_INT08U* str, CPU_INT08U* err);
extern int FiscPrintDayReportNoClear(CPU_INT32U admpass, CPU_INT08U* err);
extern int FiscPrintDayReportClear(CPU_INT32U admpass, CPU_INT08U* err);
extern int FiscPrintSectionReport(CPU_INT32U admpass, CPU_INT08U* err);
extern int FiscPrintTaxesReport(CPU_INT32U admpass, CPU_INT08U* err);
extern int FiscMakeDeposit(CPU_INT32U pass, CPU_INT32U sum, CPU_INT16U* doc, CPU_INT08U* err);
extern int FiscMakePayout(CPU_INT32U pass, CPU_INT32U sum, CPU_INT16U* doc, CPU_INT08U* err);
extern int FiscPrintCliche(CPU_INT32U pass, CPU_INT08U* err);
extern int FiscEndDoc(CPU_INT32U pass, CPU_INT08U param, CPU_INT08U* err);
extern int FiscPrintAdvText(CPU_INT32U pass, CPU_INT08U* err);
extern int FiscGetDeviceType(TFiscDevType* dev, CPU_INT08U* err);
extern int FiscPrintContinue(CPU_INT32U pass, CPU_INT08U* err);

extern int FiscOpenBill(CPU_INT32U pass, CPU_INT08U type, CPU_INT08U* err);
extern int FiscMakeSell(CPU_INT32U pass, CPU_INT64U *count, CPU_INT64U *price, CPU_INT08U department, CPU_INT08U* tax, char* text, CPU_INT08U* err);
extern int FiscCloseBill(CPU_INT32U pass, CPU_INT64U *cash, CPU_INT08U* tax, char* text, CPU_INT08U* err);
extern int FiscCanselBill(CPU_INT32U pass, CPU_INT08U* err);

extern int FiscPrintDayReportToBuf(CPU_INT32U admpass, CPU_INT08U* err);
extern int FiscPrintDayReportsFromBuf(CPU_INT32U admpass, CPU_INT08U* err);

extern int FiscPollExt(void);
extern int FiscOpenDay(CPU_INT32U pass, CPU_INT08U* err);

extern int FiscMakeSellV2(CPU_INT32U pass, CPU_INT64U *count, CPU_INT64U *price, CPU_INT08U department, CPU_INT08U* tax, CPU_INT08U subj, char* text, CPU_INT08U* err);
extern int FiscCloseBillV2(CPU_INT32U pass, CPU_INT64U *cash, CPU_INT08U taxsys, char* text, CPU_INT08U* err);
extern int FiscCloseBillV2Online(CPU_INT32U pass, CPU_INT64U *cash, CPU_INT08U taxsys, char* text, CPU_INT08U* err);

extern int FiscWriteTableData(CPU_INT32U pass, CPU_INT08U table, CPU_INT16U row, CPU_INT08U field, CPU_INT08U field_len, CPU_INT08U* data, CPU_INT08U* err);
extern int FiscReadTableData(CPU_INT32U pass, CPU_INT08U table, CPU_INT16U row, CPU_INT08U field, CPU_INT08U field_len, CPU_INT08U* data, CPU_INT08U* err);

#endif //#ifndef _FISCAL_H_
