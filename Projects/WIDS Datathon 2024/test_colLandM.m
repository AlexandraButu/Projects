table=readtable('test_colK_modified.xlsx');
coloanaL_cod=table.('metastatic_first_novel_treatment');
coloanaM_cod=table.('metastatic_first_novel_treatment_type');

for i=1:size(coloanaL_cod)
     coloanaL_cod(i) = "0";
end

for j=1:size(coloanaM_cod)
     coloanaM_cod(j) = "0";
end

table.metastatic_first_novel_treatment = coloanaL_cod;
table.metastatic_first_novel_treatment_type = coloanaM_cod;

writetable(table, 'test_colL&M_modified.xlsx', 'Sheet', 1);