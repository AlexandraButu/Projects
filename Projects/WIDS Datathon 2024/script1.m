clear all
close all
clc

%% ce coloane folosim
% fara 1 (id), 2 (race), 7 (F), 8 (bmi), 10 (med code), 13 (idem cu 12), 28 (idem cu 29)
% numai una dintre coloanele  5 (zip), 4 (state), 15 (division), 14 (region),

idxCol=[10, 3, 15, 6, 9, 11, 12, 23, 24, 31, 34, 59, 64, 78, 80:82];

% citire date dupa corectie si selectare coloane de interes
dataTrainInit=readtable('trainModifV2.csv');
dataTrain=dataTrainInit(:, [idxCol,83]);

dataTestInit=readtable('testModifV2.csv');
dataTest=dataTestInit(:, idxCol);

%% separare train - validare:
NoSamples = size(dataTrain,1);
YTrainInit=categorical(table2array(dataTrain(:,end)));

hpartition = cvpartition(YTrainInit,'Holdout',0.3,'Stratify',true); %70%-training, 30%-validation
idxTrain = training(hpartition);
idxValidation = test(hpartition);
XTrain = dataTrain(idxTrain,1:end-1); %table – only relevant columns!!
YTrain = categorical(table2array(dataTrain(idxTrain,end))); %table
XValidation = dataTrain(idxValidation,1:end-1); %table – only relevant columns!!
YValidation = categorical(table2array(dataTrain(idxValidation,end)));

%% model
NumTrees=100;
metoda='Bag';
% sau
%alte modele pentru "Binary classification": LPBoost, AdaBoost, RUSoost
mdl=fitcensemble(XTrain,YTrain,'Method',metoda,'NumLearningCycles',NumTrees); 

%% verificare rezultate
[y_model_Train, scores_Train]=predict(mdl,XTrain);
[y_model_Validation, scores_Validation]=predict(mdl,XValidation);

ACC_Validation=mean(double(YValidation)==double(y_model_Validation))
ACC_Train=mean(double(YTrain)==double(y_model_Train))

figure, plotconfusion(YTrain,y_model_Train)
figure, plotconfusion(YValidation,y_model_Validation)

[y_model_Test, scores_Test]=predict(mdl,dataTest);

%% Fisier rezultat
aux= array2table(y_model_Test,'VariableNames',{'DiagPeriodL90D'});
aux =  [dataTestInit(:,1), aux];
writetable(aux,'sample.csv')

%% Salvare model si date
numeFisier=['V1_',metoda,'_',num2str(NumTrees),'.mat'];
feval(@save,numeFisier);


%% pentru model exportat din classifierLearner:
%trainedModel.predictFcn(dataTest)
