#include <new>
#include <memory>

#include <wx/gbsizer.h>
#include <wx/image.h>

#include "graphics/GraphicsHelpers.h"
#include "graphics/Palette.h"
#include "graphics/BMPFile.h"

#include "game/studiomodel/StudioModel.h"

#include "../CHLMV.h"
#include "../../CHLMVState.h"

#include "CTexturesPanel.h"

namespace hlmv
{
wxBEGIN_EVENT_TABLE( CTexturesPanel, CBaseControlPanel )
	EVT_COMBOBOX( wxID_TEX_CHANGED, CTexturesPanel::TextureChanged )
	EVT_SLIDER( wxID_TEX_SCALE, CTexturesPanel::ScaleChanged )
	EVT_CHECKBOX( wxID_TEX_CHECKBOX, CTexturesPanel::CheckBoxChanged )
	EVT_COMBOBOX( wxID_TEX_MESH, CTexturesPanel::MeshChanged )
	EVT_BUTTON( wxID_TEX_IMPORTTEXTURE, CTexturesPanel::ImportTexture )
	EVT_BUTTON( wxID_TEX_EXPORTTEXTURE, CTexturesPanel::ExportTexture )
	EVT_BUTTON( wxID_TEX_EXPORTUVMAP, CTexturesPanel::ExportUVMap )
wxEND_EVENT_TABLE()

CTexturesPanel::CTexturesPanel( wxWindow* pParent, CHLMV* const pHLMV )
	: CBaseControlPanel( pParent, "Textures", pHLMV )
{
	//Helps catch errors if we miss one.
	memset( m_pCheckBoxes, 0, sizeof( m_pCheckBoxes ) );

	wxWindow* const pElemParent = GetBox();

	m_pTextureSize = new wxStaticText( pElemParent, wxID_ANY, "Texture (size: Undefined x Undefined)" );

	m_pTexture = new wxComboBox( pElemParent, wxID_TEX_CHANGED, "" );
	m_pTexture->SetEditable( false );

	m_pScaleTextureViewSize = new wxStaticText( pElemParent, wxID_ANY, "Scale Texture View (Undefinedx)" );

	m_pScaleTextureView = new wxSlider( pElemParent, wxID_TEX_SCALE, TEXTUREVIEW_SLIDER_DEFAULT, TEXTUREVIEW_SLIDER_MIN, TEXTUREVIEW_SLIDER_MAX );

	m_pCheckBoxes[ CheckBox::CHROME ]				= new wxCheckBox( pElemParent, wxID_TEX_CHECKBOX, "Chrome" );
	m_pCheckBoxes[ CheckBox::SHOW_UV_MAP ]			= new wxCheckBox( pElemParent, wxID_TEX_CHECKBOX, "Show UV Map" );
	m_pCheckBoxes[ CheckBox::ADDITIVE ]				= new wxCheckBox( pElemParent, wxID_TEX_CHECKBOX, "Additive" );
	m_pCheckBoxes[ CheckBox::OVERLAY_UV_MAP ]		= new wxCheckBox( pElemParent, wxID_TEX_CHECKBOX, "Overlay UV Map" );
	m_pCheckBoxes[ CheckBox::TRANSPARENT ]			= new wxCheckBox( pElemParent, wxID_TEX_CHECKBOX, "Transparent" );
	m_pCheckBoxes[ CheckBox::ANTI_ALIAS_LINES ]		= new wxCheckBox( pElemParent, wxID_TEX_CHECKBOX, "Anti-Alias Lines" );

	m_pMesh = new wxComboBox( pElemParent, wxID_TEX_MESH, "" );
	m_pMesh->SetEditable( false );

	m_pImportTexButton = new wxButton( pElemParent, wxID_TEX_IMPORTTEXTURE, "Import Texture" );
	m_pExportTexButton = new wxButton( pElemParent, wxID_TEX_EXPORTTEXTURE, "Export Texture" );
	m_pExportUVButton = new wxButton( pElemParent, wxID_TEX_EXPORTUVMAP, "Export UV Map" );

	//Layout
	wxGridBagSizer* pSizer = new wxGridBagSizer( 5, 5 );

	pSizer->Add( m_pTextureSize, wxGBPosition( 0, 0 ), wxDefaultSpan, wxEXPAND | wxALIGN_CENTER_VERTICAL );
	pSizer->Add( m_pTexture, wxGBPosition( 1, 0 ), wxDefaultSpan, wxEXPAND );
	pSizer->Add( m_pScaleTextureViewSize, wxGBPosition( 3, 0 ), wxDefaultSpan, wxEXPAND );
	pSizer->Add( m_pScaleTextureView, wxGBPosition( 4, 0 ), wxDefaultSpan, wxEXPAND );

	for( size_t uiIndex = CheckBox::FIRST; uiIndex < CheckBox::COUNT; ++uiIndex )
	{
		if( !m_pCheckBoxes[ uiIndex ] )
			wxLogError( "CTexturesPanel::CTexturesPanel: Null checkbox %u!\n", uiIndex );

		pSizer->Add( m_pCheckBoxes[ uiIndex ], wxGBPosition( uiIndex / NUM_CHECKBOXES_PER_ROW, 1 + ( uiIndex % NUM_CHECKBOXES_PER_ROW ) ), wxDefaultSpan, wxEXPAND );

		m_pCheckBoxes[ uiIndex ]->SetClientData( reinterpret_cast<void*>( uiIndex ) );
	}

	pSizer->Add( m_pMesh, wxGBPosition( 4, 1 ), wxGBSpan( 1, 2 ), wxEXPAND );

	pSizer->Add( m_pImportTexButton, wxGBPosition( 0, 3 ), wxDefaultSpan, wxEXPAND );
	pSizer->Add( m_pExportTexButton, wxGBPosition( 1, 3 ), wxDefaultSpan, wxEXPAND );
	pSizer->Add( m_pExportUVButton, wxGBPosition( 2, 3 ), wxDefaultSpan, wxEXPAND );

	GetBoxSizer()->Add( pSizer );
}

CTexturesPanel::~CTexturesPanel()
{
}

void CTexturesPanel::PanelActivated()
{
	m_pHLMV->GetState()->showTexture = true;
}

void CTexturesPanel::PanelDeactivated()
{
	m_pHLMV->GetState()->showTexture = false;
}

void CTexturesPanel::InitializeUI()
{
	m_pTexture->Clear();

	m_pMesh->Clear();

	bool bSuccess = false;

	if( auto pModel = m_pHLMV->GetState()->GetStudioModel() )
	{
		const studiohdr_t* const pHdr = pModel->GetTextureHeader();

		if( pHdr )
		{
			m_pTexture->Enable( true );

			const mstudiotexture_t* const pTextures = ( mstudiotexture_t* ) ( ( byte* ) pHdr + pHdr->textureindex );

			//Insert all names into the array, then append the array to the combo box. This is much faster than appending each name to the combo box directly.
			wxArrayString names;

			for( int i = 0; i < pHdr->numtextures; ++i )
			{
				const mstudiotexture_t& texture = pTextures[ i ];

				names.Add( texture.name );
			}

			m_pTexture->Append( names );

			SetTexture( 0 );

			bSuccess = true;
		}
	}

	SetScale( TEXTUREVIEW_SLIDER_DEFAULT );

	if( !bSuccess )
	{
		m_pTexture->Enable( false );
		m_pMesh->Enable( false );
	}
}

void CTexturesPanel::TextureChanged( wxCommandEvent& event )
{
	SetTexture( m_pTexture->GetSelection() );
}

void CTexturesPanel::ScaleChanged( wxCommandEvent& event )
{
	SetScale( m_pScaleTextureView->GetValue(), false );
}

void CTexturesPanel::CheckBoxChanged( wxCommandEvent& event )
{
	auto pModel = m_pHLMV->GetState()->GetStudioModel();

	if( !pModel )
		return;

	const studiohdr_t* const pHdr = pModel->GetTextureHeader();

	if( !pHdr )
		return;

	wxCheckBox* const pCheckBox = static_cast<wxCheckBox*>( event.GetEventObject() );

	const CheckBox::Type checkbox = static_cast<CheckBox::Type>( reinterpret_cast<int>( pCheckBox->GetClientData() ) );

	if( checkbox < CheckBox::FIRST || checkbox > CheckBox::LAST )
		return;

	mstudiotexture_t& texture = ( ( mstudiotexture_t* ) ( ( byte* ) pHdr + pHdr->textureindex ) )[ m_pTexture->GetSelection() ];

	switch( checkbox )
	{
	case CheckBox::CHROME:
		{
			texture.flags &= ~STUDIO_NF_RENDER_FLAGS;

			if( pCheckBox->GetValue() )
			{
				texture.flags |= STUDIO_NF_CHROME;
			}

			m_pCheckBoxes[ CheckBox::ADDITIVE ]->SetValue( false );
			m_pCheckBoxes[ CheckBox::TRANSPARENT ]->SetValue( false );

			break;
		}

	case CheckBox::ADDITIVE:
		{
			texture.flags &= ~STUDIO_NF_RENDER_FLAGS;

			if( pCheckBox->GetValue() )
				texture.flags |= STUDIO_NF_ADDITIVE;

			m_pCheckBoxes[ CheckBox::CHROME ]->SetValue( false );
			m_pCheckBoxes[ CheckBox::TRANSPARENT ]->SetValue( false );

			break;
		}

	case CheckBox::TRANSPARENT:
		{
			texture.flags &= ~STUDIO_NF_RENDER_FLAGS;

			if( pCheckBox->GetValue() )
				texture.flags |= STUDIO_NF_MASKED;

			m_pCheckBoxes[ CheckBox::CHROME ]->SetValue( false );
			m_pCheckBoxes[ CheckBox::ADDITIVE ]->SetValue( false );

			break;
		}

	case CheckBox::SHOW_UV_MAP:
		{
			m_pHLMV->GetState()->showUVMap = pCheckBox->GetValue();

			break;
		}

	case CheckBox::OVERLAY_UV_MAP:
		{
			m_pHLMV->GetState()->overlayUVMap = pCheckBox->GetValue();

			break;
		}

	case CheckBox::ANTI_ALIAS_LINES:
		{
			m_pHLMV->GetState()->antiAliasUVLines = pCheckBox->GetValue();

			break;
		}

	default: break;
	}
}

void CTexturesPanel::MeshChanged( wxCommandEvent& event )
{
	const int iIndex = m_pMesh->GetSelection();

	if( iIndex == wxNOT_FOUND )
		return;

	const ui::CMeshClientData* pMesh = static_cast<const ui::CMeshClientData*>( m_pMesh->GetClientObject( iIndex ) );

	//Null client data means it's "All"
	m_pHLMV->GetState()->pUVMesh = pMesh ? pMesh->m_pMesh : nullptr;
}

void CTexturesPanel::ImportTexture( wxCommandEvent& event )
{
	auto pModel = m_pHLMV->GetState()->GetStudioModel();

	if( !pModel || !pModel->GetTextureHeader() )
	{
		wxMessageBox( "No model loaded!" );
		return;
	}

	const int iTextureIndex = m_pTexture->GetSelection();

	if( iTextureIndex == wxNOT_FOUND )
	{
		wxMessageBox( "No texture selected" );
		return;
	}

	wxFileDialog dlg( this, wxFileSelectorPromptStr, wxEmptyString, wxEmptyString, "Windows Bitmap (*.bmp)|*.bmp" );

	if( dlg.ShowModal() == wxID_CANCEL )
		return;

	const wxString szFilename = dlg.GetPath();

	//Must be BMP
	wxImage image( szFilename, wxBITMAP_TYPE_BMP );

	if( !image.IsOk() )
	{
		wxMessageBox( wxString::Format( "Failed to load image \"%s\"!", szFilename.c_str() ) );
		return;
	}

	const wxPalette& palette = image.GetPalette();

	if( !palette.IsOk() )
	{
		wxMessageBox( wxString::Format( "Palette for image \"%s\" does not exist!", szFilename.c_str() ) );
		return;
	}

	studiohdr_t* const pHdr = pModel->GetTextureHeader();

	mstudiotexture_t& texture = ( ( mstudiotexture_t* ) ( ( byte* ) pHdr + pHdr->textureindex ) )[ iTextureIndex ];

	if( texture.width != image.GetWidth() || texture.height != image.GetHeight() )
	{
		wxMessageBox( wxString::Format( "Image \"%s\" does not have matching dimensions to the current texture (src: %d x %d, dest: %d x %d)", 
										szFilename.c_str(), 
										image.GetWidth(), image.GetHeight(), 
										texture.width, texture.height ) );
		return;
	}

	//Convert to 8 bit palette based image.
	std::unique_ptr<byte[]> texData = std::make_unique<byte[]>( image.GetWidth() * image.GetHeight() );

	byte* pDest = texData.get();

	const unsigned char* pSourceData = image.GetData();

	for( int i = 0; i < image.GetWidth() * image.GetHeight(); ++i, ++pDest, pSourceData += 3 )
	{
		*pDest = palette.GetPixel( pSourceData[ 0 ], pSourceData[ 1 ], pSourceData[ 2 ] );
	}

	byte convPal[ PALETTE_SIZE ];

	memset( convPal, 0, sizeof( convPal ) );

	unsigned char r, g, b;

	for( size_t uiIndex = 0; uiIndex < PALETTE_ENTRIES; ++uiIndex )
	{
		if( palette.GetRGB( uiIndex, &r, &g, &b ) )
		{
			convPal[ uiIndex * PALETTE_CHANNELS ]		= r;
			convPal[ uiIndex * PALETTE_CHANNELS + 1 ]	= g;
			convPal[ uiIndex * PALETTE_CHANNELS + 2 ]	= b;
		}
	}

	//Copy over the new image data to the texture.
	memcpy( ( byte* ) pHdr + texture.index, texData.get(), image.GetWidth() * image.GetHeight() );
	memcpy( ( byte* ) pHdr + texture.index + image.GetWidth() * image.GetHeight(), convPal, PALETTE_SIZE );

	pModel->ReplaceTexture( &texture, texData.get(), convPal, pModel->GetTextureId( iTextureIndex ) );
}

void CTexturesPanel::ExportTexture( wxCommandEvent& event )
{
	auto pModel = m_pHLMV->GetState()->GetStudioModel();

	if( !pModel )
	{
		wxMessageBox( "No model loaded!" );
		return;
	}

	const int iTextureIndex = m_pTexture->GetSelection();

	if( iTextureIndex == wxNOT_FOUND )
	{
		wxMessageBox( "No texture selected" );
		return;
	}

	wxFileDialog dlg( this, wxFileSelectorPromptStr, wxEmptyString, wxEmptyString, "Windows Bitmap (*.bmp)|*.bmp", wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

	if( dlg.ShowModal() == wxID_CANCEL )
		return;

	const wxString szFilename = dlg.GetPath();

	studiohdr_t* const pHdr = pModel->GetTextureHeader();

	mstudiotexture_t& texture = ( ( mstudiotexture_t* ) ( ( byte* ) pHdr + pHdr->textureindex ) )[ iTextureIndex ];

	if( !graphics::bmpfile::SaveBMPFile( szFilename.c_str(), texture.width, texture.height, ( uint8_t* ) pHdr + texture.index, ( uint8_t* ) pHdr + texture.index + texture.width * texture.height ) )
	{
		wxMessageBox( wxString::Format( "Failed to save image \"%s\"!", szFilename.c_str() ) );
	}
}

void CTexturesPanel::ExportUVMap( wxCommandEvent& event )
{
	auto pModel = m_pHLMV->GetState()->GetStudioModel();

	if( !pModel )
	{
		wxMessageBox( "No model loaded!" );
		return;
	}

	const int iTextureIndex = m_pTexture->GetSelection();

	if( iTextureIndex == wxNOT_FOUND )
	{
		wxMessageBox( "No texture selected" );
		return;
	}

	wxFileDialog dlg( this, wxFileSelectorPromptStr, wxEmptyString, wxEmptyString, "Windows Bitmap (*.bmp)|*.bmp", wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

	if( dlg.ShowModal() == wxID_CANCEL )
		return;

	const wxString szFilename = dlg.GetPath();

	m_pHLMV->SaveUVMap( szFilename, iTextureIndex );
}

void CTexturesPanel::SetTexture( int iIndex )
{
	StudioModel* const pStudioModel = m_pHLMV->GetState()->GetStudioModel();

	const studiohdr_t* const pHdr = pStudioModel->GetStudioHeader();
	const studiohdr_t* const pTexHdr = pStudioModel->GetTextureHeader();

	m_pMesh->Clear();

	if( !pHdr || !pTexHdr )
	{
		m_pMesh->Enable( false );
		return;
	}

	if( iIndex < 0 || iIndex >= pTexHdr->numtextures )
		iIndex = 0;

	const mstudiotexture_t& texture = ( ( mstudiotexture_t* ) ( ( byte* ) pTexHdr + pTexHdr->textureindex ) ) [ iIndex ];

	m_pTexture->Select( iIndex );

	m_pTextureSize->SetLabelText( wxString::Format( "Texture (size: %d x %d)", texture.width, texture.height ) );

	m_pCheckBoxes[ CheckBox::CHROME ]->SetValue( ( texture.flags & STUDIO_NF_CHROME ) != 0 );
	m_pCheckBoxes[ CheckBox::ADDITIVE ]->SetValue( ( texture.flags & STUDIO_NF_ADDITIVE ) != 0 );
	m_pCheckBoxes[ CheckBox::TRANSPARENT ]->SetValue( ( texture.flags & STUDIO_NF_MASKED ) != 0 );

	const StudioModel::MeshList_t* pMeshList = pStudioModel->GetMeshListByTexture( iIndex );

	if( pMeshList )
	{
		m_pMesh->Enable( true );

		size_t uiIndex;

		for( uiIndex = 0; uiIndex < pMeshList->size(); ++uiIndex )
		{
			m_pMesh->Append( wxString::Format( "Mesh %u", uiIndex + 1 ), new ui::CMeshClientData( ( *pMeshList )[ uiIndex ] ) );
		}

		if( uiIndex > 0 )
		{
			m_pHLMV->GetState()->pUVMesh = ( *pMeshList )[ 0 ];

			if( uiIndex > 1 )
			{
				m_pMesh->Append( "All" );
			}
		}

		m_pMesh->Select( 0 );
	}

	m_pHLMV->GetState()->texture = iIndex;
}

void CTexturesPanel::SetScale( int iScale, const bool bSetSlider )
{
	if( iScale < TEXTUREVIEW_SLIDER_MIN || iScale > TEXTUREVIEW_SLIDER_MAX )
		iScale = TEXTUREVIEW_SLIDER_DEFAULT;

	m_pHLMV->GetState()->textureScale = iScale;

	if( bSetSlider )
		m_pScaleTextureView->SetValue( iScale );

	m_pScaleTextureViewSize->SetLabelText( wxString::Format( "Scale Texture View (%dx)", iScale ) );
}
}