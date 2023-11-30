# Dekódování starých textů

Při průzkumu na nejodlehlejším místě naší planety byly objeveny pozůstatky dávné civilizace, pečlivě ukryté v temných hlubinách jeskyní. Mezi těmito vzácnými nálezy jsme narazili na hliněné tabulky s tajemnými texty, jejichž význam zůstává zahalen v mlze. Co je zvláštní, každá tabulka se zdá být psána jiným jazykem.

Profesoři z univerzity se ocitli před výzvou: Zjistí jaký jazyk koresponduje s texty na jednotlivých tabulkách? Aby odhalili tuto záhadu, shromáždili informace o všech možných jazycích, kterými by mohly být texty napsány. Tyto jazyky formálně popsali pomocí gramatik.

Pro usnadnění procesu automatického zpracování těchto jazyků navrhli profesoři standardizovaný zápis všech pravidel a převedli gramatiky do této jednotné formy. Tato reprezentace umožňuje přepsat levou stranu pravidla na dva neterminály nebo jeden terminál. V případě potřeby generování prázdného slova dovoluje i pravidlo, které přepíše počáteční neterminál na nic, přičemž však počáteční neterminál pak nesmí být obsažen v pravé straně jakéhokoliv pravidla této gramatiky.

Následující noc byla věnována přepisu symbolů z hliněných tabulek. K našemu obrovskému potěšení používala tato starodávná civilizace sadu symbolů, která přesně odpovídá symbolům v ASCII kódování, což nám umožnilo snadněji převést symboly do moderní podoby a lehce zpracovávat počítači.

Na univerzitě však nejsou zdatní programátoři, proto shání pomoc. Univerzita hledá ty, kdo by byli schopni napsat program, který by rozhodl, zda dané slovo patří do jazyka reprezentovaného gramatikami popsanými výše. V případě kladné odpovědi je třeba navíc najít způsob, jak může být toto slovo vytvořeno podle pravidel gramatiky.

Úkol implementujte v podobě funkce programu v C++, jejíž signatura je: `std::vector<size_t> trace(const Grammar&, const Word&);`. Vstupem funkce je gramatika jazyka (reprezentována jako struktura Grammar) a slovo (Word), pro které máte zjistit, zda patří do jazyka. Tyto struktury jsou definovány v testovacím prostředí, vizte ukázkový soubor. Gramatika je definována standardně jako v přednáškách. Pro zjednodušení jsou symboly gramatiky definovány jako jakékoliv hodnoty typu char. Pokud slovo do jazyka patří, pak vraťte nějakou posloupnost indexů pravidel, ze kterých vzniklo postupným derivováním nejlevějšího neterminálu. Pravidla jsou číslována od nuly. Pokud slovo do jazyka nepatří, pak je očekáváno, že vrátíte prázdnou posloupnost.

Je zaručeno, že na vstupu přijde validní gramatika s pravidly podle zadání, tedy:
- množiny symbolů (`Grammar::m_Terminals`, `Grammar::m_Nonterminals`) nebudou mít průnik,
- veškerá pravidla budou obsahovat pouze symboly z těchto dvou množin,
- všechna pravidla budou mít na pravé straně právě jeden terminál, dva neterminály, nebo, pokud je levou stranou počáteční neterminál, nic (ale pak se ale počáteční neterminál nevyskytuje nikde na pravé straně jiného pravidla),
- počáteční symbol gramatiky se vyskytuje v množině neterminálů a
- mezi pravidly nejsou duplicity