
---CLIENT 


insert into client(nume,prenume,numar_telefon,email) values('Chiriac','Iolanda','0707111309','iolaiolanda711@yahoo.com');
insert into client(nume,prenume,numar_telefon,email) values('Iacob','Matei','0707615499','mateiiacboob9@yahoo.com');
insert into client(nume,prenume,numar_telefon,email) values('Cusma','Andrei','0721122980','cusmandrr21@yahoo.com');
insert into client(nume,prenume,numar_telefon,email) values('Razgan','Daniela','0736264948','daniella91@yahoo.com');
insert into client(nume,prenume,numar_telefon,email) values('Mihaila','Ion','0707121303','mihailaion8@yahoo.com');

select * from client;


---PRODUS

 
insert into produs(denumire_produs,pret,gramaj_grame) values('Deliciu de toamna cu mere',40,400);
insert into produs(denumire_produs,pret,gramaj_grame) values('Briosa aromata',15,200);
insert into produs(denumire_produs,pret,gramaj_grame) values('Fursec crocant si delicios',12,150);
insert into produs(denumire_produs,pret,gramaj_grame) values('Extaz dulce cu ciocolata',25,250);
insert into produs(denumire_produs,pret,gramaj_grame) values('Fericire in straturi',100,1000);

select * from produs;


--- DETALII PRODUS
---am selectat o data mai inaintata pentru expirare pentru a nu avea erori la rularea proiectului cand il testati
insert into detalii_produs(alergeni, data_fabricarii, data_expirarii,eticheta, produs_id_produs) values('gluten',SYSDATE,to_date('1.03.2024','DD.MM.YYYY'),'produs de post',1);
insert into detalii_produs(alergeni, data_fabricarii, data_expirarii,eticheta, produs_id_produs) values('ou, ciocolata',SYSDATE,to_date('1.03.2024','DD.MM.YYYY'),'produs de frupt',2);
insert into detalii_produs(alergeni, data_fabricarii, data_expirarii,eticheta, produs_id_produs) values('soia',SYSDATE,to_date('4.03.2024','DD.MM.YYYY'),'vegan',3);
insert into detalii_produs(alergeni, data_fabricarii, data_expirarii,eticheta, produs_id_produs) values('fructe cu coaja',SYSDATE,to_date('11.03.2024','DD.MM.YYYY'),'produs de post',4);
insert into detalii_produs(alergeni, data_fabricarii, data_expirarii,eticheta, produs_id_produs) values('ciocolata, ou, lactoza',SYSDATE,to_date('15.03.2024','DD.MM.YYYY'),'de frupt',5);

select * from detalii_produs;



--- INGREDIENT


insert into ingredient(nume_ingredient, cantitate, unitate_de_masura) values('oua',1,'buc');
insert into ingredient(nume_ingredient, cantitate, unitate_de_masura) values('ciocolata',100,'g');
insert into ingredient(nume_ingredient, cantitate, unitate_de_masura) values('lapte',100,'ml');
insert into ingredient(nume_ingredient, cantitate, unitate_de_masura) values('mar',100,'g');
insert into ingredient(nume_ingredient, cantitate, unitate_de_masura) values('faina',100,'g');
insert into ingredient(nume_ingredient, cantitate, unitate_de_masura) values('drojdie',10,'g');
insert into ingredient(nume_ingredient, cantitate, unitate_de_masura) values('praf de copt',10,'g');
insert into ingredient(nume_ingredient, cantitate, unitate_de_masura) values('soia',50,'g');
insert into ingredient(nume_ingredient, cantitate, unitate_de_masura) values('zahar',100,'g');
insert into ingredient(nume_ingredient, cantitate, unitate_de_masura) values('unt',100,'g');

select * from ingredient;


---RETETA

---reteta 1 - Tarta cu mere care contine mar(4),faina(5),drojdie(6),praf de copt(7)
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(1,4,1,'Tarta cu mere', 60,'Merele se rad la final',5);
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(1,5,1,'Tarta cu mere', 60,'Merele se rad la final',3);
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(1,6,1,'Tarta cu mere', 60,'Merele se rad la final',4);
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(1,7,1,'Tarta cu mere', 60,'Merele se rad la final',2);

---reteta 2 - Briosa care contine: oua(1), ciocolata(2), lapte(3) , faina(5)
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(2,1,2,'Briosa', 45,'Se adauga topping de ciocolata',1);
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(2,2,2,'Briosa', 45,'Se adauga topping de ciocolata',2);
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(2,3,2,'Briosa', 45,'Se adauga topping de ciocolata',2);
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(2,5,2,'Briosa', 45,'Se adauga topping de ciocolata',1);

---reteta 3- Fursec care contine: faina(5), drojdie(6), praf de copt(7), soia(8)
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(3,5,3,'Fursec', 30,'Se adauga esenta de vanilie pentru aroma',2);
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(3,6,3,'Fursec', 30,'Se adauga esenta de vanilie pentru aroma',3);
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(3,7,3,'Fursec', 30,'Se adauga esenta de vanilie pentru aroma',1);
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(3,8,3,'Fursec', 30,'Se adauga esenta de vanilie pentru aroma',5);

---reteta 4 - Negresa care contine: ciocolata(2), faina(5), zahar(9)
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(4,2,4,'Negresa',NULL ,'Se presara zahar pudra',3);
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(4,5,4,'Negresa', NULL,'Se presara zahar pudra',2);
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(4,9,4,'Negresa', NULL,'Se presara zahar pudra',2);

---reteta 5 - Tort de ciocolata care continte: oua(1), ciocolata(2),lapte(3), faina(5), unt(10)
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(5,1,5,'Tort de ciocolata', 120,'Se imbiba in sirop',6);
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(5,2,5,'Tort de ciocolata', 120,'Se imbiba in sirop',7);
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(5,3,5,'Tort de ciocolata', 120,'Se imbiba in sirop',10);
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(5,5,5,'Tort de ciocolata', 120,'Se imbiba in sirop',10);
insert into reteta( produs_id_produs,ingredient_id_ingredient,id_reteta,nume_reteta,timp_preparare_minute, specificatii, cantitate) values(5,10,5,'Tort de ciocolata', 120,'Se imbiba in sirop',5);

select * from reteta;



---COMANDA

---Clientul 1 a plasat comanda 1 ce contine produsele : 2 buc x id_produs 3 
insert into comanda(id_comanda,data_comenzii,status,metoda_plata,client_id_client,produs_id_produs) values(1,SYSDATE,'Livrata','Cash',1,3);
insert into comanda(id_comanda,data_comenzii,status,metoda_plata,client_id_client,produs_id_produs) values(1,SYSDATE,'Livrata','Cash',1,3);

---Clientul 2 a plasat comanda 2 ce contine produsele : 1 buc x id_produs 1 , 1 buc x id_produs 2 , 1 buc x id_produs 3
insert into comanda(id_comanda,data_comenzii,status,metoda_plata,client_id_client,produs_id_produs) values(2,SYSDATE,'Livrata','Cu card',2,1);
insert into comanda(id_comanda,data_comenzii,status,metoda_plata,client_id_client,produs_id_produs) values(2,SYSDATE,'Livrata','Cu card',2,2);
insert into comanda(id_comanda,data_comenzii,status,metoda_plata,client_id_client,produs_id_produs) values(2,SYSDATE,'Livrata','Cu card',2,3);

---Clientul 3 a plasat comanda 3 ce contine produsele : 1 buc x id_produs 5 
insert into comanda(id_comanda,data_comenzii,status,metoda_plata,client_id_client,produs_id_produs) values(3,SYSDATE,'In curs de preparare','Cu card',3,5);

---Clientul 4 a plasat comanda 4 ce contine produsele : 1 buc x id_produs 4 , 1 buc x id_produs 1 
insert into comanda(id_comanda,data_comenzii,status,metoda_plata,client_id_client,produs_id_produs) values(4,SYSDATE,'Preluata','Cash',4,1);
insert into comanda(id_comanda,data_comenzii,status,metoda_plata,client_id_client,produs_id_produs) values(4,SYSDATE,'Preluata','Cash',4,4);

---Clientul 5 a plasat comanda 5 ce contine produsele : 1 buc x id_produs 2
insert into comanda(id_comanda,data_comenzii,status,metoda_plata,client_id_client,produs_id_produs) values(5,SYSDATE,'Preluata','Cash',5,2);

select * from comanda;
