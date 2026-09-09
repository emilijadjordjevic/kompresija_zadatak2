# Zadatak 2: LDPC

Poslali smo `000`, a stiglo je `100`. Kako otkriti koji bit se promenio?
Provere sabiraju odabrane bitove modulo 2. Njihovi rezultati cine sindrom.
Sindromski dekoder trazi najmanju gresku koja bi dala taj sindrom.
Gallager umesto toga ponavlja glasanje izmedju provera i bitova.

## Pokretanje

```bash
make
./ldpc 49
make test
```

Broj `1` je demonstraciono seme. Zameni ga brojem indeksa koji profesor
trazi. Tvoj broj indeksa jos nije dostavljen. Program pravi tri fajla u
`results`: `matrica.txt`, `sindromi.txt` i `izvestaj.txt`. Postojece fajlove
sa tim imenima prepisuje. Za pokretanje su dovoljni C++17 i standardna biblioteka.

## Gde sta stoji

- `src/ldpc.h`: definicije tipova i deklaracije funkcija.
- `src/matrix.cpp`: konstrukcija matrice, sindrom i rang.
- `src/syndrome.cpp`: tabela korektora, rastojanje i pretraga gresaka.
- `src/gallager.cpp`: glasanje i ponavljanje koraka.
- `src/report.cpp`: zapis rezultata. `src/main.cpp`: pokretanje.
- `tests/tests.cpp`: nekoliko jednostavnih provera.

`Word` je drugo ime za `std::uint32_t`, broj sa 32 bita. Svaki njegov bit
predstavlja jednu poziciju reci. `Matrix` cuva broj kolona (`length`) i
vektor redova (`rows`). Svaki red je jedan `Word`. Na ispisu je prva kolona
levo, a u broju joj odgovara najnizi bit. Na primer, broj 5 daje `10100`.

## Kako postupak radi

Prva tri reda imaju po pet uzastopnih jedinica. Sledece dve grupe su
permutacije kolona prve. Generator je `std::mt19937` sa zadatim semenom.
Kratak Fisher-Yates postupak daje ponovljive permutacije.

Za svaku od 32768 reci racunamo sindrom. U tabeli ostaje korektor najmanje
tezine; kod iste tezine biramo najmanju brojnu masku. Dekodiranje je
`primljena_rec XOR korektor`. Najmanja tezina nenulte kodne reci daje d.

Devet redova nisu svi nezavisni: zbir redova svake grupe je isti.
Zato racunamo rang. Stvarna dimenzija je `15 - rang`, a nisu svi sindromi
dostizni. Nedostizne oznacavamo bez korektora.

Gallager B prati skriptu, odeljak 8.8.2. Svaka provera salje bitu XOR
ostalih suseda. Sa tri provere i pragovima 0.5 odlucuju dva od tri glasa.
Svi bitovi se menjaju istovremeno. Ponavljanje istog stanja znaci ciklus.

Ispitujemo greske po rastucoj tezini, dok ne nadjemo neuspeh. Sve manje
tezine se potpuno provere. Poslata rec je nulta; neuspeh je ciklus ili
zavrsetak u drugoj kodnoj reci. Linearnost i simetricno glasanje za wc=3
omogucuju da se za ovaj zadatak ispituje samo nulta poslata rec.

Uporedjujemo prvu neuspesnu tezinu sa `d` i garantovanom ispravkom
`floor((d-1)/2)`. Garancija se odnosi na SVAKU gresku te tezine.
Potpuna pretraga je namenjena malom zadatom kodu (najvise 20 bitova).
