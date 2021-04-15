#include <includes.h>
#include "app_serv.h"
#include "menu.h"
#include "menudesc.h"
#include "data.h"
#include "datadesc.h"
#include "control.h"
#include "journal.h"
#include "time.h"
#include "mode.h"
#include "version.h"

char FlagForPrintReport=0;

/***********************************
  ��������� ����-��������
***********************************/
const CPU_INT08U str_StartMenu_0[] = "-------------------";
const CPU_INT08U str_StartMenu_1[] = "  ���� ��������";
const CPU_INT08U str_StartMenu_2[] = "  ������������!";
const CPU_INT08U str_StartMenu_3[] = "-------------------";

const TMenuLine line_StartMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_StartMenu_0,         // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_StartMenu_1 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_StartMenu_1,         // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_StartMenu_2 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_StartMenu_2,         // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_StartMenu_3 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_StartMenu_3,         // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};


const TMenuLineArray arr_StartMenuArray[] = {&line_StartMenu_0, &line_StartMenu_1, &line_StartMenu_2, &line_StartMenu_3, NULL};
const TMenuPanel StartMenuPanel[] = {arr_StartMenuArray, NULL, 4, MENU_PANEL_STATIC};

/***********************************
  ���� ����������
***********************************/
const CPU_INT08U str_IncasMenu_0[] = "-------------------";
const CPU_INT08U str_IncasMenu_1[] = "     ��������     ";
const CPU_INT08U str_IncasMenu_2[] = "    ����������    ";
CPU_INT08U str_IncasMenu_3[32];

const TMenuLine line_IncasMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_IncasMenu_0,         // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_IncasMenu_1 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_IncasMenu_1,         // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_IncasMenu_2 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_IncasMenu_2,         // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_IncasMenu_3 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_IncasMenu_3,         // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};


const TMenuLineArray arr_IncasMenuArray[] = {&line_IncasMenu_0, &line_IncasMenu_1, &line_IncasMenu_2, &line_IncasMenu_3, NULL};
const TMenuPanel IncasMenuPanel[] = {arr_IncasMenuArray, NULL, 4, MENU_PANEL_STATIC};

/***********************************
 ���� "� ������� ������� ���"
***********************************/
const CPU_INT08U str_JournalEmptyMenu_0[] = "";
const CPU_INT08U str_JournalEmptyMenu_1[] = "    � �������";
const CPU_INT08U str_JournalEmptyMenu_2[] = "   ������� ���";
const CPU_INT08U str_JournalEmptyMenu_3[] = "";

const TMenuLine line_JournalEmptyMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_JournalEmptyMenu_0,         // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_JournalEmptyMenu_1 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_JournalEmptyMenu_1,         // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_JournalEmptyMenu_2 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_JournalEmptyMenu_2,         // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_JournalEmptyMenu_3 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_JournalEmptyMenu_3,         // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};


const TMenuLineArray arr_JournalEmptyMenuArray[] = {&line_JournalEmptyMenu_0, &line_JournalEmptyMenu_1, &line_JournalEmptyMenu_2, &line_JournalEmptyMenu_3, NULL};
const TMenuPanel JournalEmptyMenuPanel[] = {arr_JournalEmptyMenuArray, NULL, 4, MENU_PANEL_STATIC};

/***********************************
  ��������� ����
***********************************/

const CPU_INT08U str_ServiceMenu_0[] = "������ ���.�� "DEVICE_FW_VERSION;
const CPU_INT08U str_ServiceMenu_1[] = "���������";
const CPU_INT08U str_ServiceMenu_2[] = "����������";
const CPU_INT08U str_ServiceMenu_3[] = "�������";
const CPU_INT08U str_ServiceMenu_4[] = "������";

const TMenuLine line_ServiceMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_ServiceMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ServiceMenu_1 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_ServiceMenu_1,       // ��������� �� ��������� ������ ��� ����������
  (void*)&SettingsMenuPanel                            // ������ ��� ��������
};

const TMenuLine line_ServiceMenu_2 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_ServiceMenu_2,        // ��������� �� ��������� ������ ��� ����������
  (void*)&StatisticsMenuPanel                            // ������ ��� ��������
};

const TMenuLine line_ServiceMenu_3 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_ServiceMenu_3,        // ��������� �� ��������� ������ ��� ����������
  (void*)&SelectJournalMenuPanel                           // ������ ��� ��������
};

const TMenuLine line_ServiceMenu_4 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_ServiceMenu_4,        // ��������� �� ��������� ������ ��� ����������
  (void*)&ReportMenuPanel                           // ������ ��� ��������
};

const TMenuLineArray arr_ServiceMenuArray[] = {&line_ServiceMenu_0, &line_ServiceMenu_1, &line_ServiceMenu_2, &line_ServiceMenu_3, &line_ServiceMenu_4, NULL};
const TMenuPanel ServiceMenuPanel[] = {arr_ServiceMenuArray, NULL, 5, MENU_PANEL_STANDARD};


/***********************************
  ���� ����������
***********************************/

const CPU_INT08U str_StatisticsMenu_0[] = "    ����������";
const CPU_INT08U str_StatisticsMenu_1[] = "�� �������";
const CPU_INT08U str_StatisticsMenu_2[] = "�����";
const CPU_INT08U str_StatisticsMenu_3[] = "�������� �����";
const CPU_INT08U str_StatisticsMenu_4[] = "������� ����������";


const TMenuLine line_StatisticsMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_StatisticsMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_StatisticsMenu_1 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_StatisticsMenu_1,       // ��������� �� ��������� ������ ��� ����������
  (void*)ChanStatMenuPanel                            // ������ ��� ��������
};

const TMenuLine line_StatisticsMenu_2 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_StatisticsMenu_2,        // ��������� �� ��������� ������ ��� ����������
  (void*)CommStatMenuPanel                           // ������ ��� ��������
};

const TMenuLine line_StatisticsMenu_3 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_StatisticsMenu_3,        // ��������� �� ��������� ������ ��� ����������
  (void*)BillCountersPanel                           // ������ ��� ��������
};

const TMenuLine line_StatisticsMenu_4 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_StatisticsMenu_4,        // ��������� �� ��������� ������ ��� ����������
  (void*)ClearStatMenu                           // ������ ��� ��������
};

const TMenuLineArray arr_StatisticsMenuArray[] = {&line_StatisticsMenu_0, &line_StatisticsMenu_1, &line_StatisticsMenu_2, &line_StatisticsMenu_3, &line_StatisticsMenu_4, NULL};
const TMenuPanel StatisticsMenuPanel[] = {arr_StatisticsMenuArray, NULL, 5, MENU_PANEL_STANDARD};


/***********************************
  ���� ������� ����������
***********************************/
const char str_ClearStatMenu_0[] = "   ��� �������";
const char str_ClearStatMenu_1[] = "���������� �������";

void OnEnterPanelClearStat(void)
{
  TempPass = 0;
}

const TMenuLine line_ClearStatMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_ClearStatMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ClearStatMenu_1 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_ClearStatMenu_1,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ClearStatMenu_2 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&PassTempDesc2,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLineArray arr_ClearStatMenuArray[] = {&line_ClearStatMenu_0, &line_ClearStatMenu_1, &line_ClearStatMenu_2, NULL};
const TMenuPanel ClearStatMenu[] = {arr_ClearStatMenuArray, OnEnterPanelClearStat, 3, MENU_PANEL_STANDARD};


/***********************************
���� ������� ��������
***********************************/
const char str_ClearJournalMenu_0[] = "   ��� �������";
const char str_ClearJournalMenu_1[] = " �������� �������";

void OnEnterPanelClearJournal(void)
{
  TempPass = 0;
}

const TMenuLine line_ClearJournalMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_ClearJournalMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ClearJournalMenu_1 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_ClearJournalMenu_1,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ClearJournalMenu_2 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&PassTempDesc2,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLineArray arr_ClearJournalMenuArray[] = {&line_ClearJournalMenu_0, &line_ClearJournalMenu_1, &line_ClearJournalMenu_2, NULL};
const TMenuPanel ClearJournalMenuPanel[] = {arr_ClearJournalMenuArray, OnEnterPanelClearStat, 3, MENU_PANEL_STANDARD};

/***********************************
  ���� ��������� ����������
***********************************/
const TMenuLine line_ChannelCountersMenu_0 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  MENU_FIXED_LINE|MENU_INDEX_LINE,                // ���. �����  
  (void*)&ChannelStIndexDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ChannelCountersMenu_1 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&CounterChannelRunDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ChannelCountersMenu_2 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&CounterChannelMoneyDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ChannelCountersMenu_3 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&CounterChannelTimeDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLineArray arr_ChannelCountersArray[] = {&line_ChannelCountersMenu_0, &line_ChannelCountersMenu_1, &line_ChannelCountersMenu_2, &line_ChannelCountersMenu_3, NULL};
const TMenuPanel ChannelCountersPanel[] = {arr_ChannelCountersArray, NULL, 4, MENU_PANEL_STATIC};


/***********************************
  ���� �������� �����
***********************************/
const TMenuLine line_BillCountersMenu_0 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  MENU_FIXED_LINE|MENU_INDEX_LINE,                // ���. �����  
  (void*)&BillnomIndexDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_BillCountersMenu_1 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&BillnomDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_BillCountersMenu_2 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&BillnomCountersDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_BillCountersMenu_3 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&BillCounterDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

extern CPU_INT32U  BillnomViewIndex;

void OnEnterBillCountersMenu()
{
    BillnomViewIndex = 0;
}

const TMenuLineArray arr_BillCountersArray[] = {&line_BillCountersMenu_0, &line_BillCountersMenu_1, &line_BillCountersMenu_2, &line_BillCountersMenu_3, NULL};
const TMenuPanel BillCountersPanel[] = {arr_BillCountersArray, OnEnterBillCountersMenu, 4, MENU_PANEL_STATIC};


/***********************************
  ���� ����� ����������
***********************************/
const CPU_INT08U str_CommonCountersMenu_0[] = "�������� ��������";

const TMenuLine line_CommonCountersMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)&str_CommonCountersMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_CommonCountersMenu_1 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&CounterRunDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_CommonCountersMenu_2 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&CounterMoneyDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_CommonCountersMenu_3 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&CounterTimeDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLineArray arr_CommonCountersArray[] = {&line_CommonCountersMenu_0, &line_CommonCountersMenu_1, &line_CommonCountersMenu_2, &line_CommonCountersMenu_3, NULL};
const TMenuPanel CommonCountersPanel[] = {arr_CommonCountersArray, NULL, 4, MENU_PANEL_STATIC};

/***********************************
  ���� ����� ���������� ������� ��������
***********************************/
const CPU_INT08U str_CommonCountersLongMenu_0[] = "������� ��������";

const TMenuLine line_CommonCountersLongMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)&str_CommonCountersLongMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_CommonCountersLongMenu_1 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&CounterLongRunDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_CommonCountersLongMenu_2 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&CounterLongMoneyDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_CommonCountersLongMenu_3 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&CounterLongTimeDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLineArray arr_CommonCountersLongArray[] = {&line_CommonCountersLongMenu_0, &line_CommonCountersLongMenu_1, &line_CommonCountersLongMenu_2, &line_CommonCountersLongMenu_3,  NULL};
const TMenuPanel CommonCountersLongPanel[] = {arr_CommonCountersLongArray, NULL, 4, MENU_PANEL_STATIC};

/***********************************
  ���� ���������
***********************************/

const CPU_INT08U str_SettingsMenu_0[] = "    ���������";
const CPU_INT08U str_SettingsMenu_1[] = "������";
const CPU_INT08U str_SettingsMenu_2[] = "������������";
const CPU_INT08U str_SettingsMenu_3[] = "��������� ������";
const CPU_INT08U str_SettingsMenu_4[] = "��������� �������";
const CPU_INT08U str_SettingsMenu_5[] = "����� ��������";

const TMenuLine line_SettingsMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_SettingsMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_SettingsMenu_1 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_SettingsMenu_1,       // ��������� �� ��������� ������ ��� ����������
  (void*)&ChannelMenuPanel                            // ������ ��� ��������
};

const TMenuLine line_SettingsMenu_2 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_SettingsMenu_2,        // ��������� �� ��������� ������ ��� ����������
  (void*)&DeviceMenuPanel                           // ������ ��� ��������
};

const TMenuLine line_SettingsMenu_3 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_SettingsMenu_3,        // ��������� �� ��������� ������ ��� ����������
  (void*)&SetPassMenuPanel                            // ������ ��� ��������
};

const TMenuLine line_SettingsMenu_5 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_SettingsMenu_4,        // ��������� �� ��������� ������ ��� ����������
  (void*)&TimeSetupMenuPanel                           // ������ ��� ��������
};

const TMenuLine line_SettingsMenu_6 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_SettingsMenu_5,        // ��������� �� ��������� ������ ��� ����������
  (void*)&ResetSettingsMenuPanel                           // ������ ��� ��������
};

const TMenuLine line_SettingsMenu_7 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&StartButtonNameDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLineArray arr_SettingsMenuArray[] = {&line_SettingsMenu_0, &line_SettingsMenu_1, &line_SettingsMenu_2, &line_SettingsMenu_5, &line_SettingsMenu_3, &line_SettingsMenu_6, &line_SettingsMenu_7, NULL};
const TMenuPanel SettingsMenuPanel[] = {arr_SettingsMenuArray, NULL, 7, MENU_PANEL_STANDARD};



/***********************************
  ���� ��������� �������
***********************************/
const TMenuLine line_ChannelMenu_0 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  MENU_FIXED_LINE|MENU_INDEX_LINE,                // ���. �����  
  (void*)&ChannelIndexDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ChannelMenu_1 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&EnableChannelDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ChannelMenu_2 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&NameChannelDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ChannelMenu_3 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&TimeOutBeforeDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ChannelMenu_4 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&TimeOutAfterDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ChannelMenu_5 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&MinWorkTimeDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ChannelMenu_6 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&MaxWorkTimeDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ChannelMenu_7 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&WeekEndDesc,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};

const TMenuLine line_ChannelMenu_8 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&DeferredStartDesc,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};

const TMenuLine line_ChannelMenu_9 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  "���� � �����",        // ��������� �� ��������� ������ ��� ����������
  (void*)PriceWeekdaysMenuPanel                           // ������ ��� ��������
};

const TMenuLine line_ChannelMenu_10 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  "���� � ��������",        // ��������� �� ��������� ������ ��� ����������
  (void*)PriceWeekendMenuPanel                           // ������ ��� ��������
};

const TMenuLineArray arr_ChannelMenuArray[] = {&line_ChannelMenu_0,
                                               &line_ChannelMenu_1,
                                               &line_ChannelMenu_2,
                                               &line_ChannelMenu_3, 
                                               &line_ChannelMenu_4, 
                                               &line_ChannelMenu_5, 
                                               &line_ChannelMenu_6, 
                                               &line_ChannelMenu_7, 
                                               &line_ChannelMenu_8,  
                                               &line_ChannelMenu_9, 
                                               &line_ChannelMenu_10, 
                                               NULL};
char flag_enter_periods=0;

void OnEnterChannelSettingsMenu(void)
{
  if (!flag_enter_periods)
    {
      ChannelIndex = 0;
    }
  else
    {
      flag_enter_periods = 0;
    }
}

const TMenuPanel ChannelMenuPanel[] = {arr_ChannelMenuArray, OnEnterChannelSettingsMenu, 11, MENU_PANEL_STANDARD};


/***********************************
  ���� ��������� ���� �� ������
***********************************/
void OnEnterPanelPrice(void)
{
  PeriodIndex = ChannelIndex*PRICE_PERIODS_NUM;
  flag_enter_periods = 1;
}

const TMenuLine line_PriceMenu_0 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  MENU_FIXED_LINE|MENU_INDEX_LINE,                // ���. �����  
  (void*)&PeriodWeekdaysIndexDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_PriceMenu_1 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&T_Start_WeekdaysDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_PriceMenu_2 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&T_End_WeekdaysDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_PriceMenu_3 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&PriceWeekdaysDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_PriceMenu_4 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&PriceTimeWeekdaysDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLineArray arr_PriceMenuArray[] = {&line_PriceMenu_0, &line_PriceMenu_1, &line_PriceMenu_2, &line_PriceMenu_3, &line_PriceMenu_4, NULL};
const TMenuPanel PriceWeekdaysMenuPanel[] = {arr_PriceMenuArray, OnEnterPanelPrice, 5, MENU_PANEL_STANDARD};


/***********************************
  ���� ���� �������� ������
***********************************/
const char str_SetPassMenu_0[] = "  ������� �������";

void OnEnterPanelSetPass(void)
{
  TempPass = 0;
}

const TMenuLine line_SetPassMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_SetPassMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_SetPassMenu_1 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&PassTempDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_SetPassMenu_2 = {
  MENU_LINE_GOTO_MENU,          // ��� ������ ����
  0,                            // ���. �����  
  "���� ������-������",       // ��������� �� ��������� ������ ��� ����������
  (void*)MasterPassMenuPanel                          // ������ ��� ��������
};

const TMenuLineArray arr_SetPassMenuArray[] = {&line_SetPassMenu_0, &line_SetPassMenu_1, &line_SetPassMenu_2, NULL};
const TMenuPanel SetPassMenuPanel[] = {arr_SetPassMenuArray, OnEnterPanelSetPass, 3, MENU_PANEL_STANDARD};

/***********************************
  ���� ���� ������-������
***********************************/
const char str_MasterPassMenu_0[] = "������� ������-";

void OnEnterPanelMasterPass(void)
{
  TempPass = 0;
}

const TMenuLine line_MasterPassMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_MasterPassMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_MasterPassMenu_1 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&MasterPassTempDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLineArray arr_MasterPassMenuArray[] = {&line_MasterPassMenu_0, &line_MasterPassMenu_1, NULL};
const TMenuPanel MasterPassMenuPanel[] = {arr_MasterPassMenuArray, OnEnterPanelSetPass, 3, MENU_PANEL_STANDARD};

/***********************************
  ���� ���� �������� ������
***********************************/
const char str_ResetSetingsMenu_0[] = "    ��� ������";
const char str_ResetSetingsMenu_1[] = " �������� �������";

void OnEnterPanelResetSetings(void)
{
  TempPass = 0;
}

const TMenuLine line_ResetSetingsMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_ResetSetingsMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ResetSetingsMenu_1 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_ResetSetingsMenu_1,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ResetSetingsMenu_2 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&PassTempDesc1,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLineArray arr_ResetSettingsMenuArray[] = {&line_ResetSetingsMenu_0, &line_ResetSetingsMenu_1, &line_ResetSetingsMenu_2, NULL};
const TMenuPanel ResetSettingsMenuPanel[] = {arr_ResetSettingsMenuArray, OnEnterPanelResetSetings, 3, MENU_PANEL_STANDARD};

/***********************************
  ���� ���� ������ ������
***********************************/
const char str_SetNewPassMenu_0[] = "  ������� �����";

const TMenuLine line_SetNewPassMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_SetNewPassMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_SetNewPassMenu_1 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&PassDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLineArray arr_SetNewPassMenuArray[] = {&line_SetNewPassMenu_0, &line_SetNewPassMenu_1, NULL};
const TMenuPanel SetNewPassMenuPanel[] = {arr_SetNewPassMenuArray, NULL, 2, MENU_PANEL_STANDARD};

/***********************************
  ���� ��������� ���� �� ��������
***********************************/
const TMenuLine line_PriceMenuWend_0 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  MENU_FIXED_LINE|MENU_INDEX_LINE,                // ���. �����  
  (void*)&PeriodWeekendIndexDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_PriceMenuWend_1 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&T_Start_WeekendDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_PriceMenuWend_2 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&T_End_WeekendDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_PriceMenuWend_3 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&PriceWeekendDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_PriceMenuWend_4 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&PriceTimeWeekendDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLineArray arr_PriceMenuArrayWend[] = {&line_PriceMenuWend_0, &line_PriceMenuWend_1, &line_PriceMenuWend_2, &line_PriceMenuWend_3, &line_PriceMenuWend_4, NULL};
const TMenuPanel PriceWeekendMenuPanel[] = {arr_PriceMenuArrayWend, OnEnterPanelPrice, 5, MENU_PANEL_STANDARD};


/***********************************
  ���� ��������� ������������
***********************************/

const CPU_INT08U str_DeviceMenu_0[] = "   ������������";
const CPU_INT08U str_DeviceMenu_1[] = "��";
const CPU_INT08U str_DeviceMenu_2[] = "��������������";
const CPU_INT08U str_DeviceMenu_3[] = "�����";

const TMenuLine line_DeviceMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_DeviceMenu_0,        // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_DeviceMenu_1 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_DeviceMenu_1,       // ��������� �� ��������� ������ ��� ����������
  (void*)&FrMenuPanel                            // ������ ��� ��������
};

const TMenuLine line_DeviceMenu_2 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&EnableValidatorDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_DeviceMenu_3 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_DeviceMenu_2,       // ��������� �� ��������� ������ ��� ����������
  (void*)&CoinSetupPanel                            // ������ ��� ��������
};

const TMenuLine line_DeviceMenu_4 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_DeviceMenu_3,       // ��������� �� ��������� ������ ��� ����������
  (void*)&ModemSetupPanel                            // ������ ��� ��������
};

const TMenuLineArray arr_DeviceMenuArray[] = {&line_DeviceMenu_0, &line_DeviceMenu_1, &line_DeviceMenu_2, &line_DeviceMenu_3, &line_DeviceMenu_4, NULL};
const TMenuPanel DeviceMenuPanel[] = {arr_DeviceMenuArray, NULL, 5, MENU_PANEL_STANDARD};


/***********************************
  ���� ��������� ��
***********************************/
const CPU_INT08U str_FrMenu_0[] = "   ��������� ��";

const TMenuLine line_FrMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_FrMenu_0,        // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_FrMenu_1 = {
  MENU_LINE_SHOW_DESC, 
  0,                   
  (void*)&EnableFiscalDesc,
  NULL                     
};

const TMenuLine line_FrMenu_2 = {
  MENU_LINE_SHOW_DESC,               // ??? ?????? ????
  0,                              // ???. ?????  
  (void*)&EnableFiscalDayClearDesc,       // ????????? ?? ????????? ?????? ??? ??????????
  NULL                            // ?????? ??? ????????
};

const TMenuLine line_FrMenu_3 = {
  MENU_LINE_SHOW_DESC,               // ??? ?????? ????
  0,                              // ???. ?????  
  (void*)&BillFormatDesc,       // ????????? ?? ????????? ?????? ??? ??????????
  NULL                            // ?????? ??? ????????
};

const TMenuLine line_FrMenu_4 = {
  MENU_LINE_SHOW_DESC,               // ??? ?????? ????
  0,                              // ???. ?????  
  (void*)&ServiceNameDesc,       // ????????? ?? ????????? ?????? ??? ??????????
  NULL                            // ?????? ??? ????????
};

const TMenuLine line_FrMenu_5 = {
  MENU_LINE_SHOW_DESC, 
  0,                   
  (void*)&DisableFiscalErrorsDesc,
  NULL                     
};

const TMenuLineArray arr_FrMenuArray[] = {&line_FrMenu_0, &line_FrMenu_1, &line_FrMenu_2, &line_FrMenu_3, &line_FrMenu_4, &line_FrMenu_5, NULL};
const TMenuPanel FrMenuPanel[] = {arr_FrMenuArray, NULL, 6, MENU_PANEL_STANDARD};

/***********************************
  ���� ��������� ���������������
***********************************/
const CPU_INT08U str_CoinMenu_0[] = "��������� ��������.";

const TMenuLine line_CoinMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_CoinMenu_0,        // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_CoinMenu_1 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&EnableCoinDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_CoinMenu_2 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&CoinPerPulseDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLineArray arr_CoinMenuArray[] = {&line_CoinMenu_0, &line_CoinMenu_1, &line_CoinMenu_2, NULL};
const TMenuPanel CoinSetupPanel[] = {arr_CoinMenuArray, NULL, 3, MENU_PANEL_STANDARD};

/***********************************
  ���� ��������� ������
***********************************/
const CPU_INT08U str_ModemMenu_0[] = " ��������� ������";

const TMenuLine line_ModemMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_ModemMenu_0,        // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ModemMenu_1 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&EnableModemDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ModemMenu_2 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&ModemStatusDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ModemMenu_3 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&StatSendHourMinDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ModemMenu_4 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&EnableEmailJournalSendDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ModemMenu_5 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&DeviceIDDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ModemMenu_6 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&SendTestEmailDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};


const TMenuLineArray arr_ModemMenuArray[] = {&line_ModemMenu_0, &line_ModemMenu_1, &line_ModemMenu_2, &line_ModemMenu_3, &line_ModemMenu_4, &line_ModemMenu_5, &line_ModemMenu_6, NULL};
const TMenuPanel ModemSetupPanel[] = {arr_ModemMenuArray, NULL, 7, MENU_PANEL_STANDARD};

/***********************************
  �������� ��� �������
***********************************/

char str_UserMenu_0[22] = "";
char str_UserMenu_1[22] = "";
char str_UserMenu_2[22] = "";
char str_UserMenu_3[22] = "";

char str_buf[22];


void PrintUserMenuStr(char* str, CPU_INT08U n)
{
  char *strptr;
  char *instr;
  // ��������� �� ������ ��� �������
    
  switch (n)
  {
    case 0:
      strptr = str_UserMenu_0;
      break;
    case 1:
      strptr = str_UserMenu_1;
      break;
    case 2:
      strptr = str_UserMenu_2;
      break;
    case 3:
      strptr = str_UserMenu_3;
      break;
    default:
      return;
  }

  // ������ ������ ������, �������� �� �������
  instr = str;
  while (*instr==0x20) instr++;
  
  memset(strptr, 0x20, 20);
  
  int len = strlen(instr);
  if ((len >= 20) || ((10-len/2-1) < 0)) {strcpy(strptr, instr); return;}

  strcpy(&strptr[10-len/2-1], instr);
}

const TMenuLine line_FirstMenu_0 = {
  MENU_LINE_STRING,                 // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_UserMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_FirstMenu_1 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_UserMenu_1,        // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_FirstMenu_2 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_UserMenu_2,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};

const TMenuLine line_FirstMenu_3 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_UserMenu_3,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};


const TMenuLineArray FirstMenuArray[] = {&line_FirstMenu_0, &line_FirstMenu_1, &line_FirstMenu_2, &line_FirstMenu_3, NULL};
const TMenuPanel FirstMenuPanel[] = {FirstMenuArray, InitUserMenu, 4, MENU_PANEL_STATIC};

/***********************************
  ������ "�������� ������"
***********************************/
const CPU_INT08U str_ErrPass_0[] = "     ������ ";
const CPU_INT08U str_ErrPass_1[] = " �������� ������";
const CPU_INT08U str_ErrPass_2[] = "";

const TMenuLine line_ErrPassMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                // ���. �����  
  (void*)str_ErrPass_0,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};

const TMenuLine line_ErrPassMenu_1 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                // ���. �����  
  (void*)str_ErrPass_1,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};
const TMenuLine line_ErrPassMenu_2 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                // ���. �����  
  (void*)str_ErrPass_2,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};

const TMenuLineArray ErrPassMenuArray[] = {&line_ErrPassMenu_2, &line_ErrPassMenu_0, &line_ErrPassMenu_1, &line_ErrPassMenu_2, NULL};
const TMenuPanel ErrorPassPanel[] = {ErrPassMenuArray, NULL, 4, MENU_PANEL_STATIC};


/***********************************
  ������ "��������� ��������"
***********************************/
const CPU_INT08U str_SettingsIsResetPass_0[] = "   �����������  ";
const CPU_INT08U str_SettingsIsResetPass_1[] = "    ��������� ";
const CPU_INT08U str_SettingsIsResetPass_2[] = "     ��������";
const CPU_INT08U str_SettingsIsResetPass_3[] = "    ����������";

const TMenuLine line__SettingsIsResetMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                // ���. �����  
  (void*)str_SettingsIsResetPass_0,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};

const TMenuLine line__SettingsIsResetMenu_1 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                // ���. �����  
  (void*)str_SettingsIsResetPass_1,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};
const TMenuLine line__SettingsIsResetMenu_2 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                // ���. �����  
  (void*)str_SettingsIsResetPass_2,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};

const TMenuLine line__SettingsIsResetMenu_3 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                // ���. �����  
  (void*)str_SettingsIsResetPass_3,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};

const TMenuLineArray SettingsIsResetMenuArray[] = {&line__SettingsIsResetMenu_0, &line__SettingsIsResetMenu_1, &line__SettingsIsResetMenu_2, &line__SettingsIsResetMenu_3, NULL};
const TMenuPanel SettingsIsReset[] = {SettingsIsResetMenuArray, NULL, 4, MENU_PANEL_STATIC};


/***********************************
  ������ "���������� �������"
***********************************/
const CPU_INT08U str_StatIsResetPass_0[] = "    ����������";
const CPU_INT08U str_StatIsResetPass_1[] = "     �������";
const CPU_INT08U str_StatIsResetPass_2[] = "";

const TMenuLine line__StatIsResetMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                // ���. �����  
  (void*)str_StatIsResetPass_0,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};

const TMenuLine line__StatIsResetMenu_1 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                // ���. �����  
  (void*)str_StatIsResetPass_1,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};
const TMenuLine line__StatIsResetMenu_2 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                // ���. �����  
  (void*)str_StatIsResetPass_2,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};

const TMenuLineArray StatIsResetMenuArray[] = {&line__StatIsResetMenu_2, &line__StatIsResetMenu_0, &line__StatIsResetMenu_1, &line__StatIsResetMenu_2, NULL};
const TMenuPanel StatIsReset[] = {StatIsResetMenuArray, NULL, 4, MENU_PANEL_STATIC};


/***********************************
������ "������� �������"
***********************************/
const CPU_INT08U str_JournalIsResetPass_0[] = "     ������� ";
const CPU_INT08U str_JournalIsResetPass_1[] = "     �������";
const CPU_INT08U str_JournalIsResetPass_2[] = "";

const TMenuLine line__JournalIsResetMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                // ���. �����  
  (void*)str_JournalIsResetPass_0,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};

const TMenuLine line__JournalIsResetMenu_1 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                // ���. �����  
  (void*)str_JournalIsResetPass_1,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};
const TMenuLine line__JournalIsResetMenu_2 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                // ���. �����  
  (void*)str_JournalIsResetPass_2,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};

const TMenuLineArray JournalIsResetMenuArray[] = {&line__JournalIsResetMenu_2, &line__JournalIsResetMenu_0, &line__JournalIsResetMenu_1, &line__JournalIsResetMenu_2, NULL};
const TMenuPanel JournalIsReset[] = {JournalIsResetMenuArray, NULL, 4, MENU_PANEL_STATIC};

/***********************************
  ������ �������� �����
***********************************/

const CPU_INT08U str_GetMoney_0[] = " ������� ������";

const TMenuLine line_GetMoneyMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_GetMoney_0,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};

const TMenuLine line_GetMoneyMenu_1 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)&AcceptedMoneyDesc,        // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_GetMoneyMenu_2 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_GetMoney_0,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};

const TMenuLine line_GetMoneyMenu_3 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_GetMoney_0,        // ��������� �� ��������� ������ ��� ����������
  NULL                           // ������ ��� ��������
};


const TMenuLineArray GetMoneyMenuArray[] = {&line_GetMoneyMenu_0, &line_GetMoneyMenu_1, &line_GetMoneyMenu_2, &line_GetMoneyMenu_3, NULL};
const TMenuPanel GetMoneyMenuPanel[] = {GetMoneyMenuArray, NULL, 4, MENU_PANEL_STATIC};


/***********************************
  ���� ����� ������� ��� ���������
***********************************/
const CPU_INT08U str_SelectJournalMenu_0[] = "�������� ��������";
const CPU_INT08U str_SelectJournalMenu_1[] = "������ ������";
const CPU_INT08U str_SelectJournalMenu_2[] = "������ �������";
const CPU_INT08U str_SelectJournalMenu_3[] = "������� ��������";

const TMenuLine line_SelectJournalMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_SelectJournalMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_SelectJournalMenu_1 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_SelectJournalMenu_1,       // ��������� �� ��������� ������ ��� ����������
  (void*)&ErrorJournalMenuPanel                            // ������ ��� ��������
};

const TMenuLine line_SelectJournalMenu_2 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_SelectJournalMenu_2,        // ��������� �� ��������� ������ ��� ����������
  (void*)&EventJournalMenuPanel                            // ������ ��� ��������
};

const TMenuLine line_SelectJournalMenu_3 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_SelectJournalMenu_3,        // ��������� �� ��������� ������ ��� ����������
  (void*)&ClearJournalMenuPanel                            // ������ ��� ��������
};

const TMenuLineArray arr_SelectJournalMenuArray[] = {&line_SelectJournalMenu_0, &line_SelectJournalMenu_1, &line_SelectJournalMenu_2, &line_SelectJournalMenu_3, NULL};
const TMenuPanel SelectJournalMenuPanel[] = {arr_SelectJournalMenuArray, NULL, 4, MENU_PANEL_STANDARD};

/***********************************
  ���� ������ �������
***********************************/
const CPU_INT08U str_ReportMenu_0[] = "      ������";
const CPU_INT08U str_ReportMenu_1[] = "X-�����";
const CPU_INT08U str_ReportMenu_2[] = "Z-�����";
const CPU_INT08U str_ReportMenu_3[] = "Z-����� �� ������";

const TMenuLine line_ReportMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_ReportMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ReportMenu_1 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_ReportMenu_1,       // ��������� �� ��������� ������ ��� ����������
  (void*)&xReportMenuPanel                            // ������ ��� ��������
};

const TMenuLine line_ReportMenu_2 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_ReportMenu_2,        // ��������� �� ��������� ������ ��� ����������
  (void*)&zReportMenuPanel                            // ������ ��� ��������
};

const TMenuLine line_ReportMenu_3 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_ReportMenu_3,        // ��������� �� ��������� ������ ��� ����������
  (void*)&bufReportMenuPanel                            // ������ ��� ��������
};


void OnEnterReportsPanel(void)
{
  CPU_INT32U enable;
  GetData(&EnableFiscalDesc, &enable, 0, DATA_FLAG_SYSTEM_INDEX);  
  if (!enable) 
    {
      GoToPreviousMenu();
      GoToMenu(FrIsOffMenuPanel);
    }
  FlagForPrintReport = 0;
}

const TMenuLineArray arr_ReportMenuArray[] = {&line_ReportMenu_0, &line_ReportMenu_1, &line_ReportMenu_2, &line_ReportMenu_3, NULL};
const TMenuPanel ReportMenuPanel[] = {arr_ReportMenuArray, OnEnterReportsPanel, 4, MENU_PANEL_STANDARD};

/***********************************
  ���� ������ X-������
***********************************/
const CPU_INT08U str_xReportMenu_0[] = "     X-�����";
const CPU_INT08U str_xReportMenu_1[] = "���������� ��������";
const CPU_INT08U str_xReportMenu_2[] = "����� ��� �������?";
const CPU_INT08U str_xReportMenu_3[] = "��-START   STOP-���";

const TMenuLine line_xReportMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_xReportMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_xReportMenu_1 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_xReportMenu_1,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_xReportMenu_2 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_xReportMenu_2,        // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_xReportMenu_3 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_xReportMenu_3,        // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

void OnEnterZXReportPanel(void)
{
  OSTimeDly(20);
  FlagForPrintReport = 1;
}

const TMenuLineArray arr_xReportMenuArray[] = {&line_xReportMenu_0, &line_xReportMenu_1, &line_xReportMenu_2, &line_xReportMenu_3, NULL};
const TMenuPanel xReportMenuPanel[] = {arr_xReportMenuArray, OnEnterZXReportPanel, 4, MENU_PANEL_STATIC};


/***********************************
  ���� ������ Z-������
***********************************/
const CPU_INT08U str_zReportMenu_0[] = "     Z-�����";
const CPU_INT08U str_zReportMenu_1[] = "���������� ��������";
const CPU_INT08U str_zReportMenu_2[] = "����� � ��������?";

const TMenuLine line_zReportMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_zReportMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_zReportMenu_1 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_zReportMenu_1,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_zReportMenu_2 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_zReportMenu_2,        // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_zReportMenu_3 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_xReportMenu_3,        // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLineArray arr_zReportMenuArray[] = {&line_zReportMenu_0, &line_zReportMenu_1, &line_zReportMenu_2, &line_zReportMenu_3, NULL};
const TMenuPanel zReportMenuPanel[] = {arr_zReportMenuArray, OnEnterZXReportPanel, 4, MENU_PANEL_STATIC};


/***********************************
  ���� ������ Z-������� �� ������
***********************************/
const CPU_INT08U str_bufReportMenu_0[] = " Z-������ �� ������";
const CPU_INT08U str_bufReportMenu_1[] = "���������� ��������";
const CPU_INT08U str_bufReportMenu_2[] = " ������ �� ������?";

const TMenuLine line_bufReportMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_bufReportMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_bufReportMenu_1 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_bufReportMenu_1,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_bufReportMenu_2 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_bufReportMenu_2,        // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_bufReportMenu_3 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_xReportMenu_3,        // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLineArray arr_bufReportMenuArray[] = {&line_bufReportMenu_0, &line_bufReportMenu_1, &line_bufReportMenu_2, &line_xReportMenu_3, NULL};
const TMenuPanel bufReportMenuPanel[] = {arr_bufReportMenuArray, OnEnterZXReportPanel, 4, MENU_PANEL_STATIC};

/***********************************
  ���� �� ��������
***********************************/
const CPU_INT08U str_FrIsOff_0[] = "";
const CPU_INT08U str_FrIsOff_1[] = "   �� ��������";
const CPU_INT08U str_FrIsOff_2[] = "������ ����������";

const TMenuLine line_FrIsOff_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_FrIsOff_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_FrIsOff_1 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_FrIsOff_1,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_FrIsOff_2 = {
  MENU_LINE_STRING,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_FrIsOff_2,        // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLineArray arr_FrIsOffMenuArray[] = {&line_FrIsOff_0, &line_FrIsOff_1, &line_FrIsOff_2, NULL};
const TMenuPanel FrIsOffMenuPanel[] = {arr_FrIsOffMenuArray, NULL, 3, MENU_PANEL_STATIC};


/***********************************
  ���� �������� ������� ������
***********************************/
const TMenuLine line_ErrorJournalMenu_0 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  MENU_FIXED_LINE|MENU_INDEX_LINE,                // ���. �����  
  (void*)&ErrorJournalIndexDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ErrorJournalMenu_1 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&JournalErrorNumberDesc0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ErrorJournalMenu_2 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&JournalErrorNumberDesc1,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ErrorJournalMenu_3 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&JournalErrorTimeDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

void OnEnterErrorJournal(void)
{
  TErrorRecord record;
  ErrorJournalIndex = 0;
  CPU_INT32U last = 0;
  CPU_INT32U i;
  
  for (i=0; i<ERROR_RECORDS_NUM; i++)
    {
      GetErrorRecord(&record, i);
      if (record.time >= last)
        {
          last = record.time;
          ErrorJournalIndex = i;
        }
    }
  if (!last) {SetMenu(JournalEmptyMenuPanel); return;}
}

const TMenuLineArray arr_ErrorJournalMenuArray[] = {&line_ErrorJournalMenu_0, &line_ErrorJournalMenu_1, &line_ErrorJournalMenu_2, &line_ErrorJournalMenu_3, NULL};
const TMenuPanel ErrorJournalMenuPanel[] = {arr_ErrorJournalMenuArray, OnEnterErrorJournal, 4, MENU_PANEL_STATIC};


/***********************************
  ���� �������� ������� �������
***********************************/
char str_EventNumber[24];
char str_EventData[24];

const TMenuLine line_EventJournalMenu_0 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  MENU_FIXED_LINE|MENU_INDEX_LINE,                // ���. �����  
  (void*)&EventJournalIndexDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_EventJournalMenu_1 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  0,                // ���. �����  
  (void*)&JournalEventTimeDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_EventJournalMenu_2 = {
  MENU_LINE_STRING,                    // ��� ������ ����
  0,                                   // ���. �����  
  (void*)str_EventNumber,      // ��������� �� ��������� ������ ��� ����������
  NULL                                 // ������ ��� ��������
};

const TMenuLine line_EventJournalMenu_3 = {
  MENU_LINE_STRING,                    // ��� ������ ����
  0,                                   // ���. �����  
  (void*)str_EventData,      // ��������� �� ��������� ������ ��� ����������
  NULL                                 // ������ ��� ��������
};


void PrintEventJournalRecord(TEventRecord *record)
{
  if (record->event)
    { // ���� �������
      GetEventStr(str_EventNumber, record->event);
      if ((record->event == JOURNAL_EVENT_MONEY_NOTE) || (record->event == JOURNAL_EVENT_MONEY_COIN))
        {
          sprintf(&str_EventNumber[strlen(str_EventNumber)], "���.%d", record->channel+1);
          sprintf(str_EventData, "%d ���.", record->data);
        }
      else if (record->event == JOURNAL_EVENT_START_SESSION)
        {
          sprintf(&str_EventNumber[strlen(str_EventNumber)], "���.%d", record->channel+1);
          PrintSecToHourMinSec(str_EventData, record->data);
        }
      else if (record->event == JOURNAL_EVENT_END_SESSION)
        {
          sprintf(&str_EventNumber[strlen(str_EventNumber)], "���.%d", record->channel+1);
          sprintf(str_EventData, "");
        }
      else if (record->event == JOURNAL_EVENT_DEVICE_ON)
        {
          sprintf(str_EventData, "");
        }
      else if (record->event == JOURNAL_EVENT_PRINT_BILL)
        {
          sprintf(str_EventData, "����� %d", record->channel+1);
        }
      else if (record->event == JOURNAL_EVENT_PRINT_Z)
        {
          sprintf(str_EventData, "");
        }
      else if (record->event == JOURNAL_EVENT_PRINT_X)
        {
          sprintf(str_EventData, "");
        }
      else if (record->event == JOURNAL_EVENT_PRINT_BUF)
        {
          sprintf(str_EventData, "");
        }
      else if (record->event == JOURNAL_EVENT_CHANGE_MODE)
        {
          if (record->data == MODE_WORK) sprintf(str_EventData, "������");
          else sprintf(str_EventData, "���������");
        }
      else if (record->event == JOURNAL_EVENT_INCASSATION)
        {
            sprintf(str_EventData, "%u ���.", record->data);
        }
      else if (record->event == JOURNAL_EVENT_PASS_FAIL)
        {
            sprintf(str_EventData, "%u", record->data);
        }
      else if ((record->event == JOURNAL_EVENT_EMAIL_OK) || (record->event == JOURNAL_EVENT_EMAIL_FAIL))
        {
            sprintf(str_EventData, "");
        }
      
    }
  else
    { // ������ ������
      sprintf(str_EventNumber, "�����");
      sprintf(str_EventData, "�����");
    }
}

void OnEnterEventJournal(void)
{
  TEventRecord record;
  EventJournalIndex = 0;
  CPU_INT32U last = 0;
  
  for (CPU_INT32U i=0; i<EVENT_RECORDS_NUM; i++)
    {
      GetEventRecord(&record, i);
      if (record.time >= last)
        {
          last = record.time;
          EventJournalIndex = i;
        }
    }
  if (!last) {SetMenu(JournalEmptyMenuPanel); return;}
    
  GetEventRecord(&record, EventJournalIndex);
  PrintEventJournalRecord(&record);
}

const TMenuLineArray arr_EventJournalMenuArray[] = {&line_EventJournalMenu_0, &line_EventJournalMenu_1, &line_EventJournalMenu_2, &line_EventJournalMenu_3 ,NULL};
const TMenuPanel EventJournalMenuPanel[] = {arr_EventJournalMenuArray, OnEnterEventJournal, 4, MENU_PANEL_STATIC};


/***********************************
  ���� ��������� �������
***********************************/

const CPU_INT08U str_TimeSetupMenu_0[] = "��������� �������";

const TMenuLine line_TimeSetupMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_TimeSetupMenu_0,        // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_TimeSetupMenu_1 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&SystemTimeEditDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLineArray arr_TimeSetupMenuArray[] = {&line_TimeSetupMenu_0, &line_TimeSetupMenu_1, NULL};
const TMenuPanel TimeSetupMenuPanel[] = {arr_TimeSetupMenuArray, NULL, 2, MENU_PANEL_STANDARD};


/***********************************
  ���� ����� ����������
***********************************/
const CPU_INT08U str_CommonStatMenu_0[] = "  ����� ����������";
const CPU_INT08U str_CommonStatMenu_1[] = "�������� ��������";
const CPU_INT08U str_CommonStatMenu_2[] = "������� ��������";

const TMenuLine line_StatMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_CommonStatMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_StatMenu_1 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_CommonStatMenu_1,       // ��������� �� ��������� ������ ��� ����������
  (void*)CommonCountersPanel                            // ������ ��� ��������
};

const TMenuLine line_StatMenu_2 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_CommonStatMenu_2,        // ��������� �� ��������� ������ ��� ����������
  (void*)CommonCountersLongPanel                           // ������ ��� ��������
};

const TMenuLineArray arr_StatMenuArray[] = {&line_StatMenu_0, &line_StatMenu_1, &line_StatMenu_2, NULL};
const TMenuPanel CommStatMenuPanel[] = {arr_StatMenuArray, NULL, 3, MENU_PANEL_STANDARD};


/***********************************
  ���� ��������� ���������� ����������
***********************************/
const CPU_INT08U str_ChanStatMenu_0[] = " ����-�� �� �������";

const TMenuLine line_ChanStatMenu_0 = {
  MENU_LINE_STRING,               // ��� ������ ����
  MENU_FIXED_LINE,                // ���. �����  
  (void*)str_ChanStatMenu_0,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ChanStatMenu_1 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_CommonStatMenu_1,       // ��������� �� ��������� ������ ��� ����������
  (void*)ChannelCountersPanel                            // ������ ��� ��������
};

const TMenuLine line_ChanStatMenu_2 = {
  MENU_LINE_GOTO_MENU,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)str_CommonStatMenu_2,        // ��������� �� ��������� ������ ��� ����������
  (void*)ChannelCountersLongPanel                           // ������ ��� ��������
};

const TMenuLineArray arr_ChanStatMenuArray[] = {&line_ChanStatMenu_0, &line_ChanStatMenu_1, &line_ChanStatMenu_2, NULL};
const TMenuPanel ChanStatMenuPanel[] = {arr_ChanStatMenuArray, NULL, 3, MENU_PANEL_STANDARD};

/***********************************
  ���� ��������� ���������� ������� ��������
***********************************/
const TMenuLine line_ChannelCountersLongMenu_0 = {
  MENU_LINE_SHOW_DESC,                 // ��� ������ ����
  MENU_FIXED_LINE|MENU_INDEX_LINE,                // ���. �����  
  (void*)&ChannelStLongIndexDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ChannelCountersLongMenu_1 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&CounterChannelRunLongDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ChannelCountersLongMenu_2 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&CounterChannelMoneyLongDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLine line_ChannelCountersLongMenu_3 = {
  MENU_LINE_SHOW_DESC,               // ��� ������ ����
  0,                              // ���. �����  
  (void*)&CounterChannelTimeLongDesc,       // ��������� �� ��������� ������ ��� ����������
  NULL                            // ������ ��� ��������
};

const TMenuLineArray arr_ChannelCountersLongArray[] = {&line_ChannelCountersLongMenu_0, &line_ChannelCountersLongMenu_1, &line_ChannelCountersLongMenu_2, &line_ChannelCountersLongMenu_3, NULL};
const TMenuPanel ChannelCountersLongPanel[] = {arr_ChannelCountersLongArray, NULL, 4, MENU_PANEL_STATIC};
