;Installer for Readability Studio
[Setup]
#include "setup.isl"
MergeDuplicateFiles=true
Compression=lzma
SolidCompression=true
UserInfoPage=true
ChangesAssociations=true
PrivilegesRequired=admin
LicenseFile=..\redist\license.rtf
ShowLanguageDialog=yes
LanguageDetectionMethod=locale
AppID={{FBFBEAB6-622A-4E16-893D-216C25FF6A69}
AppendDefaultDirName=no
; Windows 7 SP2 required, but don't use MinVersion because some
; customized Windows systems have their OS version messed up and InnoSetup can't
; determine the version.

[Files]
; program files
Source: release\readstudio.exe; DestDir: {app}; Components: ProgramFiles; Flags: replacesameversion restartreplace
; resource files (required part of programs)
Source: ..\resources\res.wad; DestDir: {app}; Components: ProgramFiles; Flags: replacesameversion restartreplace recursesubdirs
Source: ..\resources\words.wad; DestDir: {app}; Components: ProgramFiles; Flags: replacesameversion restartreplace recursesubdirs
Source: ..\..\resources\scripting\*.api; DestDir: {app}; Components: ProgramFiles; Flags: replacesameversion restartreplace recursesubdirs
Source: ..\..\resources\scripting\*.lua; DestDir: {app}; Components: ProgramFiles; Flags: replacesameversion restartreplace recursesubdirs
Source: ..\..\resources\report-themes\*; DestDir: {app}\report-themes; Components: ProgramFiles; Flags: replacesameversion restartreplace recursesubdirs
Source: ..\..\resources\themes\*; DestDir: {app}\themes; Components: ProgramFiles; Flags: replacesameversion restartreplace recursesubdirs
Source: ..\redist\license.rtf; DestDir: {app}; Components: ProgramFiles; Flags: replacesameversion
; help files
Source: ..\..\docs\ReadabilityStudioDocs\docs\*; DestDir: {app}\ReadabilityStudioDocs; Components: HelpFiles; Excludes: "thumbs.db,*.tex,*.log,README.md,\.git"; Flags: replacesameversion restartreplace recursesubdirs
Source: ..\..\docs\ReadabilityTestsReference\docs\ReadabilityTestReference.pdf; DestDir: {app}\ReadabilityStudioDocs; Components: HelpFiles; Flags: replacesameversion restartreplace recursesubdirs
Source: ..\..\docs\ShortcutsCheatsheet\docs\shortcuts-cheatsheet.pdf; DestDir: {app}\ReadabilityStudioDocs; Components: HelpFiles; Flags: replacesameversion restartreplace recursesubdirs
;FUTURE-RELEASE Source: ..\..\docs\ReadabilityStudioAPI\ReadabilityStudioAPI\*; DestDir: {app}\ReadabilityStudioAPI; Components: HelpFiles; Excludes: "thumbs.db,*.tex,*.log,README.md,\.git"; Flags: replacesameversion restartreplace recursesubdirs
Source: ..\..\docs\lua-5.4\doc\*; DestDir: {app}\ReadabilityStudioDocs\lua-5.4\doc; Components: HelpFiles; Excludes: "thumbs.db"; Flags: replacesameversion restartreplace recursesubdirs
; examples
Source: ..\..\examples\*; DestDir: {app}\examples; Components: Examples; Excludes: "thumbs.db,README.md,\.git"; Flags: replacesameversion restartreplace recursesubdirs

[Languages]
#include "lang.isl"

[Icons]
#include "icons.isl"

[Registry]
; Make is so that you can run the program from the command line
Root: HKLM; Subkey: SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\readstudio.exe; ValueType: string; ValueName: ; ValueData: {app}\readstudio.exe; Flags: uninsdeletekey
; Set file association (standard project)
Root: HKCR; Subkey: .rsp; ValueType: string; ValueData: ReadStudioProject; Flags: uninsdeletekey
Root: HKCR; Subkey: ReadStudioProject; ValueType: string; ValueName: ; ValueData: Readability Studio Project; Flags: uninsdeletekey
Root: HKCR; Subkey: ReadStudioProject\DefaultIcon; ValueType: string; ValueData: {app}\readstudio.EXE,0; Flags: uninsdeletekey
Root: HKCR; Subkey: ReadStudioProject\shell\open\command; ValueType: string; ValueData: """{app}\readstudio.EXE"" ""%1"""; Flags: uninsdeletekey
; Set file association (batch project)
Root: HKCR; Subkey: .rsbp; ValueType: string; ValueData: ReadStudioBatchProject; Flags: uninsdeletekey
Root: HKCR; Subkey: ReadStudioBatchProject; ValueType: string; ValueName: ; ValueData: Readability Studio Project; Flags: uninsdeletekey
Root: HKCR; Subkey: ReadStudioBatchProject\DefaultIcon; ValueType: string; ValueData: {app}\readstudio.EXE,0; Flags: uninsdeletekey
Root: HKCR; Subkey: ReadStudioBatchProject\shell\open\command; ValueType: string; ValueData: """{app}\readstudio.EXE"" ""%1"""; Flags: uninsdeletekey

[Components]
Name: ProgramFiles; Description: {cm:ProgramFiles}; Flags: fixed; Types: Typical Custom
Name: HelpFiles; Description: {cm:HelpFiles}; Types: Typical Custom
Name: Examples; Description: {cm:Examples}; Types: Typical Custom

[Types]
Name: Typical; Description: {cm:Typical}
Name: Custom; Description: {cm:Custom}; Flags: iscustom

[InstallDelete]
; Delete any old start menu icons
Type: filesandordirs ; Name:"{commonprograms}\Readability Studio"

[UninstallDelete]
Type: files; Name: {app}\rs.xml
Name: {app}\res; Type: dirifempty
Name: {app}; Type: dirifempty

[Code]
#include "helper.isl"

var
  Upgrade: Boolean;

function InitializeSetup(): Boolean;
var
  userName: String;
begin
  // next, see if this is an upgrade installation
  if RegQueryStringValue(HKLM,
    'Software\Microsoft\Windows\CurrentVersion\Uninstall\{FBFBEAB6-622A-4E16-893D-216C25FF6A69}_is1',
    'Inno Setup: User Info: Name', userName) then
    begin
      Upgrade := true
    end
  // no user info, so prompt for it later
  else
    Upgrade := false;

  Result := true
end;

// see if certain dialogs should be skipped if we are in upgrade mode
function ShouldSkipPage(PageID: Integer): Boolean;
var
   iconGroup: String;
   installDir: String;
begin
  if (PageID = wpUserInfo) then
    Result := Upgrade
  else if (PageID = wpLicense) then
    Result := Upgrade
  else if (PageID = wpReady) then
    Result := Upgrade
  // install directory dialog
  else if (PageID = wpSelectDir) then
    begin
      if (Upgrade = true) then
        begin
          // first, see if there is an installation folder already
          if RegQueryStringValue(HKLM,
              'Software\Microsoft\Windows\CurrentVersion\Uninstall\{FBFBEAB6-622A-4E16-893D-216C25FF6A69}_is1',
              'Inno Setup: App Path', installDir) then
            Result := true
          // no path found, so prompt for it
          else
            Result := false
        end
      else
        Result := false
    end
  // menu group dialog
  else if (PageID = wpSelectProgramGroup) then
    begin
      if (Upgrade = true) then
        begin
          // first, see if there is a menu group already
          if RegQueryStringValue(HKLM,
              'Software\Microsoft\Windows\CurrentVersion\Uninstall\{FBFBEAB6-622A-4E16-893D-216C25FF6A69}_is1',
              'Inno Setup: Icon Group', iconGroup) then
            Result := true
          // no menu group found, so prompt for it
          else
            Result := false
        end
      else
        Result := false
    end
  else
    Result := false
end;
