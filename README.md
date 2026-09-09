# Zadatak 2: LDPC

Poslali smo `000`, a stiglo je `100`. Negde se, očigledno, jedan bit promenio.

Kako da otkrijemo koji?

LDPC kod koristi više provera parnosti. Svaka provera sabira određene bitove modulo 2. Rezultati svih provera zajedno čine **sindrom**.

Sindromski dekoder pokušava da pronađe grešku najmanje težine koja bi proizvela taj sindrom. Gallager dekoder radi drugačije: provere i bitovi međusobno glasaju, a postupak se ponavlja dok se ne dobije ispravna kodna reč ili dekoder ne upadne u ciklus.

## Pokretanje

```bash
make
./ldpc 49
make test
```

Broj `49` je broj indeksa i koristi se kao seme pri pravljenju matrice.

Program pravi tri fajla u direktorijumu `results`:

```text
matrica.txt
sindromi.txt
izvestaj.txt
```

Ako već postoje, biće prepisani.

Za kompajliranje su dovoljni C++17 i standardna C++ biblioteka.

## Gde šta stoji

* `src/ldpc.h` — osnovni tipovi i deklaracije funkcija
* `src/matrix.cpp` — konstrukcija matrice, sindrom i rang
* `src/syndrome.cpp` — sindromsko dekodiranje i minimalno rastojanje
* `src/gallager.cpp` — Gallager dekoder
* `src/report.cpp` — zapis rezultata
* `src/main.cpp` — pokretanje programa
* `tests/tests.cpp` — osnovne provere ispravnosti

Reč koda ima 15 bitova, pa se može čuvati u jednom 32-bitnom celom broju. Svaki bit tog broja predstavlja jednu poziciju kodne reči.

Matrica se čuva kao niz takvih redova. Na ispisu se prva kolona prikazuje levo.

## Kako postupak radi

Matrica ima 15 kolona i 9 redova.

Prva tri reda imaju po pet uzastopnih jedinica. Sledeće dve grupe redova dobijaju se permutovanjem kolona prve grupe. Permutacije zavise od semena `49`, pa se pri svakom pokretanju dobija ista matrica.

Za svaku od mogućih `2^15 = 32768` reči računamo sindrom.

Za svaki sindrom čuvamo grešku sa najmanjim brojem jedinica, odnosno najmanjom **težinom**. Ako više grešaka iste težine daje isti sindrom, bira se ona sa manjom brojnom vrednošću.

Dekodiranje je zatim jednostavno:

```text
ispravljena_reč = primljena_reč XOR korektor
```

Korektor govori koje bitove treba promeniti.

Najmanja težina nenulte kodne reči daje minimalno rastojanje koda `d`.

Nisu svi redovi matrice nezavisni, pa računamo njen rang. Dimenzija koda je:

```text
k = 15 - rang
```

Zbog toga nisu ni svi mogući sindromi dostižni. Za sindrome koji se ne mogu dobiti ne postoji korektor.