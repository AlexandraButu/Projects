from flask import Flask, render_template, request, redirect, g
import cx_Oracle
import os
from datetime import datetime

app = Flask(__name__)
with open(app.root_path + '\config.cfg', 'r') as f:
    app.config['ORACLE_URI'] = f.readline()
import os

cx_Oracle.init_oracle_client(os.path.join(os.getcwd(), "instantclient_21_7"))
dsn_tns = cx_Oracle.makedsn('bd-dc.cs.tuiasi.ro', '1539', 'orcl')
con = cx_Oracle.connect("bd015", "bd015", dsn_tns)


@app.route('/')
# Tabela CLIENT
@app.route('/clients')
def cl():
    clients = []

    cur = con.cursor()
    cur.execute('select * from client')  # Modificați cu numele real al tabelului
    for result in cur:
        client = {}
        client['id_client'] = result[0]
        client['nume'] = result[1]
        client['prenume'] = result[2]
        client['numar_telefon'] = result[3]
        client['email'] = result[4]

        clients.append(client)
    cur.close()
    return render_template('Clienti.html', clients=clients)


@app.route('/addClient', methods=['GET', 'POST'])
def add_cl():
    error = None
    if request.method == 'POST':
        # Conectarea la baza de date și codul pentru inserarea datelor
        emp = 0
        cur = con.cursor()
        cur.execute('SELECT MAX(id_client) FROM client')
        for result in cur:
            emp = result[0]
        cur.close()

        emp += 1
        cur = con.cursor()
        values = []
        values.append("'" + str(emp) + "'")
        values.append("'" + request.form['nume'] + "'")
        values.append("'" + request.form['prenume'] + "'")
        values.append("'" + request.form['numar_telefon'] + "'")
        values.append("'" + request.form['email'] + "'")

        fields = ['id_client', 'nume', 'prenume', 'numar_telefon', 'email']
        query = 'INSERT INTO client (%s) VALUES (%s)' % (', '.join(fields), ', '.join(values))

        cur.execute(query)
        con.commit()
        return redirect('/clients')

    # Redirecționează către pagina de clienți după adăugare
    else:
        return render_template('addClient.html')
    # Returnează șablonul HTML pentru adăugarea unui client


@app.route('/deleteClient', methods=['POST'])
def del_cl():
    emp = request.form['id_client']

    cur = con.cursor()
    cur.execute('delete from comanda where client_id_client=' + emp)
    cur.execute('delete from client where id_client=' + emp)
    cur.execute('commit')
    return redirect('/clients')



@app.route('/editClient', methods=['POST'])


def get_clnt():
    emp = request.form['id_client']
    cur = con.cursor()
    cur.execute('select * from client where id_client=' + emp)

    emps = cur.fetchone()
    id_client = emps[0]
    numar_telefon = emps[3]
    email = emps[4]

    cur.close()

    return render_template('editClient.html' , numar_telefon=numar_telefon, email=email, id_client=id_client)


@app.route('/editClien', methods=['POST'])
def edit_clnt():
    try:
        id_client = "'" + request.form['id_client'] + "'"
        numar_telefon = "'" + request.form['numar_telefon'] + "'"
        email = "'" + request.form['email'] + "'"

        cur = con.cursor()
        query = "UPDATE client SET numar_telefon =%s, email=%s where id_client=%s" % (numar_telefon, email, id_client)
        cur.execute(query)
        cur.execute('commit')
        return redirect('/clients')

    except Exception as e:
        error_message = str(e).splitlines()[0]
        return render_template('eroare.html', error_message=error_message, back_url='/clients')

@app.route('/getClient', methods=['POST'])



@app.route('/orders')
def orders():
    orders = []

    cur = con.cursor()
    cur.execute('SELECT * FROM comanda')  # Modificați cu numele real al tabelului
    for result in cur:
        order = {
            'id_comanda': result[0],
            'data_comenzii': result[1],
            'status': result[2],
            'metoda_plata': result[3],
            'client_id_client': result[4],
            'produs_id_produs': result[5],
            'nr_bucati': result[6]
        }
        orders.append(order)
    cur.close()
    return render_template('Comenzi.html', orders=orders)




@app.route('/addComanda', methods=['GET', 'POST'])
def add_cmnd():
    error = None
    if request.method == 'POST':
        try:
            con.begin()

            # Verificați stocul pentru fiecare ingredient în reteta produsului comandat
            produs_id = request.form['produs_id_produs']
            cur = con.cursor()
            cur.execute(f"SELECT ingredient_id_ingredient, cantitate FROM reteta WHERE produs_id_produs = {produs_id}")
            results = cur.fetchall()  # Obțineți toate rândurile rezultate

            for result in results:
                ingredient_id = result[0]
                cantitate_necesara = result[1]

                # Verificați dacă există suficientă cantitate în stoc pentru fiecare ingredient
                cur.execute(f"SELECT cantitate_ramasa FROM ingredient WHERE id_ingredient = {ingredient_id}")
                stoc_disponibil = cur.fetchone()[0]

                # Verificați dacă stocul este suficient pentru comandă
                if stoc_disponibil < cantitate_necesara * int(request.form['nr_bucati']):
                    con.rollback()
                    raise Exception(f"Nu există suficientă cantitate pentru ingredientul cu ID-ul {ingredient_id}")

                # Actualizați stocul de ingrediente
                cantitate_utilizata = cantitate_necesara * int(request.form['nr_bucati'])
                query_update_stoc = 'UPDATE ingredient SET cantitate_ramasa = cantitate_ramasa - :cantitate_utilizata WHERE id_ingredient = :ingredient_id'
                cur.execute(query_update_stoc,
                            {'cantitate_utilizata': cantitate_utilizata, 'ingredient_id': ingredient_id})

            # Adăugați comanda în tabelul 'comanda' după ce toate verificările au trecut
            values_comanda = {
                'id_comanda': request.form['id_comanda'],
                'data_comenzii': datetime.strptime(str(request.form['data_comenzii']), '%d-%m-%Y').strftime('%d-%b-%y'),
                'status': request.form['status'],
                'metoda_plata': request.form['metoda_plata'],
                'client_id_client': request.form['client_id_client'],
                'produs_id_produs': request.form['produs_id_produs'],
                'nr_bucati': request.form['nr_bucati']
            }
            query_comanda = 'INSERT INTO comanda (id_comanda, data_comenzii, status, metoda_plata, client_id_client, produs_id_produs, nr_bucati) VALUES (:id_comanda,:data_comenzii, :status, :metoda_plata, :client_id_client, :produs_id_produs, :nr_bucati)'
            cur.execute(query_comanda, values_comanda)

            # Comiteți tranzacția
            con.commit()

            return redirect('/orders')

        except Exception as e:
            con.rollback()
            error_message = str(e).splitlines()[0]
            return render_template('eroare.html', error_message=error_message, back_url='/orders')

    else:
        return render_template('addComanda.html')














@app.route('/deleteComanda', methods=['POST'])
def del_cmnd():
    emp = request.form['id_comanda']
    cur = con.cursor()
    cur.execute('delete from comanda where id_comanda=' + emp)
    cur.execute('commit')
    return redirect('/orders')


@app.route('/editComanda', methods=['POST'])


def get_cmnd():
    emp = request.form['id_comanda']
    cur = con.cursor()
    cur.execute('select * from comanda where id_comanda=' + emp)

    emps = cur.fetchone()
    id_comanda = emps[0]
    status = emps[2]
    metoda_plata = emps[3]

    cur.close()

    return render_template('editComanda.html', status=status, metoda_plata=metoda_plata, id_comanda=id_comanda)


@app.route('/editComand', methods=['POST'])
def edit_cmnd():
    try:
        id_comanda = "'" + request.form['id_comanda'] + "'"
        status = "'" + request.form['status'] + "'"
        metoda_plata = "'" + request.form['metoda_plata'] + "'"

        cur = con.cursor()
        query = "UPDATE comanda SET status =%s, metoda_plata=%s where id_comanda=%s" % (status, metoda_plata, id_comanda)
        cur.execute(query)
        cur.execute('commit')
        return redirect('/orders')

    except Exception as e:
        error_message = str(e).splitlines()[0]
        return render_template('eroare.html', error_message=error_message, back_url='/orders')


@app.route('/product_details')
def product_details():
    details = []

    cur = con.cursor()
    cur.execute('SELECT * FROM detalii_produs')  # Modificați cu numele real al tabelului carduri_de_biblioteca
    for result in cur:
        detail = {
            'alergeni': result[0],
            'data_fabricarii': result[1],
            'data_expirarii': result[2],
            'eticheta': result[3],
            'produs_id_produs': result[4]
        }
        details.append(detail)
    cur.close()
    return render_template('DetaliiProdus.html', details=details)



@app.route('/addDetalii', methods=['GET', 'POST'])
def add_det():
    error = None
    if request.method == 'POST':
        # Conectarea la baza de date și codul pentru inserarea datelor
        emp = 0
        cur = con.cursor()
        cur.execute('SELECT MAX(produs_id_produs) FROM detalii_produs')
        for result in cur:
            emp = result[0]
        cur.close()

        emp += 1
        cur = con.cursor()
        values = []
    #    values.append("'" + str(emp) + "'")
        values.append("'" + request.form['alergeni'] + "'")
        values.append("'" + request.form['data_fabricarii'] + "'")
        values.append("'" + request.form['data_expirarii'] + "'")
        values.append("'" + request.form['eticheta'] + "'")
        values.append("'" + request.form['produs_id_produs'] + "'")

        fields = ['alergeni', 'data_fabricarii', 'data_expirarii', 'eticheta' , 'produs_id_produs']
        query = 'INSERT INTO detalii_produs (%s) VALUES (%s)' % (', '.join(fields), ', '.join(values))

        cur.execute(query)
        con.commit()
        return redirect('/product_details')

    else:
        return render_template('addDetalii.html')




@app.route('/deleteDetalii', methods=['POST'])
def del_det():
    emp = request.form['produs_id_produs']
    cur = con.cursor()
    cur.execute('delete from detalii_produs where produs_id_produs=' + emp)
    cur.execute('delete from comanda where produs_id_produs=' + emp)
    cur.execute('delete from detalii_produs where produs_id_produs=' + emp)
    cur.execute('delete from reteta where produs_id_produs=' + emp)
    cur.execute('delete from produs where id_produs=' + emp)
    cur.execute('commit')
    return redirect('/product_details')






@app.route('/ingredients')
def ingredients():
    ingredients = []

    cur = con.cursor()
    cur.execute('SELECT * FROM ingredient')  # Modificați cu numele real al tabelului categorii
    for result in cur:
        ingredient = {
            'id_ingredient': result[0],
            'nume_ingredient': result[1],
            'cantitate': result[2],
            'cantitate_ramasa': result[3],
            'unitate_de_masura': result[4]

        }
        ingredients.append(ingredient)
    cur.close()
    return render_template('Ingredient.html', ingredients=ingredients)


@app.route('/addIngredient', methods=['GET', 'POST'])
def add_ingr():
    error = None
    if request.method == 'POST':
        # Conectarea la baza de date și codul pentru inserarea datelor
        emp = 0
        cur = con.cursor()
        cur.execute('SELECT MAX(id_ingredient) FROM ingredient')
        for result in cur:
            emp = result[0]
        cur.close()

        emp += 1
        cur = con.cursor()
        values = []
        values.append("'" + str(emp) + "'")
        values.append("'" + request.form['nume_ingredient'] + "'")
        values.append("'" + request.form['cantitate'] + "'")
        values.append("'" + request.form['unitate_de_masura'] + "'")

        fields = ['id_ingredient', 'nume_ingredient', 'cantitate', 'unitate_de_masura']
        query = 'INSERT INTO ingredient (%s) VALUES (%s)' % (', '.join(fields), ', '.join(values))

        cur.execute(query)
        con.commit()
        return redirect('/ingredients')
    # Redirecționează către pagina de ingrediente după adăugare
    else:
        return render_template('addIngredient.html')
    # Returnează șablonul HTML pentru adăugarea unui ingredient


@app.route('/deleteIngredient', methods=['POST'])
def del_ingr():
    emp = request.form['id_ingredient']
    cur = con.cursor()
    cur.execute('delete from reteta where ingredient_id_ingredient=' + emp)
    cur.execute('delete from ingredient where id_ingredient=' + emp)
    cur.execute('commit')
    return redirect('/ingredients')



@app.route('/editIngredient', methods=['POST'])

def get_ingr():
    emp = request.form['id_ingredient']
    cur = con.cursor()
    cur.execute('select * from ingredient where id_ingredient=' + emp)

    emps = cur.fetchone()
    id_ingredient = emps[0]
    cantitate= emps[2]
    cantitate_ramasa = emps[3]

    cur.close()

    return render_template('editIngredient.html', cantitate=cantitate, cantitate_ramasa=cantitate_ramasa,id_ingredient=id_ingredient)

@app.route('/editIngr', methods=['POST'])
def edit_ingr():
    try:
        id_ingredient = request.form['id_ingredient']
        cantitate_noua = int(request.form['cantitate'])
        cantitate_ramasa = int(request.form['cantitate_ramasa'])

        # Obține cantitatea curentă și cantitatea ramasă din baza de date
        cur = con.cursor()
        cur.execute(f"SELECT cantitate, cantitate_ramasa FROM ingredient WHERE id_ingredient = {id_ingredient}")
        result = cur.fetchone()
        cantitate_curenta = result[0]
        cantitate_ramasa_curenta = result[1]

        # Verifică dacă cantitatea introdusă este corectă
        if cantitate_noua >= cantitate_curenta - cantitate_ramasa:
            # Calculează noua valoare pentru cantitate_ramasa
            cantitate_ramasa_noua = cantitate_noua -cantitate_curenta + cantitate_ramasa_curenta

            # Actualizează ambele câmpuri în baza de date
            cur.execute(f"UPDATE ingredient SET cantitate = {cantitate_noua}, cantitate_ramasa = {cantitate_ramasa_noua} WHERE id_ingredient = {id_ingredient}")
            con.commit()
            return redirect('/ingredients')
        else:
            # Dacă cantitatea nu este corectă, afișează o eroare
            error_message = "Cantitatea introdusă nu este corectă. Verificați stocul rămas."
            return render_template('eroare.html', error_message=error_message, back_url='/ingredients')

    except Exception as e:
        error_message = str(e).splitlines()[0]
        return render_template('eroare.html', error_message=error_message, back_url='/ingredients')

@app.route('/getIngredient', methods=['POST'])




@app.route('/products')
def products():
    products = []

    cur = con.cursor()
    cur.execute('SELECT * FROM produs')  # Modificați cu numele real al tabelei
    for result in cur:
        product = {
            'id_produs': result[0],
            'denumire_produs': result[1],
            'pret': result[2],
            'gramaj_grame': result[3]
        }
        products.append(product)
    cur.close()
    return render_template('Produs.html', products=products)

@app.route('/addProdus', methods=['GET', 'POST'])
def add_pr():
    error = None
    if request.method == 'POST':
        try:
            # Deschideți o tranzacție
            con.begin()

            # Aflați următorul ID disponibil pentru produs
            emp = 0
            cur = con.cursor()
            cur.execute('SELECT MAX(id_produs) FROM produs')
            for result in cur:
                emp = result[0]
            cur.close()

            # Incrementați ID-ul
            emp += 1


            # Inserează produsul în tabelul 'produs'
            cur = con.cursor()
            values = {
                'id_produs': emp,
                'denumire_produs': request.form['denumire_produs'],
                'pret': request.form['pret'],
                'gramaj_grame': request.form['gramaj_grame']
            }
            query = 'INSERT INTO produs (id_produs, denumire_produs, pret, gramaj_grame) VALUES (:id_produs, :denumire_produs, :pret, :gramaj_grame)'
            cur.execute(query, values)

            # Comiteți tranzacția
            con.commit()

            return redirect('/products')


        except Exception as e:
            # În caz de eroare, rollback tranzacția și afișați un mesaj de eroare
            con.rollback()
            error_message = str(e).splitlines()[0]
            return render_template('eroarePretNegativ.html', error_message=error_message, back_url='/products')

    else:
        return render_template('addProdus.html')


@app.route('/deleteProdus', methods=['POST'])
def del_prod():
    emp = request.form['id_produs']
    cur = con.cursor()
    cur.execute('delete from comanda where produs_id_produs=' + emp)
    cur.execute('delete from detalii_produs where produs_id_produs=' + emp)
    cur.execute('delete from reteta where produs_id_produs=' + emp)
    cur.execute('delete from produs where id_produs=' + emp)
    cur.execute('commit')
    return redirect('/products')



@app.route('/editProdus', methods=['POST'])

def get_prod():
    emp = request.form['id_produs']
    cur = con.cursor()
    cur.execute('select * from produs where id_produs=' + emp)

    emps = cur.fetchone()
    id_produs = emps[0]
    pret = emps[2]
    gramaj_grame = emps[3]

    cur.close()

    return render_template('editProdus.html', pret=pret,gramaj_grame=gramaj_grame, id_produs=id_produs)


@app.route('/editProd', methods=['POST'])
def edit_prod():
    try:
        id_produs = "'" + request.form['id_produs'] + "'"
        pret = "'" + request.form['pret'] + "'"
        gramaj_grame = "'" + request.form['gramaj_grame'] + "'"

        cur = con.cursor()
        query = "UPDATE produs SET pret =%s, gramaj_grame=%s where id_produs=%s" % (pret, gramaj_grame, id_produs)
        cur.execute(query)
        cur.execute('commit')
        return redirect('/products')

    except Exception as e:
        error_message = str(e).splitlines()[0]
        return render_template('eroare.html', error_message=error_message, back_url='/products')


@app.route('/getProdus', methods=['POST'])


@app.route('/recipes')
def recipes():
    recipes = []

    cur = con.cursor()
    cur.execute('SELECT * FROM reteta')  # Modificați cu numele real al tabelului membri
    for result in cur:
        recipe = {

            'produs_id_produs': result[0],
            'ingredient_id_ingredient': result[1],
            'id_reteta': result[2],
            'nume_reteta': result[3],
            'timp_preparare_minute': result[4],
            'specificatii': result[5],
            'cantitate': result[6]
        }
        recipes.append(recipe)
    cur.close()
    return render_template('Retete.html', recipes=recipes)

@app.route('/addReteta', methods=['GET', 'POST'])
def add_ret():
    if request.method == 'POST':

        #emp = 0
       # cur = con.cursor()
        #cur.execute('SELECT MAX(id_reteta) FROM reteta')
        #for result in cur:
         #   emp = result[0]
        #cur.close()

      #  emp += 1
        cur = con.cursor()

        values = [
            "'" + request.form.get('produs_id_produs', '') + "'",
            "'" + request.form.get('ingredient_id_ingredient', '') + "'",
         #   "'" + str(emp) + "'",
            "'" + request.form.get('id_reteta', '') + "'",
            "'" + request.form.get('nume_reteta', '') + "'",
            "'" + request.form.get('timp_preparare_minute', '') + "'",
            "'" + request.form.get('specificatii', '') + "'",
            "'" + request.form.get('cantitate', '') + "'"
        ]
        fields = ['produs_id_produs', 'ingredient_id_ingredient', 'id_reteta', 'nume_reteta', 'timp_preparare_minute',
                  'specificatii', 'cantitate']
        query = 'INSERT INTO reteta (%s) VALUES (%s)' % (', '.join(fields), ', '.join(values))

        cur.execute(query)
        con.commit()
        return redirect('/recipes')

    else:
        return render_template('addReteta.html')



@app.route('/deleteReteta', methods=['POST'])
def del_rt():
    emp = request.form['id_reteta']
    cur = con.cursor()
    cur.execute('delete from reteta where id_reteta=' + emp)
    cur.execute('commit')
    return redirect('/recipes')



# main
if __name__ == '__main__':
    app.run(debug=True)
    con.close()