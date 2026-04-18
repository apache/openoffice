/* Compatibility shim for APPCOMMAND constants added in Vista SDK (SDK 7.0
   guards them with _WIN32_WINNT >= 0x0600; SDK 6.0A exposed them
   unconditionally).  We target XP (_WIN32_WINNT=0x0500) so define them
   ourselves when the SDK omits them. */

#ifndef APPCOMMAND_MICROPHONE_VOLUME_MUTE
#define APPCOMMAND_MICROPHONE_VOLUME_MUTE   24
#endif
#ifndef APPCOMMAND_MICROPHONE_VOLUME_DOWN
#define APPCOMMAND_MICROPHONE_VOLUME_DOWN   25
#endif
#ifndef APPCOMMAND_MICROPHONE_VOLUME_UP
#define APPCOMMAND_MICROPHONE_VOLUME_UP     26
#endif
#ifndef APPCOMMAND_MIC_ON_OFF_TOGGLE
#define APPCOMMAND_MIC_ON_OFF_TOGGLE        44
#endif
#ifndef APPCOMMAND_MEDIA_PLAY
#define APPCOMMAND_MEDIA_PLAY               46
#endif
#ifndef APPCOMMAND_MEDIA_PAUSE
#define APPCOMMAND_MEDIA_PAUSE              47
#endif
#ifndef APPCOMMAND_MEDIA_RECORD
#define APPCOMMAND_MEDIA_RECORD             48
#endif
#ifndef APPCOMMAND_MEDIA_REWIND
#define APPCOMMAND_MEDIA_REWIND             50
#endif
#ifndef APPCOMMAND_MEDIA_CHANNEL_UP
#define APPCOMMAND_MEDIA_CHANNEL_UP         51
#endif
#ifndef APPCOMMAND_MEDIA_CHANNEL_DOWN
#define APPCOMMAND_MEDIA_CHANNEL_DOWN       52
#endif
