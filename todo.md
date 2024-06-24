# TODO aka something like Roadmap

- [x] Classa reels
    - [x] kdyz se tocej reels, tak svetla budou delat efekt waterfall. [1, 0, 0], [0, 1, 0], [0, 0, 1]. Moznost je mit vypnuty
    - [x] kdyz se ze zastavej, tak budou svitit vsechny. Pak jak se budou vyhodnocovat vyherni, tak ty budou blikat. TOHLE ASI UDELAT V SLOTMACHINE CLASS...

- [ ] Classa pro SlotMachine samotnej.
    - [x] musi umet ZATIM NAHODNE roztocit valce
    - [x] pocitat vyhru podle vyhernich tabulek (viz obrazek Wings)
    - [x] spravne vyhodnotit vyherni linie - kdyz je jich vic, tak od prvniho postupne s blikanim
        - [ ] hranim nakyho tonu zvyraznit, ze byly vyhrany
    - [ ] po kazdy linii do levy casti displeje pricitat castku, co vyhral, aby to bylo postupny
    - [ ] az se vsechny linie vyhodnoti, tak mit mouznost riskovat 2x vyhru nebo nic. [60     RISK    0] blika mezi 60 a 0, mackas risk "start"em. kdyz uz mas dost, tak stisknes "do multiwinu".
    - [ ] poresit BET, aby mohl tocit tzv. 5+15 -> 5 ze spoda a 15 z multiwinu
    - [ ] vymyslet featuru na posouvani valcu jako takovou minihru
        - [ ] kdyz treba neco padne, treba tri sedmicky, tak se ti muze nabit posuv a pristi otoceni, kdyz ti nic nepadne, tak na modrym displeji pobezi napis, ze to je ta minihra, a ze vybiras o kolik se posune valec. Tim mas nabito. dalsi otoceni, kdyz padne vyhra, tak normalne padne a porad mas nabito. kdyz nepadne vyhra, budes moct vybrat valec (bude mezi nima problikavano), kterej se otoci o ten pocet kroku, co sis vytocil predtim pak znova vyhodnoti. Pripadne ze by se vyhra pocitala nejak lip, to se jeste uvidi. V minihre musi bejt misto na trosku modifikovani nahody. Takze kdyz zmacknes tlacitko pri vyberu valcu, tak se nezastavej hned, ale pomalu dojedou. Tak se muze modifikovat pak kam to padne. Aspon trochu :)

- [ ] Mozna nakou classu na buttony, aby byla s nima prace jednodussi

- [x] Classa pro ovladani VFD displeje
    - [ ] moznost vypsat hodnotu do leva a zaroven do prava. popripade jen doleva a jen doprava
    - [ ] scrollovani textu s moznosti nastaveni rychlosti
    - [ ] efekt na pricitani/odecitani cisel. Jakoze vyhrajes 100, tak z 50 ti to po jednotkach rychle napocita do 150.

- [ ] Classa pro prehravani z MP3 player modulu. Musi byt dostatecne rychla
- [ ] Classa pro ovladani ciselnych LED panelu
    - [ ] efekt na pricitani/odecitani cisel. Jakoze vyhrajes 100, tak z 50 ti to po jednotkach rychle napocita do 150.

- [x] Udelat lepsi tocici algoritmus, kterej by zohlednoval ovlivnovani nahody ve prospech kasina v urcitym procentu.
- tocici algoritmus upravit, aby byl chytrejsi...