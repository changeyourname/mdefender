#define _WIN32_IE 0x0500

#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commdlg.h>

#ifndef IDC_STATIC
#define IDC_STATIC (-1)
#endif

#define IDD_SCAN_PROGRESS                       101
#define IDD_SCAN_RESULTS                        103
#define IDD_FILE_SELECTOR                       104
#define IDD_ABOUT                               105
#define IDD_START_UP                            106
#define IDD_MAIN_WIN                            199
#define IDC_LOGO                                1001
#define ID_SCAN_PAUSE                           1002
#define ID_SCAN_CANCEL                          1003
#define IDC_SCAN_PROGRESS                       1004
#define IDC_ACTION                              1005
#define IDC_ACTION_TEXT                         1006
#define IDC_DETECTIONS_COUNT                    1007
#define IDC_SPEED_MBPS                          1008
#define IDC_ETA                                 1009
#define IDC_COMBO_ACTION                        1106
#define IDC_SCAN_RESULTS_LIST_SELECTED_NAME     1109
#define IDC_SCAN_RESULTS_LIST_SELECTED_DESCRIPTION 1110
#define IDC_SCAN_RESULTS_LIST_DETECTIONS        1111
#define IDC_SCAN_RESULTS_APPLY_ACTION                1113
#define IDC_SCAN_RESULTS_CANCEL_APPLY                1114
#define IDC_SR_PIC                              1112
#define IDC_LIST_FILES                          1307
#define IDC_ADD_FILE                            1308
#define IDC_ADD_FOLDER                          1309
#define IDC_REMOVE_SEL                          1310
#define IDC_CONTINUE                            1311
#define IDC_CANCEL                              1312
#define IDC_WEB                                 1401
#define IDC_LICENSE                             1402
#define IDC_DONATE                              1404
#define IDC_ABOUT_PIC                           1405
#define IDC_STARTUP_PIC                         1500
#define IDC_FULL_SCAN                           1991
#define IDC_CUSTOM_SCAN                         1992
#define IDC_UPDATE                              1993
#define IDC_ABOUT                               1994
#define IDC_EXIT                                1995
#define IDC_INFO                                1996
#define IDC_IZAGA                               1997
#define IDC_MAIN_PIC                            1998
#define IDB_LOGO                                4001
#define IDB_BANNER                              4003
#define IDB_ABOUT                               4004
#define IDI_MDEFENDER                           100
#define IDI_ABOUT                               5001
#define IDI_EXIT                                5002
#define IDI_UPDATE                              5003
#define IDI_FULL_SCAN                           5004
#define IDI_CUST_SCAN                           5005
