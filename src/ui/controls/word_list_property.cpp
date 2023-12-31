#include "word_list_property.h"

wxIMPLEMENT_DYNAMIC_CLASS(WordListProperty, wxLongStringProperty);

//------------------------------------------------------
WordListProperty::WordListProperty(const wxString& label /*= wxPG_LABEL*/,
                                   const wxString& name /*= wxPG_LABEL*/,
                                   const wxString& value /*= wxString{}*/)
    : wxLongStringProperty(label, name, value), m_phraseMode(false)
    {
    }

//------------------------------------------------------
wxValidator* WordListProperty::DoGetValidator() const
    {
    return wxFileProperty::GetClassValidator();
    }

//------------------------------------------------------
bool WordListProperty::DoSetAttribute(const wxString& name, wxVariant& value)
    {
    if (name == wxPG_HELP_PATH)
        {
        m_helpPath = value.GetString();
        return true;
        }
    else if (name == wxPG_TOPIC_PATH)
        {
        m_topicPath = value.GetString();
        return true;
        }
    else if (name == wxPG_PHRASE_MODE)
        {
        m_phraseMode = value.GetBool();
        return true;
        }
    return wxLongStringProperty::DoSetAttribute(name, value);
    }

//------------------------------------------------------
bool WordListProperty::DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value)
    {
    assert(value.IsType(_DT("string")) && "Function called for incompatible property");

    EditWordListDlg editDlg(pg->GetPanel(), wxID_ANY,
                            m_dlgTitle.length() ? m_dlgTitle : _(L"Edit Word List"));
    if (m_helpPath.length())
        {
        editDlg.SetHelpTopic(m_helpPath, m_topicPath);
        }
    editDlg.SetPhraseFileMode(m_phraseMode);
    editDlg.SetFilePath(value.GetString());
    if (editDlg.ShowModal() == wxID_OK)
        {
        value = editDlg.GetFilePath();
        return true;
        }
    return false;
    }
