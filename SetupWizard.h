#include "GUI/GUISetupWizard.h"

class SetupWizard : public GUISetupWizard
{
public:
	SetupWizard(wxWindow* parent);

	virtual void wizardCancel( wxWizardEvent& event );

	virtual void wizardFinished( wxWizardEvent& event );

	virtual void wizardNext( wxWizardEvent& event );

	enum EFileBackupChoice
	{
		EFileBackupChoice_AllWithoutSystem,
		EFileBackupChoice_UserFiles,
		EFileBackupChoice_Manual
	};

	enum EImageBackupChoice
	{
		EImageBackupChoice_System,
		EImageBackupChoice_AllNonUsb,
		EImageBackupChoice_Manual
	};

	static void finishSetup(EFileBackupChoice fileBackupChoice, EImageBackupChoice imageBackupChoice, const std::wstring& volume_choice);

	static void readConfig(EFileBackupChoice& fileBackupChoice, EImageBackupChoice& imageBackupChoice, std::wstring& volume_choice);

	static void doDefConfig();

	virtual void manualVolumeConfig( wxCommandEvent& event );

};