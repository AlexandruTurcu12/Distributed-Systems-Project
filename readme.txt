	La prima etapa, in procesele coordonator citesc din fisiere dimensiunea clusterelor si procesele
care apartin de fiecare cluster. Apoi transmit coordonatorul fiecarui proces worker al sau. Pentru a fi
sigur ca se produc doar comunicatii valide, tagul fiecarui mesaj este rangul procesului worker, iar
acesta primeste mesajul de la MPI_ANY_SOURCE (doar de aceasta data - urmatoarele mesaje vor fi primite in
mod explicit de la coordonator).
	Doresc sa am un vector care contine toate procesele topologiei. In acest scop, am nevoie de
dimensiunile fiecarui cluster, in functie de care, la afisarea topologiei, voi pune spatiu cand se
incheie un cluster. Asadar, transmit dimensiunile clusterelor catre celelalte doua procese coordonator,
iar fiecare coordonator va trimite dimensiunile partiale catre workeri. Suma dimensiunilor reprezinta
dimensiunea vectorului final, care este calculata in fiecare coordonator si transmisa catre workeri.
Pentru fiecare coordonator, creez cate un vector partial care contine toate procesele clusterului,
inclusiv coordonatorul, si trimit vectorul celorlalti coordonatori.
	Dimensiunile sunt trimise altor procese pentru a putea aloca dinamic vectorii partiali si cel
final. Atunci cand coordonatorii primesc vectorii partiali, acestia ii introduc in vectorul final.
Dupa ce se realizeaza aceasta operatie, coordonatorii au topologia completa si afiseaza vectorul final,
apoi il trimit workerilor, care afiseaza, la randul lor, vectorul.

	La a doua etapa, creez vectorul in procesul 0. Apoi creez cate un vector auxiliar pentru
fiecare cluster si atribui fiecarui worker un numar de N / (numtasks - 3) elemente pe care le va modifica,
sau N / (numtasks - 3) + N % (numtasks - 3) pentru ultimul proces al clusterului 1 pentru cazul in care
exista un surplus de elemente in vector care altfel nu ar fi fost atribuite vreunui proces.
	Fiecare worker al clusterului 0 primeste vectorul partial 0, modifica elementele care i-au fost
atribuite si trimite inapoi vectorul catre procesul 0. Acest pas se repeta pana cand toate elementele
vectorului partial 0 sunt dublate. Apoi, modific vectorul final in functie de rezultatele obtinute,
iar procesul 0 trimite vectorii partiali 1 si 2 catre procesele 1 si 2, care, la randul lor, vor trimite
workerilor sa dubleze elementele intr-un mod echilibrat, repetandu-se procesul descris mai sus. La final,
procesul 0 primeste vectorii partiali modificati de la procesele 1 si 2, iar vectorul final este modificat
si afisat.