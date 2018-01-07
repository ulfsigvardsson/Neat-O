# Kodkavlitetrapport

Gruppen enades om att använda sig av **GNU coding standard** då denna ansågs tillhandahålla en tydlig uppdelning av block vilket skulle komma att underlätta
både läsbarheten och att följa logiska flöden. Samtliga funktionsnamn återfinns därmed i den vänstraste kolumnen för att underlätta navigering.
Verktyget **Artistic Style** har använts för att säkerställa att koden rättade sig efter GNU coding standards, samtliga källfiler formaterades med flaggorna
```
--style=gnu: parenteser för funktionskroppar och strukter placeras i den vänstraste kolumnen.

-xd padding innan en öppnande parentes

-p: sätter blanksteg runt matematiska operatorer

-xC79: sätter den maximala radlängden till 70 tecken.
```
Dessutom använde gruppen sig av verktyget **indent** vars standardinställning är att indentera koden efter GNU coding standards.

För att hålla hög läsbarhet har man försökt att använda sig av beskrivande makron på de ställen där koden annars skulle te sig skrymmande.
En operation som ofta görs är konverteringen mellan objekt och metadata, vilket i ren kod ser ut som `((object_record_t*)(object) - 1)` samt `((obj)(record + 1))`.
Koden är inte svår att förstå men blir utmattande att läsa upprepade gånger, därför används istället makrona`OBJECT_TO_RECORD`respektive `RECORD_TO_OBJECT`.
Makron används även vid if-satser där villkoren annars riskerar att bli svåröverskådliga.

Koden ämnar att underlätta underhåll och ändringar genom att exempelvis definiera egna format för datatyperna för referensantalet och storleken för ett objekt.
På så sätt kan ändringar göras på en enda rad och få genomslag i resten av programmet.
Ett undantag till detta är vid kontroll av overflow där kontrollen görs mot det literala maxvärdet av unsigned short.

Gruppen anser att den viktigaste aspekten för att hålla kod läsbar för andra gruppmedlemmar har varit att hålla den väldokumenterad.
Vi har använt oss av Doxygen-standard för dokumentation av funktioner, strukter, definitioner etc. vilket vi anser har gjort koden begriplig också för de som inte har skrivit den.
Att hålla funktioner relativt koncisa och därmed lättöverskådliga har också varit av hög prioritet, den längsta funktionen är   `allocate()` på 23 rader vilket får anses befogat
givet dess centrala roll i biblioteket.

Vid pull requests har gruppen varit konsekventa med att kräva beskrivande variabel- och funktionsnamn. Enbokstavs-namn har inte accepterats.

Konsekvenser.......
