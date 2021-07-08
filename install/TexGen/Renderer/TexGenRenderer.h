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

#include "../Core/TexGen.h"
namespace TexGen
{
	struct PROP_INFO { virtual ~PROP_INFO() {} };

	struct PROP_YARN_INFO : public PROP_INFO
	{
		string TextileName;
		int iYarn;
	};

	struct PROP_NODE_INFO : public PROP_INFO
	{
		string TextileName;
		int iYarn;
		int iNode;
	};

	struct PROP_IMAGE_INFO : public PROP_INFO
	{
		int iImageID;
	};

	/// Used for sorting algorithms
	inline bool operator < (const PROP_YARN_INFO &left, const PROP_YARN_INFO &right)
	{
		if (left.TextileName != right.TextileName)
			return left.TextileName < right.TextileName;
		else if (left.iYarn != right.iYarn)
			return left.iYarn < right.iYarn;
		else
			return false;
	}

	/// Used for sorting algorithms
	inline bool operator < (const PROP_NODE_INFO &left, const PROP_NODE_INFO &right)
	{
		if (left.TextileName != right.TextileName)
			return left.TextileName < right.TextileName;
		else if (left.iYarn != right.iYarn)
			return left.iYarn < right.iYarn;
		else if (left.iNode != right.iNode)
			return left.iNode < right.iNode;
		else
			return false;
	}

	class CTextile;	// This makes sure that swig knows CTextile is part of the TexGen namespace...
	/// Used for rendering stuff using VTK http://www.vtk.org/
	/**
	This class can either be called with a vtkRenderWindowInteractor specified where rendering will be
	sent to that window. If the instance is constructed with string parameter then a new render window will
	be created with that name, to start rendering to the new window the Start function should be called.
	*/
	class CTexGenRenderer
	{
	public:
		CTexGenRenderer(vtkRenderWindowInteractor *pRenderWindowInteractor = NULL);
		~CTexGenRenderer(void);

		enum PROP_TYPE
		{
			PROP_NODE,
			PROP_PATH,
			PROP_SURFACE,
			PROP_VOLUME,
			PROP_AXES,
			PROP_INTERFERENCE,
			PROP_ORIENTATION,
			PROP_DOMAIN,
			PROP_DOMAINAXES,
			PROP_IMAGE,
			PROP_INTERFERENCE_DEPTH,
		};

		enum EXPORT_FLAG
		{
			EXPORT_NODES = 1<<0,
			EXPORT_PATH = 1<<1,
			EXPORT_SURFACE = 1<<2,
			EXPORT_DOMAIN = 1<<3,
		};

		/// Create the mesh and add it to the scene to be rendered
		/**
		\param TextileName The name of the textile to be renderered, if left blank will render all textiles
		\param dOpacity Opacity of the textile, set to values less than 1 to allow some transparency
		*/
		void RenderTextile(string TextileName = "", double dOpacity = 1);

		/// Create the mesh and add it to the scene to be rendered
		/**
		\param Textile Reference to the textile to render
		\param dOpacity Opacity of the textile, set to values less than 1 to allow some transparency
		*/
		void RenderTextile(CTextile &Textile, double dOpacity = 1);

		/// Render a single yarn with given color
		vtkProp* RenderYarn(CYarn &Yarn, const CDomain *pDomain = NULL, COLOR Color = COLOR(1, 1, 1), double dOpacity = 1);

		/// Create the volume mesh and add it to the scene to be rendered
		/**
		\param TextileName The name of the textile to be renderered, if left blank will render all textiles
		*/
		void RenderMesh(string TextileName = "");

		/// Create the volume mesh and add it to the scene to be rendered
		/**
		\param Textile Reference to the textile to render
		\param dOpacity Opacity of the textile, set to values less than 1 to allow some transparency
		*/
		void RenderMesh(CTextile &Textile);

		/// Render a single yarn mesh with given color
		vtkProp* RenderYarnMesh(CYarn &Yarn, const CDomain *pDomain = NULL, COLOR Color = COLOR(1, 1, 1));

		/// Render the domain
		void RenderDomain(string TextileName = "", COLOR Color = COLOR(1, 1, 1), double dOpacity = 0.5);

		/// Render the domain
		void RenderDomain(const CDomain &Domain, COLOR Color = COLOR(1, 1, 1), double dOpacity = 0.5);

		/// Render the dimensions of the domain
		void RenderDomainAxes(string TextileName = "");

		/// Render the dimensions of the domain
		void RenderDomainAxes(const CDomain &Domain);

		/// Render all the master nodes of each yarn in a textile
		void RenderNodes(string TextileName = "");

		/// Render all the master nodes of each yarn in a textile
		void RenderNodes(CTextile &Textile);

		/// Render all the master nodes of each yarn in a textile
		void RenderNodes(CTextile &Textile, int iYarn);

		/// Render the paths of each yarn in a textile
		void RenderPaths(string TextileName = "");

		/// Render the paths of each yarn in a textile
		void RenderPaths(CTextile &Textile);

		/// Render the path of a yarn in a textile
		void RenderPath(CTextile &Textile, int iYarn);

		/// Detect interference and render them
		/// If bDepth = true the spheres will be scaled with range from 0 to max intersection depth
		void RenderInterference(string TextileName = "", bool bDepth = false );

		/// Detect interference and render them
		/// If bDepth = true the spheres will be scaled with range from 0 to max intersection depth
		void RenderInterference(CTextile &Textile, bool bDepth = false);

		/// Render a grid of points within given box domain, each point is rendered as a tangent representing the
		/// yarn tangent at that point
		void RenderGrid(string TextileName = "", int iResX = 20, int iResY = 20, int iResZ = 20, double dLengthModifier = 1);

		/// Render a grid of points within given box domain, each point is rendered as a tangent representing the
		/// yarn tangent at that point
		void RenderGrid(CTextile &Textile, int iResX = 20, int iResY = 20, int iResZ = 20, double dLengthModifier = 1);

		/// Render an image
		//bool RenderImage(string FileName, double dMaxDim = 1, XYZ Orientation = XYZ());

		/// Set the render window background color where r g and b vary between 0 and 1
		void SetBackgroundColor(COLOR Color);

		/// Render axes at the bottom left of the window
		void RenderAxes();

		/// Remove all objects being rendered
		void ClearScene();

		/// Remove all actors of given type
		void RemoveProps(PROP_TYPE Type);

		/// Remove all actors of given type
		void SetPropsColor(PROP_TYPE Type, COLOR Color);

		/// Find out how many actors of given type exist
		int GetNumProps(PROP_TYPE Type) const;

		/// Get the vtkProps of a given type
		vector<vtkProp*> GetProps(PROP_TYPE Type);

		/// Get the vtkProps of a set of given types
		vector<vtkProp*> GetProps(set<PROP_TYPE> Types);

		/// Get information about the surface prop
		const PROP_YARN_INFO *GetYarnPropInfo(vtkProp* pProp) const;

		/// Get information about the node prop
		const PROP_NODE_INFO *GetNodePropInfo(vtkProp* pProp) const;

		/// Get information about the image prop
		const PROP_IMAGE_INFO *GetImagePropInfo(vtkProp* pProp) const;

		/// Get the vtkProps associated with the given object
		vector<vtkProp*> GetProps(PROP_INFO* pInfo, bool bIgnoreTextileName = false);

		/// Get the vtkProps associated with the given object
		vector<vtkProp*> GetNodeProps(PROP_YARN_INFO Info, bool bIgnoreTextileName = false);

		/// Re-render the yarn after it has been updated
		void RefreshYarn(PROP_YARN_INFO YarnInfo);

		/// Re-render the textile after it has been updated
		void RefreshTextile(string TextileName);

		/// Reset the camera so as to see all objects that are rendered
		void ResetCamera(XYZ LookDirection = XYZ());

		/// Re-render the scene to take into account updates to it
		void RefreshView();

		/// Create a new render window and render to it (optionally taking control of the thread)
		/**
		Note: this function should only be called if the instance was created without a vtkRenderWindowInteractor specified.
		\param bTakeControl If set to true then a message loop will begin a control will return once the window is closed
							otherwise the render window will be created and control will return immediatly
		\param iWidth Set the width of the window
		\param iHeight Set the height of the window
		\param bFullScreen Display window in fullscreen or not
		*/
		void Start(bool bTakeControl = true, int iWidth = 800, int iHeight = 600, bool bFullScreen = false);

		/// Saves the current view as a PNG image on the hard disk
		/**
		The file format is PNG hence should have the extension .PNG. Other filetypes
		are supported by VTK but not implemented here. If additional file types are needed
		please request the feature.
		\param FileName Name of the PNG file to save, should have .png extension
		\param iMagnification Provides ability to obtain very high resolution images.
		*/
		void TakeScreenShot(string FileName, int iMagnification = 1);

		/// Switch between parallel projection and perspective projection
		void SetParallelProjection(bool bParallelProjection);

		/// Get the renderer object
		vtkRenderer * GetRenderer() { return m_pRenderer; }

		/// Get the renderer object
		vtkRenderWindowInteractor * GetInteractor() { return m_pRenderWindowInteractor; }

		/// Set the callback function when an actor is added or deleted
		void SetCallback(void (*pCallback)());

		/// Render yarns as transparent
		void SetXRay(bool bXRay);

		/// Render yarns as transparent
		void SetTrimToDomain(bool bTrim);

		/// Set the colors of the yarns
		void SetYarnColor(int iYarn, COLOR Color);

		/// Apply the material properties
		static void ApplyColor(vtkActor *pActor, COLOR Color);

		/// Apply the default material color based on the yarn index
		void ApplyDefaultColor(vtkActor *pActor);

		/// Save textile data to VTK file format
		/**
		\param FileName The name of the file to export to, should contain .vtp extension (if it doesn't it will be appended)
		\param TextileName The name of the textile to be explorted, if left blank will export all textiles
		\param iExportFlags A list of flags from CTexGenRenderer::EXPORT_FLAG which determines what will be exported to the file
		\see CTexGenRenderer::EXPORT_FLAG
		*/
		bool SaveTextileToVTK(string FileName, string TextileName = "", int iExportFlags = EXPORT_NODES | EXPORT_PATH | EXPORT_SURFACE | EXPORT_DOMAIN);

		/// Save a Mesh to VTK file format
		bool SaveMeshToVTK(string FileName, const CMesh &Mesh);

		/// Increment batch count
		/**
		When the count is greater than zero re-rendering of is suppressed.
		Each call to BeginBatch must be matched by a later call to EndBatch.
		Code that does a lot of modification can be enclosed between BeginBatch
		and EndBatch calls to avoid excessive re-rendering.	The final EndBatch
		will cause the window to be re-rendered.
		*/
		void BeginBatch();
		
		/// Decrement batch count
		/**
		When the count is greater than zero re-rendering of is suppressed.
		Each call to BeginBatch must be matched by a later call to EndBatch.
		Code that does a lot of modification can be enclosed between BeginBatch
		and EndBatch calls to avoid excessive re-rendering.	The final EndBatch
		will cause the window to be re-rendered.
		*/
		void EndBatch();

		// Convenience functions
		void RemoveNodes() { RemoveProps(PROP_NODE); }
		void RemovePaths() { RemoveProps(PROP_PATH); }
		void RemoveTextiles() { RemoveProps(PROP_SURFACE); }
		void RemoveMeshes() { RemoveProps(PROP_VOLUME); }
		void RemoveAxes() { m_pOrientationMarkerWidget->SetEnabled(0); RefreshView();/*RemoveProps(PROP_AXES);*/ }
		void RemoveInterference() { RemoveProps(PROP_INTERFERENCE); }
		void RemoveInterferenceDepth() { RemoveProps(PROP_INTERFERENCE_DEPTH); }
		void RemoveOrientations() { RemoveProps(PROP_ORIENTATION); }
		void RemoveDomains() { RemoveProps(PROP_DOMAIN); }
		void RemoveDomainAxes() { RemoveProps(PROP_DOMAINAXES); }
		vtkPolyData *GetPolyData(const CMesh &Mesh);
		bool GetXRay() const { return m_bXRay; }
		bool GetTrimToDomain() const { return m_bTrimToDomain; }
		/// Determine if parallel projection or perspective projection is switched on
		bool GetParallelProjection() const;


	protected:
		/// Updates the axes color based on the background color
		/**
		If the background color is light then axes are drawn in black. If background
		is dark then axes are drawn in white.
		*/
		void UpdateAxesColor();

		/// Gets the best text color based on the color of the background
		/**
		The returned color will either be black or white depending on the darkness of
		the background.
		*/
		COLOR GetBestAnnotationColor();

		/// Add an actor to the scene
		void AddProp(PROP_TYPE Type, vtkProp *pProp);

		// Remove an actor from the scene
		void RemoveProp(vtkProp *pProp);

		/// Return a unique ID assigned to an image
		int GetUniqueImageID();

		/// This function will convert a vtkAlgorithm class into an vtkActor for rendering
		vtkActor *ConvertToActor(vtkAlgorithm *pAlgorithm);
		/// This function will convert a vtkPolyData class into an vtkActor for rendering
		vtkActor *ConvertToActor(vtkPolyData *pPolyData);
		vtkAlgorithm *CalculateNormals(vtkPolyData *pInput);

		/// This function will render a direction at a given point
//		void RenderDirection(XYZ Position, XYZ Direction, PROP_TYPE Type, double dScale = 1, double adColor[3] = COLOR_WHITE, bool bRefresh = true);

		multimap<PROP_TYPE, vtkProp*> m_Props;
		map<vtkProp*, PROP_YARN_INFO> m_YarnProps;
		map<vtkProp*, PROP_NODE_INFO> m_NodeProps;
		map<vtkProp*, PROP_IMAGE_INFO> m_ImageProps;

		vtkRenderer *m_pRenderer;
		vtkRenderWindow *m_pRenderWindow;
		vtkRenderWindowInteractor *m_pRenderWindowInteractor;
		vtkOrientationMarkerWidget *m_pOrientationMarkerWidget;

		void (*m_pCallback)();

		bool m_bXRay;
		bool m_bTrimToDomain;
		int m_iBatchCount;
	};

};	// namespace TexGen
