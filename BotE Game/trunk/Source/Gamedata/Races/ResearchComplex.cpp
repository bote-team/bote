// ResearchComplex.cpp: Implementierung der Klasse CResearchComplex.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IOData.h"
#include "ResearchComplex.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL (CResearchComplex, CObject, 1)

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CResearchComplex::CResearchComplex()
{
	m_byComplexStatus = NOTRESEARCHED;
	memset(m_byFieldStatus, NOTRESEARCHED, 3 * sizeof(*m_byFieldStatus));
	memset(m_iBonus, 0, 3 * sizeof(*m_iBonus));
	m_strComplexName = "";
	m_strComplexDescription = "";
}

CResearchComplex::~CResearchComplex()
{

}

//////////////////////////////////////////////////////////////////////
// Kopierkonstruktor
//////////////////////////////////////////////////////////////////////
CResearchComplex::CResearchComplex(const CResearchComplex & rhs)
{
	m_byComplexStatus = rhs.m_byComplexStatus;
	for (int i = 0; i < 3; i++)
	{
		m_byFieldStatus[i] = rhs.m_byFieldStatus[i];
		m_strFieldName[i] = rhs.m_strFieldName[i];
		m_strFieldDescription[i] = rhs.m_strFieldDescription[i];
		m_iBonus[i] = rhs.m_iBonus[i];
	}	
	m_strComplexName = rhs.m_strComplexName;
	m_strComplexDescription = rhs.m_strComplexDescription;
}

//////////////////////////////////////////////////////////////////////
// Zuweisungsoperator
//////////////////////////////////////////////////////////////////////
CResearchComplex & CResearchComplex::operator=(const CResearchComplex & rhs)
{
	if (this == &rhs)
		return *this;
	m_byComplexStatus = rhs.m_byComplexStatus;
	for (int i = 0; i < 3; i++)
	{
		m_byFieldStatus[i] = rhs.m_byFieldStatus[i];
		m_strFieldName[i] = rhs.m_strFieldName[i];
		m_strFieldDescription[i] = rhs.m_strFieldDescription[i];
		m_iBonus[i] = rhs.m_iBonus[i];
	}
	m_strComplexName = rhs.m_strComplexName;
	m_strComplexDescription = rhs.m_strComplexDescription;
	return *this;
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CResearchComplex::Serialize(CArchive &ar)		
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		ar << m_byComplexStatus;
		for (int i = 0; i < 3; i++)
		{
			ar << m_byFieldStatus[i];
			ar << m_strFieldName[i];
			ar << m_strFieldDescription[i];
			ar << m_iBonus[i];
		}
		ar << m_strComplexName;
		ar << m_strComplexDescription;
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		ar >> m_byComplexStatus;
		for (int i = 0; i < 3; i++)
		{
			ar >> m_byFieldStatus[i];
			ar >> m_strFieldName[i];
			ar >> m_strFieldDescription[i];
			ar >> m_iBonus[i];
		}
		ar >> m_strComplexName;
		ar >> m_strComplexDescription;
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////
/// Diese Funktion generiert einen Komplex, setzt also erst alle Attribute des Objektes. Als einziger Parameter
/// muss dafür eine Komplexnumme <code>complex</code> übergeben werden.
void CResearchComplex::GenerateComplex(USHORT complex)
{
    switch(complex)
	{
	case 0:		// Waffentechnik	-> eingebaut
		{
		/*	m_strComplexName = "Waffentechnik";
			m_strComplexDescription = "Es wird immer deutlicher, dass die interstellare Expansion mit erhebliche Risiken verbunden ist. Um auf saemtliche Eventualitaeten vorbereitet zu sein wurde eine nicht unerhebliche Erhoehung des Militaerhaushalts beschlossen. Die fueherenden Strategen unseres Militaers sprachen sich einstimmig fuer den weiteren Ausbau der offensiven Faehigkeiten unserer Schiffe aus. Neben der ,daraus resultierenden, Ueberlegenheit in eventuellen Kampfhandlungen wird zusaetzlich mit einer abschreckenden Wirkung unserer Flotte gerechnet.";
			m_strFieldName[0] = "30% erhoehter Phaserschaden";
			m_strFieldName[1] = "25% erhoehte Torpedogenauigkeit";
			m_strFieldName[2] = "20% erhoehte Schussfreuquenz";
			m_strFieldDescription[0] = "Durch die geziehlte Phasenremodulation koennte der Energieausstoss unserer standard Phaser massiv erhoeht werden. Wir waehren somit in der Lage den Ausstoss, um 30% zu verbessern, ohne die peripheren Schiffssysteme zu ueberlasten.";
			m_strFieldDescription[1] = "Durch die Konzentration unserer Forschungsbemuehungen auf den Bereich der Torpedotechnologie koennten wir binnen kuerzester Zeit die Zielerfassungssysteme der Torpedowerfer soweit optimieren, so dass sie um 25% genauer treffen.";
			m_strFieldDescription[2] = "Die naechste Generation der Ziel- und Feuerleitsysteme ist einsatzbereit. Sie ermoeglichen zusammen mit der kontinuierliche Weiterentwicklung der Waffengeneratoren eine Erhoehung der Schussfreuqenz aller angebauten Waffensysteme um 20%.";
		*/	m_iBonus[0] = 30;
			m_iBonus[1] = 25;
			m_iBonus[2] = 20;
			break;
		}
	case 1:	// Konstruktionstechnik	-> eingebaut
		{
		/*	m_strComplexName = "Konstruktionstechnik";
			m_strComplexDescription = "Kosmische Strahlung, Kleinstmeteorieden aber auch die zunehmende Bedrohung zum teil feindsehliger Rassen machen eine kontinuierliche Verbesserung der passiven Schutzmechanismen underer Schiffe unumgaenglich. Schwerpunkte bilden die Frueherkennunug potentieller Risiken, das Energieabsorbtionsvermoegen unserer Schilde und die Struktur der Schiffe ansich.";
			m_strFieldName[0] = "20% bessere Schilde";
			m_strFieldName[1] = "20% verstaerkte Huelle";
			m_strFieldName[2] = "50% stärkere Scanner";
			m_strFieldDescription[0] = "Mit den  neuen, auf Fusion basierenden, Schildgeneratoren waehren wir im Stande, das erzeugte Schildgitter erheblich zu verstaerken und zu stabilisieren. Die so erzeugte Schildblase wuerde bis zu 20% mehr Energie absorbieren.";
			m_strFieldDescription[1] = "Laut unseren Wissenschaftlern wuerde der Einsatz eines Energiegitters zusaetzlich zum Deflektoschild die strukturelle Integritaet um  bis zu 20% zu verstaerken.";
            m_strFieldDescription[2] = "Durch die Optimierung der Langstrecken-Sensoren-Felder, die nun aktiv oder passiv Subraum-Frequenzen scannen, koennte die Scanstaerke innerhalb des ueblichen Radius effektiv um 50% verstaerkt werden.";
		*/	m_iBonus[0] = 20;
			m_iBonus[1] = 20;
			m_iBonus[2] = 50;
			break;
		}
	case 2:	// allgemeine Schiffstechnik	-> eingebaut
		{
		/*	m_strComplexName = "allgemeine Schiffstechnik";
			m_strComplexDescription = "Die Grenzen unseres Territoriums dehnen sich bestaendig weiter aus, wodurch sich die Versorgung und die Kontrolle entfernt gelegener Systeme merklich schwieriger gestaltet. Um sicherzustellen in unserem gesamten Hoheitsgebiet praesent zu sein wurde beschlossen unsere gesamte Flotte weiter zu optimieren. Dies gilt ebenso fuer zivile als auch fuer militaerisch genutzte Schiffe.";
			m_strFieldName[0] = "erhoehte Reichweite";		// nur für Schiffe mit Reichweite kurz
			m_strFieldName[1] = "erhoehte Geschwindigkeit";	// nur für Schiffe mit Geschwindigkeit 1
			m_strFieldName[2] = "10% billigere Produktion";
			m_strFieldDescription[0] = "Durch ein neues Verfahren zur Dilithiumkristallgewinnung, bei dem die Kristalle nach dem Abbau auf molekularer Ebene optimiert werden, wuerde der Reinheitsgrad dieser deutlich verbessert. Dies wiederum haette einen positiven Effekt auf die Reichweite unserer Schiffe, welche eigentlich nur eine kurze Reichweite besitzen würden.";
			m_strFieldDescription[1] = "Grosse Fortschritte in der Warp-Feld-Theorie ermoeglichen uns eine Neumodelierung der Warp-Blasen. Erste Tests bewiesen die technische Realisierbarkeit. Die gemessene Geschwindigkeit des zuvor sehr langsamenen Experimentalschiffes erhoehte sich, bei gleichbleibender Reichweite, deutlich.";
			m_strFieldDescription[2] = "Der steigende Bedarf an Schiffen versetzt uns in die Lage die benoetigten Teile in grossen Stueckzahlen zu fertigen, was einen nicht unerheblichen Kostenvorteil von 10% mitsichbringt.";
		*/	m_iBonus[0] = RANGE_MIDDLE;
			m_iBonus[1] = 2;
			m_iBonus[2] = 10;
			break;
		}
	case 3:	// friedliche Schifftechnik		-> eingebaut, außer keine Unterhaltskosten??? -> prüfen ob das beim Schiffsbau gleich abgezogen wird.
		{
		/*	m_strComplexName = "friedliche Schifftechnik";
			m_strComplexDescription = "Zahlreichen Neuerungen in diversen militaerischen Bereichen koennten mehr und mehr auch für die zivile Raumfahrt genutzt werden. Sowohl fuehrende Vertreter der Wirtschaft als auch hochrangige Militaers sprachen sich fuer einen engeren Austausch aus. So wurde eine Veroeffentlichung von Forschungsergebnissen beschlossen, welche bisher der Sicherheitsstufe 3 unterlagen";
			m_strFieldName[0] = "25% erhoehte Transportkapazitaet";
			m_strFieldName[1] = "keine Unterhaltskosten";
			m_strFieldName[2] = "25% billigere Produktion";
			m_strFieldDescription[0] = "Die zweite Ausbaustufe unserer derzeitigen Truppentransportschiffe wuerde uns, durch die komplette Restrukturierung der internen Schiffssysteme und einem zusaetlichen Transportmodul, in die Lage versetzen pro Schiff 25% mehr Waren und Truppen zu befoerdern.";
			m_strFieldDescription[1] = "Der Einsatz neu entwickelter Verfahren bei der Wartung unserer Nicht-Kampfschiffe und die gleichzeitge Reduktion der Wartungsmannschaften wuerden laut Schaetzungen unserer Analysten die Unterhaltskosten gen Null senken.";
			m_strFieldDescription[2] = "Indem wir den wirtschaftlichen Focus auf die Produktion von zivilen Schiffen gelegt haben, ist es uns moeglich die Produktionskosten unserer Orbitalwerften um ein Viertel zu senken. Diese Veraenderungen haben keinen Effekt auf die Produktionkosten der uebrigen Schiffe.";
		*/	m_iBonus[0] = 25;
			m_iBonus[1] = 0;
			m_iBonus[2] = 25;
			break;
		}
	case 4:	// Truppen		-> eingebaut
		{
		/*	m_strComplexName = "Truppen";
			m_strComplexDescription = "Neben den grossen Fortschritten im Bereich der militaerischen Schiffskomponenten verzeichnet unsere Forschungsabteilung auch zahlreiche Neuentwicklungen im Teilbereich der tragbaren Waffen. Zusammen mit einer intensiveren taktischen Ausbildung an unseren Militärakademien sollten wir in der Lage sein unsere Infanterie noch effizienter arbeiten zu lassen.";
			m_strFieldName[0] = "20% verbesserte Offensive";
			m_strFieldName[1] = "bessere Grundausbildung";
			m_strFieldName[2] = "20% billigere Produktion";
			m_strFieldDescription[0] = "Angriff ist die beste Verteidigung. Der taktische Schwerpunkt unserer Truppen wird auf praeventiven Schlaege gegen feindliche Stellungen gelegt. Dementsprechend werden unsere Landungstruppen zusaetzlich mit schweren Sturmgewehren der letzten Generation ausgeruestet.";
			m_strFieldDescription[1] = "Eine realistischere Ausbildung und die Schulung in vielen verschiedenen Kampfsituationen bereitet unsere Truppen optimal auf einen Einsatz vor. Einheiten, welche frisch aus der Kaserne kommen besitzen nun schon ein Mindestmass an Erfahrung.";
			m_strFieldDescription[2] = "Durch eine vollstaendige Umstrukturierung unseres gesamten Militaerapperates und eine deutlich effektivere Rekrutierung waehren wir in der Lage die Ausbildungskosten unserer Truppen um bis zu 20% zu senken.";
		*/	m_iBonus[0] = 20;
			m_iBonus[1] = 500;
			m_iBonus[2] = 20;
			break;
		}
	case 5:	// Wirtschaft	-> eingebaut
		{
		/*	m_strComplexName = "Wirtschaft";
			m_strComplexDescription = "Die Fokussierung unserer oekonomischen Kraefte auf bestimmte Bereiche koennte, laut unseren Wirtschaftseliten, bisher ungeahnte Potentiale freisetzen. Hinzu kommen technologischen Fortschritte, die nahezu aller Bereiche der Industrie revolutionieren. Zusammen werden diese beiden Faktoren eine messbare Staerkung unserer Wirstschaft mit sich bringen.";
			m_strFieldName[0] = "10% erhoehte Industrieleistung";
			m_strFieldName[1] = "5% billigere Produktion";
			m_strFieldName[2] = "25% erhoehte Latinumeinnahmen";
			m_strFieldDescription[0] = "Wir sind nun in der Lage die Replikatortechnologie sowie die Ort-zu-Ort-Teleportation industriell einzusaetzen. Wir koennten die Gesamtindustrieleistung unserer Produktionsanlagen, Schaetzungen zu folge, um rund 10% erhoehen.";
			m_strFieldDescription[1] = "Die Systemweite Vernetzung ganzer Industriezweige wuerde uns umgerechnet auf die Produktion Kostenvorteile von bis zu 5% einbringen. Diese Einsparung haette keinerlei Effekte auf die Qualitaet sowie auf den Fertigungszeitraum.";
			m_strFieldDescription[2] = "Eine Reform des Finanzwesens und die daraus resultierende Umverteilung aller finanziellen Belastungen auf saemtliche Systeme unseres Einflussbereiches wuerde uns Mehreinnahmen von 25% ermoeglichen.";
		*/	m_iBonus[0] = 10;
			m_iBonus[1] = 5;
			m_iBonus[2] = 25;
			break;
		}
	case 6:	// Produktion	-> eingebaut
		{
		/*	m_strComplexName = "Produktion";
			m_strComplexDescription = "Der flaechendeckende Einsatz neuester An -bzw Abbautechnologien haette eine signifikante Erhoehung der Produktivitaet in zahlreichen Bereichen der produzierenden Intustrie zur Folge. Die Infrastruktur unserer neu erschlossenen Welten kann ebenfalls noch spuehrbar optimiert werden, was ebenfalls positive Effekte fuer unsere Gesamtproduktionsleisung mitsichbraechte.  ";
			m_strFieldName[0] = "15% erhoehte Nahrungsmittelproduktion";
			m_strFieldName[1] = "5% erhoehte Ressourcenproduktion";
			m_strFieldName[2] = "20% erhoehte Energieleistung";
			m_strFieldDescription[0] = "Bahnbrechende Neuentwicklungen im Bereich der Genetik ermoeglichen deutlich gesteigerte Ertraege im Gebiet der Nahrungsmittelproduktion. Sowohl die Qualitaet als auch die Quantitaet konnte, bei gleichbleibenden Kosten, um 15% gesteigert werden.";
			m_strFieldDescription[1] = "Die Moeglichkeit grosse Teile der bekannten Kristalle und Erze direkt zur Weiterverabrbeitung in die entsprechenden Anlagen zu teleportieren steigert unsere Ressourcenproduktion um 5%.";
			m_strFieldDescription[2] = "Der industrielle Einsatz eines bis dato experimentellen Kuehlsystems ermoeglicht es uns, den Energieausstoss unserer Generatoen um 20% zu verbessern. Langzeitstudien konnten keine Beeintraechtigung der Lebensdauer feststellen.";
		*/	m_iBonus[0] = 15;
			m_iBonus[1] = 5;
			m_iBonus[2] = 20;
			break;
		}
	case 7:	// Entwicklung & Sicherheit	-> eingebaut
		{
		/*	m_strComplexName = "Entwicklung und Sicherheit";
			m_strComplexDescription = "Um auch in Zukunft die bestaendige Weiterentwicklung unserer Zivilisation zu garantieren und unsere Errungenschaften ausreichend gegen potentielle Agressoren zu schuetzen sollten wir den entsprechenden Sektionen erhoete finanzielle Mittel zur Verfuegung stellen. Ein entsprechender Entwurf wurde bereits ausgearbeitet und liegt nun zur Abstimmung vor.";
			m_strFieldName[0] = "10% erhoehte Forschung";
			m_strFieldName[1] = "10% erhoehter Geheimdienst";
			m_strFieldName[2] = "5% erhoehte Forschung und Geheimdiesnt";
			m_strFieldDescription[0] = "Die Ausstattung all unserer Laboratorien mit der neueste Computersgeneration wuerde unsere Wissenschaftler in die Lage versetzen auch komplexeste theoretische Modelle binnen kuerzester Zeit zu simulieren. Die Effektivitaet des Forschungs -und Entwicklungsbereiches sollte somit um 10%  gesteigert werden koennen. ";
			m_strFieldDescription[1] = "Die Systemweite Vernetzung all unsere Agenten und die daraus resultierende Moeglichkeit Informationen praktisch ohne Zeitverlust weiterzuleiten sollte unseren Geheimdienst deutlich effektiver Arbeiten lassen. ";
			m_strFieldDescription[2] = "Eine engere Zusammenarbeit der Forschungs -und Geheimdienstressorts wuerde, laut der einhelligen Meinung beider Ressortchefs, zu einem messbar gesteigerten Output beider Bereiche fuehren.";
		*/	m_iBonus[0] = 10;
			m_iBonus[1] = 10;
			m_iBonus[2] = 5;
			break;
		}
	case 8:	// Forschung	-> eingebaut
		{
		/*	m_strComplexName = "Forschung";
			m_strComplexDescription = "Das oberste Gremium unserer Eliteakademien beschloss eine weitere Spezialisierung ihrer jeweiligen Forschungsabteilungen. Gekoppelt mit erheblichen finanziellen Zuwendungen privater Sponsoren wird den Forschungsprozess in zahlreichen Gebieten signifikant beschleunigt werden koennen. ";
			m_strFieldName[0] = "20% erhoehte Biogenetik und Energietechnik";
			m_strFieldName[1] = "20% erhoehte Computertechnik und Antriebstechnik";
			m_strFieldName[2] = "20% erhoehte Bautechnik und Waffentechnik";
			m_strFieldDescription[0] = "Eine engere Kooperation zweier bisher getrennt agierender Forschungsinstitute, der Biogenetik und dem Bereich der Energietechnik, koennte zu einem andauerden Anstieg der Forschungsleistung beider Institute fuehren.";
			m_strFieldDescription[1] = "Mit der Einfuehrung eines, mit 10000 Barren goldgepresstem Latinum dotierten, jaehrlich vergebenen, Forschungspreises im Bereich Computer -und Antriebstechnologie wuerde die Weiterentwlicklung in diesen Bereichen enorm befluegelt.";
			m_strFieldDescription[2] = "Ueberschuesse des Militaerhaushaltes fliessen von nun an geziehlt in die Weiterentwicklung der Waffentechnologie und in Forschungsprojekte aus dem Gebiet des Bauwesens.";
		*/	m_iBonus[0] = 20;
			m_iBonus[1] = 20;
			m_iBonus[2] = 20;
			break;
		}
	case 9:	// Sicherheit	-> eingebaut
		{
		/*	m_strComplexName = "Sicherheit";
			m_strComplexDescription = "Direkte militaerische Bedrohungen stellen, nach Meinung unserer Strategen, nur einen Teil der Gefahren fuer uns dar. Sie schlagen einen weiteren Ausbau unseres Geheimdienstes vor. Sowohl die Struktur als auch die Arbeitsweise sollte ueberdacht und gegebenenfalls reorganisiert werden. Die geforderte pauschale Optimierung des Geheimdienstes ist, wegen der nicht zu realisierenden Finanzierung, nicht moeglich.";
			m_strFieldName[0] = "20% verbesserte innere Sicherheit";
			m_strFieldName[1] = "20% verbesserte Sabotage";
			m_strFieldName[2] = "20% verbesserte Spionage";
			m_strFieldDescription[0] = "Die verbesserte Codierung saemtlicher Komunikationskanaele sowie die gezielte Streuung von Fehlinformationen sollte eine deutliche Verbesserung der internen Sicherheit mitsichbringen.";
			m_strFieldDescription[1] = "Unsere Chirurgen sind, dank der Fortschritte in der Medizintechnik, in der Lage reversible plastische Eingriffe vorzunehmen. Diese chirugischen Veraenderungen wuerden unseren Agenten die Infiltration des Gegners enorm vereinfachen. Die Risiken bei Sabotageakten koennten somit stark minimiert werden.";
			m_strFieldDescription[2] = "Ein dichteres Netz stationaerer Sensorfelder wuerde es unserem Geheimdienst ermoeglichen deutlich mehr Subraumnachrichten abzufangen welche dann gegebenenfalls durch unsere Spezielisten dechiffriert werden koennen.";
		*/	m_iBonus[0] = 20;
			m_iBonus[1] = 15;
			m_iBonus[2] = 25;
			break;
		}
	case 10:	// Lager und Transport	-> eingebaut
		{
		/*	m_strComplexName = "Lager und Transport";
			m_strComplexDescription = "Durch eine Neujustierung unserer Lagertechniken und der internen Transportwege, sind wir nun in der Lage, enorme Vorteile daraus zu erhalten.";
			m_strFieldName[0] = "doppeltes Dilithiumlager";
			m_strFieldName[1] = "kein Abzug beim Stellaren Lager";
			m_strFieldName[2] = "eine Ressourcenroute mehr";
			m_strFieldDescription[0] = "Neue Eindämmungsfelder erhöhen enorm die Sicherheit bei der Lagerung von Dilithium. Damit sind wir in der Lage doppelt so viel Dilithium einzulagern.";
			m_strFieldDescription[1] = "Eine Optimierung der Transportwege und ein besserer Schutz vor Überfällen lässt die Kosten enorm sinken. Mit diesen Techniken hätten wir keinerlei Verluste mehr zu beklagen.";
			m_strFieldDescription[2] = "Eine Optimierung in unserem Tranportsystem würde die Vernetzung unserer Lager auf verschiedenen Systemen vereinfachen. Dadurch wären wir in der Lage ein dichteres Handelsnetz aufzubauen.";
		*/	m_iBonus[0] = 2;
			m_iBonus[1] = 0;
			m_iBonus[2] = 1;
			break;
		}
	case 11:	// Handel
		{
		/*	m_strComplexName = "Handel";
			m_strComplexDescription = "Unsere besten Finanzhaie und Börsenmakler haben sich zusammengefunden, um über eine Verbesserung unserer veralteten Handelstechniken nachzudenken. Nach einiger Zeit haben sie verschiedenste Möglichkeiten gefunden, wie wir bei gleichem Auffwand noch mehr Gewinn machen können.";
			m_strFieldName[0] = "keine Handelsgebühr";
			m_strFieldName[1] = "30% niedrigere Monopolkosten";
			m_strFieldName[2] = "mindestens eine Handelsroute";
			m_strFieldDescription[0] = "Durch Einbringen mehrerer Börsenmakler aus unseren Systemen an der globalen Börse, wären wir in der Lage die übliche Handelsgebühr der Börsenmakler zu umgehen.";
			m_strFieldDescription[1] = "Unsere besten Finanzexperten haben sich die globale Börse noch einmal genauer angeschaut. Durch dieses neue Wissen können wären wir in der Lage, ein Monopol zu verringerten Kosten zu erhalten.";
			m_strFieldDescription[2] = "Durch die vermehrte Einbeziehung der Bevölkerung in den interstellaren Handel, können wir auch in kleinen Systemen eine Handelroute einrichten. Grosse Systeme sind nicht davon betroffen.";
		*/	m_iBonus[0] = 0;
			m_iBonus[1] = 30;
			m_iBonus[2] = 1;
			break;
		}
	case 12:	// Finanzen
		{
		/*	m_strComplexName = "Finanzen";
			m_strComplexDescription = "Durch eine genauer Abstimmung der einzelnen Finanzmaerkte unsere Teilsysteme waehren wir in der Lage, die staetige Ausdehnung des Gesamtmarktes besser zu nutzen. Somit koennten ohne grossen Aufwand zahlreiche Kostenvorteile erwirstschaftet werden.";
			m_strFieldName[0] = "5% niedrigere Kaufkosten";
			m_strFieldName[1] = "10% niedrigere Unterhaltskosten";
			m_strFieldName[2] = "35% Steuern in Heimatsystem";
			m_strFieldDescription[0] = "Das steigende Angebot auf unseren Systemen senkt den Kaufpreis um 5%. Diese Kostenvorteile koennten wir sowohl beim Kauf einzelner Schiffe als auch beim Kauf stationaerer Strukturen auf all unseren Sytemen geltend machen.";
			m_strFieldDescription[1] = "Die staendige Weiterentwicklung der verbauten Materialien und die zunehmende Automatisierung versaetzt uns in die Lage saemltiche Wartungsintervalle zu vergroessern. Das haette eine Senkung der Unterhaltskosten um 10% zur Folge";
			m_strFieldDescription[2] = "Hier die Beschreibung";
		*/	m_iBonus[0] = 5;
			m_iBonus[1] = 10;
			m_iBonus[2] = 35;
			break;
		}
	
	default:
		{
			m_strComplexName = "";
			m_strComplexDescription = "";
			m_strFieldName[0] = "";
			m_strFieldName[1] = "";
			m_strFieldName[2] = "";
			m_strFieldDescription[0] = "";
			m_strFieldDescription[1] = "";
			m_strFieldDescription[2] = "";
			m_iBonus[0] = 0;
			m_iBonus[1] = 0;
			m_iBonus[2] = 0;
			break;
		}
	}
	if (complex <= NoUC)
		ReadSpecialTech((BYTE)complex);
}

/// Resetfunktion für die Klasse CResearchComplex
void CResearchComplex::Reset()
{
	m_byComplexStatus = NOTRESEARCHED;
	memset(m_byFieldStatus, NOTRESEARCHED, 3 * sizeof(*m_byFieldStatus));
	memset(m_iBonus, 0, 3 * sizeof(*m_iBonus));
	m_strComplexName = "";
	m_strComplexDescription = "";
}

//////////////////////////////////////////////////////////////////////
// private Funktionen
//////////////////////////////////////////////////////////////////////

/// Diese private Funktion liest die Beschreibungen zu den Spezialforschungen aus einer Datei ein und speichert sie
/// auf die entsprechenden Variablen.
void CResearchComplex::ReadSpecialTech(BYTE complex)
{
	int i = 0;
	int j = complex * 8;
	int z = 0;
	BOOLEAN found = FALSE;
	CString data[8];
	CString csInput;												// auf csInput wird die jeweilige Zeile gespeichert
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Names\\Specialtechs.data";	// Name des zu Öffnenden Files 
	CStdioFile file;												// Varibale vom Typ CStdioFile
	if (file.Open(fileName, CFile::shareDenyNone | CFile::modeRead | CFile::typeText))	// Datei wird geöffnet
	{
		while (file.ReadString(csInput))
		{
			if (i == j)
			{
				data[z++] = csInput;
				found = TRUE;
			}
			else if (found)
				data[z++] = csInput;
			if (z == 8)
				break;
			i++;
		}
	}
	else
	{	
		AfxMessageBox("Error! Could not open file \"Specialtechs.data\" ...");
		exit(1);
	}
	file.Close();												// Datei wird geschlossen

	m_strComplexName = data[0];
	m_strComplexDescription = data[1];
	m_strFieldName[0] = data[2];
	m_strFieldName[1] = data[3];
	m_strFieldName[2] = data[4];
	m_strFieldDescription[0] = data[5];
	m_strFieldDescription[1] = data[6];
	m_strFieldDescription[2] = data[7];
}