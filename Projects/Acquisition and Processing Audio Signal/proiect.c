#include <ansi_c.h>
#include <formatio.h>
#include <cvirte.h>		
#include <userint.h>
#include <stdio.h>
#include <stdlib.h>
#include <analysis.h>
#include <utility.h>
#include "proiect.h"


#define SAMPLE_RATE		0	//pe pozitia 0 in waveInfo este frecventa de esantionare
#define NPOINTS			1	//pe pozitia 1 in waveInfo este nr de puncte


static int panelHandle;	//panoul pentru procesare in domneniul timp
static int panelHandle2; //panoul pentru procesare in frecventa

int waveInfo[2]; 	
					
int npoints = 0;	//numarul de esantioane
int secValStanga = 0;	//secunda la care incepe afisarea	
int secValDreapta = 6;	//secunda la care se termina afisarea
int dimFereastra = 16;	//dimensiunea la care se face filtrarea prin mediere
int numarDepuncte = 2048;	//numarul de puncte pentru ferestruire/filtrare in frecventa (DIMENSIUNEA FERESTREI DE TIMP)
int cadru = 0;// specifica intervalul din semnalul de intrare 
//in frecventa, se afiseaza pe dimFerestra puncte, cadrul este folosit pentru a spune ce interval se afiseaza
int numberOfCoeffs = 240;	//pentru filtru cu raspuns infinit

double sampleRate = 0.0;	//recventa de esantioanare
double alpha = 0.0;	//parametru pentru filtrul de ordin I
double* waveDataFiltrat = 0;	//datele filtrate in timp
double* derivata = 0;	//vectorul cu derivata semnalului
double* waveData = 0;	//datele de intare
double* amvelopa;	//vectorul cu valorile amvelopei
double* filteredSignal = 0;	//vectorul cu datele filtrate in frecventa

//pentru salvari, scriu doar daca am aflat datele
BOOL flag_derivataCalculata = 0;
BOOL flag_dateCitite = 0;
BOOL flag_dateFiltrate = 0;
BOOL afisareAmvelopa = 0;
BOOL flag_dateDerivate = 0;
BOOL filtrareInFrecventa = 0;

int main(int argc, char* argv[])
{
	if (InitCVIRTE(0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel(0, "proiect.uir", PANEL)) < 0)
		return -1;
	if ((panelHandle2 = LoadPanel(0, "proiect.uir", PANEL_2)) < 0)
		return -1;
	
	DisplayPanel(panelHandle);
	RunUserInterface();
	
	if (panelHandle > 0)
		DiscardPanel(panelHandle);

	free(waveData);
	free(waveDataFiltrat);
	free(derivata);
	free(amvelopa);
	free(filteredSignal);

	return 0;
}

/*La schimbarea intervalului de afisare se va modifica graficul!*/
void afisareSemnal(int panel)
{
	if (flag_dateCitite == 1) {
		DeleteGraphPlot(panel, PANEL_GRAPH_RAW, -1, VAL_IMMEDIATE_DRAW);	/*-1 se sterg toate graficele, fara intarziere*/

		int indexStart = (int)secValStanga * npoints / 6;
		int indexStop = (int)secValDreapta * npoints / 6;

		PlotY(panel, PANEL_GRAPH_RAW, waveData + indexStart, indexStop - indexStart, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS,  VAL_RED);
	}
}

void afisareSemnalFiltrat(int panel)
{
	if (flag_dateCitite == 1 && flag_dateFiltrate == 1) {
		/*int DeleteGraphPlot (int panelHandle, int controlID, int plotHandle, int refresh);*/
		DeleteGraphPlot(panel, PANEL_GRAPH_FILTRED, -1, VAL_IMMEDIATE_DRAW);	/*-1 se sterg toate graficele, fara intarziere*/

		int indexStart = (int)secValStanga * npoints / 6;
		int indexStop = (int)secValDreapta * npoints / 6;

		/*int PlotY (int panelHandle, int controlID, void *yArray, size_t numberOfPoints, int yDataType, int plotStyle, int pointStyle, int lineStyle, int pointFrequency, int color);*/
		PlotY(panel, PANEL_GRAPH_FILTRED, waveDataFiltrat + indexStart, indexStop - indexStart, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
	}
}

void afisareDerivata(int panel)
{
	if (flag_dateCitite == 1 && flag_derivataCalculata == 1) {
		
		DeleteGraphPlot(panel, PANEL_GRAPH_FILTRED, -1, VAL_IMMEDIATE_DRAW);	/*-1 se sterg toate graficele, fara intarziere*/

		int indexStart = (int)secValStanga * npoints / 6;
		int indexStop = (int)secValDreapta * npoints / 6;

	
		PlotY(panel, PANEL_GRAPH_FILTRED, derivata + indexStart, indexStop - indexStart, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
	}
}

void afisareAmvlopa(int panel)
{
	if (flag_dateCitite == 1 && afisareAmvelopa == 1) {
		/*int DeleteGraphPlot (int panelHandle, int controlID, int plotHandle, int refresh);*/

		int indexStart = (int)secValStanga * npoints / 6;
		int indexStop = (int)secValDreapta * npoints / 6;

		PlotY(panel, PANEL_GRAPH_RAW, amvelopa + indexStart, indexStop - indexStart, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
	
	}
}

int CVICALLBACK OnAfisareAmvelopa(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		if (flag_dateCitite == 1) 
		{
			afisareAmvelopa = 1;
			//_py_
			//LaunchExecutable("python anvelopa.py");
			//Delay(4);

			amvelopa = (double*)malloc(npoints * sizeof(double));
		
			FileToArray("anvelopa.txt", amvelopa, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);

			afisareAmvlopa(panel);
			break;
		}
	}
	return 0;
}

int CVICALLBACK OnPrev(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		secValStanga--;
		secValDreapta--;
		if (secValStanga < 0)
			secValStanga = 0;
		if (secValDreapta < 1)
			secValDreapta = 1;

		SetCtrlVal(panel, PANEL_NUMERIC_START, secValStanga);
		SetCtrlVal(panel, PANEL_NUMERIC_STOP, secValDreapta);

		afisareSemnal(panel);
		afisareSemnalFiltrat(panel);
		afisareAmvlopa(panel);
		afisareDerivata(panel);
		break;
	}
	return 0;
}

int CVICALLBACK OnNext(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		secValStanga++;
		secValDreapta++;
		if (secValStanga > 5)
			secValStanga = 5;
		if (secValDreapta > 6)
			secValDreapta = 6;

		SetCtrlVal(panel, PANEL_NUMERIC_START, secValStanga);
		SetCtrlVal(panel, PANEL_NUMERIC_STOP, secValDreapta);

		afisareSemnal(panel);
		afisareSemnalFiltrat(panel);
		afisareAmvlopa(panel);
		afisareDerivata(panel);
		break;
	}
	return 0;
}

//calcul filtru
int CVICALLBACK OnAplica(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	int tip;
	double s;

	switch (event)
	{
	case EVENT_COMMIT:
		if (flag_dateCitite == 1) 
		{
			flag_dateDerivate = 0;
			flag_dateFiltrate = 1;
			waveDataFiltrat = (double*)malloc((npoints) * sizeof(double));

			GetCtrlVal(panel, PANEL_RING_FILTRU, &tip);

			//filtrare prin mediere pe 16/32 esantioane
			if (tip == 1)
			{
				
				GetCtrlVal(panel, PANEL_RING_DIM, &dimFereastra);

				for (int i = 0; i < npoints - dimFereastra - 1; i++)
				{
					s = 0;
					for (int j = 0; j < dimFereastra; j++) {
						s += waveData[i + j];
					}

					waveDataFiltrat[i] = s / dimFereastra;
				}
				for (int i = npoints - dimFereastra - 1; i < npoints; i++)
				{
					waveDataFiltrat[i] = 0;
				}
				afisareSemnalFiltrat(panel);
			}
			//filtrare cu Alfa
			else if (tip == 2)
			{
				
				GetCtrlVal(panel, PANEL_NUMERIC, &alpha);
				waveDataFiltrat[0] = waveData[0];
				for (int i = 1; i < npoints; i++)
				{
					waveDataFiltrat[i] = (1 - alpha) * waveDataFiltrat[i - 1] + alpha * waveData[i];
				}
				
				afisareSemnalFiltrat(panel);
			}
			break;
		}
	}
	return 0;
}


int CVICALLBACK OnMainPanel(int panel, int event, void* callbackData,
	int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_GOT_FOCUS:

		break;
	case EVENT_LOST_FOCUS:

		break;
	case EVENT_CLOSE:
		QuitUserInterface(0);
		break;
	}
	return 0;
}

int CVICALLBACK OnPanel2(int panel, int event, void* callbackData,
	int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_GOT_FOCUS:

		break;
	case EVENT_LOST_FOCUS:

		break;
	case EVENT_CLOSE:
		QuitUserInterface(0);
		break;
	}
	return 0;
}

int CVICALLBACK OnDimensiuneFereastra(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{

	switch (event)
	{
	case EVENT_COMMIT:
		GetCtrlVal(panel, PANEL_RING_DIM, &dimFereastra);
		break;
	}
	return 0;
}

int CVICALLBACK OnLoadButtonCB(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	//valori de afisat in domeniul timp
	double var_medie = 0.0;
	double var_min = 0.0;
	double var_max = 0.0;
	double var_mediana = 0.0;
	double var_dispersie = 0.0;
	int indexMax, indexMin;
	double vectx[100];
	int vecty[100];
	int intervale = 20;
	int treceriPrin0 = 0;
	double skew,kurtosis;
	
	
	switch (event)
	{
	case EVENT_COMMIT:
		//executa script python pentru conversia unui fisierului .wav in .txt
		//__py__
		//LaunchExecutable("python main.py");

		//astept sa fie generate cele doua fisiere (modificati timpul daca este necesar)
		//Delay(2);

		//incarc informatiile privind rata de esantionare si numarul de valori
		/*int FileToArray (char fileName[], void *array, int dataType, size_t numberOfElements, size_t numberOfGroups, int arrayDataOrder, int fileLayout, int fileType);
		VAL_GROUPS_TOGETHER -> grupurile impreuna
		VAL_GROUPS_AS_COLUMNS -> elemente separate pe linie
		*/
		FileToArray("wafeInfo.txt", waveInfo, VAL_INTEGER, 2, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
		sampleRate = waveInfo[SAMPLE_RATE];
		npoints = waveInfo[NPOINTS];

		//alocare memorie pentru numarul de puncte
		waveData = (double*)calloc(npoints, sizeof(double));

		//incarcare din fisierul .txt in memorie (vector)
		FileToArray("waveData.txt", waveData, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);

		flag_dateCitite = 1;

		afisareSemnal(panel);

		//Calculare si afisare MEDIE
		Mean(waveData, npoints, &var_medie);
		SetCtrlVal(panel, PANEL_NUMERIC_MEAN, var_medie);

		//Calculare si afisare MINIM, MAXIM, INDEXMINIM, INDEXMAXIM
		MaxMin1D(waveData, npoints, &var_max, &indexMax, &var_min, &indexMin);
		SetCtrlVal(panel, PANEL_NUMERIC_MIN, var_min);
		SetCtrlVal(panel, PANEL_NUMERIC_MAX, var_max);
		SetCtrlVal(panel, PANEL_NUMERIC_INDEX_MINIM, indexMin);
		SetCtrlVal(panel, PANEL_NUMERIC_INDEX_MAXIM, indexMax);
		
		//Calculare si afisare MEDIANA
		Median(waveData, npoints, &var_mediana);
		SetCtrlVal(panel, PANEL_NUMERIC_MEDIANA, var_mediana);

		//dispersie = abatere standard standard
		/* StdDev(double inputArray[], ssize_t numberOfElements, double *mean, double *standardDeviation);*/
		StdDev(waveData, npoints, &var_medie, &var_dispersie);
		SetCtrlVal(panel, PANEL_NUMERIC_DISPERSIE, var_dispersie);

		
	//pentru a cuprinde toate valorile in histograma : minim si maxim
		Histogram(waveData, npoints, var_min, var_max, vecty, vectx, intervale);
		DeleteGraphPlot(panel, PANEL_GRAPH_HISTOGRAMA, -1, VAL_IMMEDIATE_DRAW);
		
		PlotXY(panel, PANEL_GRAPH_HISTOGRAMA, (void*)vectx, (void*)vecty, intervale, VAL_DOUBLE, VAL_SSIZE_T, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);/*puncte imprastiate nu continue*/

		//verific daca valorile succesive din waveData trec prin zero
		for (int i = 0; i < npoints - 1; i++)
		{//valoarea curenta este mai mica decat zero si urmatoarea valoare este mai mare decat zero SAU daca valoarea curenta este mai mare decat zero si urmatoarea valoare este mai mica decat zero
			if ((waveData[i] <= 0 && waveData[i + 1] >= 0) || (waveData[i] >= 0 && waveData[i + 1] <= 0))
			{
				treceriPrin0++;
			}
		}
		SetCtrlVal(panel, PANEL_NUMERIC_TRECERI_0, treceriPrin0);

		//Asimetria
			Moment(waveData,256,3,&skew);
			SetCtrlVal (panelHandle, PANEL_NUMERIC_10, skew);
			//Aplatizarea
			Moment(waveData,256,4,&kurtosis);
			SetCtrlVal(panelHandle,PANEL_NUMERIC_11,kurtosis);
			
		break;
	}
	return 0;
}

int CVICALLBACK StartValue(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		GetCtrlVal(panel, PANEL_NUMERIC_START, &secValStanga);

		if (secValStanga >= secValDreapta)
		{
			MessagePopup("Eroare", "Valoarea nu este valida! Valorile din stanga trebuie sa fie mai mica ca cea din dreapta!");
			secValStanga = 0;
			secValDreapta = 6;
			SetCtrlVal(panel, PANEL_NUMERIC_START, 0);
			SetCtrlVal(panel, PANEL_NUMERIC_STOP, 6);
		}

		afisareSemnal(panel);
		afisareSemnalFiltrat(panel);
		afisareAmvlopa(panel);
		afisareDerivata(panel);
		break;
	}
	return 0;
}

int CVICALLBACK StopValue(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		GetCtrlVal(panel, PANEL_NUMERIC_STOP, &secValDreapta);

		if (secValStanga >= secValDreapta)
		{
			MessagePopup("Eroare", "Valoarea nu este valida! Valorile din stanga trebuie sa fie mai mica ca cea din dreapta!");
			secValStanga = 0;
			secValDreapta = 6;
			SetCtrlVal(panel, PANEL_NUMERIC_START, 0);
			SetCtrlVal(panel, PANEL_NUMERIC_STOP, 6);
		}

		afisareSemnal(panel);
		afisareSemnalFiltrat(panel);
		afisareAmvlopa(panel);
		afisareDerivata(panel);
		break;
	}
	return 0;
}

int CVICALLBACK AlphaSchimbat(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		GetCtrlVal(panel, PANEL_NUMERIC, &alpha);
		break;
	}
	return 0;
}

int CVICALLBACK OnSave(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	char fileNameSignal[256]={0};
	int bitmapID; //imaginea
	int tip; //tip filtrare 
	int dim;//dimensiune mediere 16/32
	double alfa; //elementul de ordin 1
	
	GetCtrlVal(panel, PANEL_RING_FILTRU, &tip);
	GetCtrlVal(panel, PANEL_RING_DIM, &dim);
	GetCtrlVal(panel, PANEL_NUMERIC, &alfa);

	switch (event)
	{
	case EVENT_COMMIT:
		if (flag_dateCitite == 0)
		{
			return 0;
		}

		if (afisareAmvelopa == 0)
		{
			sprintf(fileNameSignal, "RowData_la_secundele_%d_%d.jpg.jpg", secValStanga, secValDreapta);  //afisam semnalul initial
		}
		else {
			sprintf(fileNameSignal, "Anvelopa_semnalului_la_secundele_%d_%d.jpg", secValStanga, secValDreapta);//afisam anvelopa
		}
		
		GetCtrlDisplayBitmap(panel, PANEL_GRAPH_RAW, 1, &bitmapID);
		SaveBitmapToJPEGFile(bitmapID, fileNameSignal, JPEG_PROGRESSIVE, 100);
		DiscardBitmap(bitmapID);
		
		//se aplica filtrarea
		if (flag_dateFiltrate == 1)
		{
			if (tip == 1) //prin mediere
			{
				sprintf(fileNameSignal,"FiltredData_mediere_de_dimensiune_%d_la_secundele_%d_%d.jpg", dim, secValStanga, secValDreapta);
			}
			else // cu alfa
			{
				sprintf(fileNameSignal, "FiltredData_ordin_1_alpha_%f_la_secundele_%d_%d.jpg", alfa, secValStanga, secValDreapta);
			}
			
			GetCtrlDisplayBitmap(panel, PANEL_GRAPH_FILTRED, 1, &bitmapID);
			SaveBitmapToJPEGFile(bitmapID, fileNameSignal, JPEG_PROGRESSIVE, 100);
			DiscardBitmap(bitmapID);
		
		}
		//se calculeaza derivata
		if (flag_derivataCalculata == 1) 
		{
			sprintf(fileNameSignal, "Derivata_semnalului_la_secundele_%d_%d.jpg", secValStanga, secValDreapta);
			
			GetCtrlDisplayBitmap(panel, PANEL_GRAPH_FILTRED, 1, &bitmapID);
			SaveBitmapToJPEGFile(bitmapID, fileNameSignal, JPEG_PROGRESSIVE, 100);
			DiscardBitmap(bitmapID);
		}
		
		 //Histograma
			  sprintf(fileNameSignal,"Histograma_%d_%d.jpg",secValStanga,secValDreapta);
			  GetCtrlDisplayBitmap(panel,PANEL_GRAPH_HISTOGRAMA,1, &bitmapID);
			  SaveBitmapToJPEGFile( bitmapID,fileNameSignal,JPEG_PROGRESSIVE,100);
			  DiscardBitmap( bitmapID);
		break;
	}
	return 0;
}

int CVICALLBACK OnDerivata(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		flag_dateDerivate = 1;
		if (flag_dateCitite == 1)
		{
			flag_derivataCalculata = 1;
			flag_dateFiltrate = 0;

			derivata = (double*)malloc(npoints * sizeof(double));
			//AnalysisLibErrType Difference (double inputArray[], ssize_t numberOfElements, 
			//double samplingInterval, double initialCondition, double finalCondition, double outputArray[]);
			Difference(waveData, npoints, 1.0, 0.0, 0.0, derivata);

			afisareDerivata(panel);
			break;
		}
	}
	return 0;
}












//in FRECVENTA

//Comutare intre panel-uri
int CVICALLBACK OnTime_Freq(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		if (panel == panelHandle)
		{
			SetCtrlVal(panelHandle2, PANEL_2_BINARYSWITCH, 0);
			DisplayPanel(panelHandle2);
			HidePanel(panel);
		}
		else
		{
			SetCtrlVal(panelHandle, PANEL_BINARYSWITCH, 1);
			DisplayPanel(panelHandle);
			HidePanel(panel);
		}
		break;
	}
	return 0;
}

//pe intervale de dimensiune  "dimFereastra"
int  OnSpectrum(int panel)
{
	WindowConst winConst;
	double* autoSpectrum = (double*)malloc((int)(npoints / 6) * sizeof(double));//vector ce contine spectrul semnalului convertit la volti
	double df = 0.0;// pasul in domeniul frecventei
	double freqPeak = 0.0;//valoarea maxima din spectru de putere (din autoSpectrum)
	double powerPeak = 0.0;//frecventa estimata pentru spectrul de putere (maxim) din vectorul autoSpectrum
	double* convertedSpectrum = (double*)malloc((int)(npoints / 6) * sizeof(double));//vector ce contine spectrul semnalului convertit la volti
	char unit[32] = "V";
	int numarFerestre;
	double* waveData1;//pentru zeropadding
	//variabila cadru e folosita pentru a accesa un anumit interval de date din waveData1

	//daca s-au citit datele
	if (flag_dateCitite == 1)
	{
		//obtin numarul de puncte pentru ferestruire/filtrare in frecventa (DIMENSIUNEA FERESTREI DE TIMP)
		GetCtrlVal(panel, PANEL_2_RING, &numarDepuncte);

		//calculez numarul de ferestre
		numarFerestre = (int)(((double)npoints) / numarDepuncte); //nr total de esantioane / nr de puncte pe care le folosesc pentru o fereastra
		
		if (((double)npoints) / numarDepuncte - numarFerestre > 0.0)
		{
			//Se adauga o fereastra suplimentara daca exista esantioane suplimentare care nu incap complet in ferestre intregi
			numarFerestre++;
		}

		waveData1 = (double*)malloc(numarFerestre * numarDepuncte * sizeof(double));
		
		//IN CAZUL IN CARE NU SUNT SUFICIENTE DATE PENTRU A UMPLE O FEREASTRA - > ZERO PADDING
		
	//cand impartim nr de esantioane la N, va rezulta un numar cu virgula,unde acele nr nu sunt intr-o fereastra completa
		for (int i = 0; i < numarDepuncte * numarFerestre; i++)
		//nr care nu sunt intr-o fereastra completa le vom inlocui cu 0 pt a putea aplica fereastra si filtru pe N puncte

		{
			if (i < npoints)
			{
				waveData1[i] = waveData[i];
			}
			else {
				waveData1[i] = 0;
			}
		}

		//ferestruirea semnalului 
		ScaledWindowEx(waveData1 + +cadru * numarDepuncte, numarDepuncte, RECTANGLE_, 0, &winConst);
		//obtinerea spectrului de putere
		AutoPowerSpectrum(waveData1 + +cadru * numarDepuncte, numarDepuncte, 1.0 / sampleRate, autoSpectrum, &df);
		//Determinarea amplitudinilor si frecventelor liniilor spectrale
		PowerFrequencyEstimate(autoSpectrum, numarDepuncte, -1.0, winConst, df, 7, &freqPeak, &powerPeak);
		//Reprezentarea convenabilã a spectrului de putere
		SpectrumUnitConversion(autoSpectrum, numarDepuncte, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, df, winConst, convertedSpectrum, unit);
		//sterg graph-ul unde urmeaza sa plotez spectrul semnalului
		DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_SPECTRUM, -1, VAL_IMMEDIATE_DRAW);
	
		
		//PLOT SPECTRU SEMNAL INITIAL
		PlotWaveform(panelHandle2, PANEL_2_GRAPH_SPECTRUM, convertedSpectrum, npoints / 12, VAL_DOUBLE, 1.0, 0.0, 0.0, df,VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
		
		
		DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_ROW, -1, VAL_IMMEDIATE_DRAW);	
		//PLOT SEMNAL ACHIZITIONAT
		PlotY(panelHandle2, PANEL_2_GRAPH_ROW, waveData1 + cadru * numarDepuncte, numarDepuncte, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);

		
		//afisez valorile
		SetCtrlVal(panel, PANEL_2_NUMERIC_F_PEAK, freqPeak);
		SetCtrlVal(panel, PANEL_2_NUMERIC_POWER_PEAK, powerPeak);
		SetCtrlVal(panel, PANEL_2_NR_FEREASTRA, cadru + 1); //un anumit interval de date din semnal
	}
	return 0;
}

//SPECTRUL DE PUTERE PE O SECUNDA SPECIFICATA
int  OnSpectrumFull(int panel)
{
	WindowConst winConst;
	double* autoSpectrum = (double*)malloc((int)(npoints / 6) * sizeof(double));//vector ce contine spectrul semnalului convertit la volti
	double df = 0.0; //variabila ce reprezinta pasul in domeniul frecventei
	double freqPeak = 0.0;//valoarea maxima din spectru de putere (din autoSpectrum)
	double powerPeak = 0.0;//frecventa estimata pentru spectrul de putere (maxim) din vectorul autoSpectrum
	double* convertedSpectrum = (double*)malloc((int)(npoints / 6) * sizeof(double));//vector ce contine spectrul semnalului convertit la volti
	char unit[32] = "V";
	int secunda;
	
	//daca s au citit datele
	if (flag_dateCitite)
	{
		//vedem pe care secunda suntem si care e numarul de puncte
		GetCtrlVal(panel, PANEL_2_RING, &numarDepuncte);
		GetCtrlVal(panel, PANEL_2_RING_SECOND, &secunda);
		
		//ferestruirea semnalului pe secunda specificata
		ScaledWindowEx(waveData + secunda * npoints / 6, npoints/6, RECTANGLE_, 0, &winConst);
		//obtinerea spectrului de putere
		AutoPowerSpectrum(waveData + secunda * npoints / 6, npoints / 6, 1.0 / sampleRate, autoSpectrum, &df);
		//Determinarea amplitudinilor si frecventelor liniilor spectrale
		PowerFrequencyEstimate(autoSpectrum, npoints / 6, -1.0, winConst, df, 7, &freqPeak, &powerPeak);
		//Reprezentarea convenabilã a spectrului de putere
		SpectrumUnitConversion(autoSpectrum, npoints/6, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, df, winConst, convertedSpectrum, unit);

		
		DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_SPECTRUM, -1, VAL_IMMEDIATE_DRAW);
		DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_ROW, -1, VAL_IMMEDIATE_DRAW);
		
		//plotez SPECTRU SEMNAL INITIAL
		PlotWaveform(panelHandle2, PANEL_2_GRAPH_SPECTRUM, convertedSpectrum, npoints / 12, VAL_DOUBLE, 1.0, 0.0, 0.0, df, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
		//plotez SEMNAL ACHIZITIONAT
		PlotY(panel, PANEL_2_GRAPH_ROW, waveData + secunda * npoints / 6, npoints / 6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
	
		//afisez valorile
		SetCtrlVal(panel, PANEL_2_NUMERIC_F_PEAK, freqPeak);
		SetCtrlVal(panel, PANEL_2_NUMERIC_POWER_PEAK, powerPeak);
	}
	return 0;
}


//PE TOT SEMNALUL (6 SECUNDE)
int  OnSpectrumFull2(int panel)
{
	WindowConst winConst;
	double* autoSpectrum = (double*)malloc((int)(npoints) * sizeof(double));//vector ce contine spectrul semnalului convertit la volti
	double df = 0.0;//variabila ce reprezinta pasul in domeniul frecventei
	double freqPeak = 0.0;//valoarea maxima din spectru de putere (din autoSpectrum)
	double powerPeak = 0.0;//frecventa estimata pentru spectrul de putere (maxim) din vectorul autoSpectrum
	double* convertedSpectrum = (double*)malloc((int)(npoints ) * sizeof(double));//vector ce contine spectrul semnalului convertit la volti
	char unit[32] = "V";

	if (flag_dateCitite)//s au citit datele
	{
		GetCtrlVal(panel, PANEL_2_RING, &numarDepuncte);//extrag nr de puncte
		
		//ferestruirea semnalului
		ScaledWindowEx(waveData , npoints, RECTANGLE_, 0, &winConst);
		//obtinerea spectrului de putere
		AutoPowerSpectrum(waveData , npoints , 1.0 / sampleRate, autoSpectrum, &df);
		//Determinarea amplitudinilor si frecventelor liniilor spectrale
		PowerFrequencyEstimate(autoSpectrum, npoints , -1.0, winConst, df, 7, &freqPeak, &powerPeak);
		//Reprezentarea convenabilã a spectrului de putere
		SpectrumUnitConversion(autoSpectrum, npoints, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, df, winConst, convertedSpectrum, unit);

		//sterg graph-ul unde urmeaza sa plotez spectrul semnalului
		DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_SPECTRUM, -1, VAL_IMMEDIATE_DRAW);
		DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_ROW, -1, VAL_IMMEDIATE_DRAW);
		
		//plot SPECTRU SEMNAL INITIAL
		PlotWaveform(panelHandle2, PANEL_2_GRAPH_SPECTRUM, convertedSpectrum, npoints/2, VAL_DOUBLE, 1.0, 0.0, 0.0, df,VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS,VAL_RED);
	
		//plot SEMNAL ACHIZITIONAT
		PlotY(panel, PANEL_2_GRAPH_ROW, waveData, npoints , VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS,VAL_RED);
	
		
		SetCtrlVal(panel, PANEL_2_NUMERIC_F_PEAK, freqPeak);
		SetCtrlVal(panel, PANEL_2_NUMERIC_POWER_PEAK, powerPeak);
	}

	return 0;
}



//permite trecerea la urmatorul interval de timp
//pentru AFISAREA PE DIMFEREASTRA se foloseste un timer
int CVICALLBACK OnTimer(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	int tipFereastra;
	double* window;	//fereastra
	double* windowing; //semnal ferestruit
	int tipFiltru;	
	double df;
	char unit[32] = "V";  //voltage semnal
	static WindowConst winConst;
	double* powerSpectrum;
	double* spectrum;
	double fpass, fstop; //pentru filtrul cu raspuns infinit este o banda de tranzitie
	int numarFerestre;
	double* waveData1;	//cu zero padding
	filtrareInFrecventa = 1;

	switch (event)
	{
		case EVENT_TIMER_TICK: //extrag nr de puncte, tipul filtrului si tipul ferestrei
			GetCtrlVal(panel, PANEL_2_RING, &numarDepuncte);
			GetCtrlVal(panel, PANEL_2_FILTER_TYPE, &tipFiltru);
			GetCtrlVal(panel, PANEL_2_GRAPH_WINDOW, &tipFereastra);
			
		
			numarFerestre = (int)(((double)npoints) / numarDepuncte); // numar ferestre = nr esantioane / nr esantioane pe fereastra
			if (((double)npoints) / numarDepuncte - numarFerestre > 0.0)
			{	//Se adauga o fereastra suplimentara daca exista esantioane suplimentare care nu incap complet in ferestre intregi
				numarFerestre++;
			}
			
			
			waveData1 = (double*)malloc((int)(numarFerestre * numarDepuncte * sizeof(double)));//pentru zero padding
			cadru++;
			cadru = cadru % numarFerestre; //daca cadru >= numar ferestre - > cadru -> 0

			window = (double*)malloc((int)(numarDepuncte) * sizeof(double));
			windowing = (double*)malloc((int)(numarDepuncte) * sizeof(double));
			powerSpectrum = (double*)malloc((int)(numarDepuncte) * sizeof(double));
			spectrum = (double*)malloc((int)(numarDepuncte) * sizeof(double));
			
			
			//IN CAZUL IN CARE NU SUNT SUFICIENTE DATE PENTRU A UMPLE O FEREASTRA - > ZERO PADDING
		//cand impartim nr de esantioane la N, va rezulta un numar cu virgula,unde acele nr nu sunt intr-o fereastra completa
		//nr care nu sunt intr-o fereastra completa le vom inlocui cu 0 pt a putea aplica fereastra si filtru pe N puncte
			for (int i = 0; i < numarDepuncte * numarFerestre; i++)
			{
				if (i < npoints)
				{
					waveData1[i] = waveData[i];
				}
				else {
					waveData1[i] = 0;
				}

			}
			
			//pregatire pentru creare fereastra
			// 1.0 inseamna ca nu se aplica nicio modificare (ferestruire uniforma) asupra datelor initiale
			for (int i = 0; i < numarDepuncte; i++)
			{
				window[i] = 1.0;
			}
			
			//FILTRAREA 
			switch (tipFiltru)
			{
				//FIR EquiRpl trece sus
				case 0: 
					//preiau ordinul 
					GetCtrlVal(panel,PANEL_2_NUMERIC_ORDER,&numberOfCoeffs);
					//aloc memorie pentru un vector filteredSignal pentru a stoca semnalul filtrat
					filteredSignal = (double*)malloc((int)(numarDepuncte + numberOfCoeffs) * sizeof(double));
					//preiau frecventa de trecere si de stop
					GetCtrlVal(panel, PANEL_2_NUMERIC_FPASS, &fpass);
					GetCtrlVal(panel, PANEL_2_NUMERIC_FSTOP, &fstop);
					
		
					EquiRpl_HPFiltering(waveData1 + cadru*numarDepuncte, numarDepuncte, numberOfCoeffs, sampleRate, fstop, fpass, filteredSignal);
					break;
					
				//Eliptic trece sus cu frecventa de taiere 1100 Hz
				case 1:
					
					//AnalysisLibErrType Elp_HPF (double inputArray[], ssize_t numberOfElements, double samplingFrequency,
					//double cutoffFrequency, double rippledb, double stopBandAttendb, int order, double outputArray[]);
	
					filteredSignal = (double*)malloc((int)(numarDepuncte) * sizeof(double));//aloc memorie pentru un vector filteredSignal pentru a stoca semnalul filtrat
					Elp_HPF(waveData1+ cadru*numarDepuncte, numarDepuncte, sampleRate, 1100, 0.1, 40.0, 5, filteredSignal);
					break;
					
				//Eliptic trece jos cu frecventa de taiere 1100 Hz	
				case 2:
					
					filteredSignal = (double*)malloc((int)(numarDepuncte) * sizeof(double));
					Elp_LPF(waveData1+ cadru*numarDepuncte, numarDepuncte, sampleRate, 1100, 0.1, 40.0, 5, filteredSignal);
					break;
					
				//FIR EquiRpl trece jos
				case 3:
					
					GetCtrlVal(panel,PANEL_2_NUMERIC_ORDER,&numberOfCoeffs);
					filteredSignal = (double*)malloc((int)(numarDepuncte + numberOfCoeffs) * sizeof(double));
					GetCtrlVal(panel, PANEL_2_NUMERIC_FPASS, &fpass);
					GetCtrlVal(panel, PANEL_2_NUMERIC_FSTOP, &fstop);
					
					EquiRpl_LPFiltering(waveData1 + cadru*numarDepuncte, numarDepuncte, numberOfCoeffs, sampleRate, fstop, fpass, filteredSignal);
					break;
			}
			
			///ferestruire
			switch (tipFereastra)
			{
				//Blackman-Harris
				case 0:
					//aplica si o inmultire cu o constanta din vectorul de intrare, pentru a avea fereastra fara acel coeficient vectorul window v-a fi pus initial pe 1
					BlkHarrisWin(window, numarDepuncte);
					break;
				//Welch
				case 1:
					WelchWin(window, numarDepuncte);
					break;
			}
			
			//ferestruirea efectiva
			Mul1D(filteredSignal, window, numarDepuncte, windowing);
	
			//CALCUL SPECTRU SEMNAL FILTRAT
			AutoPowerSpectrum(windowing, numarDepuncte, 1.0 / sampleRate, powerSpectrum, &df);
			SpectrumUnitConversion(powerSpectrum, numarDepuncte, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VPK, df, winConst, spectrum, unit);
			
			DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_ROW_WIN, -1, VAL_IMMEDIATE_DRAW);
			
			//plotez FEREASTRA
			PlotY(panel, PANEL_2_GRAPH_ROW_WIN, window, numarDepuncte, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
			DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_WINDOWING, -1, VAL_IMMEDIATE_DRAW);
			
			//plotez SEMNALUL FILTRAT FERESTRUIT
			PlotY(panel, PANEL_2_GRAPH_WINDOWING, windowing, numarDepuncte, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
			DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_ROW_FILTRED, -1, VAL_IMMEDIATE_DRAW);
			
			//plotez SEMNALUL FILTRAT
			PlotY(panel, PANEL_2_GRAPH_ROW_FILTRED, filteredSignal, numarDepuncte, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
			
			DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_SPECTRUM_FILTRE, -1, VAL_IMMEDIATE_DRAW);
			
			//plotez SPECTRUL SEMNALULUI FILTRAT SI FERESTRUIT
			PlotWaveform(panelHandle2, PANEL_2_GRAPH_SPECTRUM_FILTRE, spectrum, numarDepuncte / 2, VAL_DOUBLE, 1.0, 0.0, 0.0, df, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
			//Calcul spectru pe intervale de dimensiune dimFereastra
			OnSpectrum(panel);
		}
	return 0;
}

//AFISARE PE FIECARE SECUNDA 
int AfisarePeSecundeInFrecventa(int panel)
{
	int tipFereastra;
	int secunda;
	int numarFerestre;
	double* window;
	double* windowing;
	double* powerSpectrum;
	double* spectrum;
	int tipFiltru;
	double df; //pas in frecventa
	static WindowConst winConst;
	double fpass, fstop; //frecventa de trecere si de stop
	char unit[32] = "V";  //voltage semnal
	double * waveDataZeroPadding;
	
	OnSpectrumFull(panel); //AFISEZ SEMNAL ACHIZITIONAT + SPECTRU  PE SECUNDA RESPECTIVA
	
	//extrag tipul ferestrei, pe ce secunda suntem, nr de puncte si dim ferestrei
	GetCtrlVal(panel, PANEL_2_GRAPH_WINDOW, &tipFereastra);
	GetCtrlVal(panel, PANEL_2_RING_SECOND, &secunda);
	GetCtrlVal(panel, PANEL_2_RING, &numarDepuncte);
	GetCtrlVal(panel,PANEL_2_RING,&dimFereastra);
	
	//calculez numarul de ferestre
	numarFerestre = (int)(((double)npoints / 6) / numarDepuncte);
	if(numarFerestre <(((double)npoints / 6) / numarDepuncte) )
		numarFerestre++;
	
	window = (double*)malloc((int)(numarDepuncte) * sizeof(double));
	windowing = (double*)malloc((int)(6 * numarFerestre * dimFereastra*sizeof(double)));
	powerSpectrum = (double*)malloc((int)(npoints / 6) * sizeof(double));
	spectrum = (double*)malloc((int)(npoints / 6) * sizeof(double));
	waveDataZeroPadding = (double*) malloc(6 * numarFerestre * dimFereastra*sizeof(double));
	
	//pregatire coeficienti filtru
	for (int i = 0; i < numarDepuncte; i++)
	{
		window[i] = 1.0;
	}
	
	//zero padding
	for (int i = 0; i < 6*numarFerestre * dimFereastra; i++)
	{
		if(i < npoints)
			waveDataZeroPadding[i ] = waveData[i];
		else
			waveDataZeroPadding[i] = 0;
	}
	
	//filtrare
	GetCtrlVal(panel, PANEL_2_FILTER_TYPE, &tipFiltru);
	switch (tipFiltru)
	{
			
	//FIR EquiRpl trece sus
	case 0:
	
		//double samplingFrequency, double f1, double f2, double outputArray[]);
		//[0, f1] stop 
		//[f2, samplingFreq/2] trece
		

		//aloc memorie pentru un vector filteredSignal pentru a stoca semnalul filtrat, pt fiecare din cele 6 secunde
		filteredSignal = (double*)malloc((int)(6 * numarFerestre * dimFereastra*sizeof(double)));
		//preiaufrecventa de trecere si de stop
		GetCtrlVal(panel, PANEL_2_NUMERIC_FPASS, &fpass);
		GetCtrlVal(panel, PANEL_2_NUMERIC_FSTOP, &fstop);
		for (int i = 0; i < numarFerestre; i++) 
		{
			//preiau numarul de coeficienti
			GetCtrlVal(panel,PANEL_2_NUMERIC_ORDER,&numberOfCoeffs);
			EquiRpl_HPFiltering(waveDataZeroPadding + i * numarDepuncte + secunda*npoints/6, numarDepuncte,numberOfCoeffs, sampleRate, fstop, fpass, filteredSignal + i * numarDepuncte + secunda*npoints/6);
		}//filtrare pe dimfereastra puncte
		break;
		
	//Eliptic trece sus cu frecventa de taiere 1100 Hz
	case 1:
		
		filteredSignal = (double*)malloc((int)(6 * numarFerestre * dimFereastra*sizeof(double)));
		for (int i = 0; i < numarFerestre; i++) 
		{
			Elp_HPF(waveDataZeroPadding + i * numarDepuncte+ secunda*npoints/6, numarDepuncte, sampleRate, 1100, 0.1, 40.0, 10, filteredSignal + i * numarDepuncte+ secunda*npoints/6);
		}//filtrare pe dimfereastra puncte
		break;
		
	case 2:
		//Eliptic trece jos cu frecventa de taiere 1100 Hz
		filteredSignal = (double*)malloc((int)(6 * numarFerestre * dimFereastra*sizeof(double)));
		for (int i = 0; i < numarFerestre; i++) {
			Elp_LPF(waveDataZeroPadding + i * numarDepuncte+ secunda*npoints/6, numarDepuncte, sampleRate, 1100, 0.1, 40.0, 10, filteredSignal + i * numarDepuncte+ secunda*npoints/6);
		}//filtrare pe dimfereastra puncte
		break;
		
	case 3:
		//filtru cu raspuns infinit trece jos
		filteredSignal = (double*)malloc((int)(6 * numarFerestre * dimFereastra*sizeof(double)));
		GetCtrlVal(panel, PANEL_2_NUMERIC_FPASS, &fpass);
		GetCtrlVal(panel, PANEL_2_NUMERIC_FSTOP, &fstop);
		for (int i = 0; i < numarFerestre; i++) {
			GetCtrlVal(panel,PANEL_2_NUMERIC_ORDER,&numberOfCoeffs);
			EquiRpl_LPFiltering(waveDataZeroPadding + i * numarDepuncte + secunda*npoints/6, numarDepuncte,numberOfCoeffs, sampleRate, fstop, fpass, filteredSignal + i * numarDepuncte + secunda*npoints/6);
		}//filtrare pe dimfereastra puncte
		break;
		
	}
	
	//copierea valorilor din filteredSignal în waveDataZeroPadding pentru primele npoints elemente
 	//iar pentru orice element suplimentar, se seteaza la 0 (zero padding)
	for (int i = 0; i < 6*numarFerestre * dimFereastra; i++)
	{
		if(i < npoints)
			waveDataZeroPadding[i ] = filteredSignal[i];
		else
			waveDataZeroPadding[i] = 0;
	}
	
	DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_ROW_FILTRED, -1, VAL_IMMEDIATE_DRAW);
	//PLOT SEMNAL FILTRAT
	PlotY(panel, PANEL_2_GRAPH_ROW_FILTRED, filteredSignal + secunda*npoints/6, npoints / 6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
	
	//ferestruire
	switch (tipFereastra)
	{
	case 0:
		//Blackman-Harris
		//aplica si o inmultire cu o constanta din vectorul de intrare, pentru a avea fereastra fara acel coeficient vectorul window v-a fi pus initial pe 1
		BlkHarrisWin(window, numarDepuncte);
		break;
	case 1:
		//Welch
		WelchWin(window, numarDepuncte);
		break;
	}
	

	for (int i = 0; i < numarFerestre; i++) 
	{
		Mul1D(waveDataZeroPadding + secunda * npoints / 6 + i * numarDepuncte, window, numarDepuncte, windowing + i * numarDepuncte + secunda*npoints/6);
	}//ferestruire pe dimfereastra puncte
	
	//AFISARE FEREASTRA
	DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_ROW_WIN, -1, VAL_IMMEDIATE_DRAW);
	PlotY(panel, PANEL_2_GRAPH_ROW_WIN, window, numarDepuncte, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS,VAL_RED);
	//AFISARE SEMNAL FILTRAT FERESTRUIT
	DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_WINDOWING, -1, VAL_IMMEDIATE_DRAW);
	PlotY(panel, PANEL_2_GRAPH_WINDOWING, windowing + secunda*npoints/6, npoints / 6, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);

	AutoPowerSpectrum(windowing + secunda* npoints/6, npoints / 6, 1.0 / sampleRate, powerSpectrum, &df);
	SpectrumUnitConversion(powerSpectrum, npoints / 12, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VPK, df, winConst, spectrum, unit);
	DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_SPECTRUM_FILTRE, -1, VAL_IMMEDIATE_DRAW);
	//AFISARE SPECTRUL SEMNALULUI FILTRAT SI FERESTRUIT
	PlotWaveform(panelHandle2, PANEL_2_GRAPH_SPECTRUM_FILTRE, spectrum, npoints / 12, VAL_DOUBLE, 1.0, 0.0, 0.0, df,
		VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS,VAL_RED);
	
	return 0;
}


//PE TOT SUNETUL AFISARE, pe toate cele 6 secunde
//filtrare, ferestruire, spectru pe tot semnalul
int AfisareFullInFrecventa(int panel)
{
	int tipFereastra;
	int numarFerestre;
	double* window;
	double* windowing;
	double* powerSpectrum;
	double* spectrum;
	int tipFiltru;
	double df;
	static WindowConst winConst;
	double fpass, fstop;
	char unit[32] = "V";  //voltage semnal
	double * waveDataZeroPadding;
	
	OnSpectrumFull2(panel); //AFISARE SEMNAL ACHIZITIONAT SI SPECTRU PE TOT SEMNALUL
	
	GetCtrlVal(panel, PANEL_2_GRAPH_WINDOW, &tipFereastra);
	GetCtrlVal(panel, PANEL_2_RING, &numarDepuncte);
	GetCtrlVal(panel,PANEL_2_RING,&dimFereastra);
	
	numarFerestre = (int)(((double)npoints) / numarDepuncte);
	if(numarFerestre <(((double)npoints) / numarDepuncte) )
		numarFerestre++;
	
	window = (double*)malloc((int)(numarDepuncte) * sizeof(double));
	windowing = (double*)malloc((int)(numarFerestre * dimFereastra*sizeof(double)));
	powerSpectrum = (double*)malloc((int)(npoints ) * sizeof(double));
	spectrum = (double*)malloc((int)(npoints ) * sizeof(double));
	waveDataZeroPadding = (double*) malloc( numarFerestre * dimFereastra*sizeof(double));
	
	for (int i = 0; i < numarDepuncte; i++)
	{
		window[i] = 1.0;
	}
	
	for (int i = 0; i < numarFerestre * dimFereastra; i++)
	{
		if(i < npoints)
			waveDataZeroPadding[i ] = waveData[i];
		else
			waveDataZeroPadding[i] = 0;
	}
	
	//filtrare
	GetCtrlVal(panel, PANEL_2_FILTER_TYPE, &tipFiltru);
	switch (tipFiltru)
	{
			
	case 0:
	
		//[0, f1] stop 
		//[f2, samplingFreq/2] trece
		GetCtrlVal(panel,PANEL_2_NUMERIC_ORDER,&numberOfCoeffs);
		filteredSignal = (double*)malloc(( numarFerestre * dimFereastra + numberOfCoeffs)*sizeof(double) );
		
		GetCtrlVal(panel, PANEL_2_NUMERIC_FPASS, &fpass);
		GetCtrlVal(panel, PANEL_2_NUMERIC_FSTOP, &fstop);
		
		//aplic pe fiecare fereastra filtrul
		for (int i = 0; i < numarFerestre; i++)
		{
			GetCtrlVal(panel,PANEL_2_NUMERIC_ORDER,&numberOfCoeffs);
			EquiRpl_HPFiltering(waveDataZeroPadding + i * numarDepuncte , numarDepuncte,numberOfCoeffs, sampleRate, fstop, fpass, filteredSignal + i * numarDepuncte );
		}//filtrare pe dimfereastra puncte
		break;
	
	
	case 1:
		//Eliptic trece sus cu frecventa de taiere 1100 Hz
		
		filteredSignal = (double*)malloc((int)(numarFerestre * dimFereastra*sizeof(double)));
		for (int i = 0; i < numarFerestre; i++) {
			Elp_HPF(waveDataZeroPadding + i * numarDepuncte, numarDepuncte, sampleRate, 1100, 0.1, 40.0, 10, filteredSignal + i * numarDepuncte);
		}//filtrare pe dimfereastra puncte
		break;

	case 2:
		filteredSignal = (double*)malloc((int)(numarFerestre * dimFereastra*sizeof(double)));
		for (int i = 0; i < numarFerestre; i++) {
			Elp_LPF(waveDataZeroPadding + i * numarDepuncte, numarDepuncte, sampleRate, 1100, 0.1, 40.0, 10, filteredSignal + i * numarDepuncte);
		}//filtrare pe dimfereastra puncte
		break;
	
	case 3:
		
		//[0, f1] stop 
		//[f2, samplingFreq/2] trece
		GetCtrlVal(panel,PANEL_2_NUMERIC_ORDER,&numberOfCoeffs);
		filteredSignal = (double*)malloc(( numarFerestre * dimFereastra + numberOfCoeffs)*sizeof(double) );
		
		GetCtrlVal(panel, PANEL_2_NUMERIC_FPASS, &fpass);
		GetCtrlVal(panel, PANEL_2_NUMERIC_FSTOP, &fstop);
		
		for (int i = 0; i < numarFerestre; i++) {
			EquiRpl_LPFiltering(waveDataZeroPadding + i * numarDepuncte , numarDepuncte,numberOfCoeffs, sampleRate, fstop, fpass, filteredSignal + i * numarDepuncte );
		}//filtrare pe dimfereastra puncte
		break;
	}
	
	//copierea valorilor din filteredSignal în waveDataZeroPadding pentru primele npoints elemente
 	//iar pentru orice element suplimentar, se seteaza la 0 (zero padding)
	for (int i = 0; i < numarFerestre * dimFereastra; i++)
	{
		if(i < npoints)
			waveDataZeroPadding[i ] = filteredSignal[i];
		else
			waveDataZeroPadding[i] = 0;
	}
	DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_ROW_FILTRED, -1, VAL_IMMEDIATE_DRAW);
	//SEMNALUL FILTRAT
	PlotY(panel, PANEL_2_GRAPH_ROW_FILTRED, filteredSignal , npoints , VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
	
	//ferestruire
	switch (tipFereastra)
	{
	case 0:
		//Blackman-Harris
		BlkHarrisWin(window, numarDepuncte);
		break;
	case 1:
		//Welch
		WelchWin(window, numarDepuncte);
		break;
	}
	for (int i = 0; i < numarFerestre; i++) {
		Mul1D(waveDataZeroPadding + i * numarDepuncte, window, numarDepuncte, windowing + i * numarDepuncte );
	}//ferestruire pe dimfereastra puncte

	DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_ROW_WIN, -1, VAL_IMMEDIATE_DRAW);
	//FEREASTRA
	PlotY(panel, PANEL_2_GRAPH_ROW_WIN, window, numarDepuncte, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
	
	DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_WINDOWING, -1, VAL_IMMEDIATE_DRAW);
	//SEMNAL FILTRAT FERESTRUIT
	PlotY(panel, PANEL_2_GRAPH_WINDOWING, windowing , npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);

	AutoPowerSpectrum(windowing , npoints, 1.0 / sampleRate, powerSpectrum, &df);
	SpectrumUnitConversion(powerSpectrum, npoints / 2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VPK, df, winConst, spectrum, unit);
	
	DeleteGraphPlot(panelHandle2, PANEL_2_GRAPH_SPECTRUM_FILTRE, -1, VAL_IMMEDIATE_DRAW);
	//SPECTRUL SEMNALULUI FILTRAT SI FERESTRUIT
	PlotWaveform(panelHandle2, PANEL_2_GRAPH_SPECTRUM_FILTRE, spectrum, npoints / 2, VAL_DOUBLE, 1.0, 0.0, 0.0, df,
		VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
	
	return 0;
}




int functieDateSchimbate(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	///TIMER activ doar cand am afisare pe dim ferestrei, altfel inactiv
	///salvare sunet doar cand am afisare pe secunda/tot sunetul , altfel inactiv
	///secunda activ doar cand am afisare pe secunda, altfel inactiv
	///salvare ca imagine doar cand am filtrele trece sus
	int val;
	int tipAfisare;
	int tipFiltru;
	
	switch (event)
	{
	case EVENT_COMMIT:
		GetCtrlVal(panel, PANEL_2_TOGGLEBUTTON, &val);	//vad daca sunt pe ON sau OFF (start/stop afisare)
		GetCtrlVal(panel, PANEL_2_FILTER_TYPE, &tipFiltru); //extrag tipul filtrului
		
		if (val == 1) // sunt pe ON si fac AFISARE
		{
			GetCtrlVal(panel, PANEL_2_RING_TIP_AFISARE, &tipAfisare); //extrag tipul afisarii (pe secunde/dim ferestrei/totala)
			
			if (tipAfisare == 1) //pe dimensiunea ferestrei
			{ //activez timerul
				SetCtrlAttribute(panel, PANEL_2_TIMER, ATTR_ENABLED, 1);
				//dimmed la frecventa de trecere
				SetCtrlAttribute(panel, PANEL_2_NUMERIC_FPASS, ATTR_DIMMED, 1);
				
				//Eliptic trece sus/jos cu frecventa de taiere 1100 Hz
				if(tipFiltru == 1 || tipFiltru == 2)
				{//ascund parametrii pentru filtrul FIR EquiRpl
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FSTOP, ATTR_DIMMED, 1);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FPASS, ATTR_DIMMED, 1);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_ORDER, ATTR_DIMMED, 1);
				}
				else
				{//altfel nu ascund parametrii
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FSTOP, ATTR_DIMMED, 0);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FPASS, ATTR_DIMMED, 0);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_ORDER, ATTR_DIMMED, 0);
				}
				SetCtrlAttribute(panel, PANEL_2_COMMANDBUTTON_2, ATTR_DIMMED, 1);//nu pot salva ca sunet 
			}
			
			
			else if (tipAfisare == 0) //Afisarea se face pe secunde 
			{
				AfisarePeSecundeInFrecventa(panel); //apelez functia
				//dezactivez timerul 
				SetCtrlAttribute(panel, PANEL_2_TIMER, ATTR_ENABLED, 0);
			   //activez secunda
				SetCtrlAttribute(panel, PANEL_2_RING_SECOND, ATTR_DIMMED, 0);
				
				//Eliptic trece sus/jos cu frecventa de taiere 1100 Hz
				if(tipFiltru == 1 || tipFiltru == 2)
				{//ascund parametrii pentru filtrul FIR EquiRpl
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FSTOP, ATTR_DIMMED, 1);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FPASS, ATTR_DIMMED, 1);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_ORDER, ATTR_DIMMED, 1);
				}
				else
				{//nu ascund parametrii pentru filtrul FIR EquiRpl
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FSTOP, ATTR_DIMMED, 0);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FPASS, ATTR_DIMMED, 0);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_ORDER, ATTR_DIMMED, 0);
				}
				SetCtrlAttribute(panel, PANEL_2_COMMANDBUTTON_2, ATTR_DIMMED, 0);//pot salva ca sunet semnalul pe secunda respectiva
			}
			
			else if (tipAfisare == 2)//Afisare pe tot sunetul
			{
				
				AfisareFullInFrecventa(panel);//apelez functia pentru afisare pe tot semnalul
				//dezactivez timerul si secunda
				SetCtrlAttribute(panel, PANEL_2_TIMER, ATTR_ENABLED, 0);
				SetCtrlAttribute(panel, PANEL_2_RING_SECOND, ATTR_DIMMED, 1);
				
				//Eliptic trece sus/jos cu frecventa de taiere 1100 Hz
				if(tipFiltru == 1 || tipFiltru == 2)
				{//ascund parametrii pentru filtrul FIR EquiRpl
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FSTOP, ATTR_DIMMED, 1);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FPASS, ATTR_DIMMED, 1);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_ORDER, ATTR_DIMMED, 1);
				}
				else
				{//nu ascund parametrii pentru filtrul FIR EquiRpl
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FSTOP, ATTR_DIMMED, 0);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FPASS, ATTR_DIMMED, 0);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_ORDER, ATTR_DIMMED, 0);
				}
				
				SetCtrlAttribute(panel, PANEL_2_COMMANDBUTTON_2, ATTR_DIMMED, 0);//pot salva ca sunet tot semnalul
			}
		}
		else //sunt pe OFF
		{
			GetCtrlVal(panel, PANEL_2_RING_TIP_AFISARE, &tipAfisare);
			
			if (tipAfisare == 1) //pe dimensiunea ferestrei
			{//dezactivez timer si secunda
				SetCtrlAttribute(panel, PANEL_2_TIMER, ATTR_ENABLED, 0);
				SetCtrlAttribute(panel, PANEL_2_RING_SECOND, ATTR_DIMMED, 1);
				
				
				if(tipFiltru == 1 || tipFiltru == 2)
				{//ascund parametrii pentru filtrul FIR EquiRpl
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FSTOP, ATTR_DIMMED, 1);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FPASS, ATTR_DIMMED, 1);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_ORDER, ATTR_DIMMED, 1);
				}
				else
				{//nu ascund parametrii pentru filtrul FIR EquiRpl
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FSTOP, ATTR_DIMMED, 0);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FPASS, ATTR_DIMMED, 0);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_ORDER, ATTR_DIMMED, 0);
				}
				
				//dezactivare salvare
				SetCtrlAttribute(panel, PANEL_2_COMMANDBUTTON_2, ATTR_DIMMED, 1);
			}
			else if (tipAfisare == 0)//Afisarea se face pe secunde 
			{
				//dezactivez timer, activez secunda
				SetCtrlAttribute(panel, PANEL_2_TIMER, ATTR_ENABLED, 0);
				SetCtrlAttribute(panel, PANEL_2_RING_SECOND, ATTR_DIMMED, 0);
				
				if(tipFiltru == 1 || tipFiltru == 2)
				{//ascund parametrii pentru filtrul FIR EquiRpl
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FSTOP, ATTR_DIMMED, 1);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FPASS, ATTR_DIMMED, 1);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_ORDER, ATTR_DIMMED, 1);
				}
				else
				{// nu ascund parametrii pentru filtrul FIR EquiRpl
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FSTOP, ATTR_DIMMED, 0);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FPASS, ATTR_DIMMED, 0);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_ORDER, ATTR_DIMMED, 0);
				}
				
				SetCtrlAttribute(panel, PANEL_2_COMMANDBUTTON_2, ATTR_DIMMED, 0);//pot salva ca sunet pe secunda
			}
			
			else if(tipAfisare == 2)//afisare pe tot sunetul
			{
				//timer si secunda inactive
				SetCtrlAttribute(panel, PANEL_2_TIMER, ATTR_ENABLED, 0);
				SetCtrlAttribute(panel, PANEL_2_RING_SECOND, ATTR_DIMMED, 1);
				
				if(tipFiltru == 1 || tipFiltru == 2)
				{ //ascund parametrii pentru filtrul FIR EquiRpl
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FSTOP, ATTR_DIMMED,1);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FPASS, ATTR_DIMMED, 1);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_ORDER, ATTR_DIMMED, 1);
				}
				else
				{// nu ascund parametrii pentru filtrul FIR EquiRpl
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FSTOP, ATTR_DIMMED, 0);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_FPASS, ATTR_DIMMED, 0);
					SetCtrlAttribute(panel, PANEL_2_NUMERIC_ORDER, ATTR_DIMMED, 0);
				}
				SetCtrlAttribute(panel, PANEL_2_COMMANDBUTTON_2, ATTR_DIMMED, 0);//pot salva ca sunet tot semnalul
			}
		}
		//FIR EquiRpl trece jos sau Eliptic trece jos cu frecventa de taiere 1100 Hz
		if(tipFiltru == 3 || tipFiltru == 2)
		{
			//nu pot salva ca imagine
			SetCtrlAttribute(panel, PANEL_2_COMMANDBUTTON, ATTR_DIMMED, 1); 
		}
		else
		{ //FIR EquiRpl trece sus sau Eliptic trece sus cu frecventa de taiere 1100 Hz
		//pot salva ca imagine
			SetCtrlAttribute(panel, PANEL_2_COMMANDBUTTON, ATTR_DIMMED, 0); 
		}
		break;
	}
	return 0;
}

int CVICALLBACK OnStartStop(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	functieDateSchimbate( panel,  control,  event,callbackData,  eventData1,  eventData2); //apelez functia
	return 0;
}

int CVICALLBACK OnNrEsantioane(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		cadru = 0; //incep de la 0 
		functieDateSchimbate( panel,  control,  event,callbackData,  eventData1,  eventData2); //apelez functia
		break;
	}
	return 0;
}

int CVICALLBACK OnTipFiltru (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			functieDateSchimbate( panel,  control,  event,callbackData,  eventData1,  eventData2);//apelez functia
			break;
	}
	return 0;
}

int CVICALLBACK OnTipFereastra (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			functieDateSchimbate( panel,  control,  event,callbackData,  eventData1,  eventData2);//apelez functia
			break;
	}
	return 0;
}

int CVICALLBACK ChangeSecond (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			functieDateSchimbate( panel,  control,  event,callbackData,  eventData1,  eventData2);//apelez functia
			break;
	}
	return 0;
}

//parametrii pentru filtrul : FIR EquiRpl
int CVICALLBACK ChangeFpasAndFStop (int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			functieDateSchimbate( panel,  control,  event,callbackData,  eventData1,  eventData2);//apelez functia
			break;
	}
	return 0;
}
	
int CVICALLBACK OnOrder (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			functieDateSchimbate( panel,  control,  event,callbackData,  eventData1,  eventData2); //apel functie
			break;
	}
	return 0;
}

int CVICALLBACK tipAfisareSchimbat (int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			functieDateSchimbate( panel,  control,  event,callbackData,  eventData1,  eventData2);//apelez functia
			break;
	}
	return 0;
}

//salvare grafice din frecventa
int CVICALLBACK SaveFreq (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	char fileNameSignal[256]={0};
	int bitmapID;
	int tipAfisare;
	int tipFereastra;
	int tipFiltru;
	int secunda;
	
	switch (event)
	{
		case EVENT_COMMIT:
			
			GetCtrlVal(panel,PANEL_2_RING_TIP_AFISARE,&tipAfisare);
			
			
			///AFISAREA DE FACE PE DIMENSIUNEA FERESTREI
			if(tipAfisare == 1) 
			{
				//SPECTRU SEMNAL INITIAL
				GetCtrlVal(panel, PANEL_2_RING, &numarDepuncte);
				sprintf(fileNameSignal, "Grafic_spectru_semnal_initial_numar_de_puncte_%d_cadru_%d.jpg",numarDepuncte,cadru + 1);
				GetCtrlDisplayBitmap(panel, PANEL_2_GRAPH_SPECTRUM, 1, &bitmapID);
				SaveBitmapToJPEGFile(bitmapID, fileNameSignal, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(bitmapID);
				
				//SEMNAL FILTRAT
				GetCtrlVal(panel, PANEL_2_GRAPH_WINDOW, &tipFereastra);
				GetCtrlVal(panel, PANEL_2_RING, &numarDepuncte);
				GetCtrlVal(panel, PANEL_2_FILTER_TYPE, &tipFiltru);
				
				// FIR EquiRpl trece sus
				if(tipFiltru == 0)
					sprintf(fileNameSignal, "Grafic_semnal_Filtru_FIREquiRpl_numar_de_puncte_%d_cadru_%d.jpg",numarDepuncte,cadru + 1);
				
				//Eliptic trece sus cu frecventa de taiere 1100 Hz
				else if (tipFiltru == 1)
					sprintf(fileNameSignal, "Grafic_Filtru_Eliptic_numar_de_puncte_%d_cadru_%d.jpg",numarDepuncte,cadru + 1);
				
				GetCtrlDisplayBitmap(panel, PANEL_2_GRAPH_ROW_FILTRED, 1, &bitmapID);
				SaveBitmapToJPEGFile(bitmapID, fileNameSignal, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(bitmapID);
				
				//SEMNAL FILTRAT SI FERESTRUIT
				
				if(tipFereastra == 0) //IN FUNCTIE DE FEREASTRA
				{	if(tipFiltru == 0)
						sprintf(fileNameSignal, "Grafic_semnal_Fereastra_Blackman_Harris_Filtru_FIREquiRpl_numar_de_puncte_%d_cadru_%d.jpg",numarDepuncte,cadru + 1);
					else if (tipFiltru == 1)
						sprintf(fileNameSignal, "Grafic_semnal_Fereastra_Blackman_Harris_Filtru_Eliptic_numar_de_puncte_%d_cadru_%d.jpg",numarDepuncte,cadru + 1);
					}		
				else
				{	if(tipFiltru == 0) // IN FUNCTIE DE TIPUL FILTRULUI
						sprintf(fileNameSignal, "Grafic_semnal_Fereastra_Welch_Filtru_FIREquiRpl_trece_sus_numar_de_puncte_%d_cadru_%d.jpg",numarDepuncte,cadru + 1);
					else if (tipFiltru == 1)
						sprintf(fileNameSignal, "Grafic_semnal_Fereastra_Welch_Filteu_Eliptic_trece_sus_numar_de_puncte_%d_cadru_%d.jpg",numarDepuncte,cadru + 1);
				}
				GetCtrlDisplayBitmap(panel, PANEL_2_GRAPH_WINDOWING, 1, &bitmapID);
				SaveBitmapToJPEGFile(bitmapID, fileNameSignal, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(bitmapID);
				
				//SPECTRU SEMNAL FILTRAT SI FERESTRUIT
				
				if(tipFereastra == 0) //IN FUNCTIE DE FEREASTRA 
				{	if(tipFiltru == 0)
						sprintf(fileNameSignal, "Grafic_spectru_Fereastra_Blackman_Harris_Filtru_FIREquiRpl_trece_sus_numar_de_puncte_%d_cadru_%d.jpg",numarDepuncte,cadru + 1);
					else if (tipFiltru == 1)
						sprintf(fileNameSignal, "Grafic_spectru_Fereastra_Blackman_Harris_Filtru_Eliptic_trece_sus_numar_de_puncte_%d_cadru_%d.jpg",numarDepuncte,cadru + 1);
					}		
				else
				{	if(tipFiltru == 0)
						sprintf(fileNameSignal, "Grafic_spectru_Fereastra_Welch_Filtru_FIREquiRpl_trece_sus_numar_de_puncte_%d_cadru_%d.jpg",numarDepuncte,cadru + 1);
					else if (tipFiltru == 1)
						sprintf(fileNameSignal, "Grafic_spectru_Fereastra_Welch_Filteu_Eliptic_trece_sus_numar_de_puncte_%d_cadru_%d.jpg",numarDepuncte,cadru + 1);
				}
				GetCtrlDisplayBitmap(panel, PANEL_2_GRAPH_SPECTRUM_FILTRE, 1, &bitmapID);
				SaveBitmapToJPEGFile(bitmapID, fileNameSignal, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(bitmapID);
			}
			
			
			///AFISAREA SE FACE PE SECUNDA SPECIFICATA
			else if (tipAfisare == 0)
			{
				//salvare pe secunde
				
				//SPECTRU SEMNAL INITIAL
				GetCtrlVal(panel, PANEL_2_RING_SECOND, &secunda);
				GetCtrlVal(panel, PANEL_2_RING, &numarDepuncte);
				GetCtrlVal(panel, PANEL_2_GRAPH_WINDOW, &tipFereastra);
				GetCtrlVal(panel, PANEL_2_FILTER_TYPE, &tipFiltru);
				
				sprintf(fileNameSignal, "Grafic_spectru_initial_numar_de_puncte_%d_la_secundele_%d_%d.jpg",numarDepuncte,secunda,secunda+1);
				GetCtrlDisplayBitmap(panel, PANEL_2_GRAPH_SPECTRUM, 1, &bitmapID);
				SaveBitmapToJPEGFile(bitmapID, fileNameSignal, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(bitmapID);
				
				//SEMNAL FILTRAT
				if(tipFiltru == 0) //FIR EquiRpl trece sus
					sprintf(fileNameSignal, "Grafic_semnal_Filtru_FIREquiRpl_trece_sus_numar_de_puncte_%d_Interval_de_timp_%d_%d.jpg",numarDepuncte,secunda,secunda+1);
				else if (tipFiltru == 1)//Eliptic trece sus
					sprintf(fileNameSignal, "Grafic_semnal_Filtru_Eliptic_trece_sus_numar_de_puncte_%d_Interval_de_timp_%d_%d.jpg",numarDepuncte,secunda,secunda+1);
				
				GetCtrlDisplayBitmap(panel, PANEL_2_GRAPH_ROW_FILTRED, 1, &bitmapID);
				SaveBitmapToJPEGFile(bitmapID, fileNameSignal, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(bitmapID);
				
				//SEMNAL FILTRAT SI FERESTRUIT
				if(tipFereastra == 0)
				{	if(tipFiltru == 0)
						sprintf(fileNameSignal, "Grafic_semnal_Fereastra_Blackman_Harris_Filtru_FIREquiRpl_trece_sus_numar_de_puncte_%d_Interval_de_timp_%d_%d.jpg",numarDepuncte,secunda,secunda+1);
					else if (tipFiltru == 1)
						sprintf(fileNameSignal, "Grafic_semnal_Fereastra_Blackman_Harris_Filtru_Eliptic_trece_sus_numar_de_puncte_%d_Interval_de_timp_%d_%d.jpg",numarDepuncte,secunda,secunda+1);
					}		
				else
				{	if(tipFiltru == 0)
						sprintf(fileNameSignal, "Grafic_semnal_Fereastra_Welch_Filtru_FIREquiRpl_trece_sus_numar_de_puncte_%d_Interval_de_timp_%d_%d.jpg",numarDepuncte,secunda,secunda+1);
					else if (tipFiltru == 1)
						sprintf(fileNameSignal, "Grafic_semnal_Fereastra_Welch_Filteu_Eliptic_trece_sus_numar_de_puncte_%d_Interval_de_timp_%d_%d.jpg",numarDepuncte,secunda,secunda+1);
				}
				GetCtrlDisplayBitmap(panel, PANEL_2_GRAPH_WINDOWING, 1, &bitmapID);
				SaveBitmapToJPEGFile(bitmapID, fileNameSignal, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(bitmapID);
				
				//SPECTRU SEMNAL FILTRAT SI FERESTRUIT
				if(tipFereastra == 0)
				{	if(tipFiltru == 0)
						sprintf(fileNameSignal, "Grafic_spectru_Fereastra_Blackman_Harris_Filtru_FIREquiRpl_trece_sus_numar_de_puncte_%d_Interval_de_timp_%d_%d.jpg",numarDepuncte,secunda,secunda+1);
					else if (tipFiltru == 1)
						sprintf(fileNameSignal, "Grafic_spectru_Fereastra_Blackman_Harris_Filtru_Eliptic_trece_sus_numar_de_puncte_%d_Interval_de_timp_%d_%d.jpg",numarDepuncte,secunda,secunda+1);
					}		
				else
				{	if(tipFiltru == 0)
						sprintf(fileNameSignal, "Grafic_spectru_Fereastra_Welch_Filtru_FIREquiRpl_trece_sus_numar_de_puncte_%d_Interval_de_timp_%d_%d.jpg",numarDepuncte,secunda,secunda+1);
					else if (tipFiltru == 1)
						sprintf(fileNameSignal, "Grafic_spectru_Fereastra_Welch_Filteu_Eliptic_trece_sus_numar_de_puncte_%d_Interval_de_timp_%d_%d.jpg",numarDepuncte,secunda,secunda+1);
				}
				GetCtrlDisplayBitmap(panel, PANEL_2_GRAPH_SPECTRUM_FILTRE, 1, &bitmapID);
				SaveBitmapToJPEGFile(bitmapID, fileNameSignal, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(bitmapID);
			
			}
			//AFISARE PE TOT SUNETUL
			else if (tipAfisare == 2) 
			{
				//salvare tot 
				
				//SPECTRU SEMNAL INITIAL
				GetCtrlVal(panel, PANEL_2_RING_SECOND, &secunda);
				GetCtrlVal(panel, PANEL_2_RING, &numarDepuncte);
				GetCtrlVal(panel, PANEL_2_GRAPH_WINDOW, &tipFereastra);
				GetCtrlVal(panel, PANEL_2_FILTER_TYPE, &tipFiltru);
				
				sprintf(fileNameSignal, "Grafic_spectru_initial_numar_de_puncte_%d.jpg",numarDepuncte);
				GetCtrlDisplayBitmap(panel, PANEL_2_GRAPH_SPECTRUM, 1, &bitmapID);
				SaveBitmapToJPEGFile(bitmapID, fileNameSignal, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(bitmapID);
				
				//SEMNAL FILTRAT
				if(tipFereastra == 0)
				{
					if(tipFiltru == 0)
					{
						sprintf(fileNameSignal, "Grafic_semnal_Filtru_FIREquiRpl_Fereastra_Blackman-Harris_numar_de_puncte_%d.jpg",numarDepuncte);
					}
					else if (tipFiltru == 1)
					{
						sprintf(fileNameSignal, "Grafic_semnal_Filtru_Eliptic_Fereastra_Blackman-Harris_numar_de_puncte_%d.jpg",numarDepuncte);
					}
				}
				else
				{
					if(tipFiltru == 0)
					{
						sprintf(fileNameSignal, "Grafic_semnal_Filtru_FIREquiRpl_Fereastra_Weltch_numar_de_puncte_%d.jpg",numarDepuncte);
					}
					else if (tipFiltru == 1)
					{
						sprintf(fileNameSignal, "Grafic_semnal_Filtru_Eliptic_Fereastra_Weltch_numar_de_puncte_%d.jpg",numarDepuncte);
					}
				}
				
				GetCtrlDisplayBitmap(panel, PANEL_2_GRAPH_WINDOWING, 1, &bitmapID);
				SaveBitmapToJPEGFile(bitmapID, fileNameSignal, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(bitmapID);
				
				//SEMNAL FILTRAT SI FERESTRUIT
				if(tipFereastra == 0)
				{	if(tipFiltru == 0)
						sprintf(fileNameSignal, "Grafic_semnal_Filtru_FIREquiRpl_numar_de_puncte_%d.jpg",numarDepuncte);
					else if (tipFiltru == 1)
						sprintf(fileNameSignal, "Grafic_semnal_Filtru_Eliptic_numar_de_puncte_%d.jpg",numarDepuncte);
					}		
				else
				{	if(tipFiltru == 0)
						sprintf(fileNameSignal, "Grafic_semnal_Filtru_FIREquiRpl_numar_de_puncte_%d.jpg",numarDepuncte);
					else if (tipFiltru == 1)
						sprintf(fileNameSignal, "Grafic_semnal_Filtru_Eliptic_numar_de_puncte_%d.jpg",numarDepuncte);
				}
				GetCtrlDisplayBitmap(panel, PANEL_2_GRAPH_ROW_FILTRED, 1, &bitmapID);
				SaveBitmapToJPEGFile(bitmapID, fileNameSignal, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(bitmapID);
				
				//SPECTRU SEMNAL FILTRAT SI FERESTRUIT
				if(tipFereastra == 0)
				{	if(tipFiltru == 0)
						sprintf(fileNameSignal, "Grafic_spectru_Fereastra_Blackman_Harris_Filtru_FIREquiRpl_numar_de_puncte_%d.jpg",numarDepuncte);
					else if (tipFiltru == 1)
						sprintf(fileNameSignal, "Grafic_spectru_Fereastra_Blackman_Harris_Filtru_Eliptic_numar_de_puncte_%d.jpg",numarDepuncte);
					}		
				else
				{	if(tipFiltru == 0)
						sprintf(fileNameSignal, "Grafic_spectru_Fereastra_Welch_Filtru_FIREquiRpl_numar_de_puncte_%d.jpg",numarDepuncte);
					else if (tipFiltru == 1)
						sprintf(fileNameSignal, "Grafic_spectru_Fereastra_Welch_Filtru_Eliptic_numar_de_puncte_%d.jpg",numarDepuncte);
				}
				GetCtrlDisplayBitmap(panel, PANEL_2_GRAPH_SPECTRUM_FILTRE, 1, &bitmapID);
				SaveBitmapToJPEGFile(bitmapID, fileNameSignal, JPEG_PROGRESSIVE, 100);
				DiscardBitmap(bitmapID);
			}
			break;
	}
	return 0;
}

//scriere audio a rezultatelor din domeniul timp
int CVICALLBACK OnWriteOutput (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	int tip,dim;
	double alfa;
	char fileNameSignal[100] = {0};
	FILE *fp;
	
	switch (event)
	{
		case EVENT_COMMIT:
			fp = fopen("numeMelodie.txt","w");
			
			GetCtrlVal(panel, PANEL_RING_FILTRU, &tip);
			GetCtrlVal(panel, PANEL_RING_DIM, &dim);
			GetCtrlVal(panel, PANEL_NUMERIC, &alfa);
			
			int indexStart = (int)secValStanga * npoints / 6;
			int indexStop = (int)secValDreapta * npoints / 6;

			if (flag_dateFiltrate == 1) //sunetul filtrat
			{
				if (tip == 1) //prin mediere
				{
					sprintf(fileNameSignal, "sunet_filtrat_mediere_%d_%d_%d.wav", dim, secValStanga, secValDreapta);
				}
				else // cu elem de ordin 1
				{
					sprintf(fileNameSignal, "sunet_filtrat_ordin1_%.2lf_%d_%d.wav", alfa, secValStanga, secValDreapta);
				}
			}
			else if (flag_dateDerivate == 1) //sunetul derivat
			{
				sprintf(fileNameSignal, "sunet_derivat_%d_%d.wav", secValStanga, secValDreapta);
			}
			
			if ( fp ) //daca se poate deschide sau nu fisierul
   			{
	   			fputs(fileNameSignal,fp);
    		}
   			else
      		{
         		printf("Nu se poate deschide fisierul\n");
        	}
		
   			fclose(fp);//inchid fisierul
		
			if(flag_dateDerivate == 1)
			{
				ArrayToFile ("waveDataOutput.txt", derivata + indexStart, VAL_DOUBLE,indexStop - indexStart, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_SEP_BY_TAB, VAL_CONST_WIDTH, VAL_ASCII, VAL_TRUNCATE);
				LaunchExecutable("python writeWav.py");
			}
			else if(flag_dateFiltrate == 1)
			{
				ArrayToFile ("waveDataOutput.txt", waveDataFiltrat + indexStart, VAL_DOUBLE,indexStop - indexStart, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_SEP_BY_TAB, VAL_CONST_WIDTH, VAL_ASCII, VAL_TRUNCATE);
				LaunchExecutable("python writeWav.py");
			}
			
			break;
	}
	return 0;
}

//scriere audio a rezultatelor din frecventa
int CVICALLBACK SaveWaveFrequency (int panel, int control, int event,
								   void *callbackData, int eventData1, int eventData2)
{
	char fileNameSignal[100] = {0};
	FILE *fp;
	int tipAfisare = 0;
	int secunda;
	
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panel,PANEL_2_RING_TIP_AFISARE,&tipAfisare);
			fp = fopen("numeMelodie.txt","w");
			
			if(tipAfisare == 0)
			{
				GetCtrlVal(panel, PANEL_2_RING_SECOND, &secunda);
				sprintf(fileNameSignal, "filtru_frecventa_%d_%d.wav",secunda,secunda+1);
				if ( fp )
	   			{
		   			fputs(fileNameSignal,fp);
	    		}
	   			else
	      		{
	         		printf("Failed to open the file\n");
	        	}
				
	   			fclose(fp);//inchidere fisier
				ArrayToFile ("waveDataOutput.txt", filteredSignal, VAL_DOUBLE,npoints/6, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_SEP_BY_TAB, VAL_CONST_WIDTH, VAL_ASCII, VAL_TRUNCATE);
			}
			
			else if(tipAfisare == 2)
			{
				sprintf(fileNameSignal, "filtru_frecventa.wav");
				if ( fp )
	   			{
		   			fputs(fileNameSignal,fp);
	    		}
	   			else
	      		{
	         		printf("Failed to open the file\n");
	        	}
				//inchidere fisier
	   			fclose(fp);
				ArrayToFile ("waveDataOutput.txt", filteredSignal, VAL_DOUBLE,npoints, 1,VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_SEP_BY_TAB, VAL_CONST_WIDTH, VAL_ASCII, VAL_TRUNCATE);
			}
			LaunchExecutable("python writeWav.py");
			break;
	}
	return 0;
}

