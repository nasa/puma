/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2006 Martin Sherburn

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
=============================================================================*/

#pragma once

enum
{
	ID_Quit = 1,
	ID_About,
	ID_UserGuide,
	ID_LogNoteBook,
	ID_ViewerNoteBook,
	ID_ControlsWindow,
	ID_Open,
	ID_Save,
	ID_SaveScreenshot,
	ID_SaveIGES,
	ID_SaveSTEP,
	ID_SaveVolumeMesh,
	ID_SaveSurfaceMesh,
	ID_SaveGrid,
	ID_SaveVoxel,
	ID_OpenWiseTex,
	ID_OpenWeavePattern,
	ID_ToggleControls,
	ID_ToggleLogWindow,
	ID_ToggleOutliner,
	ID_SaveABAQUS,
	ID_SaveABAQUSVoxels,
	ID_SaveABAQUSSurface,
	ID_SaveTetgenMesh,
	ID_SaveVTUVoxels,

//	ID_CreateRenderWindow,
//	ID_DeleteRenderWindow,
	ID_RenderNodes,
	ID_RenderPaths,
	ID_RenderSurface,
	ID_RenderVolume,
	ID_RenderInterference,
	ID_RenderInterferenceDepth,
	ID_RenderOrientation,
	ID_RenderDomain,
	ID_RenderDomainAxes,
	ID_RenderAxes,
	ID_XRay,
	ID_ParallelProjection,
	ID_TrimtoDomain,
	ID_ChangeBackgroundColor,
	ID_ChangeSurfaceColor,
	ID_RefreshView,
//	ID_ClearScene,

	ID_CreateEmptyTextile,
	ID_Create2DWeave,
	ID_GeometrySolve,
	ID_EditTextile,
	ID_DeleteTextile,
	ID_Create3DTextile,
	ID_CreateLayeredTextile,
	ID_SetLayerOffsets,
	ID_NestLayers,
	ID_MaxNestLayers,
	ID_RotateTextile,

	ID_CreateYarn,
	ID_AssignSection,
	ID_AssignInterpolation,
	ID_AssignRepeats,
	ID_AssignProperties,
	ID_AssignMatrixProperties,
	ID_YarnFibreVolumeFraction,
	ID_SelectTool,
	ID_MoveTool,
	ID_RotateTool,
	ID_ScaleTool,
	ID_FilterNodes,
	ID_FilterPath,
	ID_FilterSurface,
	ID_Snap,
	ID_Relative,
	ID_PositionX,
	ID_PositionY,
	ID_PositionZ,
	ID_SnapSize,

	ID_CreateDomainPlanes,
	ID_CreateDomainBox,
	ID_EditDomain,
	ID_DeleteDomain,

	ID_RunScript,
	ID_RecordMacro,

	ID_PlaneListView,
	ID_AddPlane,
	ID_ReplacePlane,
	ID_DeletePlane,
	ID_BoxDomain,

	ID_Create3DWeave,
	ID_Refine,
	ID_GapSize,
	ID_Width,
	ID_Spacing,
	ID_Thickness,
	ID_InPlaneTangents,
	ID_ShearTextile,
	ID_ShearAngle,
	ID_ShearedDomain,
	ID_DefaultDomain,
	ID_CreateLayers,
	ID_DomainHeight,

	ID_OutputMessages,
	ID_PatternDraft,
	ID_QuickDomainVolumeFraction,
	ID_DomainVolumeFraction,

	ID_WarpWidth,
	ID_WarpHeight,
	ID_WarpSpacing,
	ID_WeftWidth,
	ID_WeftHeight,
	ID_WeftSpacing,
	ID_BinderWidth,
	ID_BinderHeight,
	ID_BinderSpacing,
	ID_WeftLayers,
	ID_WeftYarns,
	ID_WeaveType,
	ID_BinderLayers,
	ID_WarpYarns,
	ID_OffsetWarpYarn,
	ID_WarpPanel,
	ID_WarpPower,
	ID_WeftPanel,
	ID_WeftPower,
	ID_BinderPanel,
	ID_BinderPower,
	ID_Refine3D,
	ID_TargetThickness,
	ID_WarpLayers,
	ID_MaxVolumeFraction,
	ID_WarpRatio,
	ID_BinderRatio,
	ID_OrthogonalText,
	ID_OffsetAngleInterlockText,
	ID_AngleInterlockText,
	ID_LayerToLayerText,

};

