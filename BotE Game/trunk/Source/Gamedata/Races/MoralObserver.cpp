#include "stdafx.h"
#include "MoralObserver.h"
#include "IOData.h"

IMPLEMENT_SERIAL (CMoralObserver, CObject, 1)
// statische Variable initialisieren
short CMoralObserver::m_iMoralMatrix[][DOMINION] = {0};
CString CMoralObserver::m_strTextMatrix[][DOMINION] = {""};

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
CMoralObserver::CMoralObserver(void)
{
}

CMoralObserver::~CMoralObserver(void)
{
	m_iEvents.RemoveAll();
}

///////////////////////////////////////////////////////////////////////
// Speichern / Laden
///////////////////////////////////////////////////////////////////////
void CMoralObserver::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		m_iEvents.Serialize(ar);
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		m_iEvents.RemoveAll();
		m_iEvents.Serialize(ar);
	}
}

/// Serialisierungsfunktion für statische Variable
void CMoralObserver::SerializeStatics(CArchive &ar)
{
	// wenn gespeichert wird
	if (ar.IsStoring())
	{
		for (int i = 0; i < EVENTNUMBER; i++)
			for (int j = 0; j < DOMINION; j++)
			{
				ar << m_iMoralMatrix[i][j];
				ar << m_strTextMatrix[i][j];
			}
	}
	// wenn geladen wird
	if (ar.IsLoading())
	{
		for (int i = 0; i < EVENTNUMBER; i++)
			for (int j = 0; j < DOMINION; j++)
			{
				ar >> m_iMoralMatrix[i][j];
				ar >> m_strTextMatrix[i][j];
			}
	}
}

//////////////////////////////////////////////////////////////////////
// sonstige Funktionen
//////////////////////////////////////////////////////////////////////

/// Funktion berechnet die Moralauswirkung auf alle Systeme, abhängig von den vorgekommenen Events und
/// der jeweiligen Majorrace. Übergeben werden dafür alle Systeme <code>systems</code>, die RaceID und die
/// gemappte Nummer der Majorrace <code>byMappedRaceNumber</code>, auf welche Moralwerte sich bezogen werden soll.
void CMoralObserver::CalculateEvents(std::vector<CSystem>& systems, const CString& sRaceID, BYTE byMappedRaceNumber)
{
	// derzeit nur 6 Moralwertdatensätze vorhanden
	ASSERT(byMappedRaceNumber >= 1 && byMappedRaceNumber <= 6);


/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
													Fed	Fer	Kli	Rom	Car	Dom
#0	Eliminate an Empire								50	50	50	50	50	50		-> eingebaut
#1	Win a Major Battle								5	5	7	6	4	5
#2	Win a Significant Battle						3	3	5	4	2	2
#3	Win a Minor Battle								1	1	2	1	1	1		-> eingebaut (vereinfacht, immer Minorbattle)
#4	Lose a Major Battle								-7	-7	-13	-11	-6	-7
#5	Lose a Significant Battle						-7	-6	-10	-9	-4	-5
#6	Lose a Minor Battle								-2	-2	-4	-4	-2	-2		-> eingebaut (vereinfacht)
#7	Lose a Flagship									-10	-9	-13	-10	-8	-8		-> eingebaut
#8	Lose an Outpost									-3	-5	-1	-4	-1	-1		-> eingebaut
#9	Lose a Starbase 								-4	-5	-2	-5	-2	-2		-> eingebaut

#10	Sign a Membership treaty with Minor				12	6	6	6	4	4		-> eingebaut

#11	Take a System									-8	2	10	4	8	8		-> eingebaut
#12	Colonize a System								3	3	4	4	2	3		-> eingebaut
#13	Liberate a Minor Race System					20	1	-1	2	-5	-5		-> eingebaut
#14	Liberate a Former Native System					9	5	6	10	5	4		-> eingebaut
#15	Lose Home System to Outside Forces				-20	-20	-20	-20	-20	-30		-> eingebaut
#16	Lose a Member System to Outside Forces			-10	-10	-10	-10	-7	-10		-> eingebaut
#17	Lose a Subjugated System to Outside Forces 		-5	-7	-10	-10	-7	-10		-> eingebaut
#18	Lose a System to Rebellion						-15	-10	-10	-15	-5	-10		-> eingebaut
#19	Bombard a System								-5	-2	6	4	4	2		-> eingebaut

#20	Bombard native system that has Rebelled			-7	-2	5	3	5	5		-> eingebaut
#21	Eliminate a Minor Race Entirely					-20	-15	2	-4	4	5		-> eingebaut
#22	Suffer bombardment of System					-1	-2	1	-1	2	-1		-> eingebaut
#23	Suffer 50% pop loss in Home Sys. Bombardment	-10	-10	-10	-10	-10	-10

#24	Declare War on an Empire when Neutral			-5	1	7	1	3	3		-> eingebaut
#25	Declare War on an Empire when Non-Aggression	-6	0	6	1	3	2		-> eingebaut
#26	Declare War on an Empire with Trade Treaty		-7	-10	4	1	3	1		-> eingebaut
#27	Declare War on an Empire with Friendship Treaty	-9	-2	-2	-2	2	1		-> eingebaut
#28	Declare War on an Empire with Defense Pact		-10	-3	-2	-4	0	1		-> eingabaut
#29	Declare War on an Empire with CooperationTreaty -10	-4	-2	-6	-4	1		-> eingebaut
#30	Declare War on an Empire with Affiliation		-12	-8	-5	-10	-5	1		-> eingebaut
#31	Other Empire Declares War when Neutral			5	2	2	5	3	1		-> eingebaut
#32	Other Empire Declares War with Treaty			7	4	4	6	4	1		-> eingebaut
#33	Other Empire Declares War with an Affiliation	10	8	7	8	6	2		-> eingebaut

#34	Sign Trade Treaty								3	5	0	2	1	-3		-> eingebaut
#35	Sign Friendship/Cooperation Treaty				5	3	1	4	2	-6		-> eingebaut
#36	Sign an Affiliation Treaty						8	5	5	5	4	4		-> eingebaut

#37	Receive Acceptance of Non-Aggression Pact		2	1	-3	0	-1	-4
#38	Accept a Non-Aggression Pact					2	1	-4	-1	-2	-4
#39	Receive Acceptance of Defence Pact				5	2	-6	1	-2	-8
#40	Accept a Defence Pact							5	2	-8	-2	-4	-8
#41	Receive Acceptance of War Pact					-2	3	8	3	4	2
#42	Accept a War Pact								-6	1	6	2	4	2

#43	Refuse a Treaty (Trade)							-3	-5	0	1	-1	3
#44	Refuse a Treaty (Friendship, Coop)				-5	-2	1	2	-2	5
#45	Refuse a Treaty (Affiliation)					-10	-4	2	4	-4	10
#46	Refuse a Treaty (Non_Aggression, Defence Pact)	-5	-2	6	2	0	5
#47	Refuse a Treaty (Warpact)						5	0	-10	0	-2	0

#48	Accept a Request								0	-5	-4	-2	-1	-3
#49	Refuse a Request								0	2	2	0	0	1
#50	Receive Acceptance of a Request					1	5	4	1	1	1

#51	Accept Surrender								-50	-50	-50	-50	-50	-50
#52	Receive a Surrender								20	20	20	20	20	10
#53	Accept 'Victory' Non-Aggression Treaty			2	-2	-12	-6	-5	-12
#54	Receive Acceptance of Victory Treaty Demand		8	8	10	6	8	8

#55	Accept a System's Request for Independence		-6	-9	-5	-15	-9	-15
#56	Refuse a System's Request for Independence		-4	2	2	2	5	2

#57	Repel a Borg Attack								10	6	6	12	8	3

#58	Lose population of system to natural event		-2	-2	-2	-2	-2	-1
#59 Successful espionage							0	0	0	1	1	1		-> eingebaut
#60 Successful sabotage								0	0	-1	2	2	1		-> eingebaut
*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*	short moralMatrix[][DOMINION] = {
		50,	50,	50,	50,	50,	50,		// #0	Eliminate an Empire
		5,	5,	7,	6,	4,	5,
		3,	3,	5,	4,	2,	2,
		1,	1,	2,	1,	1,	1,
		-7,	-7,	-13,-11,-6,	-7,
		-7,	-6,	-10,-9,	-4,	-5,
		-2,	-2,	-4,	-4,	-2,	-2,
		-10,-9,	-13,-10,-8,	-8,
		-3,	-5,	-1,	-4,	-1,	-1,
		-4,	-5,	-2,	-5,	-2,	-2,		// #9	Lose a Starbase
		12,	6,	2,	5,	4,	4,		// #10	Sign a Membership treaty with Minor
		-8,	2,	10,	4,	8,	8,
		3,	3,	4,	4,	2,	3,
		20,	1,	-1,	2,	-5,	-5,
		9,	5,	6,	10,	5,	4,
		-20,-20,-20,-20,-20,-30,
		-10,-10,-10,-10,-7,	-10,
		-5,	-7,	-10,-10,-7,	-10,
		-15,-10,-10,-15,-5,	-10,
		-5,	-2,	6,	4,	4,	2,		// #19	Bombard a System if you've declared war
		-7,	-2,	5,	3,	5,	5,
		-20,-15,2,	-4,	4,	5,
		-1,	-2,	1,	-1,	2,	-1,
		-10,-10,-10,-10,-10,-10,		// #23	Suffer 50% pop loss in Home Sys. Bombardment
		-5,	1,	7,	1,	3,	3,
		-6,	0,	6,	1,	3,	2,
		-7,	-10,4,	1,	3,	1,
		-9,	-2,	-2,	-2,	2,	1,
		-10,-3,	-2,	-4,	0,	1,
		-10,-4,	-2,	-6,	-4,	1,
		-12,-8,	-5,	-10,-5,	1,
		5,	2,	2,	5,	3,	1,
		7,	4,	4,	6,	4,	1,
		10,	8,	7,	8,	6,	2,		// #33	Other Empire Declares War with an Affiliation
		3,	5,	0,	2,	1,	-3,
		5,	3,	1,	4,	2,	-6,
		20,	16,	16,	16,	10,	4,		// #36	Sign an Affiliation Treaty
		2,	1,	-3,	0,	-1,	-4,
		2,	1,	-4,	-1,	-2,	-4,
		5,	2,	-6,	1,	-2,	-8,
		5,	2,	-8,	-2,	-4,	-8,
		-2,	3,	8,	3,	4,	2,
		-6,	1,	6,	2,	4,	2,		// #42	Accept a War Pact
		-3,	-5,	0,	1,	-1,	3,
		-5,	-2,	1,	2,	-2,	5,
		-10,-4,	2,	4,	-4,	10,
		-5,	-2,	6,	2,	0,	5,
		5,	0,	-10,0,	-2,	0,		// #47	Refuse a Treaty (Warpact)
		0,	-5,	-4,	-2,	-1,	-3,
		0,	2,	2,	0,	0,	1,
		1,	5,	4,	1,	1,	1,		// #50	Receive Acceptance of a Request
		-50,-50,-50,-50,-50,-50,
		20,	20,	20,	20,	20,	10,
		2,	-2,	-12,-6,	-5,	-12,
		8,	8,	10,	6,	8,	8,		// #54	Receive Acceptance of Victory Treaty Demand
		-6,	-9,	-5,	-15,-9,	-15,
		-4,	2,	2,	2,	5,	2,
		10,	6,	6,	12,	8,	3,
		-2,	-2,	-2,	-2,	-2,	-1,		// #58	Lose population of system to natural event
		0,	0,	0,	1,	1,	1,		// #59	Successful espionage
		0,	0,	-1,	2,	2,	1		// #60	Successful sabotage
	};
*/
	for (int i = 0; i < m_iEvents.GetSize(); )
	{
		short moral = m_iMoralMatrix[m_iEvents.GetAt(i)][byMappedRaceNumber - 1];
		for (int y = 0; y < STARMAP_SECTORS_VCOUNT; y++)
			for (int x = 0; x < STARMAP_SECTORS_HCOUNT; x++)
				if (systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).GetOwnerOfSystem() == sRaceID)
					systems.at(x+(y)*STARMAP_SECTORS_HCOUNT).SetMoral(moral);
		m_iEvents.RemoveAt(i);
	}
}

/// Funktion erstellt einen Text, der in der Ereignisansicht angezeigt werden kann, aufgrund eines speziellen
/// Events. Dieser Text ist rassenabhängig und wird von der Funktion zurückgegeben. Als Parameter werden an diese
/// Funktion das Event <code>event</code> und die Nummer der Majorrace <code>major</code> übergeben. Optional muss
/// auch ein Textstring <code>param</code> übergeben werden, der in bestimmte Nachrichten eingebaut wird.
CString CMoralObserver::GenerateText(unsigned short Event, BYTE major, const CString& param) const
{
/*	CString textMatrix[][DOMINION] = {
	// #0	Eliminate an Empire
		"","","","","","",
	// #1	Win a Major Battle
		"Unsere Bürger sind sehr glücklich über den Ausgang der Schlacht",
		"Händler und Investoren sind über den Ausgang der Schlacht erfreut",
		"Krieger feiern ausgelassen unseren Erfolg in der Schlacht",
		"Die Bevölkerung feiert unseren Sieg in der Schlacht",
		"Das Volk jubelt über unseren militärischen Erfolg in der Schlacht",
		"Die Gründer sind mit dem Verlauf der Schlacht zufrieden",
	// #2	Win a Significant Battle
		"Unsere Bürger sind erfreut über den Ausgang des Kampfes",
		"Händler und Investoren sind über den Ausgang des Kampfes erfreut",
		"Krieger feiern unseren Erfolg im Kampf",
		"Die Bevölkerung freut sich über unseren Sieg im Kampf",
		"Das Volk freut sich über unseren militärischen Erfolg im Kampf",
		"Die Gründer sind mit dem Ausgang des Kampfes zufrieden",
	// #3	Win a Minor Battle
		"Unsere Bürger haben den Ausgang des Kampfes positiv aufgenommen",
		"Händler und Investoren investieren verstärkt in das Militär",
		"Krieger freuen sich über unseren Erfolg im Kampf",
		"Die Bevölkerung hat unseren Sieg im Kampf sehr positiv aufgenommen",
		"Das Volk hat unseren militärischen Erfolg im Kampf positiv aufgenommen",
		"Die Gründer wurden mit dem Ausgang des Kampfes zufriedengestellt",
	// #4	Lose a Major Battle
		"Unsere Bürger sind sehr traurig über Verluste in der Schlacht",
		"Händler und Investoren sind über unserer Niederlage schockiert",
		"Krieger sind wegen der Niederlage in der Schlacht extrem schockiert",
		"Die Bevölkerung hat unser Versagen in der Schlacht scharf verurteilt",
		"Das Volk hat wegen der Niederlage den Glauben an die Union verloren",
		"Die Gründer sind wegen der Niederlage sehr zornig",
	// #5	Lose a Significant Battle
		"Unsere Bürger sind traurig über unsere Verluste im Kampf",
		"Händler und Investoren halten Gelder wegen der Niederlage zurück",
		"Krieger sind wegen der Niederlage im Kampf schockiert",
		"Die Bevölkerung ist wegen unserem Versagen empört",
		"Das Volk versteht unsere Niederlage im Kampf nicht",
		"Die Gründer sind wegen der Niederlage zornig",
	// #6	Lose a Minor Battle
		"Unsere Bürger haben den Ausgang des Kampfes negativ aufgenommen",
		"Händler und Investoren investieren weniger in das Militär",
		"Krieger sind zornig über unsere Niederlage im Kampf",
		"Die Bevölkerung hat unser Versagen im Kampf negativ aufgenommen",
		"Das Volk hat unsere militärische Niederlage im Kampf negativ aufgenommen",
		"Die Gründer wurden mit dem Ausgang des Kampfes nicht zufriedengestellt",
	// #7	Lose a Flagship
		"Bürger sind über den Verlust unseres Flagschiffes "+param+" schockiert",
		"Investoren haben durch den Verlust des Flagschiffes "+param+" Gelder verloren",
		"Krieger wollen den Verlust des Flagschiffes "+param+" nicht wahrhaben",
		"Die Bevölkerung ist über den Verlust des Flagschiffes "+param+" empört",
		"Das Volk ist über den Verlust des Flagschiffes "+param+" schockiert",
		"Die Gründer haben den Verlust des Flagschiffes "+param+" mit Grollen aufgenommen",
	// #8	Lose an Outpost
		"Bürger sind über den Verlust eines Aussenpostens traurig",
		"Investoren haben durch den Verlust eines Aussenpostens Gelder verloren",
		"Krieger wollen den Verlust eines Aussenpostens nicht wahrhaben",
		"Die Bevölkerung ist über den Verlust eines Aussenpostens empört",
		"Das Volk hat den Verlust eines Aussenpostens negativ aufgenommen",
		"Die Gründer haben den Verlust eines Aussenpostens bemerkt",
	// #9	Lose a Starbase
		"Bürger sind über den Verlust einer Sternbasis sehr traurig",
		"Investoren haben durch den Verlust ihrer Sternbasis viel Geld verloren",
		"Krieger wollen den Verlust einer Sternbasis nicht wahrhaben",
		"Die Bevölkerung ist über den Verlust einer Sternbasis empört",
		"Das Volk hat den Verlust einer Sternbasis negativ aufgenommen",
		"Die Gründer haben den Verlust einer Sternbasis bemerkt",
	// #10	Sign a Membership treaty with Minor
		"Bürger freuen sich über den Beitritt der "+param+" in die Föderation",
		"Händler und Investoren freuen sich auf neue Märkte durch die "+param,
		"Krieger sind Stolz auf die "+param+" als neue Brüder",
		"Die Bevölkerung freut sich über den Beitritt der "+param+" zum Sternenimperiums",
		"Das Volk freut sich über den Beitritt der "+param+" zur Union",
		"Die Gründer haben die Mitgliedschaft der "+param+" positiv bemerkt",
	// #11	Take a System
		"Die Bürger heissen die Eroberung des "+param+"-Systems nicht gut",
		"Händler und Investoren freuen sich auf neue Märkte im "+param+"-System",
		"Unsere Krieger feiern die ruhmreiche Eroberung des "+param+"-Systems",
		"Das Volk begrüsst die gewaltsame Eroberung des "+param+"-Systems",
		"Die Regierung freut sich auf neue Sklaven aus dem "+param+"-System",
		"Die Gründer haben die Eroberung des "+param+"-System mit Wohlwollen aufgenommen",
	// #12	Colonize a System
		"Die Föderation feiert Kolonisierung des Systems "+param,
		"Begeisterung: neuer Handel in der Kolonie auf "+param,
		"Das System "+param+" gibt unserem Volk Raum zum Atmen",
		"Das Volk ist froh, dass "+param+" ein Teil des Imperiums ist",
		"Unionsbürger feierten die Kolonialisierung von "+param,
		"Die Gründer geben ihre Zustimmung zu Kolonisierung des "+param+"-Systems",
	// #13	Liberate a Minor Race System
		"Paraden werden zu Ehren jener abgehalten, die die "+param+" befreiten",
		"Die "+param+" könnten wieder mit uns handeln",
		"Die "+param+" wurden befreit: Unsere Krieger sind trotzdem nicht stolz",
		"Die "+param+" wurden befreit: Das Vertrauen ist wiederhergestellt",
		"Die Bevölkerung ist nicht erfreut, dass "+param+" befreit wurde",
		"Die "+param+" wurden befreit: Gründer zweifeln an dieser Taktik",
	// #14	Liberate a Former Native System
		"Die Befreiung des "+param+"-Systems sorgt für Unterstützung",
		"Der neue "+param+"-Markt ist gut für Exporteure",
		"Die Befreiung von "+param+" bringt Krieger zum Bat'leth-Orden",
		"Die Rettung von "+param+" ist willkommener Anlass zum Feiern",
		"Die Befreiung von "+param+" erfreut die loyalen Cardassianer",
		"Die Befreiung von "+param+" erfreut die Gründer",
	// #15	Lose Home System to Outside Forces
		"Demonstranten fordern Erklärung: warum wurde das Sol-System verloren?",
		"Der Verlust von Ferenginar sorgte für das grösste Börsentief",
		"Leben ohne Qo'nos verursacht Selbstmord unter den Kriegern",
		"Das Volk: Verlust von Romulus ist eine Inkompetenz der Regierung",
		"Verlust von Cardassia Prime führt zu Trauer und Selbstmorden",
		"Verlust der Gründerwelt: Gründer sehen in ihrem Dasein keinen Sinn mehr",
	// #16	Lose a Member System to Outside Forces
		"Die Bevölkerung ist über den Verlust von "+param+" erbost",
		"Das Anlegervertrauen ist durch den Verlust von "+param+" leicht gesunken",
		"Krieger sind beschämt über das Versagen bei der Verteidigung von "+param,
		"Volk hat Enttäuschung über den Verlust von "+param+" ausgedrückt",
		"Die Cardassianer bedauern den Verlust des Systems "+param,
		"Gründer können den Verlust von "+param+" noch immer nicht glauben",
	// #17	Lose a Subjugated System to Outside Forces
		"Das "+param+"-System konnte nicht rechtmässig verteidigt werden",
		"Bürger des "+param+"-Systems sind keine Kunden mehr. Das ist schlecht für den Export",
		"Wir konnten das "+param+"-System nicht verteidigen und verloren viel Ehre",
		"Wir konnten "+param+"-System nicht beschützen",
		"Das cardassianische Volk betrauert, dass "+param+"-System verloren ist",
		"Verlust des "+param+"-Systems erzörnt die Gründer",
	// #18	Lose a System to Rebellion
		"Bürger in der ganzen Föderation verurteilen die Rebellion im "+param+"-System",
		"Händler und Investoren sind unglücklich über die Rebellion "+param+"-System",
		"Krieger empfinden Wut wegen der Rebellion im "+param+"-System",
		"Das Volk kann die Rebellion im "+param+"-System absolut nicht nachvollziehen",
		"Bürger der Union haben die Rebellion im "+param+"-System negativ aufgenommen",
		"Rebellion im "+param+"-System hat die Gründer sehr verärgert",
	// #19	Bombard a System
		"Angriff auf "+param+" verringerte öffentliche Unterstützung",
		"Bombardierung des "+param+"-Systems verhindert Börsenaufschwung",
		"Bombardierung von "+param+" zeigt, wir sind  Klingonen",
		"Bombardierung des "+param+"-Systems verstummt Dissidenten",
		"Bombardierung des "+param+"-Systems verstummte Unionskritiker",
		"Bombardierung des "+param+"-Systems lässt die Gründer aufhorchen",
	// #20	Bombard native system that has Rebelled
		"Angriff auf die Rebellen im "+param+"-System wird vom Volk nicht unterstützt",
		"Bombardierung der Rebellen im "+param+"-System ist schlecht für die Börse",
		"Bombardierung unser rebellischen Brüder im "+param+"-System erfreut die wahren Klingonen",
		"Bombardierung der Rebellen im "+param+"-System kann leicht vertuscht werden",
		"Bombardierung der Rebellen im "+param+"-System erfreut die Union",
		"Die Gründer rechtfertigen die Bombardierung der Rebellen im "+param+"-System",
	// #21	Eliminate a Minor Race Entirely
		"Demonstration: Auslöschung der "+param+" kann nicht nachvollzogen werden",
		"Auslöschung der "+param+": Börse reagiert mit enormen Kursverlusten",
		"Klingonen feiern die Auslöschung der "+param,
		"Volk verurteilt die Auslöschung der "+param,
		"Auslöschung der "+param+" gerechtfertigt: Sie waren es nicht wert",
		"Vernichtung der "+param+" erfreut die Gründer",
	// #22	Suffer bombardment of System
		"Volk fordert Beendung der Bombardierung von "+param,
		"Bombardierung von "+param+" zerstört Geschäfte",
		"Klingonen fordern: Helft unseren Brüdern auf "+param,
		"Volk bittet den Seant endlich gegen die Bombardierung von "+param+" einzuschreiten",
		"Bürger der Union haben kein Mitleid mit der Bevölkerung auf "+param,
		"Gründer fordern die sofortige Beendung des Bombardements auf "+param,
	// #23	Suffer 50% pop loss in Home Sys. Bombardment
		"","","","","","",
	// #24	Declare War on an Empire when Neutral
		"Unser Krieg gegen "+param+" stösst auf starke Kritik",
		"Positive Reaktion auf die Kriegserklärung an "+param,
		"Unser Krieg gegen "+param+" erfreut die wahren Krieger",
		"Das Volk freut sich, dass wir "+param+" bekämpfen können",
		"Unser Volk begrüsst den Krieg gegen "+param+" sehr",
		"Die Gründer freuen sich auf den Krieg gegen "+param+" sehr",
	// #25	Declare War on an Empire when Non-Aggression
		"Unser Krieg gegen "+param+" stösst auf starke Kritik",
		"Geteilte Reaktionen auf die Kriegserklärung an "+param,
		"Unser Krieg gegen "+param+" erfreut die wahren Krieger",
		"Das Volk freut sich, dass wir "+param+" bekämpfen können",
		"Unser Volk begrüsst den Krieg gegen "+param+" sehr",
		"Die Gründer freuen sich auf den Krieg gegen "+param,
	// #26	Declare War on an Empire with Trade Treaty
		"Unser Krieg mit "+param+" stösst auf sehr starke Kritik",
		"Extrem negative Reaktionen auf die Kriegserklärung an "+param,
		"Unser Krieg gegen "+param+" erfreut manche Krieger",
		"Das Volk freut sich, dass wir "+param+" vernichten werden",
		"Unser Volk begrüsst den Krieg gegen "+param+" sehr",
		"Die Gründer freuen sich auf den Krieg gegen "+param,
	// #27	Declare War on an Empire with Friendship Treaty
		"Unser Krieg gegen "+param+" stösst auf sehr starke Kritik",
		"Negative Reaktionen auf die Kriegserklärung an "+param,
		"Unser Krieg gegen "+param+" wird von den wahren Kriegern nicht verstanden",
		"Das Volk ist gegen einen Krieg gegen "+param,
		"Unser Volk begrüsst den Krieg gegen "+param,
		"Die Gründer freuen sich auf den Krieg gegen "+param,
	// #28	Declare War on an Empire with Defense Pact
		"Unser Krieg gegen "+param+" stösst auf extrem starke Kritik",
		"Negative Reaktionen auf den Krieg gegen "+param,
		"Unser Krieg gegen "+param+" wird von den wahren Kriegern nicht verstanden",
		"Das Volk ist sehr gegen den Krieg gegen "+param,
		"Unser Volk hat über den Krieg gegen "+param+" eine geteilte Meinung",
		"Die Gründer freuen sich auf den Krieg gegen "+param,
	// #29	Declare War on an Empire with CooperationTreaty
		"Unser Krieg gegen "+param+" stösst auf extrem starke Kritik",
		"Sehr negative Reaktionen auf den Krieg gegen "+param,
		"Unser Krieg gegen "+param+" wird von den wahren Kriegern nicht verstanden",
		"Das gesamte Volk ist geschlossen gegen den Krieg gegen "+param,
		"Unser Volk verabscheut den Krieg gegen "+param,
		"Die Gründer freuen sich auf den Krieg gegen "+param,
	// #30	Declare War on an Empire with Affiliation
		"Unser Krieg gegen "+param+" lässt das Volk auf die Barrikaden gehen",
		"Extrem negative Reaktionen auf den Krieg gegen "+param,
		"Unser Krieg gegen "+param+" wird von allen Kriegern abgelehnt",
		"Das gesamte Volk übt starke Kritik wegen dem Krieg gegen "+param,
		"Unser Volk verabscheut den Krieg gegen "+param,
		"Die Gründer freuen sich auf den Krieg gegen "+param,
	// #31	Other Empire Declares War when Neutral
		"Krieg gegen "+param+" hat Unterstützung",
		"Krieg gegen "+param+" lässt auf ein gutes Quartal hoffen",
		"Unser Krieg gegen "+param+" erfreut die wahren Krieger",
		"Ein Krieg gegen "+param+" bedeutet Abrechnung",
		"Durch Krieg gegen "+param+" ist die Zahl der freiwilligen drastisch gestiegen",
		"Die Gründer freuen sich auf den Krieg gegen "+param,
	// #32	Other Empire Declares War with Treaty
		"Krieg gegen "+param+" hat grosse Unterstützung",
		"Krieg gegen "+param+" lässt auf ein sehr gutes Quartal hoffen",
		"Unser Krieg gegen "+param+" erfreut die wahren Krieger",
		"Ein Krieg gegen "+param+" bedeutet Abrechnung",
		"Durch Krieg gegen "+param+" ist die Zahl der freiwilligen drastisch gestiegen",
		"Die Gründer freuen sich auf den Krieg gegen "+param,
	// #33	Other Empire Declares War with an Affiliation
		"Krieg gegen "+param+" hat sehr grosse Unterstützung",
		"Krieg gegen "+param+" lässt auf viele sehr gutes Quartale hoffen",
		"Unser Krieg gegen "+param+" erfreut alle wahren Krieger sehr",
		"Ein Krieg gegen "+param+" bedeutet Abrechnung: Volk feiert",
		"Durch Krieg gegen "+param+" ist die Zahl der freiwilligen drastisch gestiegen",
		"Die Gründer freuen sich sehr auf den Krieg gegen "+param,
	// #34	Sign Trade Treaty
		"Neuer Handel mit "+param+" stimmt Bürger fröhlich",
		"Händler und Investoren freuen sich auf den Handel mit "+param,
		"Handelsvertrag mit "+param+" interessiert die wahren Krieger kaum",
		"Das Volk ist über den Handelsvertrag mit "+param+" froh",
		"Bürger finden den Handelsvertrag mit "+param+" gut",
		"Gründer haben kein Verständnis für den Handelsvertrag mit "+param,
	// #35	Sign Friendship/Cooperation Treaty
		"Bürger sind glücklich über den neuen Vertrag mit "+param+": Breite Zustimmung",
		"Der neue Vertrag mit "+param+" ist gut für das Geschäft",
		"Krieger verstehen die Unterzeichnung des Vertrages mit "+param,
		"Das Volk ist über den Vertrag mit "+param+" glücklich",
		"Bürger der Union finden den Vertrag mit "+param+" gut",
		"Gründer haben absolut kein Verständnis für den Vertrag mit "+param,
	// #36	Sign an Affiliation Treaty
		"Bürger feiern ausgelassen das neue Bündnis mit "+param,
		"Händler feiern ausgelassen das neue Bündnis mit "+param,
		"Krieger feiern ausgelassen das neue Bündnis mit "+param,
		"Das Volk feiert ausgelassen das neue Bündnis mit "+param,
		"Bürger sind glücklich über das neue Bündnis mit "+param,
		"Gründer zeigen positive Reaktionen auf das Bündnis mit "+param,
	// #37 -> #58
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
		"","","","","","",
	// #59	Successful espionage
		"Bürger sind die Spionageaktionen unseres Geheimdienstes egal",
		"Händler konzentrieren sich lieber auf Geschäfte als auf Spionage",
		"Krieger bezeichnen Spionageaktionen als unehrenhaft",
		"Das Volk findet die Spionage des Tal'Shiar gut",
		"Bürger der Union finden leichten Gefallen an den Spionageaktionen",
		"Gründer nehmen die Spionageaktionen mit einem Schmunzeln zur Kenntniss",
	// #60	Successful sabotage
		"Bürger sind die Sabotageaktionen unseres Geheimdienstes egal",
		"Händler konzentrieren sich lieber auf Geschäfte als auf Sabotage",
		"Krieger finden feige Saobtageaktionen als unehrenhaft",
		"Das Volk begrüsst die Sabotageaktionen des Tal'Shiar",
		"Bürger der Union finden Gefallen an den Sabotageaktionen des Ordens",
		"Gründer nehmen die Sabotageaktionen mit einem Schmunzeln zur Kenntniss"
	};
	*/
	CString text = m_strTextMatrix[Event][major-1];
	text.Replace("$param$", param);

	int nMoralValue = GetMoralValue(major, Event);
	CString s = "";
	if (nMoralValue > 0)
		s.Format(" (+%d)", nMoralValue);
	else if (nMoralValue < 0)
		s.Format(" (%d)", nMoralValue);
	else
		s = " (+- 0)";

	text += s;
	return text;
}
/*													Fed	Fer	Kli	Rom	Car	Dom
#37	Receive Acceptance of Non-Aggression Pact		2	1	-3	0	-1	-4
#38	Accept a Non-Aggression Pact					2	1	-4	-1	-2	-4	// KLI: Nichtangriffspakt mit "+param+" angenommen: Entäuschung" ROM: Nichtangriffspakt mit "+param+": Breite Zustimmung"
#39	Receive Acceptance of Defence Pact				5	2	-6	1	-2	-8
#40	Accept a Defence Pact							5	2	-8	-2	-4	-8
#41	Receive Acceptance of War Pact					-2	3	8	3	4	2
#42	Accept a War Pact								-6	1	6	2	4	2

#43	Refuse a Treaty (Trade)							-3	-5	0	1	-1	3
#44	Refuse a Treaty (Friendship, Coop)				-5	-2	1	2	-2	5
#45	Refuse a Treaty (Affiliation)					-10	-4	2	4	-4	10
#46	Refuse a Treaty (Non_Aggression, Defence Pact)	-5	-2	6	2	0	5	// KLI: Unser Volk ist froh, dass der Frieden abgewendet werden konnte ROM: Frieden mit "+param+" abgelehnt: das Volk ist glücklich"
#47	Refuse a Treaty (Warpact)						5	0	-10	0	-2	0

#48	Accept a Request								0	-5	-4	-2	-1	-3
#49	Refuse a Request								0	2	2	0	0	1
#50	Receive Acceptance of a Request					1	5	4	1	1	1

#51	Accept Surrender								-50	-50	-50	-50	-50	-50
#52	Receive a Surrender								20	20	20	20	20	10
#53	Accept 'Victory' Non-Aggression Treaty			2	-2	-12	-6	-5	-12
#54	Receive Acceptance of Victory Treaty Demand		8	8	10	6	8	8

#55	Accept a System's Request for Independence		-6	-9	-5	-15	-9	-15
#56	Refuse a System's Request for Independence		-4	2	2	2	5	2

#57	Repel a Borg Attack								10	6	6	12	8	3

#58	Lose population of system to natural event		-2	-2	-2	-2	-2	-1
*/

/// Funktion lädt die zu Beginn die ganzen Moralwerte für alle verschiedenen Ereignisse in eine Moralmatrix.
void CMoralObserver::InitMoralMatrix()
{
	// Moralwerte laden
	CString fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Other\\Moral.data";
	CStdioFile file;
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))
	{
		CString input;
		int i = 0;
		while (file.ReadString(input))
		{
			int pos = input.ReverseFind(':');
			input.Delete(0, pos + 1);
			input.Trim();
			pos = 0;
			for (int j = 0; j <= DOMINION; j++)
			{
				CString value = input.Tokenize("\t", pos);
				m_iMoralMatrix[i][j] = atoi(value);
			}
			i++;
		}
	}
	else
	{
		AfxMessageBox("Error! Could not open \"Moral.data\"...");
		exit(1);
	}
	file.Close();

	// Textnachrichten laden
	fileName = CIOData::GetInstance()->GetAppPath() + "Data\\Other\\MoralEvents.data";
	if (file.Open(fileName, CFile::modeRead | CFile::typeText))
	{
		CString input;
		int i = -1;
		int race = 0;
		while (file.ReadString(input))
		{
			// jedes neue Event beginnt mit einer Nummer, z.B. #17
			if (input.Left(1) != "#")
			{
				input.Trim();
				m_strTextMatrix[i][race++] = input;
			}
			else
			{
				i++;
				race = 0;
			}
		}
	}
	else
	{
		AfxMessageBox("Error! Could not open \"MoralEvents.data\"...");
		exit(1);
	}
	file.Close();

/*
	fileName = *((CBotEApp*)AfxGetApp())->GetPath() + "Data\\Other\\MoralEventsTest.txt";
	if (file.Open(fileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		for (int j = 0; j < EVENTNUMBER; j++)
		{
			CString s;
			s.Format("#%d\n", j);
			file.WriteString(s);
			for (int k = 0; k < DOMINION; k++)
				file.WriteString(m_strTextMatrix[j][k] + "\n");
		}
	}
	file.Close();
*/
}

/// Funktion gibt einen bestimmten Moralwert zurück.
/// @param byMappedRaceNumber gemappte Rassennummer
/// @param Event Eventnummer
/// @return Moralwert
short CMoralObserver::GetMoralValue(BYTE byMappedRaceNumber, unsigned short Event)
{
	// derzeit nur 6 Moralwertdatensätze vorhanden
	ASSERT(byMappedRaceNumber >= 1 && byMappedRaceNumber <= 6);

	return m_iMoralMatrix[Event][byMappedRaceNumber - 1];
}
