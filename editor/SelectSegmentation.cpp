#include "SelectSegmentation.h"

//(*InternalHeaders(SelectSegmentation)
#include <wx/string.h>
#include <wx/intl.h>
//*)

//(*IdInit(SelectSegmentation)
const long SelectSegmentation::ID_STATICBOX2 = wxNewId();
const long SelectSegmentation::ID_BUTTON1 = wxNewId();
const long SelectSegmentation::ID_BUTTON2 = wxNewId();
const long SelectSegmentation::ID_STATICBOX1 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT1 = wxNewId();
const long SelectSegmentation::ID_SPINCTRL1 = wxNewId();
const long SelectSegmentation::ID_SPINCTRL2 = wxNewId();
const long SelectSegmentation::ID_SPINCTRL3 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT2 = wxNewId();
const long SelectSegmentation::ID_SPINCTRL4 = wxNewId();
const long SelectSegmentation::ID_SPINCTRL5 = wxNewId();
const long SelectSegmentation::ID_SPINCTRL6 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT3 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT4 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT5 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT6 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL1 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL2 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT7 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL3 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL4 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT8 = wxNewId();
const long SelectSegmentation::ID_SPINCTRL7 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT9 = wxNewId();
const long SelectSegmentation::ID_SPINCTRL8 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT10 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL5 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL6 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT11 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL7 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL8 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT12 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL9 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL10 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL11 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL12 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL13 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL14 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT13 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT14 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT15 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT16 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL15 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL16 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL17 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT17 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL18 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL19 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL20 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL21 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL22 = wxNewId();
const long SelectSegmentation::ID_TEXTCTRL23 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT18 = wxNewId();
const long SelectSegmentation::ID_STATICTEXT19 = wxNewId();
const long SelectSegmentation::ID_CHOICE1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(SelectSegmentation,wxDialog)
	//(*EventTable(SelectSegmentation)
	//*)
END_EVENT_TABLE()

SelectSegmentation::SelectSegmentation(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(SelectSegmentation)
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxSize(748,561));
	StaticBox2 = new wxStaticBox(this, ID_STATICBOX2, _("Depth"), wxPoint(376,16), wxSize(352,432), 0, _T("ID_STATICBOX2"));
	ButtonCancel = new wxButton(this, ID_BUTTON1, _("Cancel"), wxPoint(640,464), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	ButtonOk = new wxButton(this, ID_BUTTON2, _("Ok"), wxPoint(376,464), wxSize(256,27), 0, wxDefaultValidator, _T("ID_BUTTON2"));
	StaticBox1 = new wxStaticBox(this, ID_STATICBOX1, _("RGB"), wxPoint(16,16), wxSize(352,432), 0, _T("ID_STATICBOX1"));
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Minimum : "), wxPoint(40,66), wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	minR = new wxSpinCtrl(this, ID_SPINCTRL1, _T("0"), wxPoint(128,64), wxSize(56,23), 0, 0, 255, 0, _T("ID_SPINCTRL1"));
	minR->SetValue(_T("0"));
	minG = new wxSpinCtrl(this, ID_SPINCTRL2, _T("0"), wxPoint(200,64), wxSize(56,23), 0, 0, 100, 0, _T("ID_SPINCTRL2"));
	minG->SetValue(_T("0"));
	minB = new wxSpinCtrl(this, ID_SPINCTRL3, _T("0"), wxPoint(272,64), wxSize(56,23), 0, 0, 100, 0, _T("ID_SPINCTRL3"));
	minB->SetValue(_T("0"));
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Maximum :"), wxPoint(40,100), wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	maxR = new wxSpinCtrl(this, ID_SPINCTRL4, _T("255"), wxPoint(128,96), wxSize(56,23), 0, 0, 255, 255, _T("ID_SPINCTRL4"));
	maxR->SetValue(_T("255"));
	maxG = new wxSpinCtrl(this, ID_SPINCTRL5, _T("255"), wxPoint(200,96), wxSize(56,23), 0, 0, 255, 255, _T("ID_SPINCTRL5"));
	maxG->SetValue(_T("255"));
	maxB = new wxSpinCtrl(this, ID_SPINCTRL6, _T("255"), wxPoint(272,96), wxSize(56,23), 0, 0, 255, 255, _T("ID_SPINCTRL6"));
	maxB->SetValue(_T("255"));
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("R"), wxPoint(150,40), wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("G"), wxPoint(225,40), wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("B"), wxPoint(294,40), wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Crop:"), wxPoint(40,160), wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, _("0"), wxPoint(80,156), wxSize(48,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	TextCtrl2 = new wxTextCtrl(this, ID_TEXTCTRL2, _("0"), wxPoint(130,156), wxSize(48,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _("---->"), wxPoint(188,158), wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	TextCtrl3 = new wxTextCtrl(this, ID_TEXTCTRL3, _("Max"), wxPoint(232,156), wxSize(48,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
	TextCtrl4 = new wxTextCtrl(this, ID_TEXTCTRL4, _("Max"), wxPoint(284,156), wxSize(48,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL4"));
	StaticText8 = new wxStaticText(this, ID_STATICTEXT8, _("Minimum :"), wxPoint(392,66), wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	SpinCtrl1 = new wxSpinCtrl(this, ID_SPINCTRL7, _T("0"), wxPoint(480,64), wxDefaultSize, 0, 0, 10000, 0, _T("ID_SPINCTRL7"));
	SpinCtrl1->SetValue(_T("0"));
	StaticText9 = new wxStaticText(this, ID_STATICTEXT9, _("Maximum :"), wxPoint(392,100), wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	SpinCtrl2 = new wxSpinCtrl(this, ID_SPINCTRL8, _T("10000"), wxPoint(480,96), wxDefaultSize, 0, 0, 10000, 10000, _T("ID_SPINCTRL8"));
	SpinCtrl2->SetValue(_T("10000"));
	StaticText10 = new wxStaticText(this, ID_STATICTEXT10, _("Crop:"), wxPoint(392,160), wxDefaultSize, 0, _T("ID_STATICTEXT10"));
	TextCtrl5 = new wxTextCtrl(this, ID_TEXTCTRL5, _("0"), wxPoint(432,156), wxSize(48,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL5"));
	TextCtrl6 = new wxTextCtrl(this, ID_TEXTCTRL6, _("0"), wxPoint(482,156), wxSize(48,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL6"));
	StaticText11 = new wxStaticText(this, ID_STATICTEXT11, _("---->"), wxPoint(536,158), wxDefaultSize, 0, _T("ID_STATICTEXT11"));
	TextCtrl7 = new wxTextCtrl(this, ID_TEXTCTRL7, _("Max"), wxPoint(584,156), wxSize(48,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL7"));
	TextCtrl8 = new wxTextCtrl(this, ID_TEXTCTRL8, _("Max"), wxPoint(640,156), wxSize(48,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL8"));
	StaticText12 = new wxStaticText(this, ID_STATICTEXT12, _("Bounding Box"), wxPoint(392,208), wxDefaultSize, 0, _T("ID_STATICTEXT12"));
	TextCtrl9 = new wxTextCtrl(this, ID_TEXTCTRL9, _("0.0"), wxPoint(496,224), wxSize(56,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL9"));
	TextCtrl10 = new wxTextCtrl(this, ID_TEXTCTRL10, _("0.0"), wxPoint(560,224), wxSize(56,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL10"));
	TextCtrl11 = new wxTextCtrl(this, ID_TEXTCTRL11, _("0.0"), wxPoint(624,224), wxSize(56,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL11"));
	TextCtrl12 = new wxTextCtrl(this, ID_TEXTCTRL12, _("0.0"), wxPoint(496,256), wxSize(56,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL12"));
	TextCtrl13 = new wxTextCtrl(this, ID_TEXTCTRL13, _("0.0"), wxPoint(560,256), wxSize(56,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL13"));
	TextCtrl14 = new wxTextCtrl(this, ID_TEXTCTRL14, _("0.0"), wxPoint(624,256), wxSize(56,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL14"));
	StaticText13 = new wxStaticText(this, ID_STATICTEXT13, _("Minimum"), wxPoint(424,228), wxDefaultSize, 0, _T("ID_STATICTEXT13"));
	StaticText14 = new wxStaticText(this, ID_STATICTEXT14, _("Maximum"), wxPoint(424,258), wxDefaultSize, 0, _T("ID_STATICTEXT14"));
	StaticText15 = new wxStaticText(this, ID_STATICTEXT15, _("Plane"), wxPoint(392,288), wxDefaultSize, 0, _T("ID_STATICTEXT15"));
	StaticText16 = new wxStaticText(this, ID_STATICTEXT16, _("Point A"), wxPoint(424,312), wxDefaultSize, 0, _T("ID_STATICTEXT16"));
	TextCtrl15 = new wxTextCtrl(this, ID_TEXTCTRL15, _("0.0"), wxPoint(496,308), wxSize(56,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL15"));
	TextCtrl16 = new wxTextCtrl(this, ID_TEXTCTRL16, _("0.0"), wxPoint(560,308), wxSize(56,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL16"));
	TextCtrl17 = new wxTextCtrl(this, ID_TEXTCTRL17, _("0.0"), wxPoint(624,308), wxSize(56,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL17"));
	StaticText17 = new wxStaticText(this, ID_STATICTEXT17, _("Point B"), wxPoint(424,340), wxDefaultSize, 0, _T("ID_STATICTEXT17"));
	TextCtrl18 = new wxTextCtrl(this, ID_TEXTCTRL18, _("0.0"), wxPoint(496,336), wxSize(56,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL18"));
	TextCtrl19 = new wxTextCtrl(this, ID_TEXTCTRL19, _("0.0"), wxPoint(560,336), wxSize(56,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL19"));
	TextCtrl20 = new wxTextCtrl(this, ID_TEXTCTRL20, _("0.0"), wxPoint(624,336), wxSize(56,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL20"));
	TextCtrl21 = new wxTextCtrl(this, ID_TEXTCTRL21, _("0.0"), wxPoint(496,364), wxSize(56,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL21"));
	TextCtrl22 = new wxTextCtrl(this, ID_TEXTCTRL22, _("0.0"), wxPoint(560,364), wxSize(56,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL22"));
	TextCtrl23 = new wxTextCtrl(this, ID_TEXTCTRL23, _("0.0"), wxPoint(624,364), wxSize(56,23), 0, wxDefaultValidator, _T("ID_TEXTCTRL23"));
	StaticText18 = new wxStaticText(this, ID_STATICTEXT18, _("Point C"), wxPoint(424,368), wxDefaultSize, 0, _T("ID_STATICTEXT18"));
	StaticText19 = new wxStaticText(this, ID_STATICTEXT19, _("Combination Method"), wxPoint(24,468), wxDefaultSize, 0, _T("ID_STATICTEXT19"));
	ChoiceCombination = new wxChoice(this, ID_CHOICE1, wxPoint(192,464), wxSize(168,25), 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	ChoiceCombination->SetSelection( ChoiceCombination->Append(_("No combination")) );
	ChoiceCombination->Append(_("AND combination"));
	ChoiceCombination->Append(_("OR combination"));
	ChoiceCombination->Append(_("XOR combination "));
	ChoiceCombination->Append(_("RGB only"));
	ChoiceCombination->Append(_("Depth only "));

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SelectSegmentation::OnButtonCancelClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&SelectSegmentation::OnButtonOkClick);
	//*)
}

SelectSegmentation::~SelectSegmentation()
{
	//(*Destroy(SelectSegmentation)
	//*)
}


void SelectSegmentation::OnButtonOkClick(wxCommandEvent& event)
{
    Close();
}

void SelectSegmentation::OnButtonCancelClick(wxCommandEvent& event)
{
    Close();
}