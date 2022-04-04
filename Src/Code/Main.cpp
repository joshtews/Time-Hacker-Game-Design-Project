/// \file main.cpp 
/// \brief Main file for for Demo 5.

/// \mainpage Demo 5: Artificial Intelligence
///
/// Crows have simple rule-based artificial intelligence with some randomness
/// thrown in to make them behave slightly differently.  
/// They try to avoid the plane as much as they can given a limited attention span.
/// You might see some flocking as emergent behaviour.
/// The plane fires a bullet each time you hit the space bar.
/// Bullets have a fixed lifetime.
/// When you hit a crow with a bullet, the crow explodes and turns into
/// a falling corpse, which disappears when it hits the ground. 

#include <windows.h>
#include <windowsx.h>

#include "defines.h"
#include "abort.h"
#include "gamerenderer.h"
#include "imagefilenamelist.h"
#include "debug.h"
#include "timer.h"
#include "sprite.h"
#include "object.h"
#include "spriteman.h"
#include "EnemyTestCharacter.h"
#include "objman.h"
#include "Random.h"
#include "player.h"
#include "Weapon.h"
#include "Sound.h"
#include "SubWeapons.h"
#include "Menu.h"

//globals
bool g_bAtMainMenu;
bool g_bPlayerDead;
bool g_bPlayerHit;
int g_nNPCsKilled;
int g_nRound;
ObjectType g_nWeapons[3];
char * g_nWeaponNames[3];
float g_fPitch;
Vector3 g_vSpawnLocations[8];
Vector3 g_vRewardLocation;
BOOL g_bActiveApp;  ///< TRUE if this is the active application
HWND g_HwndApp; ///< Application window handle.
HINSTANCE g_hInstance; ///< Application instance handle.
char g_szGameName[256]; ///< Name of this game
char g_szShaderModel[256]; ///< The shader model version used
CImageFileNameList g_cImageFileName; ///< List of image file names.
CTimer g_cTimer; ///< The game timer.

CSpriteManager g_cSpriteManager;  ///< The sprite manager.
CObjectManager g_cProjectileManager;
CObjectManager g_cNPCManager;
CObjectManager g_cDeadManager;    ///<holds dead objects
CSoundManager* g_pSoundManager;  ///< The Sound Manager.

CMenu g_cMainMenu;

CObjectManager g_cWallManager;       ///< manages wall objects.
CObjectManager g_cObjectManager;     ///< manages all other objects.

CPlayerCharacter *g_cPlayer = NULL;

CRandom g_cRandom; ///< The random number generator.

//graphics settings
int g_nScreenWidth; ///< Screen width.
int g_nScreenHeight; ///< Screen height.
BOOL g_bWireFrame = FALSE; ///< TRUE for wireframe rendering.

//XML settings
tinyxml2::XMLDocument g_xmlDocument, g_xmlDocument2; ///< TinyXML document for settings.
XMLElement* g_xmlSettings = nullptr; ///< TinyXML element for settings tag.
XMLElement* g_xmlObjects = nullptr;///< TinyXML element for objects tag.

//debug variables
#ifdef DEBUG_ON
  CDebugManager g_cDebugManager; ///< The debug manager.
#endif //DEBUG_ON

CGameRenderer GameRenderer; ///< The game renderer.

//functions in Window.cpp
void InitGraphics();
HWND CreateDefaultWindow(char* name, HINSTANCE hInstance, int nCmdShow);

void XMLObjects()//place objects
{
	const char* xmlFileName = "test.xml"; //Settings file name.
	if (g_xmlDocument2.LoadFile(xmlFileName) != 0)
		ABORT("Cannot load settings file %s.", xmlFileName);
	//get settings tag
	g_xmlObjects = g_xmlDocument2.FirstChildElement("level"); //settings tag
	if (g_xmlObjects == nullptr) //abort if tag not found
		ABORT("Cannot find <level> tag in %s.", xmlFileName);

	if (!g_xmlObjects)return;
	XMLElement* objSettings = g_xmlObjects->FirstChildElement("objects");

	if (objSettings)
	{
		XMLElement* obj;
		for (obj = objSettings->FirstChildElement("object"); obj != NULL; obj = obj->NextSiblingElement("object"))
		{
			if (obj)//sugoi
			{
				string name = obj->Attribute("name");
				ObjectType type = (ObjectType)obj->IntAttribute("type");
				float x = obj->FloatAttribute("x");
				float y = obj->FloatAttribute("y");
				float z = obj->FloatAttribute("z");
				float angle = obj->FloatAttribute("orientation");

				if(type == WALL_OBJECT)
				g_cWallManager.createObject(type, name.c_str(), Vector3(x, y, z), Vector3(0, 0, 0), angle);
				else
				g_cObjectManager.createObject(type, name.c_str(), Vector3(x, y, z), Vector3(0, 0, 0), angle);
			}
		}
	}
}

/// \brief Initialize XML settings.
///
/// Open an XML file and prepare to read settings from it. Settings
/// tag is loaded to XML element g_xmlSettings for later processing. Abort if it
/// cannot load the file or cannot find settings tag in loaded file.

void InitXMLSettings(){
  //open and load XML file
  const char* xmlFileName = "gamesettings.xml"; //Settings file name.
  if(g_xmlDocument.LoadFile(xmlFileName) != 0)
    ABORT("Cannot load settings file %s.", xmlFileName);
  //get settings tag
  g_xmlSettings = g_xmlDocument.FirstChildElement("settings"); //settings tag
  if(g_xmlSettings == nullptr) //abort if tag not found
    ABORT("Cannot find <settings> tag in %s.", xmlFileName);
} //InitXMLSettings

/// \brief Load game settings.
///
/// Load and parse game settings from a TinyXML element g_xmlSettings.

void LoadGameSettings(){
  if(!g_xmlSettings)return; //bail and fail

  //get game name
  XMLElement* ist = g_xmlSettings->FirstChildElement("game"); 
  if(ist){
    size_t len = strlen(ist->Attribute("name")); //length of name string
    strncpy_s(g_szGameName, len+1, ist->Attribute("name"), len); 
  } //if

  //get renderer settings
  XMLElement* renderSettings = 
    g_xmlSettings->FirstChildElement("renderer"); //renderer tag
  if(renderSettings){ //read renderer tag attributes
    g_nScreenWidth = renderSettings->IntAttribute("width");
    g_nScreenHeight = renderSettings->IntAttribute("height");

    size_t len = strlen(renderSettings->Attribute("shadermodel")); //length shader model string
    strncpy_s(g_szShaderModel, len + 1, renderSettings->Attribute("shadermodel"), len);
  } //if

  //get image file names
  g_cImageFileName.GetImageFileNames(g_xmlSettings);

  //get debug settings
  #ifdef DEBUG_ON
    g_cDebugManager.GetDebugSettings(g_xmlSettings);
  #endif //DEBUG_ON
} //LoadGameSettings

/// \brief Create game objects. 
///
/// Create a plane and a collection of crows at hard-coded positions.

void CreateMenus(){
  RECT r1 = {410, 488, 619, 531};
  RECT r2 = {462, 615, 566, 657};
  g_cMainMenu.insertTextComponent("HACK TIME", Vector3((g_nScreenWidth / 2.0f) - 83.0f, g_nScreenHeight / 3.0, 900.0f), r1);
  g_cMainMenu.insertTextComponent("EXIT", Vector3((g_nScreenWidth / 2.0f) - 30.0f, g_nScreenHeight / 6.0, 900.0f), r2);
  g_cMainMenu.insertSpriteComponent(TITLECARD, Vector3(15.0f, g_nScreenHeight / 2.0f, 1000.0f));
}

void CreateObjects(){
  g_cObjectManager.InsertObjectType("pistol", PISTOL_OBJECT);
  g_cObjectManager.InsertObjectType("player", PLAYER_OBJECT);
  //g_cObjectManager.InsertObjectType("enemytest", ENEMYTEST_OBJECT);
  g_cProjectileManager.InsertObjectType("bullet", BULLET_OBJECT);
  g_cObjectManager.InsertObjectType("shotgun", SHOTGUN_OBJECT);
  g_cObjectManager.InsertObjectType("testwall", WALL_OBJECT);
  g_cObjectManager.InsertObjectType("flooring", FLOORING);
  g_cObjectManager.InsertObjectType("lmg", LMG_OBJECT);
  g_cObjectManager.InsertObjectType("slugger", SLUGGER_OBJECT);
  g_cObjectManager.InsertObjectType("knife", KNIFE_OBJECT);
  g_cObjectManager.InsertObjectType("chicagoslugger", CHICAGO_SLUGGER_OBJECT);
  
  XMLObjects();

  //create wall for collision testing
  //g_cObjectManager.createObject(TESTWALL, "testwall",
    //Vector3(g_nScreenWidth / 2.0f + 200, g_nScreenHeight / 2.0f, (float)nPlayerZ - 1), Vector3(0, 0, 0));
  //the player
  const int nPlayerZ = 0;
  g_cPlayer = new CPlayerCharacter("player", Vector3(0, 0, (float)nPlayerZ), Vector3(0, 0, 0));

  g_cNPCManager.AddObjectByName("enemytest", new CEnemyTestCharacter("enemytest",
    Vector3(0, 800, (float)nPlayerZ)));
  g_cNPCManager.AddObjectByName("enemytest", new CEnemyTestCharacter("enemytest",
    Vector3(1200, 0, (float)nPlayerZ)));
  g_cNPCManager.AddObjectByName("enemytest", new CEnemyTestCharacter("enemytest",
    Vector3(0, -1500, (float)nPlayerZ)));
  g_cNPCManager.AddObjectByName("enemytest", new CEnemyTestCharacter("enemytest",
    Vector3(-1600, 0, (float)nPlayerZ), 6, new CSlugger(Vector3(-1600, 0, (float)nPlayerZ), Vector3(0,0,0))));
  g_cObjectManager.createObject(PISTOL_OBJECT, "pistol", Vector3(-100, 100, (float)nPlayerZ), Vector3(0, 0, 0), 0.0f);
  g_cObjectManager.createObject(KNIFE_OBJECT, "knife", Vector3(100, 100, (float)nPlayerZ), Vector3(0, 0, 0), 0.0f);

} //CreateObjects

void InitCursor(HWND hwnd)
{
  RECT r;
  GetWindowRect(hwnd, &r); //get window coordinates and adjust
  r.top += 32;             //*********NEEDS FIXING**********
  r.left += 8;             //*********NEEDS FIXING**********
  r.bottom -= 8;           //*********NEEDS FIXING**********
  r.right -= 8;            //*********NEEDS FIXING**********
  ClipCursor(&r);          //confine cursor
  SetCursor(LoadCursorFromFile("cursor.cur")); //load cursor image and set it
}

void Reset()
{
  g_cDeadManager.clean();
  g_cNPCManager.clean();
  g_cObjectManager.clean();
  g_cProjectileManager.clean();

  delete g_cPlayer;
  g_nNPCsKilled = 0;
  CreateObjects();

  g_bAtMainMenu = true;
  g_bPlayerDead = false;
  for(int i = 0; i < 500; i++)
    GameRenderer.m_bRoundHit[i] = true;
  g_cTimer.start();
  g_cTimer.pause();
}

/// \brief Keyboard handler.
///
/// Handler for keyboard messages from the Windows API. Takes the appropriate
/// action when the user presses a key on the keyboard.
/// \param keystroke Virtual key code for the key pressed
/// \return TRUE if the game is to exit
BOOL KeyboardHandler(WPARAM keystroke, UINT message){ 
  CPlayerCharacter* player = g_cPlayer;
  if(!g_bPlayerDead)
  {
    if(message == WM_KEYDOWN)
      switch(keystroke){
        case VK_ESCAPE: //exit game
          if (!g_bPlayerHit)
          {
            g_bAtMainMenu = true; //exit keyboard handler
            g_cTimer.pause();
          }
          break;

      //case VK_F1: //flip camera mode
      //  if (g_cTimer.m_bPaused && !g_bAtMainMenu)
      //    g_cTimer.resume();
      //  else if (!g_bAtMainMenu)
      //    g_cTimer.pause();
      //  break;

        case VK_F2: //toggle wireframe mode
          //g_bWireFrame = !g_bWireFrame;
          //GameRenderer.SetWireFrameMode(g_bWireFrame);
          break;
      
        case VK_UP: case 'W':
          if(player)
            player->setDesiredVelocity(Vector3(player->getDesiredVelocity().x, player->getMoveSpeed(), 0)); 
          break;

        case VK_DOWN: case 'S':
          if(player)
            player->setDesiredVelocity(Vector3(player->getDesiredVelocity().x, -player->getMoveSpeed(), 0));
          break;

        case VK_LEFT: case 'A':
          if(player)
            player->setDesiredVelocity(Vector3(-player->getMoveSpeed(), player->getDesiredVelocity().y, 0));
          break;

        case VK_RIGHT: case 'D':
          if(player)
            player->setDesiredVelocity(Vector3(player->getMoveSpeed(), player->getDesiredVelocity().y, 0));
          break;

        case VK_SPACE: 
          break;
      } //switch
    else if (message == WM_KEYUP)
    { //keys released
      switch (keystroke)
      { 
        case VK_UP: case 'W': 
		  if (player) //reset verticle velocity
		  {
		    if ((1 << 16) & GetAsyncKeyState('S') || (1 << 16) & GetAsyncKeyState(VK_DOWN)) //apply verticle velocity if opposite is pressed
			  player->setDesiredVelocity(Vector3(player->getDesiredVelocity().x, -player->getMoveSpeed(), 0));
		    else //reset vertical velocity if opposite is not pressed
	          player->setDesiredVelocity(Vector3(player->getDesiredVelocity().x, 0, 0));
		  }
          break;
        case VK_DOWN: case 'S':
		  if (player)
		  {
		    if ((1 << 16) & GetAsyncKeyState('W') || (1 << 16) & GetAsyncKeyState(VK_UP)) //apply verticle velocity if opposite is pressed
		      player->setDesiredVelocity(Vector3(player->getDesiredVelocity().x, player->getMoveSpeed(), 0));
		    else //reset vertical velocity if opposite is not pressed
              player->setDesiredVelocity(Vector3(player->getDesiredVelocity().x, 0, 0));
		  }
          break;
        case VK_LEFT: case 'A':
		  if (player) //reset horizontal velocity
		  {
		    if ((1 << 16) & GetAsyncKeyState('D') || (1 << 16) & GetAsyncKeyState(VK_RIGHT)) //apply horizontal velocity if opposite is pressed
		      player->setDesiredVelocity(Vector3(player->getMoveSpeed(), player->getDesiredVelocity().y, 0));
		    else //reset horizontal velocity if opposite is not pressed
		      player->setDesiredVelocity(Vector3(0, player->getDesiredVelocity().y, 0));
		  }
          break;
        case VK_RIGHT: case 'D':
	      if (player) //reset horizontal velocity
		  {
		    if ((1 << 16) & GetAsyncKeyState('A') || (1 << 16) & GetAsyncKeyState(VK_LEFT)) //apply horizontal velocity if opposite is pressed
			  player->setDesiredVelocity(Vector3(-player->getMoveSpeed(), player->getDesiredVelocity().y, 0));
		    else //reset horizontal velocity if opposite is not pressed
		      player->setDesiredVelocity(Vector3(0, player->getDesiredVelocity().y, 0));
		  }
          break;
      }
    }
  }
  else
  {
    if (message == WM_KEYDOWN)
      switch (keystroke) {
      case VK_ESCAPE: //exit game
        g_bAtMainMenu = true; //exit keyboard handler
        g_cTimer.pause();
        break;
      case VK_RETURN:
        Reset();
        break;
    }
  }
  return FALSE; //normal exit
} //KeyboardHandler

void CreateSpawnPoints()
{
  g_vSpawnLocations[0] = Vector3(1750, 1500, 0);
  g_vSpawnLocations[1] = Vector3(-1750, -1500, 0);
  g_vSpawnLocations[2] = Vector3(-1750, 1500, 0);
  g_vSpawnLocations[3] = Vector3(1750, -1500, 0);
  g_vSpawnLocations[4] = Vector3(1290, 0, 0);
  g_vSpawnLocations[5] = Vector3(0, 1290, 0);
  g_vSpawnLocations[6] = Vector3(-1290, 0, 0);
  g_vSpawnLocations[7] = Vector3(0, -1290, 0);
}


void Left_Click(WPARAM wParam, UINT message)
{
  if(g_bAtMainMenu)
  {
    char * text = g_cMainMenu.checkCollision();
    if (text == "HACK TIME")
    {
      g_cTimer.resume();
      g_bAtMainMenu = false;
    }
    else if(text == "EXIT")
    {
      GameRenderer.Release(); //release textures
      SAFE_DELETE(g_pSoundManager);
      PostQuitMessage(0);
    }
  }
  else
  {
    CPlayerCharacter* player = g_cPlayer;
    CWeapon* weapon = player->get_weapon();
    if(!g_bPlayerDead)
    {
      if (weapon != nullptr)
      {
        weapon->activate();
      }
    }
  }
}

void Left_Click_Release(WPARAM wParam, UINT message)
{
  CPlayerCharacter* player = g_cPlayer;
  CWeapon* weapon = player->get_weapon();
  if(!g_bPlayerDead)
  {
    if (weapon != nullptr)
    {
      weapon->deactivate();
    }
  }
}

void Right_Click(WPARAM wParam, UINT messgae)
{
  CPlayerCharacter* player = g_cPlayer;
  CGameObject* pickup = g_cObjectManager.FindCollisionSimple(player, PICKUP_COLLISION);
  if(!g_bPlayerDead)
  {
    if (player->get_weapon())
      player->unequip_weapon();

    if(pickup)
      player->equip_weapon((CWeapon *)pickup);
  }
}

/// \brief Window procedure.
///
/// Handler for messages from the Windows API. 
/// \param hwnd Window handle
/// \param message Message code
/// \param wParam Parameter for message 
/// \param lParam Second parameter for message
/// \return 0 if message is handled

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
  switch(message){ //handle message
    case WM_ACTIVATEAPP: g_bActiveApp = (BOOL)wParam; break; //iconize

    case WM_KEYDOWN: //keyboard hit
      if(KeyboardHandler(wParam,message))DestroyWindow(hwnd);
      break;
    case WM_KEYUP: //keyboard up
      KeyboardHandler(wParam, message);
      break;
    case WM_LBUTTONDOWN:
      Left_Click(wParam, message);
      break;
    case WM_LBUTTONUP:
      Left_Click_Release(wParam, message);
      break;
    case WM_RBUTTONDOWN:
      Right_Click(wParam, message);
       break;
    case WM_DESTROY: //on exit
      GameRenderer.Release(); //release textures
      SAFE_DELETE(g_pSoundManager);
      PostQuitMessage(0); //this is the last thing to do on exit
      break;

    default: //default window procedure
      return DefWindowProc(hwnd, message, wParam, lParam);
  } //switch(message)

  return 0;
} //WindowProc
          
/// \brief Winmain.  
///
/// Main entry point for this application. 
/// \param hInst Handle to the current instance of this application.
/// \param hPrevInst Handle to previous instance, deprecated.
/// \param lpCmdLine Command line string, unused. 
/// \param nShow Specifies how the window is to be shown.
/// \return TRUE if application terminates correctly.

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShow){
  MSG msg; //current message
  HWND hwnd; //handle to fullscreen window

  #ifdef DEBUG_ON
    g_cDebugManager.open(); //open debug streams, settings came from XML file
  #endif //DEBUG_ON

  g_hInstance = hInst;
  g_cTimer.start(); //start game timer
  InitXMLSettings(); //initialize XML settings reader
  LoadGameSettings();

  //create fullscreen window
  hwnd = CreateDefaultWindow(g_szGameName, hInst, nShow);
  if(!hwnd)return FALSE; //bail if problem creating window
  g_HwndApp = hwnd; //save window handle
  
  g_fPitch = 1;
  g_pSoundManager = new CSoundManager();
  g_pSoundManager->Load();
  g_pSoundManager->loop(THEME_MUSIC_SOUND);

  InitGraphics(); //initialize graphics
  GameRenderer.LoadTextures(); //load images
  CreateMenus();

  g_bAtMainMenu = true;
  g_bPlayerDead = false;
  g_bPlayerHit = false;
  g_cTimer.pause();

  g_nWeapons[0] = PISTOL_OBJECT;
  g_nWeapons[1] = SHOTGUN_OBJECT;
  g_nWeapons[2] = LMG_OBJECT;
  g_nWeaponNames[0] = "pistol";
  g_nWeaponNames[1] = "shotgun";
  g_nWeaponNames[2] = "lmg";
  g_nRound = 0;
  CreateSpawnPoints();
  CreateObjects(); //create game objects
  InitCursor(g_HwndApp); //initialize cursor

  //message loop
  while (TRUE)
  {
	  if (PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE)) { //if message waiting
		  if (!GetMessage(&msg, nullptr, 0, 0))return (int)msg.wParam; //get it
		  TranslateMessage(&msg); DispatchMessage(&msg); //translate it
	  } //if
	  else
		  if (g_bActiveApp)
			  GameRenderer.ProcessFrame();
		  else WaitMessage();
      //DEBUGPRINTF("DEBUGGING\n");
  }
} //WinMain
