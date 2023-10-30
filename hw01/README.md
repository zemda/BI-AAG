# Determinizace konečného automatu s více počátečními stavy

Algoritmus determinizace konečného automatu je klíčovým nástrojem v teorii formálních jazyků. Nalézá rozsáhlé praktické využití ve zpracování textu, analýze dat a v různých oblastech informatiky. Tím, že konvertuje nedeterministické automaty na deterministické, umožňuje efektivní analýzu jazyků, což je klíčové v kompilátorech, analýze textu, vyhledávání regulárních výrazů, řízení sítí, databázových systémech a mnoha dalších aplikacích.

Úkolem je najít deterministický konečný automat bez zbytečných a nedosažitelných stavů takový, aby jazyk, který přijímá byl ekvivalentní k jazyku zadaného nedeterministického konečného automatu s více počátečními stavy.

Determinizaci implementujte v podobě funkce programu v C++, jejíž signatura je: DFA determinize ( const MISNFA & nfa );. Vstupem, resp. výstupem algoritmu jsou automaty v podobě struktur MISNFA, resp. DFA reprezentující nedeterministický konečný automat s více poč. stavy, resp. deterministický konečný automat. Tyto struktury jsou definovány v testovacím prostředí, viz ukázka níže. Pro zjednodušení jsou stavy definovány jako hodnoty typu int a symboly abecedy jako hodnoty typu char.

Porovnání automatů s referenčním výsledkem se provádí přes převod na minimální deterministický konečný automat. Vaše výstupy se mohou lišit (např. v pojmenování stavů). Po převedení na minimální automat, které provede testovací prostředí, však musí dát ekvivalentní automat (pojmenování stavů nehraje roli).

Je zaručeno, že na vstupu přijde validní nedeterministický konečný automat s více počátečními stavy, tedy:
- množiny stavů (MISNFA::m_States), počátečních stavů (MISNFA::m_InitialStates) a symbolů abecedy (MISNFA::m_Alphabet) budou neprázdné,
- počáteční a koncové stavy z množin MISNFA::m_InitialStates a MISNFA::m_FinalStates budou také prvky množiny stavů MISNFA::m_States,
- pokud nebude pro nějaký stav q a symbol abecedy a definovaný přechod v automatu, pak v mapě MISNFA::m_Transitions nebude ke klíči (q, a) přiřazená hodnota prázdná množina, ale tento klíč nebude vůbec existovat,
- v mapě přechodů MISNFA::m_Transitions se vyskytují také jen prvky, které jsou specifikovány v množině symbolů abecedy a stavů.

Výsledný DFA musí také splňovat podmínky definice automatu, tedy musí platit to samé co výše pro MISNFA (až na zřejmé změny kvůli rozdílným definicím počátečního stavu a přechodové funkce).

V případě, že jazyk automatu je prázdný, odevzdávejte jednostavový automat nad stejnou abecedou, jako je původní automat.