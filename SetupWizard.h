#include "gui/GUISetupWizard.h"
#include <map>
#include <string>

struct SVolumesCache
{
	std::map<std::wstring, bool> is_ntfs_info;
	std::map<std::wstring, bool> is_usb_info;
};

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

	static void readConfig(EFileBackupChoice& fileBackupChoice, EImageBackupChoice& imageBackupChoice, std::wstring& volume_choice, bool& no_setupwizard);

	static void doDefConfig();

	static bool runSetupWizard();

	virtual void manualVolumeConfig( wxCommandEvent& event );

private:
	static SVolumesCache* cache;
};