/// \file sndlist.h
/// \brief Enumerated types for sounds.

#pragma once

/// \brief Game sound enumerated type. 
///
/// Sounds must be listed here in the same order that they
/// are in the sound settings XML file.

enum GameSoundType{ 
  PISTOL_SHOT_SOUND, THEME_MUSIC_SOUND, BULLET_IMPACT_SOUND, SHOTGUN_SHOT_SOUND, LMG_SHOT_SOUND, 
  BAT_IMPACT_SOUND, BAT_MISS_SOUND_0, BAT_MISS_SOUND_1, KNIFE_IMPACT_SOUND, KNIFE_MISS_SOUND_0, KNIFE_MISS_SOUND_1,
  NPC_DEATH_0, NPC_DEATH_1, NPC_DEATH_2, NPC_HIT_0, NPC_HIT_1, NPC_HIT_2, EQUIP_SOUND, UNEQUIP_SOUND, SLOW_DOWN_SOUND,
  SPEED_UP_SOUND
}; //GameSoundType
