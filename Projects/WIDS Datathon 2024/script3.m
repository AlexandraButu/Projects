YTrain1=trainedModel1.predictFcn(XTrain);
YTrain2=trainedModel2.predictFcn(XTrain);
YTrain3=trainedModel3.predictFcn(XTrain);

ACC_Train1=mean(double(YTrain)==double(YTrain1))
ACC_Train2=mean(double(YTrain)==double(YTrain2))
ACC_Train3=mean(double(YTrain)==double(YTrain3))

YVal1=trainedModel1.predictFcn(XValidation);
YVal2=trainedModel2.predictFcn(XValidation);
YVal3=trainedModel3.predictFcn(XValidation);

ACC_Val1=mean(double(YValidation)==double(YVal1))
ACC_Val2=mean(double(YValidation)==double(YVal2))
ACC_Val3=mean(double(YValidation)==double(YVal3))

YTest1=trainedModel1.predictFcn(dataTest);
YTest2=trainedModel2.predictFcn(dataTest);
YTest3=trainedModel3.predictFcn(dataTest);

%ACC_Test1=mean(double(YTest)==double(YTest1))
%ACC_Test2=mean(double(YTest)==double(YTest2))
%ACC_Test3=mean(double(YTest)==double(YTest3))

aux=YTrain1+YTrain2+YTrain3;
y_model_Train=(aux>=2); %pune val 1, altfel 0
aux=YVal1+YVal2+YVal3;
y_model_Validation=(aux>=2);
aux=YTest1+YTest2+YTest3;
y_model_Test=(aux>=2);

ACC_Validation=mean(double(YValidation)==double(y_model_Validation))
ACC_Train=mean(double(YTrain)==double(y_model_Train))

% Fisier rezultat
aux= array2table(y_model_Test,'VariableNames',{'DiagPeriodL90D'});
aux =  [dataTestInit(:,1), aux];
writetable(aux,'sample.csv')
