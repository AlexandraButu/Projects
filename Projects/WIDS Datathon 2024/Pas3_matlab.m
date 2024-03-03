dataRawTrain=readtable('training_colL&M_modified.xlsx');
%dataTest=readtable('test_colL&M_modified.xlsx');
idx_col=[3,4,5,6,83]; %pt a selecta anumite coloane, de ex col cu indicii 2 si 3
dataTrain=dataRawTrain(:,idx_col);
%dataTest=dataTest(:,idx_col);


NoSamples = size(dataRawTrain,1);
hpartition = cvpartition(NoSamples,'Holdout',0.3); %70%-training, 30%-validation
idxTrain = training(hpartition);
idxValidation = test(hpartition);
XTrain = dataRawTrain(idxTrain,2:82); %table – only relevant columns!!
YTrain = dataRawTrain(idxTrain,83); %table
XValidation = dataRawTrain(idxValidation,2:82); %table – only relevant columns!!
YValidation = dataRawTrain(idxValidation,83);

%Bagged:
mdl=fitcensemble(XTrain,YTrain,'Method','Bag','NumLearningCycles',100);
[y_model, scores]=predict(mdl,XTrain);

