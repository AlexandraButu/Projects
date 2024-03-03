---Afisare continut toate tabelele
select * from client;
select * from produs;
select * from detalii_produs;
select * from ingredient;
select * from reteta;
select * from comanda;


---VALIDARE
---Verificam CHECK urile:

---la CLIENT verificam :NUMELE si PRENUMELE(conditia este sa contina doar litere ) 
insert into client(id_client,nume,prenume,numar_telefon,email) values(6,'Botezatu','Aurelia4','0716266648','aureliabotezatu@yahoo.com');

---NUMARUL DE TELEFON sa contina exact 10 cifre si sa fie unic

---mai jos este gresit si avem eroare, deoarece are 11 cifre 
insert into client(id_client,nume,prenume,numar_telefon,email) values(6,'Botezatu','Aurelia','07021681309','aureliabotezatu@yahoo.com');
---am pus un numar de telefon identic a altui client deja existent si da eroare, deoarece nu e unic
insert into client(id_client,nume,prenume,numar_telefon,email) values(6,'Botezatu','Aurelia','0736264948','aureliabotezatu@yahoo.com');

---EMAIL sa respecte formatul denumire@email.com si sa fie UNIC

---mai jos avem eroare, deoarece lipseste @
insert into client(id_client,nume,prenume,numar_telefon,email) values(6,'Botezatu','Aurelia4','0716266648','aureliabotezatuyahoo.com');
---avem eroare deoarece e identic cu al altui client, nu e unic
insert into client(id_client,nume,prenume,numar_telefon,email) values(6,'Botezatu','Aurelia4','0716266648','daniella91@yahoo.com');

---la COMANDA avem:

---STATUS ul sa respecte formatul 'In curs de preparare', 'Livrata', 'Preluata' , mai jos nu respecta
insert into comanda(id_comanda,data_comenzii,status,metoda_plata,client_id_client,produs_id_produs) values(6,SYSDATE,'In asteptare ','Cash',5,2);

---METODA PLATA sa respecte formatul  'Cash', 'Cu card' , mai jos nu respecta
insert into comanda(id_comanda,data_comenzii,status,metoda_plata,client_id_client,produs_id_produs) values(6,SYSDATE,'Preluata','Numerar',4,4);

---la INGREDIENT avem :

---denumirea ingredientului >=3 , mai jos nu se respecta
insert into ingredient(id_ingredient,nume_ingredient, cantitate, unitate_de_masura) values(11,'ab',100,'g');
--- denumirea ingredientului sa fie unica, mai jos nu se respecta, avem deja aceeasi denumire la id 10
insert into ingredient(id_ingredient,nume_ingredient, cantitate, unitate_de_masura) values(11,'unt',100,'g');

---la PRODUS avem:

---denumirea produsului >=5, mai jos nu se respecta:
insert into produs(id_produs,denumire_produs,pret,gramaj_grame) values(6,'abcd',100,100);
--- denumirea produsului sa fie unica, mai jos nu respecta , exista deja denumirea la id 2
insert into produs(id_produs,denumire_produs,pret,gramaj_grame) values(6,'Briosa aromata',100,100);

---la DETALII_PRODUS avem : 

---Data fabricarii trebuie sa fie <= data de azi , acum suntem in 17 decembrie, incerc sa introduc data de 18 dec si imi da eroare
insert into produs(id_produs,denumire_produs,pret,gramaj_grame) values(6,'Produs nou',100,1000);
insert into detalii_produs(alergeni, data_fabricarii, data_expirarii,eticheta, produs_id_produs) values('or',to_date('18.12.2023','DD.MM.YYYY'),to_date('21.12.2023','DD.MM.YYYY'),'de frupt',6);

---Data expirarii trebuie sa fie > data de azi, azi suntem in 17 decembrie, incerc sa introduc tot data de azi, dar da eroare
insert into detalii_produs(alergeni, data_fabricarii, data_expirarii,eticheta, produs_id_produs) values('or',SYSDATE,SYSDATE,'de frupt',6);
---Data fabricarii trebuie sa fie <= data curenta
insert into detalii_produs(alergeni, data_fabricarii, data_expirarii,eticheta, produs_id_produs) values('or',to_date('26.12.2023','DD.MM.YYYY'),to_date('25.12.2023','DD.MM.YYYY'),'de frupt',6);

 


---VIZUALIZARE


---Ce clienti au cumparat prajitura cu id ul 3
SELECT DISTINCT c.nume, c.prenume
FROM client c
JOIN comanda co ON c.id_client = co.client_id_client
JOIN produs p ON co.produs_id_produs = p.id_produs
WHERE p.id_produs = 3;


--- Ce ingrediente contine o briosa
SELECT i.nume_ingredient, i.cantitate, i.unitate_de_masura
FROM produs p
JOIN reteta r ON p.id_produs = r.produs_id_produs
JOIN ingredient i ON r.ingredient_id_ingredient = i.id_ingredient
WHERE p.denumire_produs = 'Briosa aromata';


---Suma platita de clientul 4 pentru comanda sa 
SELECT SUM(p.pret) AS total_platit
FROM comanda c
JOIN produs p ON c.produs_id_produs = p.id_produs
WHERE c.client_id_client = 4;


---Cati clienti au cumparat produse de post
SELECT COUNT(DISTINCT c.client_id_client) AS numar_clienti
FROM comanda c
JOIN detalii_produs dp ON c.produs_id_produs = dp.produs_id_produs
WHERE dp.eticheta = 'produs de post';


---Afisare nume, prenume si numar de telefon al clientului care comanda un produs ce are in compozitia sa UNT
SELECT c.nume, c.prenume, c.numar_telefon
FROM client c
JOIN comanda co ON c.id_client = co.client_id_client
JOIN produs p ON co.produs_id_produs = p.id_produs
JOIN reteta r ON p.id_produs = r.produs_id_produs
JOIN ingredient i ON r.ingredient_id_ingredient = i.id_ingredient
WHERE i.nume_ingredient = 'unt';



---UPDATE

---Actualizez toate înregistrarile din tabela RETETA unde coloana timp_preparare_minute este null, cu valoarea 50
UPDATE reteta
SET TIMP_PREPARARE_MINUTE = 50
WHERE TIMP_PREPARARE_MINUTE IS NULL;

---Actualizarea pretului produsului cu id-ul 5 dupa o scumpire cu 10%
UPDATE produs
SET pret = pret * 1.1
WHERE id_produs = 5;

---Actualizarea adresei de email a clientului cu id-ul 3
UPDATE client
SET email = 'andreicusma21@exemplu.com'
WHERE id_client = 3;




---STERGERE

select * from detalii_produs;
---stergere detaliile produselor care expira inainte de data de 5 martie (am pus data asa inaintata pentru a nu fi erori cand testati proiectul)
DELETE FROM detalii_produs WHERE data_expirarii < TO_DATE('2024-3-5', 'YYYY-MM-DD');
select * from detalii_produs;


select * from comanda;
---stergere inregistrari din comanda care contin produse ce au ca alergeni : soia
DELETE FROM comanda
WHERE produs_id_produs IN (
    SELECT dp.produs_id_produs
    FROM detalii_produs dp
    WHERE dp.alergeni LIKE '%soia%'
   
);

select * from client;
 -- stergere din tabela client clientul a carui nume incepe cu litera I 
 
 -- Stergere inregistrari din tabela "comanda" care sunt legate de clientul cu numele care incepe cu "I"
DELETE FROM comanda
WHERE client_id_client IN (
    SELECT id_client
    FROM client
    WHERE nume LIKE 'I%'
);

-- Stergere inregistrari din tabela "client" care au numele care incepe cu "I"
DELETE FROM client
WHERE nume LIKE 'I%';

select * from client;


