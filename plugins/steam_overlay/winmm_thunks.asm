; winmm_thunks.asm  --  GENERATED (regen: see plugins/steam_overlay/README-winmm.md). Do not hand-edit.
; Transparent x64 forwarders for winmm.dll. Each thunk tail-jumps through its slot
; in g_winmm_ptrs[], which winmm_resolve.cpp fills from the REAL System32\winmm.dll in
; DllMain (only when this binary is deployed AS winmm.dll). DllMain runs pre-OEP; the
; game makes its first winmm call after OEP, so the slots are always ready in time.
; x64 has no __declspec(naked), so these must be real assembly.

EXTERN g_winmm_ptrs:QWORD

PUBLIC mciExecute
PUBLIC CloseDriver
PUBLIC DefDriverProc
PUBLIC DriverCallback
PUBLIC DrvGetModuleHandle
PUBLIC GetDriverModuleHandle
PUBLIC OpenDriver
PUBLIC PlaySound
PUBLIC PlaySoundA
PUBLIC PlaySoundW
PUBLIC SendDriverMessage
PUBLIC WOWAppExit
PUBLIC auxGetDevCapsA
PUBLIC auxGetDevCapsW
PUBLIC auxGetNumDevs
PUBLIC auxGetVolume
PUBLIC auxOutMessage
PUBLIC auxSetVolume
PUBLIC joyConfigChanged
PUBLIC joyGetDevCapsA
PUBLIC joyGetDevCapsW
PUBLIC joyGetNumDevs
PUBLIC joyGetPos
PUBLIC joyGetPosEx
PUBLIC joyGetThreshold
PUBLIC joyReleaseCapture
PUBLIC joySetCapture
PUBLIC joySetThreshold
PUBLIC mciDriverNotify
PUBLIC mciDriverYield
PUBLIC mciFreeCommandResource
PUBLIC mciGetCreatorTask
PUBLIC mciGetDeviceIDA
PUBLIC mciGetDeviceIDFromElementIDA
PUBLIC mciGetDeviceIDFromElementIDW
PUBLIC mciGetDeviceIDW
PUBLIC mciGetDriverData
PUBLIC mciGetErrorStringA
PUBLIC mciGetErrorStringW
PUBLIC mciGetYieldProc
PUBLIC mciLoadCommandResource
PUBLIC mciSendCommandA
PUBLIC mciSendCommandW
PUBLIC mciSendStringA
PUBLIC mciSendStringW
PUBLIC mciSetDriverData
PUBLIC mciSetYieldProc
PUBLIC midiConnect
PUBLIC midiDisconnect
PUBLIC midiInAddBuffer
PUBLIC midiInClose
PUBLIC midiInGetDevCapsA
PUBLIC midiInGetDevCapsW
PUBLIC midiInGetErrorTextA
PUBLIC midiInGetErrorTextW
PUBLIC midiInGetID
PUBLIC midiInGetNumDevs
PUBLIC midiInMessage
PUBLIC midiInOpen
PUBLIC midiInPrepareHeader
PUBLIC midiInReset
PUBLIC midiInStart
PUBLIC midiInStop
PUBLIC midiInUnprepareHeader
PUBLIC midiOutCacheDrumPatches
PUBLIC midiOutCachePatches
PUBLIC midiOutClose
PUBLIC midiOutGetDevCapsA
PUBLIC midiOutGetDevCapsW
PUBLIC midiOutGetErrorTextA
PUBLIC midiOutGetErrorTextW
PUBLIC midiOutGetID
PUBLIC midiOutGetNumDevs
PUBLIC midiOutGetVolume
PUBLIC midiOutLongMsg
PUBLIC midiOutMessage
PUBLIC midiOutOpen
PUBLIC midiOutPrepareHeader
PUBLIC midiOutReset
PUBLIC midiOutSetVolume
PUBLIC midiOutShortMsg
PUBLIC midiOutUnprepareHeader
PUBLIC midiStreamClose
PUBLIC midiStreamOpen
PUBLIC midiStreamOut
PUBLIC midiStreamPause
PUBLIC midiStreamPosition
PUBLIC midiStreamProperty
PUBLIC midiStreamRestart
PUBLIC midiStreamStop
PUBLIC mixerClose
PUBLIC mixerGetControlDetailsA
PUBLIC mixerGetControlDetailsW
PUBLIC mixerGetDevCapsA
PUBLIC mixerGetDevCapsW
PUBLIC mixerGetID
PUBLIC mixerGetLineControlsA
PUBLIC mixerGetLineControlsW
PUBLIC mixerGetLineInfoA
PUBLIC mixerGetLineInfoW
PUBLIC mixerGetNumDevs
PUBLIC mixerMessage
PUBLIC mixerOpen
PUBLIC mixerSetControlDetails
PUBLIC mmDrvInstall
PUBLIC mmGetCurrentTask
PUBLIC mmTaskBlock
PUBLIC mmTaskCreate
PUBLIC mmTaskSignal
PUBLIC mmTaskYield
PUBLIC mmioAdvance
PUBLIC mmioAscend
PUBLIC mmioClose
PUBLIC mmioCreateChunk
PUBLIC mmioDescend
PUBLIC mmioFlush
PUBLIC mmioGetInfo
PUBLIC mmioInstallIOProcA
PUBLIC mmioInstallIOProcW
PUBLIC mmioOpenA
PUBLIC mmioOpenW
PUBLIC mmioRead
PUBLIC mmioRenameA
PUBLIC mmioRenameW
PUBLIC mmioSeek
PUBLIC mmioSendMessage
PUBLIC mmioSetBuffer
PUBLIC mmioSetInfo
PUBLIC mmioStringToFOURCCA
PUBLIC mmioStringToFOURCCW
PUBLIC mmioWrite
PUBLIC mmsystemGetVersion
PUBLIC sndPlaySoundA
PUBLIC sndPlaySoundW
PUBLIC timeBeginPeriod
PUBLIC timeEndPeriod
PUBLIC timeGetDevCaps
PUBLIC timeGetSystemTime
PUBLIC timeGetTime
PUBLIC timeKillEvent
PUBLIC timeSetEvent
PUBLIC waveInAddBuffer
PUBLIC waveInClose
PUBLIC waveInGetDevCapsA
PUBLIC waveInGetDevCapsW
PUBLIC waveInGetErrorTextA
PUBLIC waveInGetErrorTextW
PUBLIC waveInGetID
PUBLIC waveInGetNumDevs
PUBLIC waveInGetPosition
PUBLIC waveInMessage
PUBLIC waveInOpen
PUBLIC waveInPrepareHeader
PUBLIC waveInReset
PUBLIC waveInStart
PUBLIC waveInStop
PUBLIC waveInUnprepareHeader
PUBLIC waveOutBreakLoop
PUBLIC waveOutClose
PUBLIC waveOutGetDevCapsA
PUBLIC waveOutGetDevCapsW
PUBLIC waveOutGetErrorTextA
PUBLIC waveOutGetErrorTextW
PUBLIC waveOutGetID
PUBLIC waveOutGetNumDevs
PUBLIC waveOutGetPitch
PUBLIC waveOutGetPlaybackRate
PUBLIC waveOutGetPosition
PUBLIC waveOutGetVolume
PUBLIC waveOutMessage
PUBLIC waveOutOpen
PUBLIC waveOutPause
PUBLIC waveOutPrepareHeader
PUBLIC waveOutReset
PUBLIC waveOutRestart
PUBLIC waveOutSetPitch
PUBLIC waveOutSetPlaybackRate
PUBLIC waveOutSetVolume
PUBLIC waveOutUnprepareHeader
PUBLIC waveOutWrite
PUBLIC PlaySound_ord2

.code

mciExecute PROC
    jmp QWORD PTR [g_winmm_ptrs+0]
mciExecute ENDP
CloseDriver PROC
    jmp QWORD PTR [g_winmm_ptrs+8]
CloseDriver ENDP
DefDriverProc PROC
    jmp QWORD PTR [g_winmm_ptrs+16]
DefDriverProc ENDP
DriverCallback PROC
    jmp QWORD PTR [g_winmm_ptrs+24]
DriverCallback ENDP
DrvGetModuleHandle PROC
    jmp QWORD PTR [g_winmm_ptrs+32]
DrvGetModuleHandle ENDP
GetDriverModuleHandle PROC
    jmp QWORD PTR [g_winmm_ptrs+40]
GetDriverModuleHandle ENDP
OpenDriver PROC
    jmp QWORD PTR [g_winmm_ptrs+48]
OpenDriver ENDP
PlaySound PROC
    jmp QWORD PTR [g_winmm_ptrs+56]
PlaySound ENDP
PlaySoundA PROC
    jmp QWORD PTR [g_winmm_ptrs+64]
PlaySoundA ENDP
PlaySoundW PROC
    jmp QWORD PTR [g_winmm_ptrs+72]
PlaySoundW ENDP
SendDriverMessage PROC
    jmp QWORD PTR [g_winmm_ptrs+80]
SendDriverMessage ENDP
WOWAppExit PROC
    jmp QWORD PTR [g_winmm_ptrs+88]
WOWAppExit ENDP
auxGetDevCapsA PROC
    jmp QWORD PTR [g_winmm_ptrs+96]
auxGetDevCapsA ENDP
auxGetDevCapsW PROC
    jmp QWORD PTR [g_winmm_ptrs+104]
auxGetDevCapsW ENDP
auxGetNumDevs PROC
    jmp QWORD PTR [g_winmm_ptrs+112]
auxGetNumDevs ENDP
auxGetVolume PROC
    jmp QWORD PTR [g_winmm_ptrs+120]
auxGetVolume ENDP
auxOutMessage PROC
    jmp QWORD PTR [g_winmm_ptrs+128]
auxOutMessage ENDP
auxSetVolume PROC
    jmp QWORD PTR [g_winmm_ptrs+136]
auxSetVolume ENDP
joyConfigChanged PROC
    jmp QWORD PTR [g_winmm_ptrs+144]
joyConfigChanged ENDP
joyGetDevCapsA PROC
    jmp QWORD PTR [g_winmm_ptrs+152]
joyGetDevCapsA ENDP
joyGetDevCapsW PROC
    jmp QWORD PTR [g_winmm_ptrs+160]
joyGetDevCapsW ENDP
joyGetNumDevs PROC
    jmp QWORD PTR [g_winmm_ptrs+168]
joyGetNumDevs ENDP
joyGetPos PROC
    jmp QWORD PTR [g_winmm_ptrs+176]
joyGetPos ENDP
joyGetPosEx PROC
    jmp QWORD PTR [g_winmm_ptrs+184]
joyGetPosEx ENDP
joyGetThreshold PROC
    jmp QWORD PTR [g_winmm_ptrs+192]
joyGetThreshold ENDP
joyReleaseCapture PROC
    jmp QWORD PTR [g_winmm_ptrs+200]
joyReleaseCapture ENDP
joySetCapture PROC
    jmp QWORD PTR [g_winmm_ptrs+208]
joySetCapture ENDP
joySetThreshold PROC
    jmp QWORD PTR [g_winmm_ptrs+216]
joySetThreshold ENDP
mciDriverNotify PROC
    jmp QWORD PTR [g_winmm_ptrs+224]
mciDriverNotify ENDP
mciDriverYield PROC
    jmp QWORD PTR [g_winmm_ptrs+232]
mciDriverYield ENDP
mciFreeCommandResource PROC
    jmp QWORD PTR [g_winmm_ptrs+240]
mciFreeCommandResource ENDP
mciGetCreatorTask PROC
    jmp QWORD PTR [g_winmm_ptrs+248]
mciGetCreatorTask ENDP
mciGetDeviceIDA PROC
    jmp QWORD PTR [g_winmm_ptrs+256]
mciGetDeviceIDA ENDP
mciGetDeviceIDFromElementIDA PROC
    jmp QWORD PTR [g_winmm_ptrs+264]
mciGetDeviceIDFromElementIDA ENDP
mciGetDeviceIDFromElementIDW PROC
    jmp QWORD PTR [g_winmm_ptrs+272]
mciGetDeviceIDFromElementIDW ENDP
mciGetDeviceIDW PROC
    jmp QWORD PTR [g_winmm_ptrs+280]
mciGetDeviceIDW ENDP
mciGetDriverData PROC
    jmp QWORD PTR [g_winmm_ptrs+288]
mciGetDriverData ENDP
mciGetErrorStringA PROC
    jmp QWORD PTR [g_winmm_ptrs+296]
mciGetErrorStringA ENDP
mciGetErrorStringW PROC
    jmp QWORD PTR [g_winmm_ptrs+304]
mciGetErrorStringW ENDP
mciGetYieldProc PROC
    jmp QWORD PTR [g_winmm_ptrs+312]
mciGetYieldProc ENDP
mciLoadCommandResource PROC
    jmp QWORD PTR [g_winmm_ptrs+320]
mciLoadCommandResource ENDP
mciSendCommandA PROC
    jmp QWORD PTR [g_winmm_ptrs+328]
mciSendCommandA ENDP
mciSendCommandW PROC
    jmp QWORD PTR [g_winmm_ptrs+336]
mciSendCommandW ENDP
mciSendStringA PROC
    jmp QWORD PTR [g_winmm_ptrs+344]
mciSendStringA ENDP
mciSendStringW PROC
    jmp QWORD PTR [g_winmm_ptrs+352]
mciSendStringW ENDP
mciSetDriverData PROC
    jmp QWORD PTR [g_winmm_ptrs+360]
mciSetDriverData ENDP
mciSetYieldProc PROC
    jmp QWORD PTR [g_winmm_ptrs+368]
mciSetYieldProc ENDP
midiConnect PROC
    jmp QWORD PTR [g_winmm_ptrs+376]
midiConnect ENDP
midiDisconnect PROC
    jmp QWORD PTR [g_winmm_ptrs+384]
midiDisconnect ENDP
midiInAddBuffer PROC
    jmp QWORD PTR [g_winmm_ptrs+392]
midiInAddBuffer ENDP
midiInClose PROC
    jmp QWORD PTR [g_winmm_ptrs+400]
midiInClose ENDP
midiInGetDevCapsA PROC
    jmp QWORD PTR [g_winmm_ptrs+408]
midiInGetDevCapsA ENDP
midiInGetDevCapsW PROC
    jmp QWORD PTR [g_winmm_ptrs+416]
midiInGetDevCapsW ENDP
midiInGetErrorTextA PROC
    jmp QWORD PTR [g_winmm_ptrs+424]
midiInGetErrorTextA ENDP
midiInGetErrorTextW PROC
    jmp QWORD PTR [g_winmm_ptrs+432]
midiInGetErrorTextW ENDP
midiInGetID PROC
    jmp QWORD PTR [g_winmm_ptrs+440]
midiInGetID ENDP
midiInGetNumDevs PROC
    jmp QWORD PTR [g_winmm_ptrs+448]
midiInGetNumDevs ENDP
midiInMessage PROC
    jmp QWORD PTR [g_winmm_ptrs+456]
midiInMessage ENDP
midiInOpen PROC
    jmp QWORD PTR [g_winmm_ptrs+464]
midiInOpen ENDP
midiInPrepareHeader PROC
    jmp QWORD PTR [g_winmm_ptrs+472]
midiInPrepareHeader ENDP
midiInReset PROC
    jmp QWORD PTR [g_winmm_ptrs+480]
midiInReset ENDP
midiInStart PROC
    jmp QWORD PTR [g_winmm_ptrs+488]
midiInStart ENDP
midiInStop PROC
    jmp QWORD PTR [g_winmm_ptrs+496]
midiInStop ENDP
midiInUnprepareHeader PROC
    jmp QWORD PTR [g_winmm_ptrs+504]
midiInUnprepareHeader ENDP
midiOutCacheDrumPatches PROC
    jmp QWORD PTR [g_winmm_ptrs+512]
midiOutCacheDrumPatches ENDP
midiOutCachePatches PROC
    jmp QWORD PTR [g_winmm_ptrs+520]
midiOutCachePatches ENDP
midiOutClose PROC
    jmp QWORD PTR [g_winmm_ptrs+528]
midiOutClose ENDP
midiOutGetDevCapsA PROC
    jmp QWORD PTR [g_winmm_ptrs+536]
midiOutGetDevCapsA ENDP
midiOutGetDevCapsW PROC
    jmp QWORD PTR [g_winmm_ptrs+544]
midiOutGetDevCapsW ENDP
midiOutGetErrorTextA PROC
    jmp QWORD PTR [g_winmm_ptrs+552]
midiOutGetErrorTextA ENDP
midiOutGetErrorTextW PROC
    jmp QWORD PTR [g_winmm_ptrs+560]
midiOutGetErrorTextW ENDP
midiOutGetID PROC
    jmp QWORD PTR [g_winmm_ptrs+568]
midiOutGetID ENDP
midiOutGetNumDevs PROC
    jmp QWORD PTR [g_winmm_ptrs+576]
midiOutGetNumDevs ENDP
midiOutGetVolume PROC
    jmp QWORD PTR [g_winmm_ptrs+584]
midiOutGetVolume ENDP
midiOutLongMsg PROC
    jmp QWORD PTR [g_winmm_ptrs+592]
midiOutLongMsg ENDP
midiOutMessage PROC
    jmp QWORD PTR [g_winmm_ptrs+600]
midiOutMessage ENDP
midiOutOpen PROC
    jmp QWORD PTR [g_winmm_ptrs+608]
midiOutOpen ENDP
midiOutPrepareHeader PROC
    jmp QWORD PTR [g_winmm_ptrs+616]
midiOutPrepareHeader ENDP
midiOutReset PROC
    jmp QWORD PTR [g_winmm_ptrs+624]
midiOutReset ENDP
midiOutSetVolume PROC
    jmp QWORD PTR [g_winmm_ptrs+632]
midiOutSetVolume ENDP
midiOutShortMsg PROC
    jmp QWORD PTR [g_winmm_ptrs+640]
midiOutShortMsg ENDP
midiOutUnprepareHeader PROC
    jmp QWORD PTR [g_winmm_ptrs+648]
midiOutUnprepareHeader ENDP
midiStreamClose PROC
    jmp QWORD PTR [g_winmm_ptrs+656]
midiStreamClose ENDP
midiStreamOpen PROC
    jmp QWORD PTR [g_winmm_ptrs+664]
midiStreamOpen ENDP
midiStreamOut PROC
    jmp QWORD PTR [g_winmm_ptrs+672]
midiStreamOut ENDP
midiStreamPause PROC
    jmp QWORD PTR [g_winmm_ptrs+680]
midiStreamPause ENDP
midiStreamPosition PROC
    jmp QWORD PTR [g_winmm_ptrs+688]
midiStreamPosition ENDP
midiStreamProperty PROC
    jmp QWORD PTR [g_winmm_ptrs+696]
midiStreamProperty ENDP
midiStreamRestart PROC
    jmp QWORD PTR [g_winmm_ptrs+704]
midiStreamRestart ENDP
midiStreamStop PROC
    jmp QWORD PTR [g_winmm_ptrs+712]
midiStreamStop ENDP
mixerClose PROC
    jmp QWORD PTR [g_winmm_ptrs+720]
mixerClose ENDP
mixerGetControlDetailsA PROC
    jmp QWORD PTR [g_winmm_ptrs+728]
mixerGetControlDetailsA ENDP
mixerGetControlDetailsW PROC
    jmp QWORD PTR [g_winmm_ptrs+736]
mixerGetControlDetailsW ENDP
mixerGetDevCapsA PROC
    jmp QWORD PTR [g_winmm_ptrs+744]
mixerGetDevCapsA ENDP
mixerGetDevCapsW PROC
    jmp QWORD PTR [g_winmm_ptrs+752]
mixerGetDevCapsW ENDP
mixerGetID PROC
    jmp QWORD PTR [g_winmm_ptrs+760]
mixerGetID ENDP
mixerGetLineControlsA PROC
    jmp QWORD PTR [g_winmm_ptrs+768]
mixerGetLineControlsA ENDP
mixerGetLineControlsW PROC
    jmp QWORD PTR [g_winmm_ptrs+776]
mixerGetLineControlsW ENDP
mixerGetLineInfoA PROC
    jmp QWORD PTR [g_winmm_ptrs+784]
mixerGetLineInfoA ENDP
mixerGetLineInfoW PROC
    jmp QWORD PTR [g_winmm_ptrs+792]
mixerGetLineInfoW ENDP
mixerGetNumDevs PROC
    jmp QWORD PTR [g_winmm_ptrs+800]
mixerGetNumDevs ENDP
mixerMessage PROC
    jmp QWORD PTR [g_winmm_ptrs+808]
mixerMessage ENDP
mixerOpen PROC
    jmp QWORD PTR [g_winmm_ptrs+816]
mixerOpen ENDP
mixerSetControlDetails PROC
    jmp QWORD PTR [g_winmm_ptrs+824]
mixerSetControlDetails ENDP
mmDrvInstall PROC
    jmp QWORD PTR [g_winmm_ptrs+832]
mmDrvInstall ENDP
mmGetCurrentTask PROC
    jmp QWORD PTR [g_winmm_ptrs+840]
mmGetCurrentTask ENDP
mmTaskBlock PROC
    jmp QWORD PTR [g_winmm_ptrs+848]
mmTaskBlock ENDP
mmTaskCreate PROC
    jmp QWORD PTR [g_winmm_ptrs+856]
mmTaskCreate ENDP
mmTaskSignal PROC
    jmp QWORD PTR [g_winmm_ptrs+864]
mmTaskSignal ENDP
mmTaskYield PROC
    jmp QWORD PTR [g_winmm_ptrs+872]
mmTaskYield ENDP
mmioAdvance PROC
    jmp QWORD PTR [g_winmm_ptrs+880]
mmioAdvance ENDP
mmioAscend PROC
    jmp QWORD PTR [g_winmm_ptrs+888]
mmioAscend ENDP
mmioClose PROC
    jmp QWORD PTR [g_winmm_ptrs+896]
mmioClose ENDP
mmioCreateChunk PROC
    jmp QWORD PTR [g_winmm_ptrs+904]
mmioCreateChunk ENDP
mmioDescend PROC
    jmp QWORD PTR [g_winmm_ptrs+912]
mmioDescend ENDP
mmioFlush PROC
    jmp QWORD PTR [g_winmm_ptrs+920]
mmioFlush ENDP
mmioGetInfo PROC
    jmp QWORD PTR [g_winmm_ptrs+928]
mmioGetInfo ENDP
mmioInstallIOProcA PROC
    jmp QWORD PTR [g_winmm_ptrs+936]
mmioInstallIOProcA ENDP
mmioInstallIOProcW PROC
    jmp QWORD PTR [g_winmm_ptrs+944]
mmioInstallIOProcW ENDP
mmioOpenA PROC
    jmp QWORD PTR [g_winmm_ptrs+952]
mmioOpenA ENDP
mmioOpenW PROC
    jmp QWORD PTR [g_winmm_ptrs+960]
mmioOpenW ENDP
mmioRead PROC
    jmp QWORD PTR [g_winmm_ptrs+968]
mmioRead ENDP
mmioRenameA PROC
    jmp QWORD PTR [g_winmm_ptrs+976]
mmioRenameA ENDP
mmioRenameW PROC
    jmp QWORD PTR [g_winmm_ptrs+984]
mmioRenameW ENDP
mmioSeek PROC
    jmp QWORD PTR [g_winmm_ptrs+992]
mmioSeek ENDP
mmioSendMessage PROC
    jmp QWORD PTR [g_winmm_ptrs+1000]
mmioSendMessage ENDP
mmioSetBuffer PROC
    jmp QWORD PTR [g_winmm_ptrs+1008]
mmioSetBuffer ENDP
mmioSetInfo PROC
    jmp QWORD PTR [g_winmm_ptrs+1016]
mmioSetInfo ENDP
mmioStringToFOURCCA PROC
    jmp QWORD PTR [g_winmm_ptrs+1024]
mmioStringToFOURCCA ENDP
mmioStringToFOURCCW PROC
    jmp QWORD PTR [g_winmm_ptrs+1032]
mmioStringToFOURCCW ENDP
mmioWrite PROC
    jmp QWORD PTR [g_winmm_ptrs+1040]
mmioWrite ENDP
mmsystemGetVersion PROC
    jmp QWORD PTR [g_winmm_ptrs+1048]
mmsystemGetVersion ENDP
sndPlaySoundA PROC
    jmp QWORD PTR [g_winmm_ptrs+1056]
sndPlaySoundA ENDP
sndPlaySoundW PROC
    jmp QWORD PTR [g_winmm_ptrs+1064]
sndPlaySoundW ENDP
timeBeginPeriod PROC
    jmp QWORD PTR [g_winmm_ptrs+1072]
timeBeginPeriod ENDP
timeEndPeriod PROC
    jmp QWORD PTR [g_winmm_ptrs+1080]
timeEndPeriod ENDP
timeGetDevCaps PROC
    jmp QWORD PTR [g_winmm_ptrs+1088]
timeGetDevCaps ENDP
timeGetSystemTime PROC
    jmp QWORD PTR [g_winmm_ptrs+1096]
timeGetSystemTime ENDP
timeGetTime PROC
    jmp QWORD PTR [g_winmm_ptrs+1104]
timeGetTime ENDP
timeKillEvent PROC
    jmp QWORD PTR [g_winmm_ptrs+1112]
timeKillEvent ENDP
timeSetEvent PROC
    jmp QWORD PTR [g_winmm_ptrs+1120]
timeSetEvent ENDP
waveInAddBuffer PROC
    jmp QWORD PTR [g_winmm_ptrs+1128]
waveInAddBuffer ENDP
waveInClose PROC
    jmp QWORD PTR [g_winmm_ptrs+1136]
waveInClose ENDP
waveInGetDevCapsA PROC
    jmp QWORD PTR [g_winmm_ptrs+1144]
waveInGetDevCapsA ENDP
waveInGetDevCapsW PROC
    jmp QWORD PTR [g_winmm_ptrs+1152]
waveInGetDevCapsW ENDP
waveInGetErrorTextA PROC
    jmp QWORD PTR [g_winmm_ptrs+1160]
waveInGetErrorTextA ENDP
waveInGetErrorTextW PROC
    jmp QWORD PTR [g_winmm_ptrs+1168]
waveInGetErrorTextW ENDP
waveInGetID PROC
    jmp QWORD PTR [g_winmm_ptrs+1176]
waveInGetID ENDP
waveInGetNumDevs PROC
    jmp QWORD PTR [g_winmm_ptrs+1184]
waveInGetNumDevs ENDP
waveInGetPosition PROC
    jmp QWORD PTR [g_winmm_ptrs+1192]
waveInGetPosition ENDP
waveInMessage PROC
    jmp QWORD PTR [g_winmm_ptrs+1200]
waveInMessage ENDP
waveInOpen PROC
    jmp QWORD PTR [g_winmm_ptrs+1208]
waveInOpen ENDP
waveInPrepareHeader PROC
    jmp QWORD PTR [g_winmm_ptrs+1216]
waveInPrepareHeader ENDP
waveInReset PROC
    jmp QWORD PTR [g_winmm_ptrs+1224]
waveInReset ENDP
waveInStart PROC
    jmp QWORD PTR [g_winmm_ptrs+1232]
waveInStart ENDP
waveInStop PROC
    jmp QWORD PTR [g_winmm_ptrs+1240]
waveInStop ENDP
waveInUnprepareHeader PROC
    jmp QWORD PTR [g_winmm_ptrs+1248]
waveInUnprepareHeader ENDP
waveOutBreakLoop PROC
    jmp QWORD PTR [g_winmm_ptrs+1256]
waveOutBreakLoop ENDP
waveOutClose PROC
    jmp QWORD PTR [g_winmm_ptrs+1264]
waveOutClose ENDP
waveOutGetDevCapsA PROC
    jmp QWORD PTR [g_winmm_ptrs+1272]
waveOutGetDevCapsA ENDP
waveOutGetDevCapsW PROC
    jmp QWORD PTR [g_winmm_ptrs+1280]
waveOutGetDevCapsW ENDP
waveOutGetErrorTextA PROC
    jmp QWORD PTR [g_winmm_ptrs+1288]
waveOutGetErrorTextA ENDP
waveOutGetErrorTextW PROC
    jmp QWORD PTR [g_winmm_ptrs+1296]
waveOutGetErrorTextW ENDP
waveOutGetID PROC
    jmp QWORD PTR [g_winmm_ptrs+1304]
waveOutGetID ENDP
waveOutGetNumDevs PROC
    jmp QWORD PTR [g_winmm_ptrs+1312]
waveOutGetNumDevs ENDP
waveOutGetPitch PROC
    jmp QWORD PTR [g_winmm_ptrs+1320]
waveOutGetPitch ENDP
waveOutGetPlaybackRate PROC
    jmp QWORD PTR [g_winmm_ptrs+1328]
waveOutGetPlaybackRate ENDP
waveOutGetPosition PROC
    jmp QWORD PTR [g_winmm_ptrs+1336]
waveOutGetPosition ENDP
waveOutGetVolume PROC
    jmp QWORD PTR [g_winmm_ptrs+1344]
waveOutGetVolume ENDP
waveOutMessage PROC
    jmp QWORD PTR [g_winmm_ptrs+1352]
waveOutMessage ENDP
waveOutOpen PROC
    jmp QWORD PTR [g_winmm_ptrs+1360]
waveOutOpen ENDP
waveOutPause PROC
    jmp QWORD PTR [g_winmm_ptrs+1368]
waveOutPause ENDP
waveOutPrepareHeader PROC
    jmp QWORD PTR [g_winmm_ptrs+1376]
waveOutPrepareHeader ENDP
waveOutReset PROC
    jmp QWORD PTR [g_winmm_ptrs+1384]
waveOutReset ENDP
waveOutRestart PROC
    jmp QWORD PTR [g_winmm_ptrs+1392]
waveOutRestart ENDP
waveOutSetPitch PROC
    jmp QWORD PTR [g_winmm_ptrs+1400]
waveOutSetPitch ENDP
waveOutSetPlaybackRate PROC
    jmp QWORD PTR [g_winmm_ptrs+1408]
waveOutSetPlaybackRate ENDP
waveOutSetVolume PROC
    jmp QWORD PTR [g_winmm_ptrs+1416]
waveOutSetVolume ENDP
waveOutUnprepareHeader PROC
    jmp QWORD PTR [g_winmm_ptrs+1424]
waveOutUnprepareHeader ENDP
waveOutWrite PROC
    jmp QWORD PTR [g_winmm_ptrs+1432]
waveOutWrite ENDP
; ordinal-2 NONAME alias of PlaySound (identical entry in the real winmm)
PlaySound_ord2 PROC
    jmp QWORD PTR [g_winmm_ptrs+56]
PlaySound_ord2 ENDP

END
