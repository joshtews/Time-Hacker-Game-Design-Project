/// \file sound.cpp
/// \brief Code for the sound manager class CSoundManager.

#include <stdio.h>

#include "sound.h"
#include "ObjMan.h"
#include "debug.h"
#include "Defines.h"
#include "player.h"

extern XMLElement* g_xmlSettings;
extern CObjectManager g_cObjectManager;
extern CPlayerCharacter* g_cPlayer;

/// Set member variables to sensible values and initialize the XAudio Engine using DirectXTK.

CSoundManager::CSoundManager(): m_nCount(0){
  CoInitializeEx(nullptr, COINIT_MULTITHREADED); 
  AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;

#ifdef _DEBUG
  eflags = eflags | AudioEngine_Debug;
#endif

  m_pAudioEngine = new AudioEngine(eflags);
} //constructor

/// Reclaim all dynamic memory and shut down the audio engine.

CSoundManager::~CSoundManager(){ 
  delete [] m_nInstanceCount;

  for(int i=0; i<m_nCount; i++)
    delete [] m_pInstance[i];

  delete [] m_pInstance;

  for(int i=0; i<(int)m_pSoundEffects.size(); i++)
    delete m_pSoundEffects[i];

  delete m_pAudioEngine;
} //destructor

/// Load a sound from a file.
/// \param filename name of file to be loaded
/// \return Size of sound effect loaded

int CSoundManager::Load(wchar_t *filename){
  m_pSoundEffects.push_back(new SoundEffect(m_pAudioEngine, filename));
  return m_pSoundEffects.size() - 1;
} //load

/// Create sound instances.
/// \param Index of sound.
/// \param n Number of instances of sound wanted.
/// \param flags Flags.

void CSoundManager::createInstances(int index, int n, SOUND_EFFECT_INSTANCE_FLAGS flags){
  m_nInstanceCount[index] = n;
  m_pInstance[index] = new SoundEffectInstance*[n];

  for(int i=0; i<n; i++)
    m_pInstance[index][i] = m_pSoundEffects[index]->CreateInstance(flags).release();
} //createInstances

/// Get the index into the instance array of the next unplayed instance of a sound.
/// \param index Index of sound in the sound array.
/// \return Index of  next unplayed instance in its instance array.

int CSoundManager::getNextInstance(int index){  
  int instance = 0; //current instance

  //get status of first instance
  SoundState soundState = m_pInstance[index][instance]->GetState();

  //find next unplayed instance, if any
  while(instance < m_nInstanceCount[index] && (soundState == PLAYING)) //while current copy in use
    if(++instance < m_nInstanceCount[index]) //go to next copy
      soundState = m_pInstance[index][instance]->GetState();

  return instance;
} //getNextInstance

/// Play a sound.
/// \param index index of sound to be played

int CSoundManager::play(int index){
  if(index < 0 || index >= m_nCount)return -1; //bail if bad index

  int instance = getNextInstance(index);
  if(instance < m_nInstanceCount[index]) //if unused copy found
    m_pInstance[index][instance]->Play(); //play it
  
  m_nLastPlayedSound = index;
  m_nLastPlayedInstance = instance;

  return instance;
} //play

/// Play a sound looped.
/// \param index index of sound to be played

int CSoundManager::loop(int index){
  if(index < 0 || index >= m_nCount)return -1; //bail if bad index

  int instance = getNextInstance(index);
  if(instance < m_nInstanceCount[index]) //if unused copy found
    m_pInstance[index][instance]->Play(true); //play it looped
  
  m_nLastPlayedSound = index;
  m_nLastPlayedInstance = instance;

  return instance;
} //loop

/// Load the sound files from the file list in g_xmlSettings.
/// Processes sound file names in \<sound\> tags within a \<sounds\>\</sounds\> pair.
/// Starts by counting the number of sound files needed, and creating arrays of
/// the right size.

void CSoundManager::Load(){
  if(g_xmlSettings == nullptr)return;

  //find <sounds> tag 
  XMLElement* snd =
    g_xmlSettings->FirstChildElement("sounds"); //<sounds> tag

  //count number of sounds in list, and create arrays of the right size
  m_nMaxSounds = 0; //counter
  for(XMLElement* s = snd->FirstChildElement("sound"); s; s=s->NextSiblingElement("sound"))
    ++m_nMaxSounds;

  //create arrays and initialize
  m_pInstance = new SoundEffectInstance**[m_nMaxSounds];
  m_nInstanceCount = new int[m_nMaxSounds];

  for(int i=0; i<m_nMaxSounds; i++){
    m_pInstance[i] = nullptr;
    m_nInstanceCount[i] = 0;
  } //for

  //load sounds from sound list
  const int bufsize = 64;
  char buffer[bufsize]; //buffer for file name
  for(XMLElement* s=snd->FirstChildElement("sound"); s; s=s->NextSiblingElement("sound")){
    strncpy_s(buffer, bufsize, s->Attribute("file"), sizeof(buffer)); //get file name
    int n = s->IntAttribute("instances"); //get number of instances
    if(n == 0) n = 1; //no instances attribute default to a single instance
    int newsize = (int)strlen(buffer) + 1;
    wchar_t * wcstring = new wchar_t[newsize];
    size_t count = 0;
    mbstowcs_s(&count, wcstring, newsize, buffer, _TRUNCATE);
    createInstances(Load(wcstring), n, 
      SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters);
     m_nCount++;
  } //for
} //Load

/// Set the position of a sound instance.
/// If the index or instance are -1, it uses the ones in
/// m_nLastPlayedSound and m_nLastPlayedInstance, respectively.
/// \param index Index of sound (defaults to -1)
/// \param instance Instance of sound (defaults to -1)
/// \param ePos The position of the object emitting the sound

void CSoundManager::move(Vector3 ePos, int instance, int index){
  if(index == -1)
    index = m_nLastPlayedSound;

  if(instance == -1)
    instance = m_nLastPlayedInstance;

  Vector3 vPlane = g_cPlayer->m_vPos;
  vPlane.z = ePos.z = 0.0f; //ignore depth

  AudioListener listener;
  AudioEmitter emitter;
  const float SCALE = 200.0f;

  listener.SetPosition(vPlane/SCALE);
  emitter.SetPosition(ePos/SCALE);

  if(instance >= 0 && instance < m_nInstanceCount[index])
    m_pInstance[index][instance]->Apply3D(listener, emitter);
} //move

/// Set the pitch of a sound instance.
/// If the index or instance are -1, it uses the ones in
/// m_nLastPlayedSound and m_nLastPlayedInstance, respectively.
/// \param index Index of sound (defaults to -1)
/// \param instance Instance of sound (defaults to -1)
/// \param p The new pitch.

void CSoundManager::pitch(float p, int instance, int index){ 
  if(index == -1)
    index = m_nLastPlayedSound;

  if(instance == -1)
    instance = m_nLastPlayedInstance;

  if(instance >= 0 &&instance < m_nInstanceCount[index])
    m_pInstance[index][instance]->SetPitch(p);
} //pitch

/// Set the volume of a sound instance.
/// If the index or instance are -1, it uses the ones in
/// m_nLastPlayedSound and m_nLastPlayedInstance, respectively.
/// \param index Index of sound (defaults to -1)
/// \param instance Instance of sound (defaults to -1)
/// \param v The new volume.
/// Volume is Default at 1.f

void CSoundManager::volume(float v, int instance, int index){ 
  if(index == -1)
    index = m_nLastPlayedSound;

  if(instance == -1)
    instance = m_nLastPlayedInstance;

  if(instance >= 0 && instance < m_nInstanceCount[index])
    m_pInstance[index][instance]->SetVolume(v);
} //volume