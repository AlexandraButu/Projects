table=readtable('training_colK_modified.xlsx');
coloanaL_cod=table.('metastatic_first_novel_treatment');
coloanaM_cod=table.('metastatic_first_novel_treatment_type');

for i=1:size(coloanaL_cod)
    if coloanaL_cod{i} == "OLAPARIB" ||  coloanaL_cod{i} == "PEMBROLIZUMAB" 
         coloanaL_cod{i} = '1';
    else
        coloanaL_cod{i} = '0';
    end
end

for j=1:size(coloanaM_cod)
    if  coloanaM_cod{j} == "Antineoplastics"
        coloanaM_cod{j} = '1';
    else 
        coloanaM_cod{j} = '0';
    end
end

table.metastatic_first_novel_treatment = coloanaL_cod;
table.metastatic_first_novel_treatment_type = coloanaM_cod;

writetable(table, 'training_colL&M_modified.xlsx', 'Sheet', 1);

