#include "driver2.h"
#include "FEmain.h"

#include "LIBGPU.H"
#include "LIBETC.H"

#include "C/cd_icon.h"

#include "C/system.h"
#include "C/glaunch.h"
#include "C/mission.h"
#include "C/time.h"
#include "C/loadsave.h"
#include "C/pad.h"
#include "C/loadview.h"
#include "C/pres.h"
#include "C/sound.h"
#include "C/debris.h"
#include "C/E3stuff.h"
#include "C/fmvplay.h"
#include "C/scores.h"
#include "C/loadsave.h"

#include "MEMCARD/MAIN.H"

#include "STRINGS.H"

struct FE_CHARDATA
{
	u_char u;
	u_char v;
	u_char w;
	u_char h;
};

struct FE_FONT
{
	int NumFonts;
	FE_CHARDATA CharInfo[256];
};

struct SCREEN_LIMITS
{
	short minx;
	short miny;
	short maxx;
	short maxy;
};

struct BOTCH
{
	int missNum;
	char** name;
};

typedef int(*screenFunc)(int bSetup);

extern ACTIVE_CHEATS AvailableCheats;
extern ACTIVE_CHEATS ActiveCheats;

screenFunc fpUserFunctions[] = {
	CentreScreen,
	CarSelectScreen,
	CopDiffLevelScreen,
	VibroOnOffScreen,
	MissionSelectScreen,
	MissionCityScreen,
	CutSceneCitySelectScreen,
	CutSceneSelectScreen,
	SetVolumeScreen,
	ScoreScreen,
	SubtitlesOnOffScreen,
	CityCutOffScreen,
	ControllerScreen,
	MainScreen,
	CheatScreen,
	ImmunityOnOffScreen,
	InvincibleOnOffScreen,
	GamePlayScreen,
	GameNameScreen,
	CheatNumlayerSelect,

};

char* gfxNames[4] = {
	"DATA\\CARS\\CCARS.RAW",
	"DATA\\CARS\\HCARS.RAW",
	"DATA\\CARS\\VCARS.RAW",
	"DATA\\CARS\\RCARS.RAW",
};

char* cutGfxNames[4] = {
	"DATA\\CUTS\\CCUTS.RAW",
	"DATA\\CUTS\\HCUTS.RAW",
	"DATA\\CUTS\\VCUTS.RAW",
	"DATA\\CUTS\\RCUTS.RAW",
};

int CutAmounts[5] = {
	0, 8, 7, 5, 8
};

int CutAmountsTotal[5] = {
	0, 8, 15, 20, 40
};

char* CutSceneNames[28] =
{
	M_LTXT_ID(MTXT_RedRiver),
	M_LTXT_ID(MTXT_Themorgue),
	M_LTXT_ID(MTXT_TheWitness),
	M_LTXT_ID(MTXT_Lennysapartment),
	M_LTXT_ID(MTXT_TheCubaConnection),
	M_LTXT_ID(MTXT_TheIntruder),
	M_LTXT_ID(MTXT_MeetingCaine),
	M_LTXT_ID(MTXT_LeavingTown),

	M_LTXT_ID(MTXT_Lookingforalead),
	M_LTXT_ID(MTXT_Movingout),
	M_LTXT_ID(MTXT_Watchingthetruck),
	M_LTXT_ID(MTXT_RosannaSotoClue),
	M_LTXT_ID(MTXT_TheDockyard),
	M_LTXT_ID(MTXT_TheHit),
	M_LTXT_ID(MTXT_SeizingJericho),

	M_LTXT_ID(MTXT_VasquezinVegas),
	M_LTXT_ID(MTXT_TradingJericho),
	M_LTXT_ID(MTXT_CBankjob),
	M_LTXT_ID(MTXT_ThePoolHall),
	M_LTXT_ID(MTXT_CainesWarpath),

	M_LTXT_ID(MTXT_CaineinRio),
	M_LTXT_ID(MTXT_WarningJones),
	M_LTXT_ID(MTXT_TheShootout),
	M_LTXT_ID(MTXT_Lennysgetaway),
	M_LTXT_ID(MTXT_Lennygetsit),
	M_LTXT_ID(MTXT_BackinChicago),
	M_LTXT_ID(MTXT_VasquezmeetsCaine),
	M_LTXT_ID(MTXT_Credits),
};

char *areaNames[4][8] = {
	{
		M_LTXT_ID(MTXT_Downtown),
		M_LTXT_ID(MTXT_Wrigleyville),
		M_LTXT_ID(MTXT_Greektown),
		M_LTXT_ID(MTXT_GrantPark),
		M_LTXT_ID(MTXT_MeigsField),
		M_LTXT_ID(MTXT_UkrainianVillage),
		M_LTXT_ID(MTXT_RiverNorth),
		M_LTXT_ID(MTXT_CabriniGreen),
	},
	{
		M_LTXT_ID(MTXT_NecropolisdeColon),
		M_LTXT_ID(MTXT_Capitolio),
		M_LTXT_ID(MTXT_OldHavana),
		M_LTXT_ID(MTXT_TheDocks),
		M_LTXT_ID(MTXT_Vedado),
		M_LTXT_ID(MTXT_Plaza),
		M_LTXT_ID(MTXT_PlazadelaRevolucion),
		NULL,
	},
	{
		M_LTXT_ID(MTXT_Downtown),
		M_LTXT_ID(MTXT_UpperStrip),
		M_LTXT_ID(MTXT_Lakeside),
		M_LTXT_ID(MTXT_MidStrip),
		M_LTXT_ID(MTXT_NorthVegas),
		M_LTXT_ID(MTXT_Lakes),
		M_LTXT_ID(MTXT_GhostTown),
		NULL,
	},
	{
		M_LTXT_ID(MTXT_Centro),
		M_LTXT_ID(MTXT_Copacabana),
		M_LTXT_ID(MTXT_SantaTereza),
		M_LTXT_ID(MTXT_LagoaRodrigodeFreitas),
		M_LTXT_ID(MTXT_PracadaBandeira),
		M_LTXT_ID(MTXT_Leblon),
		M_LTXT_ID(MTXT_Flamengo),
		NULL,
	},
};

static char gameAreas[64] = {
	// Getaway (0-6)
	0, 1, // Downtown, Wrigleyville
	0, 1, // Necropolis De Colon, Capitolio
	0, 1, // Downtown, Upper Strip
	0, 1, // Centro, Copacabana

	// Gate race (8-14)
	2, 3, // Greektown, Grant Park
	0, 2, // Necropolis de Colon, Old Havana
	2, 3, // Lakeside, Mid Strip
	1, 2, // Copacabana, Santa Tereza

	// Checkpoint (16-22)
	0, 4, // Downtown, Meigs Field
	3, 2, // The Docks, Old Havana
	4, 2, // North Vegas, Lakeside
	3, 4, // Lagoa Rodrigo de Freitas, Praca da Bandeira

	// Trailblazer (24-30)
	3, 0, // Grant Park, Downtown
	2, 4, // Old Havana, Vedado
	0, 1, // Downtown, Upper Strip
	5, 4, // Leblon, Praca da Bandeira

	// [MP] Cops 'n Robbers (32-38)
	5, 0, // Ukrainian Village, Downtown
	4, 0, // Vedado, Necropolis de Colon
	3, 0, // Mid Strip, Downtown
	1, 0, // Copacabana, Centro

	// [MP] Capture the Flag (40-46)
	7, 6, // Cabrini Green, River North
	2, 5, // Old Havana, Plaza
	5, 6, // Lakes, Ghost Town
	6, 0, // Flamengo, Centro

	// [MP] Take a Ride (48-54)
	6, 7, // River North, Cabrini Green
	5, 2, // Plaza, Old Havana
	0, 5, // Downtown, Lakes
	0, 6, // Centro, Flamengo

	// [MP] Checkpoint (56-62)
	6, 7, // River North, Cabrini Green
	2, 6, // Old Havana, Plaza de la Revolucion
	6, 4, // Ghost Town, North Vegas
	0, 6, // Centro, Flamengo
};

#define AREA_NAME(level, index) GET_MISSION_TXT(areaNames[level][index])

#define GAMEMODE_AREA(level, offset, index) gameAreas[offset + level * 2 + index]
#define GAMEMODE_AREA_NAME(level, offset, index) AREA_NAME(level, GAMEMODE_AREA(level, offset, index))

int CarAvailability[4][10] = {
	{1,1,1,1,0,0,0,0,0,0},
	{1,1,1,1,0,0,0,0,0,0},
	{1,1,1,1,0,0,0,0,0,0},
	{1,1,1,1,0,0,0,0,0,0},
};

char carNumLookup[4][10] = {
	{1, 2, 3, 4, 0, 8, 9, 10, 11, 12},
	{1, 2, 3, 4, 0, 8, 9, 10, 11, 12},
	{1, 2, 3, 4, 0, 8, 9, 10, 11, 12},
	{1, 2, 3, 4, 0, 8, 9, 10, 11, 12},
};

int minmaxSelections[4][2] = {

	{ 0, 8 },
	{ 8, 17 },
	{ 18, 27 },
	{ 28, 37 }
};

BOTCH botch[38] = {
	{ 1, &MissionName[0]},
	{ 2, &MissionName[1]},
	{ 3, &MissionName[2]},
	{ 4, &MissionName[3]},
	{ 5, &MissionName[4]},
	{ 6, &MissionName[5]},
	{ 7, &MissionName[6]},
	{ 9, &MissionName[7]},
	{ 10, &MissionName[8]},
	{ 11, &MissionName[9]},
	{ 13, &MissionName[10]},
	{ 14, &MissionName[11]},
	{ 15, &MissionName[12]},
	{ 16, &MissionName[13]},
	{ 17, &MissionName[14]},
	{ 18, &MissionName[15]},
	{ 19, &MissionName[16]},
	{ 20, &MissionName[17]},
	{ 21, &MissionName[18]},
	{ 22, &MissionName[19]},
	{ 23, &MissionName[20]},
	{ 24, &MissionName[21]},
	{ 25, &MissionName[22]},
	{ 26, &MissionName[23]},
	{ 27, &MissionName[24]},
	{ 28, &MissionName[25]},
	{ 29, &MissionName[26]},
	{ 30, &MissionName[27]},
	{ 31, &MissionName[28]},
	{ 32, &MissionName[29]},
	{ 33, &MissionName[30]},
	{ 34, &MissionName[31]},
	{ 35, &MissionName[32]},
	{ 37, &MissionName[33]},
	{ 38, &MissionName[34]},
	{ 39, &MissionName[35]},
	{ 40, &MissionName[36]},
	{ 0,NULL}
};

char ScreenTitle[128];
char ScoreName[128];

int bDoingScores = 0;
int bInCutSelect = 0;
int cutSelection = 0;
int currCity = 0;

int bRedrawFrontend = 0;
int bReturnToMain = 1;

int idle_timer = 0;
int currPlayer = 1;
int fePad = 0;
int ScreenDepth = 0;

int gIdleReplay = 0;

int padsConnected[2] = { 1, 0 };
int feVariableSave[4] = { -1 };
int bCdIconSetup = 0;
int bMissionSelect = 0;
int bDoingCarSelect = 0;
int bDoingCutSelect = 0;
int bDrawExtra = 0;
int mainScreenLoaded = 1;
int bDoneAllready = 0;
int allowVibration = 1;
int bQuitToSystem = 0;
int bQuitToSystemSel = 0;

int carSelection = 0;
int currSelIndex = 0;
int lastCutCity = -1;
int lastCity = -1;
int currMission = -1;
int missionSetup = 0;

char* ScreenNames[12] = { 0 };

char loaded[3] = { -1, -1, -1 };

PSXSCREEN* pCurrScreen = NULL;
PSXSCREEN* pNewScreen = NULL;

PSXBUTTON* pNewButton = NULL;
PSXBUTTON* pCurrButton = NULL;

PSXSCREEN PsxScreens[42] = { 0 };
PSXSCREEN* pScreenStack[10] = { 0 };
PSXBUTTON* pButtonStack[10] = { 0 };

POLY_FT4 BackgroundPolys[6];
FE_FONT feFont;

RECT16 extraRect = { 896, 256, 64, 219 };

POLY_FT4 cd_sprite;

SPRT extraSprt;
POLY_FT3 extraDummy;

// [D] [T]
void SetVariable(int var)
{
	int code = (var >> 8);
	int value = (var & 0xff);

	switch (code)
	{
		case 1:
			GameLevel = value;
			break;
		case 2:
			GameType = (GAMETYPE)value;

			if (value != GAME_COPSANDROBBERS)
				gWantNight = 0;

			break;
		case 3:
			NumPlayers = (value + 1);
			break;
		case 4:
			gWantNight = value > 0;

			break;
		case 5:
			gCurrentMissionNumber = 1;
			GameType = GAME_MISSION;
			break;
		case 6:
			pScreenStack[ScreenDepth] = pCurrScreen;
			pButtonStack[ScreenDepth] = pCurrButton;

			if (value == 1) 
			{
#ifdef PSX
				if (CallMemoryCard(0x11, 0) == 0)
#else
				if(LoadReplayFromFile("CHASE.D2RP") == 0)		// [A] temporary
#endif
				{
					ReInitFrontend();
				}
				else 
				{
					StoredGameType = GameType;
					GameType = GAME_LOADEDREPLAY;
					GameStart();
				}
			}
			else
			{
				CallMemoryCard(0x81, 0);
				ReInitFrontend();
				SetMasterVolume(gMasterVolume);
				SetXMVolume(gMusicVolume);
			}

			break;
		case 7:
			pScreenStack[ScreenDepth] = pCurrScreen;
			pButtonStack[ScreenDepth] = pCurrButton;

			if (value == 0) 
			{
#ifdef PSX
				CallMemoryCard(0x80, 0);
				ReInitFrontend();
#else
				// [A] save configuration
				SaveCurrentProfile();
#endif
			}
			else 
			{
#ifdef PSX
				if (CallMemoryCard(0x21, 0) == 0) 
				{
					ReInitFrontend();
				}
				else
#else
				if(LoadCurrentGame())
#endif
				{
					GameType = GAME_CONTINUEMISSION;
					GameStart();
				}
			}

			break;
		case 8:
			gSubGameNumber = value;
			break;
		case 9:
			gSubtitles = (value == 0) ? 0 : 1;
			break;
		case 10:
			gInvincibleCar = value;
			ActiveCheats.cheat3 = value;
			break;
		case 11:
			gPlayerImmune = value;
			ActiveCheats.cheat4 = value;
			break;
		case 12:
			GameLevel = 3;
			gSubGameNumber = (value == 0) ? 2 : 0;
			break;
		case 13:
			GameType = GAME_IDLEDEMO;
			gCurrentMissionNumber = (value + 400);
			break;
		case 14: // [A]
		{
			ShowBonusGallery();

			LoadFrontendScreens();
		}
	}
}

// [D] [T]
void LoadFrontendScreens(void)
{
	int iNumScreens;
	char *ptr;
	RECT16 rect;

	ShowLoadingScreen("GFX\\FELOAD.TIM", 1, 12);
	ShowLoading();

	Loadfile("DATA\\SCRS.BIN", _frontend_buffer);

	ptr = _frontend_buffer + 20; // skip header and number of screens
	iNumScreens = (int)_frontend_buffer[16];

	for (int i = 0; i < iNumScreens; i++)
	{
		PsxScreens[i].numButtons = *ptr++;
		PsxScreens[i].userFunctionNum = *ptr++;

		for (int j = 0; j < PsxScreens[i].numButtons; j++)
		{
			memcpy((u_char*)&PsxScreens[i].buttons[j], (u_char*)ptr, sizeof(PSXBUTTON));
			ptr += sizeof(PSXBUTTON);
		}
	}

	rect.w = 64;
	rect.h = 256;

	ShowLoading();
	LoadBackgroundFile("DATA\\GFX.RAW");

	for (int i = 0; i < 2; i++)
	{
		ShowLoading();
		LoadfileSeg("DATA\\GFX.RAW", _frontend_buffer, 0x30000 + (i * 0x8000), 0x8000);

		rect.x = 640 + (i * 64);
		rect.y = 256;

		LoadImage(&rect, (u_long *)_frontend_buffer);
		DrawSync(0);
	}

	ShowLoading();	
	LoadfileSeg("DATA\\GFX.RAW", _frontend_buffer, 0x58000, 0x8000);

	rect.x = 960;
	rect.y = 256;

	// load font
	LoadImage(&rect, (u_long *)_frontend_buffer);
	DrawSync(0);

	Loadfile("DATA\\FEFONT.BNK", (char*)&feFont);
}

// [D] [T]
void LoadBackgroundFile(char *name)
{
	int iTpage;
	int p;
	RECT16 rect;
	int pages[6];
	int i;
	
	iTpage = 11;

	pages[0] = 0;
	pages[1] = 1;
	pages[2] = 2;
	pages[3] = 3;
	pages[4] = 4;
	pages[5] = 5;

	mainScreenLoaded = (strcmp(name, "DATA\\GFX.RAW") == 0);

	rect.w = 64;
	rect.h = 256;

	for (i = 0; i < 6; i++)
	{
		FEDrawCDicon();
		
		p = pages[i];

		LoadfileSeg(name, _overlay_buffer, p * 0x8000, 0x8000);
		FEDrawCDicon();

		rect.y = (short)(p / 6);
		rect.x = ((short)p + rect.y * -6) * 64 + 640;
		rect.y *= 256;

		LoadImage(&rect, (u_long *)_overlay_buffer);
		FEDrawCDicon();
	}

	LoadfileSeg(name, _overlay_buffer, iTpage * 0x8000, 0x800);
	FEDrawCDicon();

	rect.h = 1;
	rect.y = (short)(iTpage / 6);
	rect.x = ((short)iTpage + rect.y * -6) * 64 + 640;
	rect.y *= 256;

	LoadImage(&rect, (u_long *)_overlay_buffer);
	DrawSync(0);

	SetupBackgroundPolys();
}

// [D] [T]
void SetupBackgroundPolys(void)
{
	POLY_FT4 *poly;
	int i;

	for (i = 0; i < 6; i++)
	{
		poly = &BackgroundPolys[i];

		setPolyFT4(poly);
		setRGB0(poly, 128, 128, 128);
	}

	poly = &BackgroundPolys[0];
	setXYWH(poly, 0, 0, 256, 256);
	setUVWH(poly, 0, 0, 255, 255);
	setClut(poly, 960, 256);
	setTPage(poly, 0, 0, 640, 0);

	poly = &BackgroundPolys[1];
	setXYWH(poly, 256, 0, 256, 256);
	setUVWH(poly, 0, 1, 255, 254);
	setClut(poly, 960, 256);
	setTPage(poly, 0, 0, 704, 0);

	poly = &BackgroundPolys[2];
	setXYWH(poly, 512, 0, 128, 256);
	setUVWH(poly, 0, 1, 128, 254);
	setClut(poly, 960, 256);
	setTPage(poly, 0, 0, 768, 0);

	poly = &BackgroundPolys[3];
	setXYWH(poly, 0, 256, 256, 256);
	setUVWH(poly, 0, 0, 255, 255);
	setClut(poly, 960, 256);
	setTPage(poly, 0, 0, 832, 0);

	poly = &BackgroundPolys[4];
	setXYWH(poly, 256, 256, 256, 256);
	setUVWH(poly, 0, 0, 255, 255);
	setClut(poly, 960, 256);
	setTPage(poly, 0, 0, 896, 0);

	poly = &BackgroundPolys[5];
	setXYWH(poly, 512, 256, 128, 256);
	setUVWH(poly, 0, 0, 128, 255);
	setClut(poly, 960, 256);
	setTPage(poly, 0, 0, 960, 0);
}

SPRT HighlightSprt;
POLY_FT4 HighlightDummy;

// [D] [T]
void SetupScreenSprts(PSXSCREEN *pScr)
{
	setSprt(&HighlightSprt);
	setRGB0(&HighlightSprt, 128, 128, 128);
	setXY0(&HighlightSprt, 364, 198);
	setUV0(&HighlightSprt, 0, 0);
	setWH(&HighlightSprt, 256, 36);
	setClut(&HighlightSprt, 960, 258);

	setPolyFT4(&HighlightDummy);
	setXY3(&HighlightDummy, -1, -1, -1, -1, -1, -1);
	setTPage(&HighlightDummy, 0, 0, 704, 256);

	pNewScreen = NULL;
	pCurrScreen = pScr;

	if (pScr->userFunctionNum == 0) 
	{
		if (pNewButton != NULL) 
		{
			pCurrButton = pNewButton;
			pNewButton = NULL;
		}
		else 
			pCurrButton = pScr->buttons;
	}
	else
	{
		if ((fpUserFunctions[pScr->userFunctionNum - 1])(1) == 0)
		{
			if (pNewButton != NULL) 
			{
				pCurrButton = pNewButton;
				pNewButton = NULL;
			}
			else
				pCurrButton = pCurrScreen->buttons;
		}
	}
}

DR_MOVE In;
DR_MOVE Out;
RECT16 storeRect = { 768, 475, 255, 36 };

// [D] [T]
void DrawScreen(PSXSCREEN *pScr)
{
	char version_info[32];
	int numBtnsToDraw;
	int i;
#ifndef PSX
	if (bRedrawFrontend)
	{
		// flush the old screen
		//EndFrame();

		bRedrawFrontend = 0;
	}
#endif
	for (i = 0; i < 6; i++)
		addPrim(current->ot + 11, &BackgroundPolys[i]);

	if (pScr != NULL)
	{
		GetTimeStamp(version_info);

		if (bDoingCutSelect && (gFurthestMission < 39)) 
		{
			// hide the Director's Cut button
			numBtnsToDraw = pScr->numButtons - 1;
		}
		else 
		{
			numBtnsToDraw = pScr->numButtons;
		}
#ifndef PSX
		NewSelection(0);
#endif
		for (i = 0; i < numBtnsToDraw; i++)
		{
			PSXBUTTON *button = &pScr->buttons[i];
			int status = button->action >> 8;

#ifdef PSX
			if (status != 5)
			{
				if (button == pCurrButton)
				{
					if (status == 3)
					{
						FEPrintString(button->Name, button->x * 2 + button->w, button->y, 4, 32, 32, 32);
					}
					else
					{
						if (bMissionSelect && (i == 0 || i == 5) ||
							bDoingCarSelect && (i == 0 || i == 2) ||
							bInCutSelect && (i == 0 || i == 2))
						{
							FEPrintString(button->Name, button->x * 2 + button->w, button->y, 4, 124, 108, 40);
						}
						else
						{
							FEPrintString(button->Name, button->x * 2 + button->w, button->y, 4, 128, 128, 128);
						}
					}
				}
				else
				{
					if (status == 3)
					{
						FEPrintString(button->Name, button->x * 2 + button->w, button->y, 4, 32, 32, 32);
					}
					else
					{
						if (bMissionSelect && (i == 0 || i == 5) ||
							bDoingCarSelect && (i == 0 || i == 2) ||
							bInCutSelect && (i == 0 || i == 2))
						{
							FEPrintString(button->Name, button->x * 2 + button->w, button->y, 4, 124, 108, 40);
						}
						else
						{
							FEPrintString(button->Name, button->x * 2 + button->w, button->y, 4, 128, 128, 128);
						}
					}
				}
			}
#else
			int draw = (status != 5);

			if (button == pCurrButton)
			{
				RECT16 rect;

				rect.x = pCurrButton->s_x;
				rect.y = pCurrButton->s_y;
				rect.w = 255;
				rect.h = 36;

				SetDrawMove(&Out, &rect, storeRect.x, storeRect.y);
				addPrim(current->ot + 8, &Out);

				setXY0(&HighlightSprt, rect.x, rect.y);

				addPrim(current->ot + 6, &HighlightSprt);
				addPrim(current->ot + 7, &HighlightDummy);

				draw = 1;
			}

			if (draw)
			{
				if (status == 3)
				{
					FEPrintString(button->Name, button->x * 2 + button->w, button->y, 4, 32, 32, 32);
				}
				else
				{
					if (bMissionSelect && (i == 0 || i == 5) ||
						bDoingCarSelect && (i == 0 || i == 2) ||
						bInCutSelect && (i == 0 || i == 2))
					{
						FEPrintString(button->Name, button->x * 2 + button->w, button->y, 4, 124, 108, 40);
					}
					else
					{
						FEPrintString(button->Name, button->x * 2 + button->w, button->y, 4, 128, 128, 128);
					}
				}
			}
#endif
		}

#ifndef PSX
		if(bQuitToSystem)
		{
			FEPrintString(G_LTXT(GTXT_QuitToSystem), 220, 256, 0, 128, 128, 128);

			FEPrintString(G_LTXT(GTXT_YES), 240, 288, 0, 128, 128, bQuitToSystemSel ? 0 : 128);
			FEPrintString(G_LTXT(GTXT_NO), 340, 288, 0, 128, 128, bQuitToSystemSel ? 128 : 0);
		}
#endif

#if defined(_DEBUG) || defined(DEBUG_OPTIONS)
		FEPrintString(version_info, 40, 16, 0, 128, 128, 0);
		FEPrintString("--- " GAME_VERSION " ---", 320, 16, 0, 128, 128, 0);
#endif

		SetTextColour(128, 0, 0);

		DisplayOnScreenText();
		
		if (bDrawExtra) 
		{
			addPrim(&current->ot[2], &extraSprt);
			addPrim(&current->ot[3], &extraDummy);
		}
	}
#ifdef PSX
	else 
	{
		EndFrame();
	}
#else
	EndFrame();
#endif
}

char cutUnlock[] = { 
	2, 2, 2, 4, 5, 
	6, 7, 7, 7, 9,
	11, 11, 11, 11, 12,
	12, 13, 13, 14, 15,
	17, 17, 17, 17, 18,
	18, 19, 19, 19, 19,
	21, 21, 21, 21, 22,
	22, 22, 23, 23, 23,
	28 };

char* NullStr = "\0";

// [D] [T]
void DisplayOnScreenText(void)
{
	char *text;

	sprintf(ScreenTitle, NullStr);

	if (padsConnected[0] == 0) {
		int transparent = 0;

		if (Pads[0].type == 0) {
			text = "Please insert controller into Port 1";
			transparent = 1;
		}
		else {
			if (Pads[0].type != 1)
				return;

			text = "Incompatible controller in Port 1";
		}

		FEPrintStringSized(text, 40, 400, 0xc00, transparent, 64, 64, 64);
	}
	else 
	{
		if (!bDoingScores && !bDoingCarSelect) 
		{
			for (int i = 0; i < ScreenDepth; i++)
			{
				if (i > 0)
					strcat(ScreenTitle, " - ");

				strcat(ScreenTitle, ScreenNames[i]);
			}

			FEPrintStringSized(ScreenTitle, 40, 400, 0xc00, 1, 64, 64, 64);
		}

		if (bInCutSelect) {
			text = GET_MISSION_TXT(CutSceneNames[cutSelection + CutAmountsTotal[currCity]]);

			FEPrintStringSized(text, 100, 226, 0xc00, 1, 64, 64, 64);
		}
	}
}

// [D] [T]
void SetupExtraPoly(char *fileName, int offset, int offset2)
{
	int i;
	RECT16 rect = extraRect;

	FEDrawCDicon();
	Loadfile(fileName, _frontend_buffer + offset2);

	rect.x = 896;
	rect.y = 256;
	rect.w = 64;
	rect.h = 219;
	
	LoadImage(&rect, (u_long *)(_frontend_buffer + offset2 + offset * 0x8000));
	
	DrawSync(0);
	VSync(0);

	if(bDrawExtra == 0)
	{
		setSprt(&extraSprt);
		setPolyFT3(&extraDummy);

		setXY0(&extraSprt, 100, 226);
		setRGB0(&extraSprt, 128, 128, 128);
		setUV0(&extraSprt, 0, 0);
		setWH(&extraSprt, 255, 219);
		setClut(&extraSprt, 960, 256);

		setXY3(&extraDummy, -1, -1, -1, -1, -1, -1);
		setTPage(&extraDummy, 0, 0, 896, 256);
	}

	bDrawExtra = 1;

	if (strcmp(fileName, "DATA\\CITY.RAW") == 0) 
	{
		loaded[0] = 1;
		loaded[1] = -1;
	}

	for (i = 0; i < 4; i++)
	{
		if (strcmp(fileName, gfxNames[i]) == 0)
		{
			loaded[0] = -1;
			loaded[1] = i;
			loaded[2] = -1;
		}
	}

	for (i = 0; i < 4; i++)
	{
		if (strcmp(fileName, cutGfxNames[i]) == 0) 
		{
			loaded[1] = -1;
			loaded[2] = i;
		}
	}
}


// [D] [T]
void ReInitScreens(void)
{
#ifndef PSX
	bCdIconSetup = 0;
	bDoingScores = 0;
	bInCutSelect = 0;
	cutSelection = 0;
	currCity = 0;

	bRedrawFrontend = 0;
	bReturnToMain = 1;

	idle_timer = 0;
	currPlayer = 1;
	fePad = 0;
	ScreenDepth = 0;

	loaded[0] = -1;
	loaded[1] = -1;
	loaded[2] = -1;
	padsConnected[0] = 1;
	padsConnected[1] = 0;
	bMissionSelect = 0;
	bDoingCarSelect = 0;
	bDoingCutSelect = 0;
	bDrawExtra = 0;
	mainScreenLoaded = 1;
	bDoneAllready = 0;
	allowVibration = 1;

	ScreenNames[0] = 0;
	gInFrontend = 1;
	currMission = -1;
	missionSetup = 0;

	extern void SwitchMappings(int menu);
	SwitchMappings(1);
#endif // !PSX

	if (!bCdIconSetup)
		FEInitCdIcon();

	if (bReturnToMain)
	{
		ScreenDepth = 0;
		NumPlayers = 1;
		gWantNight = 0;
		gNight = 0;
		gTimeOfDay = 1;
		gSubGameNumber = 0;

		feVariableSave[3] = -1;
		feVariableSave[2] = -1;
		feVariableSave[1] = -1;
		feVariableSave[0] = -1;

		if (!mainScreenLoaded)
		{
			bDrawExtra = 0;
			LoadBackgroundFile("DATA\\GFX.RAW");
		}
	}
	else 
	{
		bReturnToMain = 1;
	}

	pCurrScreen = pScreenStack[ScreenDepth];
	pNewButton = pButtonStack[ScreenDepth];

	SetupScreenSprts(pCurrScreen);
	SetupBackgroundPolys();

	idle_timer = VSync(0xffffffff);
}

// [D] [T]
void NewSelection(short dir)
{
	PSXBUTTON *pNewB;
	RECT16 rect;

	if (pCurrScreen->numButtons == 0)
		return;

	pNewB = pCurrButton;
	int btn = 0;

	// any buttons pressed?
	if (dir != 0)
	{
		SetDrawMove(&In, &storeRect, pCurrButton->s_x, pCurrButton->s_y);
		addPrim(current->ot + 9, &In);

		if ((dir & 0x1000) != 0)
		{
			btn = pCurrButton->u;
		}
		else if ((dir & 0x4000) != 0)
		{
			btn = pCurrButton->d;
		}
		else if ((dir & 0x8000) != 0)
		{
			btn = pCurrButton->l;
		}
		else if ((dir & 0x2000) != 0)
		{
			btn = pCurrButton->r;
		}

		if (btn != 0)
		{
			FESound(3);
			pNewB = &pCurrScreen->buttons[btn - 1];
		}
	}

	pCurrButton = pNewB;

#ifdef PSX
	rect.x = pCurrButton->s_x;
	rect.y = pCurrButton->s_y;
	rect.w = 255;
	rect.h = 36;

	SetDrawMove(&Out, &rect, storeRect.x, storeRect.y);
	addPrim(current->ot + 8, &Out);

	setXY0(&HighlightSprt, rect.x, rect.y);

	addPrim(current->ot + 6, &HighlightSprt);
	addPrim(current->ot + 7, &HighlightDummy);

	if ((pNewB->action >> 8) == 3) {
		FEPrintString(pNewB->Name, pNewB->x * 2 + pNewB->w, pNewB->y, 4, 32, 32, 32);
	}
	else {
		if ((bMissionSelect && ((pNewB == &pCurrScreen->buttons[0]) || (pNewB == &pCurrScreen->buttons[5]))) ||
			((bDoingCarSelect && ((pNewB == &pCurrScreen->buttons[0]) || (pNewB == &pCurrScreen->buttons[2])))) ||
			((bInCutSelect && ((pNewB == &pCurrScreen->buttons[0]) || (pNewB == &pCurrScreen->buttons[2])))))
		{
			FEPrintString(pNewB->Name, pNewB->x * 2 + pNewB->w, pNewB->y, 4, 124, 108, 40);
		}
		else {
			FEPrintString(pNewB->Name, pNewB->x * 2 + pNewB->w, pNewB->y, 4, 128, 128, 128);
		}
	}
#endif

#ifdef PSX
	EndFrame();
#endif
}

// [D] [T]
int HandleKeyPress(void)
{
	if (pCurrScreen == NULL || pCurrButton == NULL)
		return 0;

	if (pCurrScreen->userFunctionNum != 0)
	{
		// notify the user function first
		if ((fpUserFunctions[pCurrScreen->userFunctionNum - 1])(0) != 0)
		{
			// user function handled the key press
			fePad = 0;
		}
	}

	if (fePad & 0x40)
	{
		int action = pCurrButton->action >> 8;

		if (action != 3)
		{
			FESound(2);

			if (pCurrButton->var != -1)
				SetVariable(pCurrButton->var);

			switch (action)
			{
			case 1:
				pScreenStack[ScreenDepth] = pCurrScreen;
				pButtonStack[ScreenDepth] = pCurrButton;

				ScreenNames[ScreenDepth] = pCurrButton->Name;

				pNewScreen = &PsxScreens[pCurrButton->action & 0xFF];

				if (ScreenDepth < 10)
					ScreenDepth++;

				break;
			case 2:
				if ((NumPlayers == 2) && (bDoingCarSelect != 0) && (currPlayer == 2))
				{
					(fpUserFunctions[pCurrScreen->userFunctionNum - 1])(1);
					bRedrawFrontend = 1;
				}
				else
				{
					pScreenStack[ScreenDepth] = pCurrScreen;
					pButtonStack[ScreenDepth] = pCurrButton;

					ScreenNames[ScreenDepth] = pCurrButton->Name;

					GameStart();
				}
				break;
			case 4:
				if (ScreenDepth > 0)
				{
					if (--ScreenDepth == 0)
						NumPlayers = 1;

					pNewScreen = pScreenStack[ScreenDepth];
					pNewButton = pButtonStack[ScreenDepth];
				}
				break;
			}
		}
	}
	else if (fePad & 0x10)
	{
		if (ScreenDepth > 0)
		{
			if (!bDoneAllready)
				FESound(0);
			else 
				bDoneAllready = 0;

			if (--ScreenDepth == 0)
			{
				gWantNight = 0;
				gSubGameNumber = 0;
				NumPlayers = 1;
			}

			pNewScreen = pScreenStack[ScreenDepth];
			pNewButton = pButtonStack[ScreenDepth];
		}
	}
	else
	{
		// any d-pad buttons pressed?
		if ((fePad & (0x8000 | 0x4000 | 0x2000 | 0x1000)) != 0)
		{
			NewSelection(fePad);
		}
		else
		{
			// button pressed, but we didn't handle it
			return 1;
		}
	}

	idle_timer = VSync(-1);

	return 1;
}


// [D] [T]
void PadChecks(void)
{
	int i;
	// TODO: null check for pCurrScreen

	int oldnum = numPadsConnected;

	ReadControllers();
	
	numPadsConnected = 0;
	fePad = Pads[0].mapnew;

	for (int i = 0; i < 2; i++) 
	{
		if (Pads[i].type < 2) 
		{
			padsConnected[i] = 0;
		}
		else 
		{
			padsConnected[i] = 1;
			numPadsConnected++;
		}
	}

#ifndef PSX
	// [A] quit to system
	if(fePad & 0x10)
	{
		if(ScreenDepth == 0)
		{
			bQuitToSystem ^= 1;
			FESound(0);
		}
	}

	if(bQuitToSystem)
	{
		if (fePad & 0x40)
		{
			if(bQuitToSystemSel == 1)
				bQuitToSystem = 2;
			else
				bQuitToSystem = 0;

			FESound(2);
		}
		else if ((fePad & 0x8000) || (fePad & 0x2000))
		{
			bQuitToSystemSel += 1;
			bQuitToSystemSel &= 1;

			FESound(3);
		}

		fePad = 0;
	}
#endif

	if (oldnum != numPadsConnected && (oldnum == 2 || numPadsConnected == 2) && ScreenDepth == 0) 
	{
		bRedrawFrontend = 1;
		MainScreen(1);
	}

	if ((Pads[0].dualshock == 0 || padsConnected[0] == 0) &&
		(Pads[1].dualshock == 0 || padsConnected[1] == 0)) 
	{
		if(allowVibration == 1)
		{
			allowVibration = 0;

			if (pCurrScreen->userFunctionNum == 18)
			{
				bRedrawFrontend = 1;
				GamePlayScreen(1);
			}
		}
	}
	else 
	{
		if (allowVibration == 0)
		{
			allowVibration = 1;

			if (pCurrScreen->userFunctionNum == 18)
			{
				bRedrawFrontend = 1;
				GamePlayScreen(1);
			}
		}
	}

	if (oldnum != numPadsConnected && NumPlayers == 2 && numPadsConnected != NumPlayers || 
		numPadsConnected == 0 || 
		padsConnected[0] == 0) 
	{
		bReturnToMain = 1;
		bRedrawFrontend = 1;
		fePad = 0x10;

		if (pCurrScreen->userFunctionNum != 0) 
		{
			(fpUserFunctions[pCurrScreen->userFunctionNum - 1])(0);
		}

		fePad = 0;

		if (ScreenDepth != 0) 
		{
			ReInitScreens();
		}
	}
	if (bRedrawFrontend == 0 && numPadsConnected != oldnum &&
		(gInFrontend != 0 && (pCurrScreen != NULL && pCurrScreen->userFunctionNum != 0)))
	{
		(fpUserFunctions[pCurrScreen->userFunctionNum - 1])(1);
		bRedrawFrontend = 1;
	}
}

// [A] - was inlined in DoFrontEnd
void InitFrontend(void)
{
	FEInitCdIcon();

	ResetGraph(1);
	SetDispMask(0);

	bRedrawFrontend = 0;
	gInFrontend = 1;

	idle_timer = VSync(-1);

	LoadFrontendScreens();

	SetupBackgroundPolys();
	SetupScreenSprts(&PsxScreens[0]);
}

// [A] - was inlined in DoFrontEnd
void InitDisplay(void)
{
	SetDispMask(0);
	ResetGraph(0);

	SetFEDrawMode();

	EnableDisplay();

#ifdef PSX
	DrawScreen(pCurrScreen);
	EndFrame();

	NewSelection(0);

	// REALLY make sure the screen is cleared
	EndFrame();
	EndFrame();
	EndFrame();
	EndFrame();
	EndFrame();
	EndFrame();
#endif

	SetDispMask(1);
}

// [D] [T]
void DoFrontEnd(void)
{
	InitFrontend();
	InitDisplay();

	do
	{
		PadChecks();

		if (currPlayer == 2) {
			if (Pads[1].type < 2) {
				fePad = ((fePad & 0x10) != 0) ? 0x10 : 0;
			}
			else {
				fePad = Pads[1].mapnew;
			}
		}

		if (HandleKeyPress())
		{
			if (pNewScreen != NULL)
			{
				SetupScreenSprts(pNewScreen);
				bRedrawFrontend = 1;
			}
		}

#ifndef PSX
		DrawScreen(pCurrScreen);
#else
		if (bRedrawFrontend)
		{
			DrawScreen(pCurrScreen);
			EndFrame();

			NewSelection(0);

			bRedrawFrontend = 0;
		}
#endif

		if ((VSync(-1) - idle_timer) > 1800)
		{
			if (ScreenDepth == 0)
			{
				GameType = GAME_IDLEDEMO;

				gCurrentMissionNumber = gIdleReplay + 400;
				
				if (++gIdleReplay == 4)
					gIdleReplay = 0;

				pScreenStack[0] = pCurrScreen;
				pButtonStack[0] = pCurrButton;
				ScreenNames[0] = pCurrButton->Name;

				GameStart();

				pCurrScreen = pScreenStack[0];

				bRedrawFrontend = 1;
				ScreenDepth = 0; // fail-safe?
			}

			idle_timer = VSync(-1);
		}
	} while (bQuitToSystem != 2);
}

// [D] [T]
void SetFEDrawMode(void)
{
	SetVideoMode(video_mode);

	SetDefDrawEnv(&MPBuff[0][0].draw, draw_mode.x1, draw_mode.y1, 640, 512);
	SetDefDispEnv(&MPBuff[0][0].disp, draw_mode.x1, draw_mode.y1, 640, 512);
	SetDefDrawEnv(&MPBuff[0][1].draw, draw_mode.x1, draw_mode.y1, 640, 512);
	SetDefDispEnv(&MPBuff[0][1].disp, draw_mode.x1, draw_mode.y1, 640, 512);

	MPBuff[0][0].draw.isbg = 0;
	MPBuff[0][0].disp.isinter = 1;
	MPBuff[0][0].draw.dfe = 1;
	MPBuff[0][0].disp.screen.h = 256;
	MPBuff[0][0].primtab = _primTab1;
	MPBuff[0][0].primptr = _primTab1;
	MPBuff[0][0].ot = _OT1;

	MPBuff[0][1].draw.isbg = 0;
	MPBuff[0][1].disp.isinter = 1;
	MPBuff[0][1].draw.dfe = 1;
	MPBuff[0][1].disp.screen.h = 256;
	MPBuff[0][1].primtab = _primTab2;
	MPBuff[0][1].primptr = _primTab2;
	MPBuff[0][1].ot = _OT2;

	last = &MPBuff[0][1];
	current = &MPBuff[0][0];

	MPBuff[0][0].disp.screen.x = draw_mode.framex * 2;
	MPBuff[0][0].disp.screen.y = draw_mode.framey;
	MPBuff[0][1].disp.screen.y = draw_mode.framey;
	MPBuff[0][1].disp.screen.x = MPBuff[0][0].disp.screen.x;

	PutDispEnv(&MPBuff[0][0].disp);
	PutDrawEnv(&current->draw);
}

// [D] [T]
void EndFrame(void)
{
	DB *db_hold;

	DrawSync(0);
	VSync(0);

	PutDispEnv(&current->disp);
	PutDrawEnv(&current->draw);

	db_hold = current;
	current = last;

	last->primptr = last->primtab;
	last = db_hold;

	DrawOTag((u_long*)&db_hold->ot[16]);
	ClearOTagR((u_long*)current->ot, 16);
	VSync(0);

#ifndef PSX
	PsyX_EndScene();
#endif
}

// [D] [T]
int FEPrintString(char *string, int x, int y, int justification, int r, int g, int b)
{
	if (current == NULL || string == NULL)
		return -1;

	FE_CHARDATA *pFontInfo;
	SPRT *font;
	unsigned char let;

	font = (SPRT *)current->primptr;

	if (justification & 4)
	{
		char *pString = string;
		unsigned char c = 0;

		int w = 0;

		while ((c = *pString++) != 0)
		{
			if (c == ' ')
				w += 4;
			else
				w += feFont.CharInfo[c].w;
		}

		x -= w;
	}

	int counter = 0;

	while ((let = *string++) != 0)
	{
		if (let == ' ')
		{
			x += 4;
		}
		else
		{
			pFontInfo = &feFont.CharInfo[let];

			setSprt(font);
#ifdef PSX
			setSemiTrans(font, 1);
#endif

			setRGB0(font, r, g, b);
			setXY0(font, x, y);
			setUV0(font, pFontInfo->u, pFontInfo->v);
			setWH(font, pFontInfo->w, pFontInfo->h);
			// setTPage(font, 0, 0, 640, 256); // [A]
			setClut(font, 960, 257); // [A] seems clut has a transparency bit; width is 256

			addPrim(&current->ot[1], font);
			font++;

			// add space for next character
			x += pFontInfo->w;
		}

		if (++counter >= 32)
			break;
	}

	// set tail
	current->primptr = (char *)font;

	POLY_FT3 *null = (POLY_FT3 *)current->primptr;
	setPolyFT3(null);
	setSemiTrans(null, 1);
	setXY3(null, -1, -1, -1, -1, -1, -1);
	setTPage(null, 0, 0, 640, 256);

	addPrim(&current->ot[1], null);
	null++;

	current->primptr = (char *)null;

	return x;
}


// [D] [T]
int FEPrintStringSized(char *string, int x, int y, int scale, int transparent, int r, int g, int b)
{
	if (current == NULL || string == NULL)
		return -1;

	POLY_FT4 *font;
	FE_CHARDATA *pFontInfo;
	char let;
	int w;
	int h;

	font = (POLY_FT4 *)current->primptr;

	while ((let = *string++) != 0)
	{
		if (let == '\n')
			continue;

		if (let == ' ')
		{
			// add space
			x += 4;
		}
		else
		{
			pFontInfo = &feFont.CharInfo[let];

			w = (pFontInfo->w * scale) / 4096;
			h = (pFontInfo->h * scale) / 4096;

			setPolyFT4(font);
#ifdef PSX
				setSemiTrans(font, transparent);
#endif

			setRGB0(font, 128, 128, 128);
			setUVWH(font, pFontInfo->u, pFontInfo->v, pFontInfo->w - 1, pFontInfo->h - 1);
			setXYWH(font, x, y, w, h);
			setRGB0(font, r, g, b);
			setTPage(font, 0, 0, 640, 256);
			setClut(font, 960, 257); // [A] seems clut has a transparency bit; width is 256

			addPrim(&current->ot[1], font);
			font++;

			// make room for next character
			x += w;
		}
	}

	// set tail
	current->primptr = (char *)font;

	return x;
}


SCREEN_LIMITS screen_limits[2] = {
	// min(x/y), max(x/y)
	{ -27,   5, 35, 37 },
	{ -32, -11, 32, 21 },
};

int bScreenSetup = 0;

// [D] [T]
int CentreScreen(int bSetup)
{
	if (bSetup)
		return 0;

#if defined(DEBUG) && !defined(PSX)
	char text[32];

	sprintf(text, "X1: %d, Y1: %d", current->disp.screen.x, current->disp.screen.y);
	FEPrintStringSized(text, 25, 50, 0xC00, 0, 128, 0, 0);

	sprintf(text, "X2: %d, Y2: %d", last->disp.screen.x, last->disp.screen.y);
	FEPrintStringSized(text, 25, 75, 0xC00, 0, 128, 0, 0);
#endif

	if (fePad & 0x40)
	{
		draw_mode_pal.framex = current->disp.screen.x / 2;
		draw_mode_pal.framey = current->disp.screen.y;
		draw_mode_ntsc.framex = current->disp.screen.x / 2;
		draw_mode_ntsc.framey = current->disp.screen.y;
	}
	else if (fePad & 0x10)
	{
		current->disp.screen.x = draw_mode.framex * 2;
		current->disp.screen.y = draw_mode.framey;
		last->disp.screen.x = draw_mode.framex * 2;
		last->disp.screen.y = draw_mode.framey;
	}
	else
	{
		bool done = false;

		if (fePad & 0x1000)
		{
			if (current->disp.screen.y > screen_limits[0].miny)
			{
				current->disp.screen.y--;
				last->disp.screen.y--;
				done = true;
			}
		}
		else if (fePad & 0x4000)
		{
			if (current->disp.screen.y < screen_limits[0].maxy)
			{
				current->disp.screen.y++;
				last->disp.screen.y++;
				done = true;
			}
		}
		else if (fePad & 0x8000)
		{
			if (current->disp.screen.x > screen_limits[0].minx) 
			{
				current->disp.screen.x--;
				last->disp.screen.x--;
				done = true;
			}
		}
		else if (fePad & 0x2000)
		{
			if (current->disp.screen.x < screen_limits[0].maxx)
			{
				current->disp.screen.x++;
				last->disp.screen.x++;
				done = true;
			}
		}
		else
		{
			// unhandled button press
			return 0;
		}

		FESound((done) ? 3 : 1);
	}

	return 0;
}


// [D] [T]
int CarSelectScreen(int bSetup)
{
	int newSel;
	RECT16 rect;

	newSel = carSelection;
	rect = extraRect;

	if (bSetup)
	{
		bDoingCarSelect = 1;

		// setup secret cars
		if (NumPlayers == 1)
		{
#if defined(DEBUG_OPTIONS) || defined(_DEBUG)
			CarAvailability[0][9] = 1;
			CarAvailability[1][9] = 1;
			CarAvailability[2][9] = 1;
			CarAvailability[3][9] = 1;
#else
			CarAvailability[0][9] = AvailableCheats.cheat5;
			CarAvailability[1][9] = AvailableCheats.cheat6;
			CarAvailability[2][9] = AvailableCheats.cheat7;
			CarAvailability[3][9] = AvailableCheats.cheat8;
#endif
		}

		// setup unlockable cars
		if (gFurthestMission == 40 && NumPlayers == 1)
		{
			for (int i = 4; i < 9; i++)
			{
				if (i != 8)
					CarAvailability[0][i] = 1;	// remove truck

				CarAvailability[1][i] = 1;
				CarAvailability[2][i] = 1;
				CarAvailability[3][i] = 1;
			}
		}
		else
		{
			for (int i = 4; i < 9; i++)
			{
				// unlock the cop car only
				int unlocked = (i == 4) && gFurthestMission == 40;

				CarAvailability[0][i] = unlocked; 
				CarAvailability[1][i] = unlocked;
				CarAvailability[2][i] = unlocked;
				CarAvailability[3][i] = unlocked;
			}
		}

		if (currPlayer != 1) {
			if (NumPlayers == 2)
				FEPrintStringSized(G_LTXT(GTXT_Player2), 400, 260, 0xc00, 0, 128, 128, 128);

			return 0;
		}

		LoadBackgroundFile("DATA\\CARS\\CARBACK.RAW");

		if (feVariableSave[0] != -1)
		{
			carSelection = feVariableSave[0];
			SetupExtraPoly(gfxNames[GameLevel], carSelection, 0);
		}
		else
		{
			carSelection = 0;

			if (loaded[1] == GameLevel)
			{
				bDrawExtra = 1;

				LoadImage(&rect, (u_long*)_frontend_buffer);
				DrawSync(0);
			}
			else
			{
				SetupExtraPoly(gfxNames[GameLevel], carSelection, 0);
				lastCity = GameLevel;
			}
		}

		feVariableSave[0] = -1;
		feVariableSave[1] = -1;
		feVariableSave[2] = -1;
		feVariableSave[3] = -1;

		lastCutCity = -1;
		currSelIndex = 1;

		pCurrButton = &pCurrScreen->buttons[1];

		return 1;
	}

	if (fePad & 0x10)
	{
		FESound(0);
		bDoneAllready = 1;
#ifdef PSX
		LoadBackgroundFile("DATA\\GFX.RAW");
		bDrawExtra = 0;
#else
		LoadBackgroundFile("DATA\\CITYBACK.RAW");

		if (loaded[0] == -1)
		{
			SetupExtraPoly("DATA\\CITY.RAW", currCity, 0);
		}
		else
		{
			bDrawExtra = 1;

			RECT16 rect = extraRect;
			LoadImage(&rect, (u_long*)(_frontend_buffer + currCity * 0x8000));
			DrawSync(0);
		}
#endif
		currPlayer = 1;
		bDoingCarSelect = 0;
	}

	else if (fePad & 0x40)
	{
		if (currSelIndex == 0)
		{
			// find best-fit for previous vehicle
			do
			{
				newSel--;
				if (newSel < 0)
					newSel = 9;

			} while (CarAvailability[GameLevel][newSel] == 0);

			carSelection = newSel;
		}
		else if (currSelIndex == 2)
		{
			// find best-fit for next vehicle
			do
			{
				newSel++;
				if (newSel > 9)
					newSel = 0;

			} while (CarAvailability[GameLevel][newSel] == 0);

			carSelection = newSel;
		}
		else
		{
			// select the vehicle
			if (currPlayer == 1)
			{
				feVariableSave[0] = carSelection;
				wantedCar[0] = carNumLookup[GameLevel][carSelection];
			}
			else {
				wantedCar[1] = carNumLookup[GameLevel][carSelection];
			}

			// time for player 2 to select their vehicle?
			if (NumPlayers == 2)
				currPlayer++;

			return 0;
		}
		
		rect = extraRect;
		LoadImage(&rect, (u_long *)(_frontend_buffer + carSelection * 0x8000));
		DrawSync(0);

#ifdef PSX
		DisplayOnScreenText();

		addPrim(&current->ot[2], &extraSprt);
		addPrim(&current->ot[3], &extraDummy);

		EndFrame();
#endif
	}
	else if (fePad & 0x1000)
	{
		currSelIndex = pCurrButton->u - 1;
	}
	else if (fePad & 0x4000)
	{
		currSelIndex = pCurrButton->d - 1;
	}

	return 0;
}

// [D] [T]
int CopDiffLevelScreen(int bSetup)
{
	if (bSetup) 
	{
		pCurrButton = &pCurrScreen->buttons[gCopDifficultyLevel];
		return 1;
	}

	if (fePad & 0x40)
	{
		gCopDifficultyLevel = currSelIndex;
	}
	else if (fePad & 0x1000)
	{
		currSelIndex = pCurrButton->u - 1;
	}
	else if (fePad & 0x4000)
	{
		currSelIndex = pCurrButton->d - 1;
	}

	return 0;
}

// [D] [T]
int VibroOnOffScreen(int bSetup)
{
	if (bSetup) 
	{
		currSelIndex = (gVibration ^ 1);
		pCurrButton = &pCurrScreen->buttons[currSelIndex];
		return 1;
	}

	if (fePad & 0x40)
	{
		gVibration = (currSelIndex ^ 1);
	}
	else if (fePad & 0x1000)
	{
		currSelIndex = pCurrButton->u - 1;
	}
	else if (fePad & 0x4000)
	{
		currSelIndex = pCurrButton->d - 1;
	}

	return 0;
}


// [D] [T]
int MissionSelectScreen(int bSetup)
{
	int i;

	if (bSetup) 
	{
		bMissionSelect = 1;

		if (!missionSetup)
		{
			currMission = minmaxSelections[currCity][0];
			currSelIndex = 0;

			if (GameType == GAME_REPLAYMISSION)
				LoadBackgroundFile("DATA\\CITYBACK.RAW");
		}

		if (feVariableSave[0] != -1) {
			currMission = feVariableSave[0];
			currSelIndex = feVariableSave[1];
			currCity = feVariableSave[2];
		}

		int usedB = 0;

		bool done = false;
		bool bP = false; // 'Previous' button is visible?
		bool bN = false; // 'Next' button is visible?

		for (i = 0; (i < 4) && !done; i++)
		{
			if (botch[currMission + i].missNum > gFurthestMission ||
				currMission + i > minmaxSelections[currCity][1] ||
				currMission + i > 36) 
			{
				done = true;
			}
			else
			{
				usedB++;
			}
		}

		switch (usedB)
		{
		case 1:
			pCurrScreen->buttons[1].u = 2;
			pCurrScreen->buttons[1].d = 2;
			sprintf(pCurrScreen->buttons[1].Name, GET_MISSION_TXT(MissionName[currMission]));
			sprintf(pCurrScreen->buttons[2].Name, NullStr);
			sprintf(pCurrScreen->buttons[3].Name, NullStr);
			sprintf(pCurrScreen->buttons[4].Name, NullStr);
			break;
		case 2:
			pCurrScreen->buttons[1].u = 3;
			pCurrScreen->buttons[1].d = 3;
			pCurrScreen->buttons[2].u = 2;
			pCurrScreen->buttons[2].d = 2;
			sprintf(pCurrScreen->buttons[1].Name, GET_MISSION_TXT(MissionName[currMission]));
			sprintf(pCurrScreen->buttons[2].Name, GET_MISSION_TXT(MissionName[currMission + 1]));
			sprintf(pCurrScreen->buttons[3].Name, NullStr);
			sprintf(pCurrScreen->buttons[4].Name, NullStr);
			break;
		case 3:
			pCurrScreen->buttons[1].u = 4;
			pCurrScreen->buttons[1].d = 3;
			pCurrScreen->buttons[2].u = 2;
			pCurrScreen->buttons[2].d = 4;
			pCurrScreen->buttons[3].u = 3;
			pCurrScreen->buttons[3].d = 2;
			sprintf(pCurrScreen->buttons[1].Name, GET_MISSION_TXT(MissionName[currMission]));
			sprintf(pCurrScreen->buttons[2].Name, GET_MISSION_TXT(MissionName[currMission + 1]));
			sprintf(pCurrScreen->buttons[3].Name, GET_MISSION_TXT(MissionName[currMission + 2]));
			sprintf(pCurrScreen->buttons[4].Name, NullStr);
			break;
		case 4:
			pCurrScreen->buttons[1].u = 5;
			pCurrScreen->buttons[1].d = 3;
			pCurrScreen->buttons[2].u = 2;
			pCurrScreen->buttons[2].d = 4;
			pCurrScreen->buttons[3].u = 3;
			pCurrScreen->buttons[3].d = 5;
			pCurrScreen->buttons[4].u = 4;
			pCurrScreen->buttons[4].d = 2;
			sprintf(pCurrScreen->buttons[1].Name, GET_MISSION_TXT(MissionName[currMission]));
			sprintf(pCurrScreen->buttons[2].Name, GET_MISSION_TXT(MissionName[currMission + 1]));
			sprintf(pCurrScreen->buttons[3].Name, GET_MISSION_TXT(MissionName[currMission + 2]));
			sprintf(pCurrScreen->buttons[4].Name, GET_MISSION_TXT(MissionName[currMission + 3]));
			break;
		}

		if (usedB == 4 &&
			botch[currMission + 4].missNum <= gFurthestMission &&
			currMission + 4 != minmaxSelections[currCity][1])
		{
			bN = true;
		}

		if (bN)
		{
			if (currMission != minmaxSelections[currCity][0])
			{
				bP = true;

				pCurrScreen->buttons[0].u = 6;
				pCurrScreen->buttons[0].d = 2;
				pCurrScreen->buttons[5].u = 5;
				pCurrScreen->buttons[5].d = 1;
				pCurrScreen->buttons[1].u = 1;
				pCurrScreen->buttons[4].d = 6;
				pCurrScreen->buttons[0].action = 0;
				pCurrScreen->buttons[5].action = 0;
			}
			else
			{
				pCurrScreen->buttons[1].u = 6;
				pCurrScreen->buttons[4].d = 6;
				pCurrScreen->buttons[5].u = 5;
				pCurrScreen->buttons[5].d = 2;
				pCurrScreen->buttons[5].action = 0;
				pCurrScreen->buttons[0].action = 0x500;
			}
		}
		else
		{
			if (currMission != minmaxSelections[currCity][0])
			{
				bP = true;

				pCurrScreen->buttons[0].u = usedB + 1;
				pCurrScreen->buttons[0].d = 2;
				pCurrScreen->buttons[1].u = 1;
				pCurrScreen->buttons[usedB].d = 1;
				pCurrScreen->buttons[0].action = 0;
				pCurrScreen->buttons[5].action = 0x500;
			}
			else
			{
				// list missions only
				pCurrScreen->buttons[0].action = 0x500;
				pCurrScreen->buttons[5].action = 0x500;
			}
		}

		if (bP)
		{
			pCurrButton = pCurrScreen->buttons;
			currSelIndex = 0;
		}
		else
		{
			pCurrButton = pCurrScreen->buttons + 1;
			currSelIndex = 1;
		}

		if (loaded[0] == -1)
		{
			SetupExtraPoly("DATA\\CITY.RAW", currCity, 0);
		}
		else
		{
			bDrawExtra = 1;
		}

		feVariableSave[0] = -1;
		feVariableSave[1] = -1;
		feVariableSave[2] = -1;
		feVariableSave[3] = -1;

		missionSetup = 1;

		return 1;
	}

	if (fePad & 0x40)
	{
		i = currMission;

		if (currSelIndex == 0)
		{
			i -= 4;

			if (i < minmaxSelections[currCity][0])
				return 0;
		}
		else if (currSelIndex == 5)
		{
			i += 4;

			if (i > minmaxSelections[currCity][1] || i > gFurthestMission)
				return 0;
		}
		else
		{
			bReturnToMain = 0;
			GameType = GAME_REPLAYMISSION;
			feVariableSave[0] = currMission;
			feVariableSave[1] = currSelIndex;
			feVariableSave[2] = currCity;
			gCurrentMissionNumber = botch[currMission + (currSelIndex - 1)].missNum;

			return 0;
		}

		currMission = i;
		FESound(3);
		fpUserFunctions[pCurrScreen->userFunctionNum - 1](1);
		bRedrawFrontend = 1;

		return 1;
	}
	else if (fePad & 0x10)
	{
		missionSetup = 0;
		bMissionSelect = 0;
	}
	else if (fePad & 0x1000)
	{
		currSelIndex = pCurrButton->u - 1;
	}
	else if (fePad & 0x4000)
	{
		currSelIndex = pCurrButton->d - 1;
	}

	return 0;
}

// [D] [T]
int MissionCityScreen(int bSetup)
{
	RECT16 rect;

	if (bSetup)
	{
		GameType = GAME_MISSION;

		if (gFurthestMission == 0)
		{
			pCurrScreen->buttons[0].action = 0x300;
			pCurrScreen->buttons[1].action = 0x300;
			pCurrScreen->buttons[2].action = 0x300;
			pCurrScreen->buttons[3].action = 0x300;
		}
		else if (gFurthestMission < 10)
		{
			pCurrScreen->buttons[0].action = 0x113;
			pCurrScreen->buttons[1].action = 0x300;
			pCurrScreen->buttons[2].action = 0x300;
			pCurrScreen->buttons[3].action = 0x300;
		}
		else if (gFurthestMission < 21)
		{
			pCurrScreen->buttons[0].action = 0x113;
			pCurrScreen->buttons[1].action = 0x113;
			pCurrScreen->buttons[2].action = 0x300;
			pCurrScreen->buttons[3].action = 0x300;
		}
		else if (gFurthestMission < 31)
		{
			pCurrScreen->buttons[0].action = 0x113;
			pCurrScreen->buttons[1].action = 0x113;
			pCurrScreen->buttons[2].action = 0x113;
			pCurrScreen->buttons[3].action = 0x300;
		}
		else
		{
			pCurrScreen->buttons[0].action = 0x113;
			pCurrScreen->buttons[1].action = 0x113;
			pCurrScreen->buttons[2].action = 0x113;
			pCurrScreen->buttons[3].action = 0x113;
		}

		LoadBackgroundFile("DATA\\CITYBACK.RAW");

		if (loaded[0] == -1)
		{
			SetupExtraPoly("DATA\\CITY.RAW", 0, 0);
		}
		else
		{
			bDrawExtra = 1;
		}

		return 0;
	}

	if (fePad & 0x10)
	{
		// BUGFIX: unload city image
		loaded[0] = -1;

		bDrawExtra = 0;
		FESound(0);
		bDoneAllready = 1;
		LoadBackgroundFile("DATA\\GFX.RAW");
	}
	else
	{
		if (fePad & 0x1000)
		{
			currCity = pCurrButton->u - 1;
		}
		else if (fePad & 0x4000)
		{
			currCity = pCurrButton->d - 1;
		}
		else
		{
			currCity = pCurrButton->u & 3;
			return 0;
		}

		rect = extraRect;
		LoadImage(&rect, (u_long *)(_frontend_buffer + currCity * 0x8000));
		DrawSync(0);

#ifdef PSX
		DisplayOnScreenText();

		addPrim(&current->ot[2], &extraSprt);
		addPrim(&current->ot[3], &extraDummy);

		EndFrame();
#endif
	}
	
	return 0;
}



// [D] [T]
int CutSceneSelectScreen(int bSetup)
{
	RECT16 rect;

	rect = extraRect;

	if (bSetup)
	{
		LoadBackgroundFile("DATA\\CUTS\\CUTBACK.RAW");
		currCity = feVariableSave[1];

		if (feVariableSave[0] == -1)
		{
			cutSelection = 0;
			currCity = GameLevel;
		}
		else
		{
			cutSelection = feVariableSave[0];
			feVariableSave[3] = -1;
			feVariableSave[2] = -1;
			feVariableSave[1] = -1;
			feVariableSave[0] = -1;
		}

		if (loaded[2] == GameLevel)
		{
			bDrawExtra = 1;
			LoadImage(&rect, (u_long*)_frontend_buffer + 0x20000);
			DrawSync(0);
		}
		else
		{
			SetupExtraPoly(cutGfxNames[GameLevel], cutSelection, 0x20000);
		}


		bInCutSelect = 1;
		currSelIndex = 1;
		bDoingCutSelect = 1;
		pCurrButton = pCurrScreen->buttons + 1;

		return 1;
	}

	if (fePad & 0x10) 
	{
		bDrawExtra = 0;
		bInCutSelect = 0;
		bDoingCutSelect = 0;
	}
	else if (fePad & 0x40)
	{
		if (currSelIndex == 0)
		{
			if (cutSelection == 0)
			{
				cutSelection = CutAmounts[currCity + 1] - 1;

				if (cutUnlock[gFurthestMission] < CutAmounts[currCity + 1] - 1 + CutAmountsTotal[currCity] + 1)
					cutSelection = cutUnlock[gFurthestMission];
			}
			else
			{
				cutSelection--;
			}
		}
		else
		{
			if (currSelIndex != 2)
			{
				bReturnToMain = 0;
				pCurrButton = pButtonStack[ScreenDepth];
				pScreenStack[ScreenDepth] = pCurrScreen;

				feVariableSave[1] = currCity;
				feVariableSave[0] = cutSelection;

				ScreenNames[ScreenDepth] = pCurrButton->Name;
				StartRender(feVariableSave[0] + CutAmountsTotal[feVariableSave[1]] + 1);

				return 0;
			}

			if (cutSelection == CutAmounts[currCity + 1] - 1 || cutUnlock[gFurthestMission] <= cutSelection + CutAmountsTotal[currCity] + 1)
			{
				cutSelection = 0;
			}
			else
			{
				cutSelection++;
			}
		}

		rect = extraRect;
		LoadImage(&rect, (u_long*)(_frontend_buffer + 0x20000 + cutSelection * 0x8000));
		DrawSync(0);
		DisplayOnScreenText();

#ifdef PSX
		DisplayOnScreenText();

		addPrim(&current->ot[2], &extraSprt);
		addPrim(&current->ot[3], &extraDummy);

		EndFrame();
#endif

		return 0;
	}
	else if (fePad & 0x1000)
	{
		currSelIndex = pCurrButton->u - 1;
	}
	else if (fePad & 0x4000)
	{
		currSelIndex = pCurrButton->d - 1;
	}

	return 0;
}



// [D] [T]
int CutSceneCitySelectScreen(int bSetup)
{
	RECT16 rect;

	rect = extraRect;

	if (bSetup)
	{
		LoadBackgroundFile("DATA\\CITYBACK.RAW");

		if (feVariableSave[0] == -1)
		{
			if (bDoingCutSelect == 0) {
				currCity = 0;
				bDoingCutSelect = 1;
			}
		}
		else
		{
			currCity = feVariableSave[0];

			feVariableSave[3] = -1;
			feVariableSave[2] = -1;
			feVariableSave[1] = -1;
			feVariableSave[0] = -1;
		}

		pCurrScreen->buttons[0].u = 4;
		pCurrScreen->buttons[3].d = 1;

		if (gFurthestMission == 0) 
		{
			pCurrScreen->buttons[0].action = 0x300;
			pCurrScreen->buttons[1].action = 0x300;
			pCurrScreen->buttons[2].action = 0x300;
			pCurrScreen->buttons[3].action = 0x300;
		}
		else if (gFurthestMission < 10)
		{
			pCurrScreen->buttons[0].action = 0x116;
			pCurrScreen->buttons[1].action = 0x300;
			pCurrScreen->buttons[2].action = 0x300;
			pCurrScreen->buttons[3].action = 0x300;
		}
		else if (gFurthestMission < 21)
		{
			pCurrScreen->buttons[0].action = 0x116;
			pCurrScreen->buttons[1].action = 0x116;
			pCurrScreen->buttons[2].action = 0x300;
			pCurrScreen->buttons[3].action = 0x300;
		}
		else if (gFurthestMission < 31) 
		{
			pCurrScreen->buttons[0].action = 0x116;
			pCurrScreen->buttons[1].action = 0x116;
			pCurrScreen->buttons[2].action = 0x116;
			pCurrScreen->buttons[3].action = 0x300;
		}
		else 
		{
			pCurrScreen->buttons[0].action = 0x116;
			pCurrScreen->buttons[1].action = 0x116;
			pCurrScreen->buttons[2].action = 0x116;
			pCurrScreen->buttons[3].action = 0x116;

			if (gFurthestMission == 40)
			{
				pCurrScreen->buttons[0].u = 5;
				pCurrScreen->buttons[3].d = 5;
			}
		}

		if (loaded[0] == -1)
		{
			SetupExtraPoly("DATA\\CITY.RAW", (currCity != 4) ? currCity : 0, 0);
		}
		else
		{
			bDrawExtra = 1;
			
			if (currCity == 4) 
				LoadImage(&rect, (u_long *)_frontend_buffer);
			else 
				LoadImage(&rect, (u_long *)(_frontend_buffer + currCity * 0x8000));

			DrawSync(0);
		}

		return 0;
	}

	if (fePad & 0x40)
	{
		lastCity = -1;
		lastCutCity = GameLevel;

		if (GameLevel != 4)
		{
			lastCity = -1;
		}
		else
		{
			bReturnToMain = 0;

			pScreenStack[ScreenDepth] = pCurrScreen;
			pButtonStack[ScreenDepth] = pCurrButton;

			ScreenNames[ScreenDepth] = pCurrButton->Name;

			feVariableSave[0] = currCity;

			StartRender(0x60);
		}

		return 0;
	}
	else if (fePad & 0x10)
	{
		FESound(0);
		bDoneAllready = 1;
		LoadBackgroundFile("DATA\\GFX.RAW");
		bDoingCutSelect = 0;
		bDrawExtra = 0;

		return 0;
	}
	else if (fePad & 0x1000)
	{
		currCity = pCurrButton->u - 1;
	}
	else if (fePad & 0x4000)
	{
		currCity = pCurrButton->d - 1;
	}

	GameLevel = currCity;

	rect = extraRect;

	if (GameLevel != 4)
		LoadImage(&rect, (u_long *)(_frontend_buffer + GameLevel * 0x8000));
	else
		LoadImage(&rect, (u_long *)_frontend_buffer);

	DrawSync(0);

#ifdef PSX
	DisplayOnScreenText();

	addPrim(&current->ot[2], &extraSprt);
	addPrim(&current->ot[3], &extraDummy);

	EndFrame();
#endif

	return 0;
}



// [D] [T]
int SetVolumeScreen(int bSetup)
{
	int last;
	int ypos[2] = { 292, 338 };
	char text[4];
	RECT16 rect;

	static int masterVolumeHold = 0;
	static int musicVolumeHold = 0;

	if (bSetup) 
	{
		currSelIndex = 0;

		masterVolumeHold = gMasterVolume;
		musicVolumeHold = gMusicVolume;

		LoadBackgroundFile("DATA\\VOL.RAW");

#ifdef PSX
		sprintf(text, "%d", (int)(10000 + gMasterVolume) / 100);
		FEPrintString(text, 152, ypos[0], 2, 128, 128, 128);

		sprintf(text, "%d", (int)(10000 + gMusicVolume) / 100);
		FEPrintString(text, 152, ypos[1], 2, 128, 128, 128);
#endif

		return 0;
	}

#ifndef PSX
	sprintf(text, "%d", (int)(10000 + gMasterVolume) / 100);
	FEPrintString(text, 152, ypos[0], 2, 128, 128, 128);

	sprintf(text, "%d", (int)(10000 + gMusicVolume) / 100);
	FEPrintString(text, 152, ypos[1], 2, 128, 128, 128);
#endif

	// did we make any changes?
	bool update = false;

	currSelIndex = (pCurrButton->u & 3);

	if (fePad & 0x10)
	{
		FESound(0);
		bDoneAllready = 1;

		gMasterVolume = masterVolumeHold;
		gMusicVolume = musicVolumeHold;

		LoadBackgroundFile("DATA\\GFX.RAW");

		SetMasterVolume(gMasterVolume);
		SetXMVolume(gMusicVolume);

		return 0;
	}
	else if (fePad & 0x40)
	{
		if (currSelIndex == 2)
			LoadBackgroundFile("DATA\\GFX.RAW");

		return 0;
	}
	else
	{
		int dir = -1; // -1: no action, 0: limit reached, 1: OK

		if (fePad & 0x8000)
		{
			switch (currSelIndex)
			{
			case 0:
				gMasterVolume -= 100;

				if (gMasterVolume < -10000)
				{
					gMasterVolume = -10000;
					dir = 0;
				}
				else
				{
					dir = 1;
				}

				last = gMasterVolume;
				break;
			case 1:
				gMusicVolume -= 100;

				if (gMusicVolume < -10000)
				{
					gMusicVolume = -10000;
					dir = 0;
				}
				else
				{
					dir = 1;
				}

				last = gMusicVolume;
				break;
			}
		}
		else if (fePad & 0x2000)
		{
			switch (currSelIndex)
			{
			case 0:
				gMasterVolume += 100;

				if (gMasterVolume > 0)
				{
					gMasterVolume = 0;
					dir = 0;
				}
				else
				{
					dir = 1;
				}

				last = gMasterVolume;
				break;
			case 1:
				gMusicVolume += 100;

				if (gMusicVolume > 0)
				{
					gMusicVolume = 0;
					dir = 0;
				}
				else
				{
					dir = 1;
				}

				last = gMusicVolume;
				break;
			}
		}

		if (dir != -1)
		{
			FESound((dir == 1) ? 3 : 1);
			update = true;
		}
	}

	if (update)
	{
		SetMasterVolume(gMasterVolume);
		SetXMVolume(gMusicVolume);

#ifdef PSX
		sprintf(text, "%d", (int)(10000 + last) / 100);
		FEPrintString(text, 152, ypos[currSelIndex], 2, 128, 128, 128);

		sprintf(text, "%d", (int)(10000 + last) / 100);
		FEPrintString(text, 152, ypos[currSelIndex], 2, 0, 0, 0);

		EndFrame(); // [A] inlined
#else
		// don't flush the screen
		return 1;
#endif
	}

	return 0;
}

int GameNum = 0;
CVECTOR scoreCol = { 120, 120, 120 };
CVECTOR otherCol = {125, 115, 34};

// [D] [T]
void DisplayScoreTable(void)
{
	static char* CityNames[4] = {
		M_LTXT(MTXT_Chicago),
		M_LTXT(MTXT_Havana),
		M_LTXT(MTXT_LasVegas),
		M_LTXT(MTXT_RioDeJaneiro)
	};

	int time;
	int i;
	int offset;
	char text[32];
	SCORE_ENTRY *pSE;

	gSubGameNumber = GameNum;

	OnScoreTable(&pSE);

	sprintf(text, ScreenNames[ScreenDepth - 1]);
	FEPrintString(text, 20, 200, 2, otherCol.r, otherCol.g, otherCol.b);

	sprintf(text, CityNames[GameLevel]);
	FEPrintStringSized(text, 280, 206, 3072, 2, otherCol.r, otherCol.g, otherCol.b);

	if (GameType == GAME_TAKEADRIVE && NumPlayers == 2)
		offset = 48;
	else if (GameType == GAME_COPSANDROBBERS)
		offset = 32;
	else if (GameType == GAME_CAPTURETHEFLAG)
		offset = 40;
	else if (GameType == GAME_CHECKPOINT && NumPlayers == 2)
		offset = 56;
	else
		offset = (GameType - 4U) * 8;

	if (GameType != GAME_PURSUIT && GameType != GAME_SURVIVAL) 
	{
		sprintf(text, "%s", GAMEMODE_AREA_NAME(GameLevel, offset, GameNum));
		FEPrintStringSized(text, 420, 206, 3072, 2, otherCol.r, otherCol.g, otherCol.b);
	}

	offset = 240;
	
	for (i = 0; i < 5; i++)
	{
		sprintf(text, pSE[i].name);
		FEPrintString(text, 20, offset, 2, scoreCol.r, scoreCol.g, scoreCol.b);

		if (GameType == GAME_GATERACE || GameType == GAME_TRAILBLAZER) 
		{
			if (pSE[i].items != -1) 
			{
				sprintf(text, "%d", pSE[i].items);
				FEPrintString(text, 140, offset, 2, scoreCol.r, scoreCol.g, scoreCol.b);
			}
		}

		time = pSE[i].time;

		if (time != -1) 
		{
			sprintf(text, "%d:%02d.%02d", time / 180000, time / 3000 - (time / 180000) * 60, (time % 3000) / 30);
			
			FEPrintString(text, 280, offset, 2, scoreCol.r, scoreCol.g, scoreCol.b);
		}

		offset += 36;
	}
}


// [D] [T]
int ScoreScreen(int bSetup)
{
	if (bSetup)
	{
		GameLevel = 0;

		DisplayScoreTable();

		bDoingScores = 1;
		currSelIndex = 0;

		return 0;
	}

	if (fePad & 0x40)
	{
		if (currSelIndex == 0)
		{
			if (GameType == GAME_SURVIVAL || GameType == GAME_PURSUIT)
			{
				if (--GameLevel < 0)
					GameLevel = 3;

				GameNum = 0;
			}
			else
			{
				GameNum ^= 1;

				if (GameNum == 1)
				{
					if (--GameLevel < 0)
						GameLevel = 3;
				}
			}
		}
		else
		{
			if (GameType == GAME_SURVIVAL || GameType == GAME_PURSUIT)
			{
				if (++GameLevel > 3)
					GameLevel = 0;

				GameNum = 0;
			}
			else
			{
				GameNum ^= 1;

				if (GameNum == 0)
				{
					if (++GameLevel > 3)
						GameLevel = 0;
				}
			}
		}

#ifdef PSX
		DisplayScoreTable();
		bRedrawFrontend = 1;
#endif
	}
	else if (fePad & 0x10)
	{
		bDoingScores = 0;
		return 0;
	}
	else if ((fePad & 0x1000) || (fePad & 0x4000))
	{
		currSelIndex ^= 1;
	}

#ifndef PSX
	DisplayScoreTable();
#endif

	return 0;
}

// [D] [T]
int SubtitlesOnOffScreen(int bSetup)
{
	if (bSetup)
	{
		if (gSubtitles == 0)
		{
			pCurrButton = pCurrScreen->buttons + 1;
		}
		else 
		{
			pCurrButton = pCurrScreen->buttons;
		}

		return 1;
	}
	return 0;
}


// [D] [T]
int CityCutOffScreen(int bSetup)
{
	if (bSetup)
	{
		if (gFurthestMission < 20)
			pCurrScreen->buttons[2].action = 0x300;

		if (gFurthestMission < 29)
			pCurrScreen->buttons[3].action = 0x300;

#ifndef PSX
		LoadBackgroundFile("DATA\\CITYBACK.RAW");

		if (lastCity == -1)
			currCity = 0;
		else
			currCity = lastCity;

		if (loaded[0] == -1)
		{
			SetupExtraPoly("DATA\\CITY.RAW", currCity, 0);
		}
		else
		{
			bDrawExtra = 1;

			RECT16 rect = extraRect;
			LoadImage(&rect, (u_long *)(_frontend_buffer + currCity * 0x8000));
			DrawSync(0);
		}
#endif
		return 0;
	}

#ifndef PSX
	/*
	if ((fePad & 0x40U) != 0)
	{
		lastCity = currCity;

		bDrawExtra = 0;

		LoadBackgroundFile("DATA\\GFX.RAW");
		loaded[0] = -1;

		return 0;
	}*/

	if (fePad & 0x10)
	{
		lastCity = -1;

		bDrawExtra = 0;
		FESound(0);
		bDoneAllready = 1;

		LoadBackgroundFile("DATA\\GFX.RAW");
		loaded[0] = -1;

		return 0;
	}
	else if (fePad & 0x1000)
	{
		currCity = pCurrButton->u - 1;
	}
	else if (fePad & 0x4000)
	{
		currCity = pCurrButton->d - 1;
	}
	else
	{
		currCity = pCurrButton->u & 3;
		return 0;
	}

	RECT16 rect = extraRect;
	LoadImage(&rect, (u_long *)(_frontend_buffer + currCity * 0x8000));
	DrawSync(0);
#endif
	return 0;
}


char* contNames[2] = {
	"DATA\\CARCONT.RAW",
	"DATA\\TANCONT.RAW"
};

// [D] [T]
int ControllerScreen(int bSetup)
{
	if (bSetup ) 
	{
		bDoingScores = 1;
		currSelIndex = 0;
		pCurrScreen->numButtons = 0;

		LoadBackgroundFile(contNames[currSelIndex]);
	}
	else
	{
		if (fePad & 0x40) 
		{
			currSelIndex = currSelIndex ^ 1;
			LoadBackgroundFile(contNames[currSelIndex]);

			bRedrawFrontend = 1;
		}
		else if (fePad & 0x10)
		{
			bDoingScores = 0;
			LoadBackgroundFile("DATA\\GFX.RAW");
		}
	}

	return 0;
}

// [D] [T]
int MainScreen(int bSetup)
{
	if (bSetup) 
	{
		if (numPadsConnected == 2) 
		{
			pCurrScreen->buttons[3].action = 0x106;
		}
		else 
		{
			pCurrScreen->buttons[3].action = 0x300;
		}
	}

	return 0;
}

static char* cheatText[] =
{
	G_LTXT_ID(GTXT_NoSecrets),
	G_LTXT_ID(GTXT_MountainTrack),
	G_LTXT_ID(GTXT_Circuit),
	G_LTXT_ID(GTXT_Invincibility),
	G_LTXT_ID(GTXT_Immunity),
	G_LTXT_ID(GTXT_BonusGallery)
};

// [D] [T] [A] adding bonus gallery
int CheatScreen(int bSetup)
{
	int i;
	int numOpen;
	int k;
	unsigned char cheatOn[12];
	int evilRuss[5];

	int hackLookup1[5] = {
		0x121,
		0x121,
		0x11E,
		0x11F,
		0,
	};

	int hackLookup2[5] = {
		0xC01, 0xC00, -1, -1, 0xE00
	};

	if (bSetup == 0)
	{
		return 0;
	}

	if (gFurthestMission == 40) 
		numOpen = 5;
	else 
		numOpen = AvailableCheats.cheat1 + AvailableCheats.cheat2 + AvailableCheats.cheat3 + AvailableCheats.cheat4;
	
	// "Sorry no secrets"
	if (numOpen == 0)
	{
		pCurrScreen->numButtons = 1;
		
		pCurrScreen->buttons[0].u = 1;
		pCurrScreen->buttons[0].d = 1;
		pCurrScreen->buttons[0].action = 0x400;

		sprintf(pCurrScreen->buttons[0].Name, GET_GAME_TXT(cheatText[0]));

		return 0;
	}

	pCurrScreen->numButtons = numOpen;

	evilRuss[0] = AvailableCheats.cheat1;
	evilRuss[1] = AvailableCheats.cheat2;
	evilRuss[2] = AvailableCheats.cheat3;
	evilRuss[3] = AvailableCheats.cheat4;

	if (numOpen >= 5)
		pCurrScreen->buttons[4] = pCurrScreen->buttons[3];

	k = 0;
	i = 0;

	do {
		if (evilRuss[i] || gFurthestMission == 40)
		{
			sprintf(pCurrScreen->buttons[i].Name, GET_GAME_TXT(cheatText[i + 1]));
			cheatOn[k++] = i;
		}
		i++;
	} while (i < 5);

	if (numOpen == 2) 
	{
		pCurrScreen->buttons[0].action = hackLookup1[cheatOn[0]];
		pCurrScreen->buttons[1].action = hackLookup1[cheatOn[1]];
	
		pCurrScreen->buttons[0].var = hackLookup2[cheatOn[0]];
		pCurrScreen->buttons[1].var = hackLookup2[cheatOn[1]];
		
		pCurrScreen->buttons[0].d = 2;
		pCurrScreen->buttons[0].u = 2;
		
		pCurrScreen->buttons[1].d = 1;
		pCurrScreen->buttons[1].u = 1;
		
		currSelIndex = 0;
	
		return 0;
	}

	if (numOpen == 1)
	{
		pCurrScreen->buttons[0].action = hackLookup1[cheatOn[0]];
		pCurrScreen->buttons[0].var = hackLookup2[cheatOn[0]];
		pCurrScreen->buttons[0].d = 1;
		pCurrScreen->buttons[0].u = 1;
		
		currSelIndex = 0;
	
		return 0;
	}

	if (numOpen == 3) 
	{
		pCurrScreen->buttons[0].action = hackLookup1[cheatOn[0]];
		pCurrScreen->buttons[1].action = hackLookup1[cheatOn[1]];
		pCurrScreen->buttons[2].action = hackLookup1[cheatOn[2]];
		
		pCurrScreen->buttons[0].var = hackLookup2[cheatOn[0]];
		pCurrScreen->buttons[1].var = hackLookup2[cheatOn[1]];
		pCurrScreen->buttons[2].var = hackLookup2[cheatOn[2]];
		
		pCurrScreen->buttons[0].d = 2;
		pCurrScreen->buttons[0].u = 3;
		
		pCurrScreen->buttons[1].d = 3;
		pCurrScreen->buttons[1].u = 1;
		
		pCurrScreen->buttons[2].d = 1;
		pCurrScreen->buttons[2].u = 2;
		
		currSelIndex = 0;
		return 0;
	}

	if (numOpen >= 4) 
	{
		pCurrScreen->buttons[0].action = hackLookup1[cheatOn[0]];
		pCurrScreen->buttons[1].action = hackLookup1[cheatOn[1]];
		pCurrScreen->buttons[2].action = hackLookup1[cheatOn[2]];
		pCurrScreen->buttons[3].action = hackLookup1[cheatOn[3]];

		pCurrScreen->buttons[0].var = hackLookup2[cheatOn[0]];
		pCurrScreen->buttons[1].var = hackLookup2[cheatOn[1]];
		pCurrScreen->buttons[2].var = hackLookup2[cheatOn[2]];
		pCurrScreen->buttons[3].var = hackLookup2[cheatOn[3]];
		
		pCurrScreen->buttons[0].d = 2;
		pCurrScreen->buttons[0].u = 4;
		
		pCurrScreen->buttons[1].d = 3;
		pCurrScreen->buttons[1].u = 1;
		
		pCurrScreen->buttons[2].d = 4;
		pCurrScreen->buttons[2].u = 2;
		
		pCurrScreen->buttons[3].d = 1;
		pCurrScreen->buttons[3].u = 3;

		if(numOpen >= 5)
		{
			pCurrScreen->buttons[4].action = hackLookup1[cheatOn[4]];
			pCurrScreen->buttons[4].var = hackLookup2[cheatOn[4]];
			pCurrScreen->buttons[4].y += 40;
			pCurrScreen->buttons[4].s_y += 40;

			PsxScreens[40].userFunctionNum = 21;

			pCurrScreen->buttons[0].u = 5;
			pCurrScreen->buttons[3].d = 5;
			
			pCurrScreen->buttons[4].d = 1;
			pCurrScreen->buttons[4].u = 4;
		}

		currSelIndex = 0;
		return 0;
	}

	pCurrScreen->numButtons = 0;
	currSelIndex = 0;

	return 0;
}


// [D] [T]
int ImmunityOnOffScreen(int bSetup)
{
	if (bSetup)
	{
		if (gPlayerImmune == 0)
			pCurrButton = pCurrScreen->buttons + 1;
		else
			pCurrButton = pCurrScreen->buttons;
		return 1;
	}

	return 0;
}

// [D] [T]
int InvincibleOnOffScreen(int bSetup)
{
	if (bSetup)
	{
		if (gInvincibleCar == 0) 
			pCurrButton = pCurrScreen->buttons + 1;
		else 
			pCurrButton = pCurrScreen->buttons;

		return 1;
	}
	return 0;
}

// [D] [T]
int GamePlayScreen(int bSetup)
{
	if (bSetup)
	{
		pCurrScreen->buttons[2].action = (allowVibration == 0) ? 0x300 : 0x111;
	}

	return 0;
}

// [D] [T]
int GameNameScreen(int bSetup)
{
	int offset;

	if (bSetup) 
	{
		
		if (GameType == GAME_TAKEADRIVE && NumPlayers == 2) 
			offset = 48;
		else if (GameType == GAME_COPSANDROBBERS)
			offset = 32;
		else if (GameType == GAME_CAPTURETHEFLAG)
			offset = 40;
		else if (GameType == GAME_CHECKPOINT && NumPlayers == 2)
			offset = 56;
		else
			offset = (GameType - 4U) * 8;

		strcpy(pCurrScreen->buttons[0].Name, GAMEMODE_AREA_NAME(GameLevel, offset, 0));
		strcpy(pCurrScreen->buttons[1].Name, GAMEMODE_AREA_NAME(GameLevel, offset, 1));
	}

	return 0;
}

// [D] [T]
void FEInitCdIcon(void)
{
	ushort *palette;
	RECT16 rect;

	palette = cd_icon + 10;

	for (int i = 0; i < 14; i++)
	{
		*palette &= 0x7fff;
		palette++;
	}

	cd_icon[10] = 0;
	
	rect.x = 960;
	rect.y = 434;
	rect.w = 8;
	rect.h = 32;
	
	cd_icon[24] |= 0x8000;
	cd_icon[25] |= 0x8000;

	LoadImage(&rect, (u_long *)(cd_icon + 0x18));

	setPolyFT4(&cd_sprite);
	setRGB0(&cd_sprite, 128, 128, 128);
	setUVWH(&cd_sprite, 0, 178, 32, 32);
	setXYWH(&cd_sprite, 80, 38, 38, 21);
	setClut(&cd_sprite, 960, 433);
	setTPage(&cd_sprite, 0, 0, 960, 256);

	bCdIconSetup = 1;
}


// [D] [T]
void FEDrawCDicon(void)
{
	ushort *palette;
	int i;
	RECT16 dest;

	cd_icon[23] = cd_icon[11];
	
	palette = cd_icon + 10;

	for (i = 0; i < 12; i++)
	{
		palette[1] = palette[2];
		palette++;
	}
	
	dest.x = 960;
	dest.y = 433;
	dest.w = 16;
	dest.h = 1;

	LoadImage(&dest, (u_long *)(cd_icon + 10));
	DrawPrim(&cd_sprite);
	DrawSync(0);
}

// [D] [T]
int CheatNumlayerSelect(int bSetup)
{
	if (bSetup) 
	{
		if (numPadsConnected == 2) 
		{
			pCurrScreen->buttons[1].action = 0x120;
		}
		else 
		{
			pCurrScreen->buttons[1].action = 0x300;
		}
		return 0;
	}
	return 0;
}




