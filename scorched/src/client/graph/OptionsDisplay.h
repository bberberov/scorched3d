////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#if !defined(__INCLUDE_OptionsDisplayh_INCLUDE__)
#define __INCLUDE_OptionsDisplayh_INCLUDE__

#include <common/OptionEntry.h>

class OptionsDisplay
{
public:
	static OptionsDisplay *instance();

	enum DataType
	{
		NoAccess = 0,
		RWAccess = 1,
		RAccess = 2,
		NoRestore = 4,
		DebugOnly = 8
	};

	bool getHideFinalScore() { return hideFinalScore_; }
	OptionEntryBool &getHideFinalScoreEntry() { return hideFinalScore_; }

	bool getNoPlanDraw() { return noPlanDraw_; }
	OptionEntryBool &getNoPlanDrawEntry() { return noPlanDraw_; }

	int getTankModelSize() { return tankModelSize_; }
	OptionEntryInt &getTankModelSizeEntry() { return tankModelSize_; }

	int getNumberExplosionParts() { return explosionParts_; }
	OptionEntryInt &getNumberExplosionPartsEntry() { return explosionParts_; }

	int getExplosionParticlesMult() { return explosionParticleMult_; }
	OptionEntryInt &getExplosionParticlesMultEntry() { return explosionParticleMult_; }

	int getNumberExplosionSubParts() { return explosionSubParts_; }
	OptionEntryInt &getNumberExplosionSubPartsEntry() { return explosionSubParts_; }

	int getBrightness() { return brightness_; }
	OptionEntryInt &getBrightnessEntry() { return brightness_; }

	float getDrawCullingDistance() { return drawCullingDistance_; }
	OptionEntryFloat &getDrawCullingDistanceEntry() { return drawCullingDistance_; } ;

	float getDrawFadeStartDistance() { return drawFadeStartDistance_; }
	OptionEntryFloat &getDrawFadeStartDistanceEntry() { return drawFadeStartDistance_; } ;

	bool getSmoothLines() { return smoothLines_; }
	OptionEntryBool &getSmoothLinesEntry() { return smoothLines_; }

	bool getFrameTimer() { return frameTimer_; }
	OptionEntryBool &getFrameTimerEntry() { return frameTimer_; }

	bool getNoLenseFlare() { return noLenseFlare_; }
	OptionEntryBool &getNoLenseFlareEntry() { return noLenseFlare_; }

	bool getNoObjectReflections() { return noObjectReflections_; }
	OptionEntryBool &getNoObjectReflectionsEntry() { return noObjectReflections_; }

	bool getNoParticleReflections() { return noParticleReflections_; }
	OptionEntryBool &getNoParticleReflectionsEntry() { return noParticleReflections_; }

	bool getNoShadows() { return noShadows_; }
	OptionEntryBool &getNoShadowsEntry() { return noShadows_; }

	int getLandShadowsLOD() { return landShadowsLOD_; }
	OptionEntryInt &getLandShadowsLODEntry() { return landShadowsLOD_; }

	bool getNoGLObjectShadows() { return noGLObjectShadows_; }
	OptionEntryBool &getNoGLObjectShadowsEntry() { return noGLObjectShadows_; }

	bool getNoGLShadows() { return noGLShadows_; }
	OptionEntryBool &getNoGLShadowsEntry() { return noGLShadows_; }

	bool getNoGLDrawElements() { return noGLDrawElements_; }
	OptionEntryBool &getNoGLDrawElementsEntry() { return noGLDrawElements_; }

	bool getNoDrawParticles() { return noDrawParticles_; }
	OptionEntryBool &getNoDrawParticlesEntry() { return noDrawParticles_; }

	bool getNoSimulateParticles() { return noSimulateParticles_; }
	OptionEntryBool &getNoSimulateParticlesEntry() { return noSimulateParticles_; }

	bool getNoModelLighting() { return noModelLighting_; }
	OptionEntryBool &getNoModelLightingEntry() { return noModelLighting_; }

	bool getNoVBO() { return noVBO_; }
	OptionEntryBool &getNoVBOEntry() { return noVBO_; }

	bool getNoFog() { return noFog_; }
	OptionEntryBool &getNoFogEntry() { return noFog_; }

	bool getHideMenus() { return hideMenus_; }
	OptionEntryBool &getHideMenusEntry() { return hideMenus_; }

	const char *getLastVersionPlayed() { return lastVersionPlayed_; }
	OptionEntryString &getLastVersionPlayedEntry() { return lastVersionPlayed_; }

	bool getDetailTexture() { return detailTexture_; }
	OptionEntryBool &getDetailTextureEntry() { return detailTexture_; }

	bool getUseLandscapeTexture() { return useLandscapeTexture_; }
	OptionEntryBool &getUseLandscapeTextureEntry() { return useLandscapeTexture_; }

	bool getUseWaterTexture() { return useWaterTexture_; }
	OptionEntryBool &getUseWaterTextureEntry() { return useWaterTexture_; }

	bool getFullScreen() { return fullScreen_; }
	OptionEntryBool &getFullScreenEntry() { return fullScreen_; }

	bool getMoreRes() { return moreRes_; }
	OptionEntryBool &getMoreResEntry() { return moreRes_; }

	bool getInvertElevation() { return invertElevation_; }
	OptionEntryBool &getInvertElevationEntry() { return invertElevation_; }

	bool getInvertMouse() { return invertMouse_; }
	OptionEntryBool &getInvertMouseEntry() { return invertMouse_; }

	bool getSwapYAxis() { return swapYAxis_; }
	OptionEntryBool &getSwapYAxisEntry() { return swapYAxis_; }

	bool getSideScroll() { return sideScroll_; }
	OptionEntryBool &getSideScrollEntry() { return sideScroll_; }

	bool getStorePlayerCamera() { return storePlayerCamera_; }
	OptionEntryBool &getStorePlayerCameraEntry() { return storePlayerCamera_; }

	bool getRestrictedCameraMovement() { return restricedCameraMovement_; }
	OptionEntryBool &getRestrictedCameraMovementEntry() { return restricedCameraMovement_; }

	bool getNoGLExt() { return noGLExt_; }
	OptionEntryBool &getNoGLExtEntry() { return noGLExt_; }

	bool getNoGLTexSubImage() { return noGLTexSubImage_; }
	OptionEntryBool &getNoGLTexSubImageEntry() { return noGLTexSubImage_; }

	bool getNoGLMultiTex() { return noGLMultiTex_; }
	OptionEntryBool &getNoGLMultiTexEntry() { return noGLMultiTex_; }

	bool getNoGLCubeMap() { return noGLCubeMap_; }
	OptionEntryBool &getNoGLCubeMapEntry() { return noGLCubeMap_; }

	bool getNoGLSphereMap() { return noGLSphereMap_; }
	OptionEntryBool &getNoGLSphereMapEntry() { return noGLSphereMap_; }
	
	bool getNoGLEnvCombine() { return noGLEnvCombine_; }
	OptionEntryBool &getNoGLEnvCombineEntry() { return noGLEnvCombine_; }

	bool getNoGLHardwareMipmaps() { return noGLHardwareMipmaps_; }
	OptionEntryBool &getNoGLHardwareMipmapsEntry() { return noGLHardwareMipmaps_; }

	bool getNoGLShaders() { return noGLShaders_; }
	OptionEntryBool &getNoGLShadersEntry() { return noGLShaders_; }

	bool getSimpleWaterShaders() { return simpleWaterShaders_; }
	OptionEntryBool &getSimpleWaterShadersEntry() { return simpleWaterShaders_; }

	bool getNoProgressBackdrop() { return noProgressBackdrop_; }
	OptionEntryBool &getNoProgressBackdropEntry() { return noProgressBackdrop_; }
	
	int getSoundChannels() { return soundChannels_; }
	OptionEntryInt &getSoundChannelsEntry() { return soundChannels_; }

	bool getNoCountDownSound() { return noCountDownSound_; }
	OptionEntryBool &getNoCountDownSoundEntry() { return noCountDownSound_; }

	bool getNoChannelTextSound() { return noChannelTextSound_; }
	OptionEntryBool &getNoChannelTextSoundEntry() { return noChannelTextSound_; }

	bool getNoSound() { return noSound_; }
	OptionEntryBool &getNoSoundEntry() { return noSound_; }

	bool getNoMusic() { return noMusic_; }
	OptionEntryBool &getNoMusicEntry() { return noMusic_; }

	bool getNoSkins() { return noSkins_; }
	OptionEntryBool &getNoSkinsEntry() { return noSkins_; }

	int getTankDetail() { return tankDetail_; }
	OptionEntryInt &getTankDetailEntry() { return tankDetail_; }

	int getEffectsDetail() { return effectsDetail_; }
	OptionEntryInt &getEffectsDetailEntry() { return effectsDetail_; }

	int getDeformRecalculationTime() { return deformRecalculationTime_; }
	OptionEntryInt &getDeformRecalculationTimeEntry() { return deformRecalculationTime_; }

	bool getDrawLines() { return drawLines_; }
	OptionEntryBool &getDrawLinesEntry() { return drawLines_; }

	bool getDrawLandLOD() { return drawLandLOD_; }
	OptionEntryBool &getDrawLandLODEntry() { return drawLandLOD_; }

	bool getDrawNormals() { return drawNormals_; }
	OptionEntryBool &getDrawNormalsEntry() { return drawNormals_; }

	bool getDrawGraphicalShadowMap() { return drawGraphicalShadowMap_; }
	OptionEntryBool &getDrawGraphicalShadowMapEntry() { return drawGraphicalShadowMap_; }

	bool getDrawGraphicalReflectionMap() { return drawGraphicalReflectionMap_; }
	OptionEntryBool &getDrawGraphicalReflectionMapEntry() { return drawGraphicalReflectionMap_; }

	bool getDrawCollisionGeoms() { return drawCollisionGeoms_; }
	OptionEntryBool &getDrawCollisionGeomsEntry() { return drawCollisionGeoms_; }

	bool getDrawCollisionSpace() { return drawCollisionSpace_; }
	OptionEntryBool &getDrawCollisionSpaceEntry() { return drawCollisionSpace_; }

	bool getDrawBoundingSpheres() { return drawBoundingSpheres_; }
	OptionEntryBool &getDrawBoundingSpheresEntry() { return drawBoundingSpheres_; }

	bool getDrawPlayerNames() { return drawPlayerNames_; }
	OptionEntryBool &getDrawPlayerNamesEntry() { return drawPlayerNames_; }

	bool getDrawPlayerIcons() { return drawPlayerIcons_; }
	OptionEntryBool &getDrawPlayerIconsEntry() { return drawPlayerIcons_; }

	bool getDrawPlayerSight() { return drawPlayerSight_; }
	OptionEntryBool &getDrawPlayerSightEntry() { return drawPlayerSight_; }

	bool getOldSightPosition() { return oldSightPosition_; }
	OptionEntryBool &getOldSightPositionEntry() { return oldSightPosition_; }

	bool getLargeSight() { return largeSight_; }
	OptionEntryBool &getLargeSightEntry() { return largeSight_; }

	bool getDrawPlayerHealth() { return drawPlayerHealth_; }
	OptionEntryBool &getDrawPlayerHealthEntry() { return drawPlayerHealth_; }

	bool getDrawPlayerColor() { return drawPlayerColor_; }
	OptionEntryBool &getDrawPlayerColorEntry() { return drawPlayerColor_; }

	bool getDrawWater() { return drawWater_; }
	OptionEntryBool &getDrawWaterEntry() { return drawWater_; }

	bool getDrawLandscape() { return drawLandscape_; }
	OptionEntryBool &getDrawLandscapeEntry() { return drawLandscape_; }

	bool getDrawSurround() { return drawSurround_; }
	OptionEntryBool &getDrawSurroundEntry() { return drawSurround_; }

	bool getDrawMovement() { return drawMovement_; }
	OptionEntryBool &getDrawMovementEntry() { return drawMovement_; }

	bool getNoWaterMovement() { return noWaterMovement_; }
	OptionEntryBool &getNoWaterMovementEntry() { return noWaterMovement_; }

	bool getNoWaterReflections() { return noWaterReflections_; }
	OptionEntryBool &getNoWaterReflectionsEntry() { return noWaterReflections_; }

	bool getNoWaterWaves() { return noWaterWaves_; }
	OptionEntryBool &getNoWaterWavesEntry() { return noWaterWaves_; }

	bool getNoThreadedDraw() { return noThreadedDraw_; }
	OptionEntryBool &getNoThreadedDrawEntry() { return noThreadedDraw_; }

	bool getNoWaterLOD() { return noWaterLOD_; }
	OptionEntryBool &getNoWaterLODEntry() { return noWaterLOD_; }

	bool getNoLandLOD() { return noLandLOD_; }
	OptionEntryBool &getNoLandLODEntry() { return noLandLOD_; }

	bool getNoSkyMovement() { return noSkyMovement_; }
	OptionEntryBool &getNoSkyMovementEntry() { return noSkyMovement_; }

	bool getNoSkyLayers() { return noSkyLayers_; }
	OptionEntryBool &getNoSkyLayersEntry() { return noSkyLayers_; }

	bool getNoTrees() { return noTrees_; }
	OptionEntryBool &getNoTreesEntry() { return noTrees_; }

	bool getNoTargets() { return noTargets_; }
	OptionEntryBool &getNoTargetsEntry() { return noTargets_; }

	bool getNoPrecipitation() { return noPrecipitation_; }
	OptionEntryBool &getNoPrecipitationEntry() { return noPrecipitation_; }

	bool getLowTreeDetail() { return lowTreeDetail_; }
	OptionEntryBool &getLowTreeDetailEntry() { return lowTreeDetail_; }

	int getAccessorySortKey() { return accessorySortKey_; }
	OptionEntryEnum &getAccessorySortKeyEntry() { return accessorySortKey_; }

	bool getNoDepthSorting() { return noDepthSorting_; }
	OptionEntryBool &getNoDepthSortingEntry() { return noDepthSorting_; }

	bool getSoftwareMouse() { return softwareMouse_; }
	OptionEntryBool &getSoftwareMouseEntry() { return softwareMouse_; }

	bool getShowContextHelp() { return showContextHelp_; }
	OptionEntryBool &getShowContextHelpEntry() { return showContextHelp_; }

	bool getShowContextInfo() { return showContextInfo_; }
	OptionEntryBool &getShowContextInfoEntry() { return showContextInfo_; }

	bool getClientLogToFile() { return clientLogToFile_; }
	OptionEntryBool &getClientLogToFileEntry() { return clientLogToFile_; }

	int getClientLogState() { return clientLogState_; }
	OptionEntryInt &getClientLogStateEntry() { return clientLogState_; }

	bool getValidateServerIp() { return validateServerIp_; }
	OptionEntryBool &getValidateServerIpEntry() { return validateServerIp_; }

	int getSoundVolume() { return soundVolume_; }
	OptionEntryInt &getSoundVolumeEntry() { return soundVolume_; }

	int getAmbientSoundVolume() { return ambientSoundVolume_; }
	OptionEntryInt &getAmbientSoundVolumeEntry() { return ambientSoundVolume_; }

	int getMusicVolume() { return musicVolume_; }
	OptionEntryInt &getMusicVolumeEntry() { return musicVolume_; }

	int getDialogSize() { return dialogSize_; }
	OptionEntryInt &getDialogSizeEntry() { return dialogSize_; }

	int getAntiAlias() { return antiAlias_; }
	OptionEntryInt &getAntiAliasEntry() { return antiAlias_; }

	int getTexSize() { return texSize_; }
	OptionEntryInt &getTexSizeEntry() { return texSize_; }

	int getScreenWidth() { return screenWidth_; }
	OptionEntryInt  &getScreenWidthEntry() { return screenWidth_; }

	int getScreenHeight() { return screenHeight_; }
	OptionEntryInt  &getScreenHeightEntry() { return screenHeight_; }

	int getDepthBufferBits() { return depthBufferBits_; }
	OptionEntryInt &getDepthBufferBitsEntry() { return depthBufferBits_; }

	int getColorComponentSize() { return colorComponentSize_; }
	OptionEntryInt &getColorComponentSizeEntry() { return colorComponentSize_; }

	int getBitsPerPixel() { return bitsPerPixel_; }
	OptionEntryInt &getBitsPerPixelEntry() { return bitsPerPixel_; }

	bool getDoubleBuffer() { return doubleBuffer_; }
	OptionEntryBool &getDoubleBufferEntry() { return doubleBuffer_; }

	bool getSaveWindowPositions() { return saveWindowPositions_; }
	OptionEntryBool &getSaveWindowPositionsEntry() { return saveWindowPositions_; }

	const char *getOnlineUserName() { return onlineUserName_; }
	OptionEntryString &getOnlineUserNameEntry() { return onlineUserName_; }

	const char *getOnlineTankModel() { return onlineTankModel_; }
	OptionEntryString &getOnlineTankModelEntry() { return onlineTankModel_; }

	const char *getOnlineUserIcon() { return onlineUserIcon_; }
	OptionEntryString &getOnlineUserIconEntry() { return onlineUserIcon_; }

	Vector getOnlineColor() { return onlineColor_; }
	OptionEntryVector &getOnlineColorEntry() { return onlineColor_; }

	const char *getHostDescription() { return hostDescription_; }
	OptionEntryString &getHostDescriptionEntry() { return hostDescription_; }

	const char *getBuyTab() { return buyTab_; }
	OptionEntryString &getBuyTabEntry() { return buyTab_; }

	int getToolTipTime() { return toolTipTime_; }
	OptionEntryInt &getToolTipTimeEntry() { return toolTipTime_; }

	int getToolTipSpeed() { return toolTipSpeed_; }
	OptionEntryInt &getToolTipSpeedEntry() { return toolTipSpeed_; }

	int getFramesPerSecondLimit() { return framesPerSecondLimit_; }
	OptionEntryInt &getFramesPerSecondLimitEntry() { return framesPerSecondLimit_; }

	int getLandDetailError() { return landDetialError_; }
	OptionEntryInt &getLandDetailErrorEntry() { return landDetialError_; }

	int getWaterDetailLevelRamp() { return waterDetailLevelRamp_; }
	OptionEntryInt &getWaterDetailLevelRampEntry() { return waterDetailLevelRamp_; }

	bool getFocusPause() { return focusPause_; }
	OptionEntryBool &getFocusPauseEntry() { return focusPause_; }

	bool getOpenGLWarnings() { return openGLWarnings_; }
	OptionEntryBool &getOpenGLWarningsEntry() { return openGLWarnings_; }

	void loadSafeValues();
	void loadDefaultValues();
	void loadMediumValues();
	void loadFastestValues();
	void loadUltraValues();
	bool writeOptionsToFile(bool allOptions);
	bool readOptionsFromFile();

	std::list<OptionEntry *> &getOptions() { return options_; }

protected:
	static OptionsDisplay *instance_;
	std::list<OptionEntry *> options_;

	OptionEntryBool depricatedNoBoidSound_;
	OptionEntryBool depricatedNoBOIDS_;
	OptionEntryBoundedInt depricatedMaxModelTriPercentage_;
	OptionEntryBool depricatedUseHex_;
	OptionEntryInt depricatedDayTime_;
	OptionEntryInt depricatedSunYZAng_;
	OptionEntryInt depricatedSunXYAng_;
	OptionEntryInt bannerRowsDepricated_;
	OptionEntryString depricatedUniqueUserId_;
	OptionEntryBool depricatedNoShips_;
	OptionEntryBool depricatedDrawShipPaths_;
	OptionEntryBool depricatedFirstTimePlayed_;
	OptionEntryBool depricatedNoWaves_;
	OptionEntryBool depricatedNoCg_;
	OptionEntryBool depricatedNoAmbientSound_;
	OptionEntryBool depricatedFullClear_;
	OptionEntryInt depricatedRoamVarianceStart_;
	OptionEntryInt depricatedRoamVarianceRamp_;
	OptionEntryInt depricatedRoamVarianceTank_;
	OptionEntryBool depricatedNoROAM_;
	OptionEntryBool depricatedNoTessalation_;
	OptionEntryBool depricatedNoGLCompiledArrays_;
	OptionEntryFloat depricatedDrawDistance_;
	OptionEntryFloat depricatedDrawDistanceFade_;
	OptionEntryInt depricatedLandDetailLevelRamp_;

	OptionEntryBoundedInt brightness_;
	OptionEntryBoundedInt explosionParts_;
	OptionEntryBoundedInt explosionSubParts_;
	OptionEntryBoundedInt explosionParticleMult_;
	OptionEntryBool drawMovement_;
	OptionEntryInt colorComponentSize_;
	OptionEntryInt soundChannels_;
	OptionEntryInt bitsPerPixel_;
	OptionEntryInt depthBufferBits_;
	OptionEntryInt tankModelSize_;
	OptionEntryInt toolTipTime_;
	OptionEntryInt toolTipSpeed_;
	OptionEntryInt framesPerSecondLimit_;
	OptionEntryInt landDetialError_;
	OptionEntryInt waterDetailLevelRamp_;
	OptionEntryInt landShadowsLOD_;
	OptionEntryBool noFog_;
	OptionEntryBool detailTexture_;
	OptionEntryBool saveWindowPositions_;
	OptionEntryBool doubleBuffer_;
	OptionEntryBool smoothLines_;
	OptionEntryBool moreRes_;
	OptionEntryBool fullScreen_;
	OptionEntryBool noGLTexSubImage_;
	OptionEntryBool noGLCubeMap_;
	OptionEntryBool noGLSphereMap_;
	OptionEntryBool noGLExt_;
	OptionEntryBool noGLMultiTex_;
	OptionEntryBool noGLEnvCombine_;
	OptionEntryBool noGLHardwareMipmaps_;
	OptionEntryBool noGLDrawElements_;
	OptionEntryBool noGLShaders_;
	OptionEntryBool simpleWaterShaders_;
	OptionEntryBool noThreadedDraw_;
	OptionEntryBool invertElevation_;
	OptionEntryBool invertMouse_;
	OptionEntryBool noSound_;
	OptionEntryBool noCountDownSound_;
	OptionEntryBool noChannelTextSound_;
	OptionEntryBool noMusic_;
	OptionEntryBool noShadows_;
	OptionEntryBool noGLShadows_;
	OptionEntryBool noGLObjectShadows_;
	OptionEntryBool noObjectReflections_;
	OptionEntryBool noParticleReflections_;
	OptionEntryBool noDrawParticles_;
	OptionEntryBool noSimulateParticles_;
	OptionEntryBool drawNormals_;
	OptionEntryBool drawGraphicalShadowMap_;
	OptionEntryBool drawGraphicalReflectionMap_;
	OptionEntryBool drawLines_;
	OptionEntryBool drawLandLOD_;
	OptionEntryFloat drawCullingDistance_;
	OptionEntryFloat drawFadeStartDistance_;
	OptionEntryBool drawCollisionGeoms_;
	OptionEntryBool drawCollisionSpace_;
	OptionEntryBool drawBoundingSpheres_;
	OptionEntryBool noLenseFlare_;
	OptionEntryBool noSkins_;
	OptionEntryBool depricatedNoArenaMoveVisibility_;
	OptionEntryBool deprecatedSortAccessories_;
	OptionEntryEnum accessorySortKey_;
	OptionEntryBool drawWater_;
	OptionEntryBool drawLandscape_;
	OptionEntryBool drawSurround_;
	OptionEntryBool drawPlayerNames_;
	OptionEntryBool drawPlayerIcons_;
	OptionEntryBool drawPlayerSight_;
	OptionEntryBool oldSightPosition_;
	OptionEntryBool largeSight_;
	OptionEntryBool drawPlayerColor_;
	OptionEntryBool drawPlayerHealth_;
	OptionEntryBool noTrees_;
	OptionEntryBool noTargets_;
	OptionEntryBool noDepthSorting_;
	OptionEntryBool hideFinalScore_;
	OptionEntryBool hideMenus_;
	OptionEntryBool noVBO_;
	OptionEntryBool depricatedNoModelLOD_;
	OptionEntryBool noModelLighting_;
	OptionEntryBool useLandscapeTexture_;
	OptionEntryBool useWaterTexture_;
	OptionEntryBool noSkyLayers_;
	OptionEntryBool noSkyMovement_;
	OptionEntryBool noPrecipitation_;
	OptionEntryBool frameTimer_;
	OptionEntryBool depricatedMoWaterBuffers_;
	OptionEntryBool noWaterMovement_;
	OptionEntryBool noWaterWaves_;
	OptionEntryBool noWaterLOD_;
	OptionEntryBool noLandLOD_;
	OptionEntryBool noWaterReflections_;
	OptionEntryBool noProgressBackdrop_;
	OptionEntryBool showContextHelp_;
	OptionEntryBool showContextInfo_;
	OptionEntryBool lowTreeDetail_;
	OptionEntryBool softwareMouse_;
	OptionEntryBool sideScroll_;
	OptionEntryBool storePlayerCamera_;
	OptionEntryBool restricedCameraMovement_;
	OptionEntryBool swapYAxis_;
	OptionEntryBool clientLogToFile_;
	OptionEntryInt clientLogState_;
	OptionEntryBool validateServerIp_;
	OptionEntryBool noPlanDraw_;
	OptionEntryBoundedInt antiAlias_;
	OptionEntryBoundedInt dialogSize_;
	OptionEntryBoundedInt texSize_;
	OptionEntryBoundedInt tankDetail_;
	OptionEntryBoundedInt effectsDetail_;
	OptionEntryBoundedInt deformRecalculationTime_;
	OptionEntryBoundedInt soundVolume_;
	OptionEntryBoundedInt ambientSoundVolume_;
	OptionEntryBoundedInt musicVolume_;
	OptionEntryInt screenWidth_;
	OptionEntryInt screenHeight_;
	OptionEntryString hostDescription_;
	OptionEntryString onlineUserName_;
	OptionEntryString onlineTankModel_;
	OptionEntryString onlineUserIcon_;
	OptionEntryVector onlineColor_;
	OptionEntryString buyTab_;
	OptionEntryString lastVersionPlayed_;
	OptionEntryBool focusPause_;
	OptionEntryBool openGLWarnings_;

private:
	OptionsDisplay();
	virtual ~OptionsDisplay();

};

#endif
