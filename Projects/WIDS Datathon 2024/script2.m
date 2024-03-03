clear all
close all
clc

%% Parte 1: pentru pregatire date 

% ce coloane folosim
%   fara 1 (id), 2 (race), 7 (F), 8 (bmi), 10 (med code), 13 (idem cu 12), 28 (idem cu 29)
%   numai una dintre coloanele  5 (zip), 4 (state), 15 (division), 14 (region),

idxCol=[3, 15, 6, 9, 11, 12, 16:27, 29:82];

idxPop=[16:28, 30:34, 66:73];
    % numar [16] si densitate [17]
	% varsta medie  [18]+ distributie pe categorii de varsta [19] – [27]
	% distributie pe sexe [28], [29]
	% dimensiune medie familie [34]
	% stare civila [30]-[33]  ???
	% distributie pe rase [66] –[73]
    
   %  relevante? [desnity, age30, age60, age under10, fam sizs, races, fam_size]

idxEd=[55:61, 76];
    % pe fiecare nivel [55]-[60}, % STEM [61] %limited eng 76
    % relevante? bachelor, limites or less highsch, some coll?

idxVenit= [35:54, 62:65,74,75, 77,78,79 ]
    % Mediu pe familie [35], pe individ [49], distributie pe categorii de venit [36]-[48]
    % Nr. proprietari case [50], numar case [51],  valoare medie case [52]
    % Credite [53],[54]
    % Durata medie deplasare la job [77] 
    % Structura populatie active[62]-[65]

%idcxCol=idxVenit;

idxMediu=[80:82];

%idxCol=idxVenit; %de schimabt aici idx dorit!!!!

% citire date dupa corectie si selectare coloane de interes
dataTrainInit=readtable('trainModifV2.csv');
dataTrain=dataTrainInit(:, [idxCol,83]);

dataTestInit=readtable('testModifV2.csv');
dataTest=dataTestInit(:, idxCol);

%% Export model din classificatioLeaner 
% trainedModel

%% Parte 2: pentru generare fisier rezultate

% Calcul iesirri
[y_model_Train, scores_Train]=trainedModel3.predictFcn(dataTrain);
[y_model_Test, scores_Test]=trainedModel3.predictFcn(dataTest);

% Fisier rezulta
aux= array2table(y_model_Test,'VariableNames',{'DiagPeriodL90D'});
aux =  [dataTestInit(:,1), aux];
writetable(aux,'sample38.csv')

% Salvare model si date
numeFisier='v38rez.mat';
feval(@save,numeFisier);