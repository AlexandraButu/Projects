/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: OnMainPanel */
#define  PANEL_BINARYSWITCH               2       /* control type: binary, callback function: OnTime_Freq */
#define  PANEL_GRAPH_RAW                  3       /* control type: graph, callback function: (none) */
#define  PANEL_B_NEXT                     4       /* control type: command, callback function: OnNext */
#define  PANEL_B_PREV                     5       /* control type: command, callback function: OnPrev */
#define  PANEL_RING_FILTRU                6       /* control type: ring, callback function: (none) */
#define  PANEL_B_APLICA                   7       /* control type: command, callback function: OnAplica */
#define  PANEL_B_LOAD                     8       /* control type: command, callback function: OnLoadButtonCB */
#define  PANEL_GRAPH_HISTOGRAMA           9       /* control type: graph, callback function: (none) */
#define  PANEL_NUMERIC                    10      /* control type: numeric, callback function: AlphaSchimbat */
#define  PANEL_GRAPH_FILTRED              11      /* control type: graph, callback function: (none) */
#define  PANEL_COMMANDBUTTON              12      /* control type: command, callback function: OnAfisareAmvelopa */
#define  PANEL_RING_DIM                   13      /* control type: ring, callback function: (none) */
#define  PANEL_COMMANDBUTTON_2            14      /* control type: command, callback function: OnSave */
#define  PANEL_COMMANDBUTTON_3            15      /* control type: command, callback function: OnDerivata */
#define  PANEL_NUMERIC_STOP               16      /* control type: numeric, callback function: StopValue */
#define  PANEL_NUMERIC_START              17      /* control type: numeric, callback function: StartValue */
#define  PANEL_NUMERIC_TRECERI_0          18      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_DISPERSIE          19      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_MEDIANA            20      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_MEAN               21      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_INDEX_MAXIM        22      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_MAX                23      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_INDEX_MINIM        24      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_MIN                25      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_11                 26      /* control type: numeric, callback function: (none) */
#define  PANEL_OUTPUT_WAVE                27      /* control type: command, callback function: OnWriteOutput */
#define  PANEL_NUMERIC_10                 28      /* control type: numeric, callback function: (none) */

#define  PANEL_2                          2       /* callback function: OnPanel2 */
#define  PANEL_2_BINARYSWITCH             2       /* control type: binary, callback function: OnTime_Freq */
#define  PANEL_2_GRAPH_SPECTRUM           3       /* control type: graph, callback function: (none) */
#define  PANEL_2_RING                     4       /* control type: ring, callback function: OnNrEsantioane */
#define  PANEL_2_NR_FEREASTRA             5       /* control type: numeric, callback function: (none) */
#define  PANEL_2_NUMERIC_F_PEAK           6       /* control type: numeric, callback function: (none) */
#define  PANEL_2_NUMERIC_POWER_PEAK       7       /* control type: numeric, callback function: (none) */
#define  PANEL_2_GRAPH_ROW_FILTRED        8       /* control type: graph, callback function: (none) */
#define  PANEL_2_GRAPH_SPECTRUM_FILTRE    9       /* control type: graph, callback function: (none) */
#define  PANEL_2_GRAPH_ROW_WIN            10      /* control type: graph, callback function: (none) */
#define  PANEL_2_GRAPH_WINDOWING          11      /* control type: graph, callback function: (none) */
#define  PANEL_2_GRAPH_ROW                12      /* control type: graph, callback function: (none) */
#define  PANEL_2_GRAPH_WINDOW             13      /* control type: ring, callback function: OnTipFereastra */
#define  PANEL_2_FILTER_TYPE              14      /* control type: ring, callback function: OnTipFiltru */
#define  PANEL_2_NUMERIC_FPASS            15      /* control type: numeric, callback function: ChangeFpasAndFStop */
#define  PANEL_2_NUMERIC_FSTOP            16      /* control type: numeric, callback function: ChangeFpasAndFStop */
#define  PANEL_2_TEXTMSG                  17      /* control type: textMsg, callback function: (none) */
#define  PANEL_2_TIMER                    18      /* control type: timer, callback function: OnTimer */
#define  PANEL_2_RING_SECOND              19      /* control type: ring, callback function: ChangeSecond */
#define  PANEL_2_COMMANDBUTTON            20      /* control type: command, callback function: SaveFreq */
#define  PANEL_2_RING_TIP_AFISARE         21      /* control type: ring, callback function: tipAfisareSchimbat */
#define  PANEL_2_COMMANDBUTTON_2          22      /* control type: command, callback function: SaveWaveFrequency */
#define  PANEL_2_NUMERIC_ORDER            23      /* control type: numeric, callback function: OnOrder */
#define  PANEL_2_TOGGLEBUTTON             24      /* control type: textButton, callback function: OnStartStop */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK AlphaSchimbat(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ChangeFpasAndFStop(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ChangeSecond(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnAfisareAmvelopa(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnAplica(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDerivata(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLoadButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMainPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnNext(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnNrEsantioane(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnOrder(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanel2(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPrev(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSave(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnStartStop(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTime_Freq(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTipFereastra(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTipFiltru(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnWriteOutput(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SaveFreq(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SaveWaveFrequency(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK StartValue(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK StopValue(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK tipAfisareSchimbat(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif