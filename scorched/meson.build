project(
	'Scorched3D',
	'cpp', 'c',
	version: '44.2',
	default_options : [
		'c_std=c11',
		'cpp_std=c++11'
	],
	meson_version: '>=0.60.0',
)

# Compilers
cc  = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')

# BEGIN Dependencies
expat_dep       = dependency(
	'expat',
	fallback : 'expat',
)

if   target_machine.system() == 'darwin'
fftw_dep        = dependency('fftw3')
elif target_machine.system() == 'windows'
fftw_dep        = cc.find_library(
	'fftw3',
	dirs : [meson.global_source_root() / '../scorched-dep-win32/lib/fftw3'],
)
else
fftw_dep        = dependency('fftw3f')
endif

if   target_machine.system() == 'windows'
freealut_dep    = cc.find_library(
	'Alut',
	dirs : [meson.global_source_root() / '../scorched-dep-win32/lib/al'],
)
else
freealut_dep    = dependency(
	'freealut',
	required : target_machine.system() != 'darwin',
)
endif

ft2_dep         = dependency(
	'freetype2',
	fallback : 'freetype2',
	version : ['>= 2.5.1'],
)

gl_dep          = dependency(
	'gl',
	required : target_machine.system() != 'darwin',
)

glew_dep        = dependency(
	'glew',
	fallback : 'glew',
)

glu_dep         = dependency(
	'glu',
	required : target_machine.system() == 'linux',
)

jpeg_dep        = dependency(
	'libjpeg', 'libjpeg-turbo',
	fallback : 'libjpeg-turbo',
	required : target_machine.system() != 'darwin',
)

lua_dep         = dependency(
	'lua-5.1', 'lua5.1', 'lua-5.4', 'lua5.4',
	version : [ '>= 5.1' ],
	fallback : 'lua',
)

ogg_dep         = dependency(
	'ogg',
	fallback : 'ogg',
)

if   target_machine.system() == 'windows'
openal_dep    = cc.find_library(
	'OpenAl32',
	dirs : [meson.global_source_root() / '../scorched-dep-win32/lib/al'],
)
else
openal_dep      = dependency(
	'openal',
	version : ['>= 1.17.2'],
	required : target_machine.system() != 'darwin',
)
endif

png_dep         = dependency(
	'libpng',
	fallback : 'libpng',
	required : target_machine.system() != 'darwin',
)

if   target_machine.system() == 'windows'
sdl_dep         = cc.find_library(
	'SDL',
	dirs : [meson.global_source_root() / '../scorched-dep-win32/lib/SDL'],
)
else
sdl_dep         = dependency('sdl')
endif

if   target_machine.system() == 'windows'
sdl_net_dep     = cc.find_library(
	'SDL_net',
	dirs : [meson.global_source_root() / '../scorched-dep-win32/lib/SDL'],
)
else
sdl_net_dep     = dependency('SDL_net')
endif

if   target_machine.system() == 'windows'
vorbis_dep    = cc.find_library(
	'vorbis',
	dirs : [meson.global_source_root() / '../scorched-dep-win32/lib/ogg'],
)
else
vorbis_dep      = dependency(
	'vorbis',
	fallback : 'vorbis',
)
endif

if   target_machine.system() == 'windows'
vorbisfile_dep    = cc.find_library(
	'vorbisfile',
	dirs : [meson.global_source_root() / '../scorched-dep-win32/lib/ogg'],
)
else
vorbisfile_dep  = dependency(
	'vorbisfile',
	fallback : 'vorbisfile',
)
endif

if   target_machine.system() == 'windows'
wxwidgets_dep   = cc.find_library(
	'wxmsw',
	dirs : [meson.global_source_root() / '../scorched-dep-win32/lib/wx'],
)
else
wxwidgets_dep   = dependency(
	'wxwidgets',
	version : ['>= 2.4.0'],
)
endif

zlib_dep        = dependency(
	'zlib',
	fallback : 'zlib',
)
# END   Dependencies

# BEGIN Defines and compile options
c_defines = []
cpp_defines = [
	'-DS3D_BINDIR="'  + get_option('prefix') / get_option('bindir')  + '"',
	'-DS3D_DATADIR="' + get_option('prefix') / get_option('datadir') + '"',
	'-DS3D_DOCDIR="'  + get_option('prefix') / get_option('docdir')  + '"',
]

# Functions
if cc.has_function('snprintf')
	c_defines   += '-DHAVE_SNPRINTF'
	cpp_defines += '-DHAVE_SNPRINTF'
endif

if cc.has_function('vsnprintf')
	c_defines   += '-DHAVE_VSNPRINTF'
endif

if cc.has_function('asprintf')
	c_defines   += '-DHAVE_ASPRINTF'
endif

if cc.has_function('vasprintf')
	c_defines   += '-DHAVE_VASPRINTF'
endif

# Headers
if cxx.check_header('unistd.h')
	cpp_defines += '-DHAVE_UNISTD_H'
endif

# Libraries
if ogg_dep.found()
	cpp_defines += '-DHAVE_OGG'
endif

# Build Type
if   get_option('buildtype') == 'release' or get_option('buildtype') == 'debugoptimized'
	# Known Warnings
	cpp_options = [
		'-Wno-class-memaccess',
		'-Wno-deprecated-declarations',
		'-Wno-format-truncation',
		'-Wno-int-to-pointer-cast',
		'-Wno-unused-but-set-variable',
		'-Wno-unused-function',
		'-Wno-unused-variable',
	]
else
	cpp_options = []
endif

# Binaries
launcher_c_args   = c_defines
launcher_cpp_args = [
	cpp_defines,
	cpp_options,
	'-DS3D_BROWSER="' + get_option('browser')  + '"'
]

client_c_args     = c_defines
client_cpp_args   = [
	cpp_defines,
	cpp_options,
	'-DS3D_BROWSER="' + get_option('browser')  + '"'
]

server_c_args     = c_defines
server_cpp_args   = [
	cpp_defines,
	cpp_options,
	'-DS3D_SERVER=1'
]
# END   Defines and compile options

# BEGIN Sources

# BEGIN Common sources
scorched3d_common_generic_src = files(
	'src/common/XML/XMLFile.cpp' ,
	'src/common/XML/XMLNode.cpp' ,
	'src/common/XML/XMLParser.cpp' ,
	'src/common/XML/XMLStringBuffer.cpp' ,
	'src/common/common/ARGParser.cpp' ,
	'src/common/common/Clock.cpp' ,
	'src/common/common/Counter.cpp' ,
	'src/common/common/Defines.cpp' ,
	'src/common/common/DefinesAssert.cpp' ,
	'src/common/common/DefinesFile.cpp' ,
	'src/common/common/DefinesMath.cpp' ,
	'src/common/common/DefinesScorched.cpp' ,
	'src/common/common/DefinesString.cpp' ,
	'src/common/common/FileLines.cpp' ,
	'src/common/common/FileList.cpp' ,
	'src/common/common/FileLogger.cpp' ,
	'src/common/common/FixedVector.cpp' ,
	'src/common/common/FixedVector4.cpp' ,
	'src/common/common/Keyboard.cpp' ,
	'src/common/common/KeyboardKey.cpp' ,
	'src/common/common/Logger.cpp' ,
	'src/common/common/LoggerI.cpp' ,
	'src/common/common/NumberParser.cpp' ,
	'src/common/common/OptionEntry.cpp' ,
	'src/common/common/OptionsGame.cpp' ,
	'src/common/common/OptionsParameters.cpp' ,
	'src/common/common/ProgressCounter.cpp' ,
	'src/common/common/Vector.cpp' ,
	'src/common/common/Vector4.cpp' ,
	'src/common/common/VectorLib.cpp' ,
	'src/common/common/fixed.cpp' ,
	'src/common/common/snprintf.c' ,
	'src/common/engine/ModDirs.cpp' ,
	'src/common/engine/ModFileEntry.cpp' ,
	'src/common/engine/ModFileEntryLoader.cpp' ,
	'src/common/engine/ModFiles.cpp' ,
	'src/common/engine/ModInfo.cpp' ,
	'src/common/landscapedef/LandscapeDefinitionsBase.cpp' ,
	'src/common/lang/Lang.cpp' ,
	'src/common/lang/LangImpl.cpp' ,
	'src/common/lang/LangParam.cpp' ,
	'src/common/lang/LangResource.cpp' ,
	'src/common/lang/LangString.cpp' ,
	'src/common/lang/ResourceBundle.cpp' ,
	'src/common/lang/ResourceBundleEntry.cpp' ,
	'src/common/lang/ResourceBundleEntryImpl.cpp' ,
	'src/common/net/NetBuffer.cpp' ,
	'src/common/net/NetBufferPool.cpp' ,
	'src/common/net/NetBufferUtil.cpp' ,
	'src/common/net/NetInterface.cpp' ,
	'src/common/net/NetLoopBack.cpp' ,
	'src/common/net/NetMessage.cpp' ,
	'src/common/net/NetMessageHandler.cpp' ,
	'src/common/net/NetMessagePool.cpp' ,
)
# END   Common sources

# BEGIN Common sources for Client and Server
scorched3d_common_cli_srv_src = files(
	'src/common/3dsparse/ASEModelFactory.cpp' ,
	'src/common/3dsparse/Bone.cpp' ,
	'src/common/3dsparse/Face.cpp' ,
	'src/common/3dsparse/MSModelFactory.cpp' ,
	'src/common/3dsparse/Mesh.cpp' ,
	'src/common/3dsparse/Model.cpp' ,
	'src/common/3dsparse/ModelMaths.cpp' ,
	'src/common/3dsparse/ModelStore.cpp' ,
	'src/common/3dsparse/TreeModelFactory.cpp' ,
	'src/common/3dsparse/Vertex.cpp' ,
	'src/common/3dsparse/aseFile.lex.cpp' ,
	'src/common/3dsparse/aseFile.tab.cpp' ,
	'src/common/XML/XMLDiff.cpp' ,
	'src/common/XML/XmlNamedNetBuffer.cpp' ,
	'src/common/actions/Action.cpp' ,
	'src/common/actions/ActionParams.cpp' ,
	'src/common/actions/AddTarget.cpp' ,
	'src/common/actions/CallbackWeapon.cpp' ,
	'src/common/actions/CameraPositionAction.cpp' ,
	'src/common/actions/Explosion.cpp' ,
	'src/common/actions/ExplosionParams.cpp' ,
	'src/common/actions/Laser.cpp' ,
	'src/common/actions/LaserParams.cpp' ,
	'src/common/actions/Lightning.cpp' ,
	'src/common/actions/Napalm.cpp' ,
	'src/common/actions/NapalmParams.cpp' ,
	'src/common/actions/ParticleGroup.cpp' ,
	'src/common/actions/Resurrection.cpp' ,
	'src/common/actions/ShieldHit.cpp' ,
	'src/common/actions/ShotBounce.cpp' ,
	'src/common/actions/ShotProjectile.cpp' ,
	'src/common/actions/ShowScoreAction.cpp' ,
	'src/common/actions/SkyFlash.cpp' ,
	'src/common/actions/SoundAction.cpp' ,
	'src/common/actions/TankRemove.cpp' ,
	'src/common/actions/TankSay.cpp' ,
	'src/common/actions/TanketMovement.cpp' ,
	'src/common/actions/TanketResign.cpp' ,
	'src/common/actions/TargetFalling.cpp' ,
	'src/common/actions/Teleport.cpp' ,
	'src/common/common/ChannelManager.cpp' ,
	'src/common/common/ChannelText.cpp' ,
	'src/common/common/ChannelTextParser.cpp' ,
	'src/common/common/Line.cpp' ,
	'src/common/common/ModelID.cpp' ,
	'src/common/common/OptionsMasterListServer.cpp' ,
	'src/common/common/OptionsScorched.cpp' ,
	'src/common/common/OptionsTransient.cpp' ,
	'src/common/common/RandomGenerator.cpp' ,
	'src/common/common/RollingAverage.cpp' ,
	'src/common/common/SplineCurve.cpp' ,
	'src/common/common/SplinePath.cpp' ,
	'src/common/common/ToolTip.cpp' ,
	'src/common/common/Triangle.cpp' ,
	'src/common/common/sha2.cpp' ,
	'src/common/coms/ComsAdminMessage.cpp' ,
	'src/common/coms/ComsAdminResultMessage.cpp' ,
	'src/common/coms/ComsBuyAccessoryMessage.cpp' ,
	'src/common/coms/ComsChannelMessage.cpp' ,
	'src/common/coms/ComsChannelTextMessage.cpp' ,
	'src/common/coms/ComsConnectAcceptMessage.cpp' ,
	'src/common/coms/ComsConnectAuthMessage.cpp' ,
	'src/common/coms/ComsConnectMessage.cpp' ,
	'src/common/coms/ComsConnectRejectMessage.cpp' ,
	'src/common/coms/ComsDefenseMessage.cpp' ,
	'src/common/coms/ComsFileAkMessage.cpp' ,
	'src/common/coms/ComsFileMessage.cpp' ,
	'src/common/coms/ComsGameStoppedMessage.cpp' ,
	'src/common/coms/ComsGiftMoneyMessage.cpp' ,
	'src/common/coms/ComsHaveModFilesMessage.cpp' ,
	'src/common/coms/ComsInitializeModMessage.cpp' ,
	'src/common/coms/ComsLevelLoadedMessage.cpp' ,
	'src/common/coms/ComsLinesMessage.cpp' ,
	'src/common/coms/ComsLoadLevelMessage.cpp' ,
	'src/common/coms/ComsMessage.cpp' ,
	'src/common/coms/ComsMessageHandler.cpp' ,
	'src/common/coms/ComsMessageSender.cpp' ,
	'src/common/coms/ComsNetStatMessage.cpp' ,
	'src/common/coms/ComsOperationMessage.cpp' ,
	'src/common/coms/ComsOperationResultMessage.cpp' ,
	'src/common/coms/ComsPlayedMoveMessage.cpp' ,
	'src/common/coms/ComsSimulateMessage.cpp' ,
	'src/common/coms/ComsSimulateResultMessage.cpp' ,
	'src/common/coms/ComsSyncCheckMessage.cpp' ,
	'src/common/coms/ComsTankChangeMessage.cpp' ,
	'src/common/engine/ActionController.cpp' ,
	'src/common/engine/EventContainer.cpp' ,
	'src/common/engine/GameState.cpp' ,
	'src/common/engine/GameStateI.cpp' ,
	'src/common/engine/GameStateStimulusI.cpp' ,
	'src/common/engine/MainLoop.cpp' ,
	'src/common/engine/MainLoopI.cpp' ,
	'src/common/engine/MetaClass.cpp' ,
	'src/common/engine/ObjectGroup.cpp' ,
	'src/common/engine/ObjectGroupEntry.cpp' ,
	'src/common/engine/ObjectGroupEntryDefinition.cpp' ,
	'src/common/engine/ObjectGroups.cpp' ,
	'src/common/engine/PhysicsParticle.cpp' ,
	'src/common/engine/PhysicsParticleObject.cpp' ,
	'src/common/engine/SaveGame.cpp' ,
	'src/common/engine/ScorchedCollisionIds.cpp' ,
	'src/common/engine/ScorchedContext.cpp' ,
	'src/common/engine/Simulator.cpp' ,
	'src/common/engine/Wind.cpp' ,
	'src/common/events/EventController.cpp' ,
	'src/common/events/EventHandler.cpp' ,
	'src/common/events/EventHandlerAchievement.cpp' ,
	'src/common/events/EventHandlerAchievementNumberRank.cpp' ,
	'src/common/events/EventHandlerAchievementNumberRankKills.cpp' ,
	'src/common/events/EventHandlerDataBase.cpp' ,
	'src/common/events/EventHandlerDataBaseMySQL.cpp' ,
	'src/common/events/EventHandlerDataBasePGSQL.cpp' ,
	'src/common/image/Image.cpp' ,
	'src/common/image/ImageBitmapFactory.cpp' ,
	'src/common/image/ImageData.cpp' ,
	'src/common/image/ImageFactory.cpp' ,
	'src/common/image/ImageID.cpp' ,
	'src/common/image/ImageJpgFactory.cpp' ,
	'src/common/image/ImageLuminanceFactory.cpp' ,
	'src/common/image/ImagePngFactory.cpp' ,
	'src/common/image/ImageStore.cpp' ,
	'src/common/landscapedef/LandscapeDefinition.cpp' ,
	'src/common/landscapedef/LandscapeDefinitionCache.cpp' ,
	'src/common/landscapedef/LandscapeDefinitions.cpp' ,
	'src/common/landscapedef/LandscapeDefn.cpp' ,
	'src/common/landscapedef/LandscapeDefnTankStart.cpp' ,
	'src/common/landscapedef/LandscapeEvents.cpp' ,
	'src/common/landscapedef/LandscapeInclude.cpp' ,
	'src/common/landscapedef/LandscapeMovement.cpp' ,
	'src/common/landscapedef/LandscapeMusic.cpp' ,
	'src/common/landscapedef/LandscapeOptions.cpp' ,
	'src/common/landscapedef/LandscapeSound.cpp' ,
	'src/common/landscapedef/LandscapeTex.cpp' ,
	'src/common/landscapedef/LandscapeTexDefn.cpp' ,
	'src/common/landscapemap/DeformLandscape.cpp' ,
	'src/common/landscapemap/GroundMaps.cpp' ,
	'src/common/landscapemap/HeightMap.cpp' ,
	'src/common/landscapemap/HeightMapLoader.cpp' ,
	'src/common/landscapemap/HeightMapModifier.cpp' ,
	'src/common/landscapemap/LandscapeMaps.cpp' ,
	'src/common/landscapemap/MovementMap.cpp' ,
	'src/common/landscapemap/NapalmMap.cpp' ,
	'src/common/landscapemap/RoofMaps.cpp' ,
	'src/common/lua/LUAS3DLib.cpp' ,
	'src/common/lua/LUAS3DWeaponLib.cpp' ,
	'src/common/lua/LUAScript.cpp' ,
	'src/common/lua/LUAScriptFactory.cpp' ,
	'src/common/lua/LUAScriptHook.cpp' ,
	'src/common/lua/LUAUtil.cpp' ,
	'src/common/movement/Boid2.cpp' ,
	'src/common/movement/TargetMovement.cpp' ,
	'src/common/movement/TargetMovementEntry.cpp' ,
	'src/common/movement/TargetMovementEntryBoids.cpp' ,
	'src/common/movement/TargetMovementEntryShips.cpp' ,
	'src/common/movement/TargetMovementEntrySpline.cpp' ,
	'src/common/net/NetServerTCP.cpp' ,
	'src/common/net/NetServerTCP3.cpp' ,
	'src/common/net/NetServerTCP3Coms.cpp' ,
	'src/common/net/NetServerTCP3Destination.cpp' ,
	'src/common/net/NetServerTCP3Recv.cpp' ,
	'src/common/net/NetServerTCP3Send.cpp' ,
	'src/common/net/NetServerTCPProtocol.cpp' ,
	'src/common/net/NetServerTCPRead.cpp' ,
	'src/common/placement/PlacementObject.cpp' ,
	'src/common/placement/PlacementObjectGroup.cpp' ,
	'src/common/placement/PlacementObjectHeight.cpp' ,
	'src/common/placement/PlacementObjectNone.cpp' ,
	'src/common/placement/PlacementObjectRandom.cpp' ,
	'src/common/placement/PlacementObjectShadow.cpp' ,
	'src/common/placement/PlacementObjectTarget.cpp' ,
	'src/common/placement/PlacementShadowDefinition.cpp' ,
	'src/common/placement/PlacementTankPosition.cpp' ,
	'src/common/placement/PlacementType.cpp' ,
	'src/common/placement/PlacementTypeBounds.cpp' ,
	'src/common/placement/PlacementTypeCount.cpp' ,
	'src/common/placement/PlacementTypeDirect.cpp' ,
	'src/common/placement/PlacementTypeMask.cpp' ,
	'src/common/placement/PlacementTypeTankStart.cpp' ,
	'src/common/placement/PlacementTypeTree.cpp' ,
	'src/common/simactions/AdminSimAction.cpp' ,
	'src/common/simactions/NextTurnSimAction.cpp' ,
	'src/common/simactions/PlayMovesSimAction.cpp' ,
	'src/common/simactions/RoundStartSimAction.cpp' ,
	'src/common/simactions/RoundStopSimAction.cpp' ,
	'src/common/simactions/ShowScoreSimAction.cpp' ,
	'src/common/simactions/SimAction.cpp' ,
	'src/common/simactions/SyncCheckSimAction.cpp' ,
	'src/common/simactions/TankAccessorySimAction.cpp' ,
	'src/common/simactions/TankAchievementSimAction.cpp' ,
	'src/common/simactions/TankAddSimAction.cpp' ,
	'src/common/simactions/TankBuyingSimAction.cpp' ,
	'src/common/simactions/TankChangeSimAction.cpp' ,
	'src/common/simactions/TankDefenseSimAction.cpp' ,
	'src/common/simactions/TankGiftSimAction.cpp' ,
	'src/common/simactions/TankLoadedSimAction.cpp' ,
	'src/common/simactions/TankNewGameSimAction.cpp' ,
	'src/common/simactions/TankNewMatchSimAction.cpp' ,
	'src/common/simactions/TankRankSimAction.cpp' ,
	'src/common/simactions/TankRemoveSimAction.cpp' ,
	'src/common/simactions/TankStartMoveSimAction.cpp' ,
	'src/common/simactions/TankStopMoveSimAction.cpp' ,
	'src/common/simactions/TankTeamBallanceSimAction.cpp' ,
	'src/common/tank/Tank.cpp' ,
	'src/common/tank/TankAvatar.cpp' ,
	'src/common/tank/TankCamera.cpp' ,
	'src/common/tank/TankColorGenerator.cpp' ,
	'src/common/tank/TankDeadContainer.cpp' ,
	'src/common/tank/TankLib.cpp' ,
	'src/common/tank/TankModel.cpp' ,
	'src/common/tank/TankModelContainer.cpp' ,
	'src/common/tank/TankModelStore.cpp' ,
	'src/common/tank/TankScore.cpp' ,
	'src/common/tank/TankShotHistory.cpp' ,
	'src/common/tank/TankSort.cpp' ,
	'src/common/tank/TankState.cpp' ,
	'src/common/tank/TankTeamScore.cpp' ,
	'src/common/tank/TankViewPoints.cpp' ,
	'src/common/tank/TankWeaponSwitcher.cpp' ,
	'src/common/tanket/Tanket.cpp' ,
	'src/common/tanket/TanketAccessories.cpp' ,
	'src/common/tanket/TanketAutoDefense.cpp' ,
	'src/common/tanket/TanketBatteries.cpp' ,
	'src/common/tanket/TanketShotInfo.cpp' ,
	'src/common/tanket/TanketType.cpp' ,
	'src/common/tanket/TanketTypes.cpp' ,
	'src/common/tanket/TanketWeapon.cpp' ,
	'src/common/target/Target.cpp' ,
	'src/common/target/TargetContainer.cpp' ,
	'src/common/target/TargetDamage.cpp' ,
	'src/common/target/TargetDamageCalc.cpp' ,
	'src/common/target/TargetDefinition.cpp' ,
	'src/common/target/TargetGroup.cpp' ,
	'src/common/target/TargetLife.cpp' ,
	'src/common/target/TargetParachute.cpp' ,
	'src/common/target/TargetRenderer.cpp' ,
	'src/common/target/TargetShield.cpp' ,
	'src/common/target/TargetSpace.cpp' ,
	'src/common/target/TargetSpaceContainment.cpp' ,
	'src/common/target/TargetState.cpp' ,
	'src/common/weapons/Accessory.cpp' ,
	'src/common/weapons/AccessoryCreateContext.cpp' ,
	'src/common/weapons/AccessoryPart.cpp' ,
	'src/common/weapons/AccessoryStore.cpp' ,
	'src/common/weapons/AutoDefense.cpp' ,
	'src/common/weapons/Battery.cpp' ,
	'src/common/weapons/Economy.cpp' ,
	'src/common/weapons/EconomyFreeMarket.cpp' ,
	'src/common/weapons/EconomyNone.cpp' ,
	'src/common/weapons/EconomyStore.cpp' ,
	'src/common/weapons/Parachute.cpp' ,
	'src/common/weapons/Shield.cpp' ,
	'src/common/weapons/ShieldRound.cpp' ,
	'src/common/weapons/ShieldRoundMag.cpp' ,
	'src/common/weapons/ShieldRoundReflective.cpp' ,
	'src/common/weapons/ShieldSquare.cpp' ,
	'src/common/weapons/ShieldSquareReflective.cpp' ,
	'src/common/weapons/Weapon.cpp' ,
	'src/common/weapons/WeaponAddTarget.cpp' ,
	'src/common/weapons/WeaponAimed.cpp' ,
	'src/common/weapons/WeaponAimedOver.cpp' ,
	'src/common/weapons/WeaponAimedUnder.cpp' ,
	'src/common/weapons/WeaponAnimation.cpp' ,
	'src/common/weapons/WeaponCenterPosition.cpp' ,
	'src/common/weapons/WeaponDelay.cpp' ,
	'src/common/weapons/WeaponExplosion.cpp' ,
	'src/common/weapons/WeaponGiveAccessory.cpp' ,
	'src/common/weapons/WeaponGiveLife.cpp' ,
	'src/common/weapons/WeaponGiveLives.cpp' ,
	'src/common/weapons/WeaponGiveMoney.cpp' ,
	'src/common/weapons/WeaponGivePower.cpp' ,
	'src/common/weapons/WeaponGiveScore.cpp' ,
	'src/common/weapons/WeaponGiveWin.cpp' ,
	'src/common/weapons/WeaponGotoLabel.cpp' ,
	'src/common/weapons/WeaponGroupSelect.cpp' ,
	'src/common/weapons/WeaponInvokeWeapon.cpp' ,
	'src/common/weapons/WeaponLabel.cpp' ,
	'src/common/weapons/WeaponLaser.cpp' ,
	'src/common/weapons/WeaponLeapFrog.cpp' ,
	'src/common/weapons/WeaponLightning.cpp' ,
	'src/common/weapons/WeaponMessage.cpp' ,
	'src/common/weapons/WeaponMirv.cpp' ,
	'src/common/weapons/WeaponMoveTank.cpp' ,
	'src/common/weapons/WeaponMulti.cpp' ,
	'src/common/weapons/WeaponMuzzle.cpp' ,
	'src/common/weapons/WeaponNapalm.cpp' ,
	'src/common/weapons/WeaponNull.cpp' ,
	'src/common/weapons/WeaponPosition.cpp' ,
	'src/common/weapons/WeaponProjectile.cpp' ,
	'src/common/weapons/WeaponRandomChoice.cpp' ,
	'src/common/weapons/WeaponRedirect.cpp' ,
	'src/common/weapons/WeaponReference.cpp' ,
	'src/common/weapons/WeaponRepeat.cpp' ,
	'src/common/weapons/WeaponRoller.cpp' ,
	'src/common/weapons/WeaponScatterDirection.cpp' ,
	'src/common/weapons/WeaponScatterPosition.cpp' ,
	'src/common/weapons/WeaponScript.cpp' ,
	'src/common/weapons/WeaponSelectPosition.cpp' ,
	'src/common/weapons/WeaponSkyFlash.cpp' ,
	'src/common/weapons/WeaponSound.cpp' ,
	'src/common/weapons/WeaponTankPosition.cpp' ,
	'src/common/weapons/WeaponTankVelocity.cpp' ,
	'src/common/weapons/WeaponTeamAction.cpp' ,
	'src/common/weapons/WeaponTeleport.cpp' ,
	'src/common/weapons/WeaponTranslate.cpp' ,
	'src/common/weapons/WeaponTypeAction.cpp' ,
	'src/common/weapons/WeaponVelocity.cpp' ,
)
# END   Common sources for Client and Server

# BEGIN Common sources for Client
scorched3d_common_client_src = files(
	'src/common/common/Matrix16.cpp' ,
	'src/common/common/ToolTipResource.cpp' ,
	'src/common/landscapemap/GraphicalHeightMap.cpp' ,
	'src/common/lang/CachedValueString.cpp' ,
)
# END   Common sources for Client

# BEGIN Server common sources
scorched3d_server_generic_src = files(
	'src/server/tankai/TankAINames.cpp' ,
)
# END   Server common sources

# BEGIN Server sources for Client and Server
scorched3d_server_cli_srv_src = files(
	'src/server/igd/igd.cpp' ,
	'src/server/server/ScorchedServer.cpp' ,
	'src/server/server/ScorchedServerSettings.cpp' ,
	'src/server/server/ServerAdminCommon.cpp' ,
	'src/server/server/ServerAdminHandler.cpp' ,
	'src/server/server/ServerAdminSessions.cpp' ,
	'src/server/server/ServerAuthHandler.cpp' ,
	'src/server/server/ServerAuthHandlerDefault.cpp' ,
	'src/server/server/ServerAuthHandlerForumLogin.cpp' ,
	'src/server/server/ServerAuthHandlerMinKills.cpp' ,
	'src/server/server/ServerAuthHandlerPrefered.cpp' ,
	'src/server/server/ServerAuthHandlerStore.cpp' ,
	'src/server/server/ServerBanned.cpp' ,
	'src/server/server/ServerBrowserInfo.cpp' ,
	'src/server/server/ServerBuyAccessoryHandler.cpp' ,
	'src/server/server/ServerChannelAuth.cpp' ,
	'src/server/server/ServerChannelFilter.cpp' ,
	'src/server/server/ServerChannelManager.cpp' ,
	'src/server/server/ServerCommon.cpp' ,
	'src/server/server/ServerConnectAuthHandler.cpp' ,
	'src/server/server/ServerConnectHandler.cpp' ,
	'src/server/server/ServerConsoleLogger.cpp' ,
	'src/server/server/ServerConsoleProgressCounter.cpp' ,
	'src/server/server/ServerDefenseHandler.cpp' ,
	'src/server/server/ServerDestination.cpp' ,
	'src/server/server/ServerDestinations.cpp' ,
	'src/server/server/ServerFileAkHandler.cpp' ,
	'src/server/server/ServerFileServer.cpp' ,
	'src/server/server/ServerGiftMoneyHandler.cpp' ,
	'src/server/server/ServerHandlers.cpp' ,
	'src/server/server/ServerHaveModFilesHandler.cpp' ,
	'src/server/server/ServerInitializeModHandler.cpp' ,
	'src/server/server/ServerLinesHandler.cpp' ,
	'src/server/server/ServerLoadLevel.cpp' ,
	'src/server/server/ServerLog.cpp' ,
	'src/server/server/ServerMain.cpp' ,
	'src/server/server/ServerMessageHandler.cpp' ,
	'src/server/server/ServerOperationResultHandler.cpp' ,
	'src/server/server/ServerParams.cpp' ,
	'src/server/server/ServerPlayedMoveHandler.cpp' ,
	'src/server/server/ServerRegistration.cpp' ,
	'src/server/server/ServerSimulator.cpp' ,
	'src/server/server/ServerState.cpp' ,
	'src/server/server/ServerStateBuying.cpp' ,
	'src/server/server/ServerStateEnoughPlayers.cpp' ,
	'src/server/server/ServerStateFinishWait.cpp' ,
	'src/server/server/ServerStateNewGame.cpp' ,
	'src/server/server/ServerStatePlaying.cpp' ,
	'src/server/server/ServerStateScore.cpp' ,
	'src/server/server/ServerStateStartingMatch.cpp' ,
	'src/server/server/ServerStateTankNewGame.cpp' ,
	'src/server/server/ServerSyncCheck.cpp' ,
	'src/server/server/ServerTankChangeHandler.cpp' ,
	'src/server/server/ServerTextFilter.cpp' ,
	'src/server/server/ServerTimedMessage.cpp' ,
	'src/server/server/ServerTurns.cpp' ,
	'src/server/server/ServerTurnsFree.cpp' ,
	'src/server/server/ServerTurnsSequential.cpp' ,
	'src/server/server/ServerTurnsSimultaneous.cpp' ,
	'src/server/tankai/TankAI.cpp' ,
	'src/server/tankai/TankAIAdder.cpp' ,
	'src/server/tankai/TankAIAimGuesser.cpp' ,
	'src/server/tankai/TankAICurrent.cpp' ,
	'src/server/tankai/TankAICurrentDefenses.cpp' ,
	'src/server/tankai/TankAICurrentMove.cpp' ,
	'src/server/tankai/TankAICurrentMoveWeapons.cpp' ,
	'src/server/tankai/TankAICurrentTarget.cpp' ,
	'src/server/tankai/TankAICurrentWeapons.cpp' ,
	'src/server/tankai/TankAINone.cpp' ,
	'src/server/tankai/TankAIRandom.cpp' ,
	'src/server/tankai/TankAIShallow.cpp' ,
	'src/server/tankai/TankAISniperGuesser.cpp' ,
	'src/server/tankai/TankAIStore.cpp' ,
	'src/server/tankai/TankAIStrings.cpp' ,
	'src/server/tankai/TankAIWeaponSets.cpp' ,
)
# END   Server sources for Client and Server



# BEGIN Launcher sources
scorched3d_src = files(
	'src/launcher/scorched/ScorchedParams.cpp' ,
	'src/launcher/scorched/main.cpp' ,

	'src/launcher/wxdialogs/CreateDialogs.cpp' ,
	'src/launcher/wxdialogs/DisplayDialog.cpp' ,
	'src/launcher/wxdialogs/KeyDialog.cpp' ,
	'src/launcher/wxdialogs/MainDialog.cpp' ,
	'src/launcher/wxdialogs/OptionEntrySetter.cpp' ,
	'src/launcher/wxdialogs/ServerSDialog.cpp' ,
	'src/launcher/wxdialogs/SettingsDialog.cpp' ,
	'src/launcher/wxdialogs/TrueTypeFont.cpp' ,

	'src/client/client/UniqueIdStore.cpp' ,
	'src/client/graph/OptionsDisplay.cpp' ,
)
scorched3d_src += scorched3d_common_generic_src
scorched3d_src += scorched3d_server_generic_src
# END   Launcher sources

# BEGIN Client sources
scorched3dc_src = files(
	'src/client/scorchedc/main.cpp' ,

	'src/client/GLEXT/GLCamera.cpp' ,
	'src/client/GLEXT/GLCameraFrustum.cpp' ,
	'src/client/GLEXT/GLFont2d.cpp' ,
	'src/client/GLEXT/GLFont2dFreeType.cpp' ,
	'src/client/GLEXT/GLFont2dStorage.cpp' ,
	'src/client/GLEXT/GLFrameBufferObject.cpp' ,
	'src/client/GLEXT/GLGlobalState.cpp' ,
	'src/client/GLEXT/GLImageItterator.cpp' ,
	'src/client/GLEXT/GLImageModifier.cpp' ,
	'src/client/GLEXT/GLInfo.cpp' ,
	'src/client/GLEXT/GLLenseFlare.cpp' ,
	'src/client/GLEXT/GLMenu.cpp' ,
	'src/client/GLEXT/GLMenuEntry.cpp' ,
	'src/client/GLEXT/GLMenuI.cpp' ,
	'src/client/GLEXT/GLShadowFrameBuffer.cpp' ,
	'src/client/GLEXT/GLState.cpp' ,
	'src/client/GLEXT/GLStateExtension.cpp' ,
	'src/client/GLEXT/GLTexture.cpp' ,
	'src/client/GLEXT/GLTextureBase.cpp' ,
	'src/client/GLEXT/GLTextureCubeMap.cpp' ,
	'src/client/GLEXT/GLTextureReference.cpp' ,
	'src/client/GLEXT/GLTextureReferenceData.cpp' ,
	'src/client/GLEXT/GLTextureSet.cpp' ,
	'src/client/GLEXT/GLTextureStore.cpp' ,
	'src/client/GLEXT/GLVertexBufferObject.cpp' ,
	'src/client/GLEXT/GLViewPort.cpp' ,
	'src/client/GLSL/GLSLProgram.cpp' ,
	'src/client/GLSL/GLSLShader.cpp' ,
	'src/client/GLSL/GLSLShaderSetup.cpp' ,
	'src/client/GLW/GLWBitmap.cpp' ,
	'src/client/GLW/GLWButton.cpp' ,
	'src/client/GLW/GLWChannelText.cpp' ,
	'src/client/GLW/GLWChannelView.cpp' ,
	'src/client/GLW/GLWChannelViewTextRenderer.cpp' ,
	'src/client/GLW/GLWChatView.cpp' ,
	'src/client/GLW/GLWCheckBox.cpp' ,
	'src/client/GLW/GLWCheckBoxText.cpp' ,
	'src/client/GLW/GLWColors.cpp' ,
	'src/client/GLW/GLWDragger.cpp' ,
	'src/client/GLW/GLWDropDown.cpp' ,
	'src/client/GLW/GLWDropDownColor.cpp' ,
	'src/client/GLW/GLWDropDownText.cpp' ,
	'src/client/GLW/GLWFileView.cpp' ,
	'src/client/GLW/GLWFlag.cpp' ,
	'src/client/GLW/GLWFont.cpp' ,
	'src/client/GLW/GLWIcon.cpp' ,
	'src/client/GLW/GLWIconButton.cpp' ,
	'src/client/GLW/GLWIconList.cpp' ,
	'src/client/GLW/GLWIconTable.cpp' ,
	'src/client/GLW/GLWImageList.cpp' ,
	'src/client/GLW/GLWLabel.cpp' ,
	'src/client/GLW/GLWListView.cpp' ,
	'src/client/GLW/GLWOptionEntry.cpp' ,
	'src/client/GLW/GLWPanel.cpp' ,
	'src/client/GLW/GLWPlanView.cpp' ,
	'src/client/GLW/GLWProgress.cpp' ,
	'src/client/GLW/GLWPushButton.cpp' ,
	'src/client/GLW/GLWScorchedInfo.cpp' ,
	'src/client/GLW/GLWScorchedInfoSpinner.cpp' ,
	'src/client/GLW/GLWScrollButton.cpp' ,
	'src/client/GLW/GLWScrollPanel.cpp' ,
	'src/client/GLW/GLWScrollW.cpp' ,
	'src/client/GLW/GLWScrollWBackwards.cpp' ,
	'src/client/GLW/GLWSelector.cpp' ,
	'src/client/GLW/GLWSelectorPart.cpp' ,
	'src/client/GLW/GLWSlider.cpp' ,
	'src/client/GLW/GLWSpacer.cpp' ,
	'src/client/GLW/GLWSpinBox.cpp' ,
	'src/client/GLW/GLWTab.cpp' ,
	'src/client/GLW/GLWTabContainer.cpp' ,
	'src/client/GLW/GLWTalkBox.cpp' ,
	'src/client/GLW/GLWTankFire.cpp' ,
	'src/client/GLW/GLWTankModel.cpp' ,
	'src/client/GLW/GLWTankSlider.cpp' ,
	'src/client/GLW/GLWTankTip.cpp' ,
	'src/client/GLW/GLWTankViewer.cpp' ,
	'src/client/GLW/GLWTextBox.cpp' ,
	'src/client/GLW/GLWTextButton.cpp' ,
	'src/client/GLW/GLWTime.cpp' ,
	'src/client/GLW/GLWToolTip.cpp' ,
	'src/client/GLW/GLWTracker.cpp' ,
	'src/client/GLW/GLWTranslate.cpp' ,
	'src/client/GLW/GLWWeaponModel.cpp' ,
	'src/client/GLW/GLWWindView.cpp' ,
	'src/client/GLW/GLWWindow.cpp' ,
	'src/client/GLW/GLWWindowManager.cpp' ,
	'src/client/GLW/GLWWindowSkin.cpp' ,
	'src/client/GLW/GLWWindowSkinManager.cpp' ,
	'src/client/GLW/GLWidget.cpp' ,
	'src/client/client/ClientAdmin.cpp' ,
	'src/client/client/ClientAdminResultHandler.cpp' ,
	'src/client/client/ClientChannelManager.cpp' ,
	'src/client/client/ClientChannelManagerI.cpp' ,
	'src/client/client/ClientConnectionAcceptHandler.cpp' ,
	'src/client/client/ClientConnectionAuthHandler.cpp' ,
	'src/client/client/ClientConnectionRejectHandler.cpp' ,
	'src/client/client/ClientDialog.cpp' ,
	'src/client/client/ClientDisconnected.cpp' ,
	'src/client/client/ClientFileHandler.cpp' ,
	'src/client/client/ClientGameStoppedHandler.cpp' ,
	'src/client/client/ClientInitializeModHandler.cpp' ,
	'src/client/client/ClientLinesHandler.cpp' ,
	'src/client/client/ClientLoadLevelHandler.cpp' ,
	'src/client/client/ClientMain.cpp' ,
	'src/client/client/ClientMessageHandler.cpp' ,
	'src/client/client/ClientOperationHandler.cpp' ,
	'src/client/client/ClientParams.cpp' ,
	'src/client/client/ClientProcessingLoop.cpp' ,
	'src/client/client/ClientReloadAdaptor.cpp' ,
	'src/client/client/ClientSaveScreenState.cpp' ,
	'src/client/client/ClientSimulator.cpp' ,
	'src/client/client/ClientStartGameHandler.cpp' ,
	'src/client/client/ClientState.cpp' ,
	'src/client/client/ClientWindowSetup.cpp' ,
	'src/client/client/LoadPNG.cpp' ,
	'src/client/client/ScorchedClient.cpp' ,
	'src/client/client/SecureID.cpp' ,
	'src/client/client/UniqueIdStore.cpp' ,
	'src/client/console/Console.cpp' ,
	'src/client/console/ConsoleFileReader.cpp' ,
	'src/client/console/ConsoleImpl.cpp' ,
	'src/client/console/ConsoleLines.cpp' ,
	'src/client/console/ConsoleMethods.cpp' ,
	'src/client/console/ConsoleRule.cpp' ,
	'src/client/console/ConsoleRuleFn.cpp' ,
	'src/client/console/ConsoleRuleFnIAdapter.cpp' ,
	'src/client/console/ConsoleRuleMethodIAdapter.cpp' ,
	'src/client/console/ConsoleRules.cpp' ,
	'src/client/dialogs/AdminAuthDialog.cpp' ,
	'src/client/dialogs/AdminCheckDialog.cpp' ,
	'src/client/dialogs/AdminDialog.cpp' ,
	'src/client/dialogs/AnimatedBackdropDialog.cpp' ,
	'src/client/dialogs/AuthDialog.cpp' ,
	'src/client/dialogs/AutoDefenseDialog.cpp' ,
	'src/client/dialogs/BackdropDialog.cpp' ,
	'src/client/dialogs/BuyAccessoryDialog.cpp' ,
	'src/client/dialogs/BuyAccessoryDialogTankInfo.cpp' ,
	'src/client/dialogs/CameraDialog.cpp' ,
	'src/client/dialogs/ConnectDialog.cpp' ,
	'src/client/dialogs/GiftMoneyDialog.cpp' ,
	'src/client/dialogs/HUDDialog.cpp' ,
	'src/client/dialogs/HelpButtonDialog.cpp' ,
	'src/client/dialogs/InventoryDialog.cpp' ,
	'src/client/dialogs/KibitzingDialog.cpp' ,
	'src/client/dialogs/LogDialog.cpp' ,
	'src/client/dialogs/MainMenuDialog.cpp' ,
	'src/client/dialogs/MessageDialog.cpp' ,
	'src/client/dialogs/ModSelectDialog.cpp' ,
	'src/client/dialogs/ModSubSelectDialog.cpp' ,
	'src/client/dialogs/MsgBoxDialog.cpp' ,
	'src/client/dialogs/NetworkChatDialog.cpp' ,
	'src/client/dialogs/NetworkSelectDialog.cpp' ,
	'src/client/dialogs/PlayerDialog.cpp' ,
	'src/client/dialogs/PlayerInGameDialog.cpp' ,
	'src/client/dialogs/PlayerInitialDialog.cpp' ,
	'src/client/dialogs/ProfileDialog.cpp' ,
	'src/client/dialogs/ProgressDialog.cpp' ,
	'src/client/dialogs/QuitDialog.cpp' ,
	'src/client/dialogs/SaveDialog.cpp' ,
	'src/client/dialogs/SaveSelectDialog.cpp' ,
	'src/client/dialogs/ScoreDialog.cpp' ,
	'src/client/dialogs/SettingsSelectDialog.cpp' ,
	'src/client/dialogs/SettingsSubSelectDialog.cpp' ,
	'src/client/dialogs/SkipAllDialog.cpp' ,
	'src/client/dialogs/SkipDialog.cpp' ,
	'src/client/dialogs/SoundDialog.cpp' ,
	'src/client/dialogs/StartDialog.cpp' ,
	'src/client/dialogs/TextBoxDialog.cpp' ,
	'src/client/dialogs/TipDialog.cpp' ,
	'src/client/dialogs/TutorialDialog.cpp' ,
	'src/client/geomipmap/MipMapPatchIndex.cpp' ,
	'src/client/geomipmap/MipMapPatchIndexs.cpp' ,
	'src/client/graph/Display.cpp' ,
	'src/client/graph/FrameLimiter.cpp' ,
	'src/client/graph/FrameTimer.cpp' ,
	'src/client/graph/GLSetup.cpp' ,
	'src/client/graph/Gamma.cpp' ,
	'src/client/graph/Main2DCamera.cpp' ,
	'src/client/graph/MainCamera.cpp' ,
	'src/client/graph/ModelRenderer.cpp' ,
	'src/client/graph/ModelRendererMesh.cpp' ,
	'src/client/graph/ModelRendererSimulator.cpp' ,
	'src/client/graph/ModelRendererStore.cpp' ,
	'src/client/graph/ModelRendererTree.cpp' ,
	'src/client/graph/Mouse.cpp' ,
	'src/client/graph/OptionsDisplay.cpp' ,
	'src/client/graph/OptionsDisplayConsole.cpp' ,
	'src/client/graph/Particle.cpp' ,
	'src/client/graph/ParticleEmitter.cpp' ,
	'src/client/graph/ParticleEngine.cpp' ,
	'src/client/graph/ParticleRenderer.cpp' ,
	'src/client/graph/ShotCountDown.cpp' ,
	'src/client/graph/SoftwareMouse.cpp' ,
	'src/client/graph/SpeedChange.cpp' ,
	'src/client/graph/TargetCamera.cpp' ,
	'src/client/graph/TutorialFile.cpp' ,
	'src/client/land/HeightMapVisibilityPatch.cpp' ,
	'src/client/land/LandAndTargetVisibilityPatch.cpp' ,
	'src/client/land/LandSurround.cpp' ,
	'src/client/land/LandVisibilityPatch.cpp' ,
	'src/client/land/RoofVisibilityPatch.cpp' ,
	'src/client/land/TargetList.cpp' ,
	'src/client/land/TargetVisibilityPatch.cpp' ,
	'src/client/land/VisibilityPatchGrid.cpp' ,
	'src/client/land/VisibilityPatchInfo.cpp' ,
	'src/client/land/VisibilityPatchQuad.cpp' ,
	'src/client/land/WaterAndTargetVisibilityPatch.cpp' ,
	'src/client/land/WaterVisibilityPatch.cpp' ,
	'src/client/landscape/DeformTextures.cpp' ,
	'src/client/landscape/GraphicalLandscapeMap.cpp' ,
	'src/client/landscape/InfoMap.cpp' ,
	'src/client/landscape/Landscape.cpp' ,
	'src/client/landscape/LandscapeMusicManager.cpp' ,
	'src/client/landscape/LandscapePoints.cpp' ,
	'src/client/landscape/LandscapeSoundManager.cpp' ,
	'src/client/landscape/LandscapeStateHandler.cpp' ,
	'src/client/landscape/MapPoints.cpp' ,
	'src/client/landscape/ShadowMap.cpp' ,
	'src/client/landscape/Smoke.cpp' ,
	'src/client/landscape/Wall.cpp' ,
	'src/client/serverbrowser/ServerBrowser.cpp' ,
	'src/client/serverbrowser/ServerBrowserCollect.cpp' ,
	'src/client/serverbrowser/ServerBrowserRefresh.cpp' ,
	'src/client/serverbrowser/ServerBrowserServerList.cpp' ,
	'src/client/sky/Hemisphere.cpp' ,
	'src/client/sky/LargeHemisphere.cpp' ,
	'src/client/sky/Sky.cpp' ,
	'src/client/sky/SkyDome.cpp' ,
	'src/client/sky/SkyLine.cpp' ,
	'src/client/sky/SkyRoof.cpp' ,
	'src/client/sky/Sun.cpp' ,
	'src/client/sound/PlayingSoundSource.cpp' ,
	'src/client/sound/Sound.cpp' ,
	'src/client/sound/SoundBuffer.cpp' ,
	'src/client/sound/SoundBufferDynamicOVSourceInstance.cpp' ,
	'src/client/sound/SoundBufferEmpty.cpp' ,
	'src/client/sound/SoundBufferFactory.cpp' ,
	'src/client/sound/SoundBufferOV.cpp' ,
	'src/client/sound/SoundBufferStaticSourceInstance.cpp' ,
	'src/client/sound/SoundBufferWav.cpp' ,
	'src/client/sound/SoundListener.cpp' ,
	'src/client/sound/SoundSource.cpp' ,
	'src/client/sound/SoundUtils.cpp' ,
	'src/client/sound/VirtualSoundSource.cpp' ,
	'src/client/sprites/DebrisActionRenderer.cpp' ,
	'src/client/sprites/ExplosionLaserBeamRenderer.cpp' ,
	'src/client/sprites/ExplosionNukeRenderer.cpp' ,
	'src/client/sprites/ExplosionTextures.cpp' ,
	'src/client/sprites/MetaActionRenderer.cpp' ,
	'src/client/sprites/MissileActionRenderer.cpp' ,
	'src/client/sprites/NapalmRenderer.cpp' ,
	'src/client/sprites/SmokeActionRenderer.cpp' ,
	'src/client/sprites/SphereActionRenderer.cpp' ,
	'src/client/sprites/TalkRenderer.cpp' ,
	'src/client/sprites/TeleportRenderer.cpp' ,
	'src/client/sprites/TextActionRenderer.cpp' ,
	'src/client/sprites/WallActionRenderer.cpp' ,
	'src/client/tankgraph/MissileMesh.cpp' ,
	'src/client/tankgraph/ModelRendererTank.cpp' ,
	'src/client/tankgraph/ModelRendererTankStore.cpp' ,
	'src/client/tankgraph/RenderGeoms.cpp' ,
	'src/client/tankgraph/RenderTargets.cpp' ,
	'src/client/tankgraph/RenderTracer.cpp' ,
	'src/client/tankgraph/TankKeyboardControl.cpp' ,
	'src/client/tankgraph/TankKeyboardControlUtil.cpp' ,
	'src/client/tankgraph/TankMenus.cpp' ,
	'src/client/tankgraph/TargetParticleRenderer.cpp' ,
	'src/client/tankgraph/TargetRendererImpl.cpp' ,
	'src/client/tankgraph/TargetRendererImplTank.cpp' ,
	'src/client/tankgraph/TargetRendererImplTarget.cpp' ,
	'src/client/water/Water.cpp' ,
	'src/client/water/Water2.cpp' ,
	'src/client/water/Water2Patch.cpp' ,
	'src/client/water/Water2Patches.cpp' ,
	'src/client/water/Water2Renderer.cpp' ,
	'src/client/water/WaterMapPoints.cpp' ,
	'src/client/water/WaterWaveDistance.cpp' ,
	'src/client/water/WaterWaves.cpp' ,
)
scorched3dc_src += scorched3d_common_generic_src
scorched3dc_src += scorched3d_common_cli_srv_src
scorched3dc_src += scorched3d_common_client_src
scorched3dc_src += scorched3d_server_generic_src
scorched3dc_src += scorched3d_server_cli_srv_src
# END   Client sources

# BEGIN Server sources
scorched3ds_src = files(
	'src/server/scorcheds/main.cpp' ,

	'src/server/webserver/ServerWebAppletHandler.cpp' ,
	'src/server/webserver/ServerWebHandler.cpp' ,
	'src/server/webserver/ServerWebServer.cpp' ,
	'src/server/webserver/ServerWebServerI.cpp' ,
	'src/server/webserver/ServerWebServerQueue.cpp' ,
	'src/server/webserver/ServerWebServerUtil.cpp' ,
	'src/server/webserver/ServerWebSettingsHandler.cpp' ,
)
scorched3ds_src += scorched3d_common_generic_src
scorched3ds_src += scorched3d_common_cli_srv_src
scorched3ds_src += scorched3d_server_generic_src
scorched3ds_src += scorched3d_server_cli_srv_src
# END   Server sources

# END   Sources

# BEGIN Binaries
if not get_option('serveronly')
	executable(
		'scorched3d',
		c_args:   launcher_c_args,
		cpp_args: launcher_cpp_args,
		sources:  scorched3d_src,
		include_directories : include_directories(
			'src/common/porting',
			'src/common',
			'src/client',
			'src/server',
			'src/launcher',
		),
		dependencies: [
			expat_dep,
			ft2_dep,
			sdl_dep,
			sdl_net_dep,
			wxwidgets_dep,
			zlib_dep,
		],
		install: true
	)

	executable(
		'scorched3dc',
		c_args:   client_c_args,
		cpp_args: client_cpp_args,
		sources:  scorched3dc_src,
		include_directories : include_directories(
			'src/common/porting',
			'src/common',
			'src/client',
			'src/server',
		),
		dependencies: [
			expat_dep,
			fftw_dep,
			freealut_dep,
			ft2_dep,
			gl_dep,
			glew_dep,
			glu_dep,
			jpeg_dep,
			lua_dep,
			ogg_dep,
			openal_dep,
			png_dep,
			sdl_dep,
			sdl_net_dep,
			vorbis_dep,
			vorbisfile_dep,
			zlib_dep,
		],
		install: true
	)
endif

executable(
	'scorched3ds',
	c_args:   server_c_args,
	cpp_args: server_cpp_args,
	sources:  scorched3ds_src,
	include_directories : include_directories(
		'src/common/porting',
		'src/common',
		'src/server',
	),
	dependencies: [
		expat_dep,
		jpeg_dep,
		lua_dep,
		png_dep,
		sdl_dep,
		sdl_net_dep,
		zlib_dep,
	],
	install: true
)
# END   Binaries

# Data
install_subdir('data',          install_dir: get_option('datadir'))
install_data(
	'documentation/README-apoc.txt' ,
	'documentation/README-EXPAT.txt' ,
	'documentation/README-ft2.txt' ,
	'documentation/README-libjpg.txt' ,
	'documentation/README-ODE.txt' ,
	'documentation/README-ogg.txt' ,
	'documentation/README-png.txt' ,
	'documentation/README-SDLMIXER.txt' ,
	'documentation/README-SDLNET.txt' ,
	'documentation/README-SDL.txt' ,
	'documentation/README-wxwindows.txt' ,
	'documentation/README-zlib.txt' ,
	install_dir: get_option('docdir'),
)
