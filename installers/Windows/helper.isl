function IsCurrentVersionUpToDate(VersionString: String; MajorVersion, MinorVersion: Cardinal): Boolean;
var
  MS, LS: Cardinal;
begin
  // if the file does not exist then obviously it is not up to date
	if not GetVersionNumbers(VersionString, MS, LS) then
    Result := false
  else
  if (MS > MajorVersion) or
     ((MS = MajorVersion) and (LS >= MinorVersion)) then
    Result := true
  else
    Result := false
end;

function ShouldInstallVCPlusPlus8SP1x86(): Boolean;
begin
  // {A49F249F-0C91-497F-86DF-B2585E8E76B7} is the product code,
  // but checking that is a bit unreliable, just always install this update.
  Result := true
end;

// Returns false if a reboot is required (or other error occurred)
function InstallSystemUpdates(): Boolean;
var
  ResultCode: Integer;
begin
  if (ShouldInstallVCPlusPlus8SP1x86()) then
    begin
    ExtractTemporaryFile('VC_redist.x86.exe');
    if (Exec(ExpandConstant('{tmp}\VC_redist.x86.exe'), '/quiet', '', SW_SHOWNORMAL,
        ewWaitUntilTerminated, ResultCode) ) then
      begin
        if (ResultCode = 3010) or (ResultCode = 1641) then  //ERROR_SUCCESS_REBOOT_REQUIRED, ERROR_SUCCESS_REBOOT_INITIATED
          begin
            MsgBox(ExpandConstant('{cm:VCUpdateComplete}'), mbInformation, MB_OK);
            Result := false
          end
        else if (ResultCode = 1638) then  //ERROR_PRODUCT_VERSION, just means already installed
          begin
            Result := true
          end
        else if (ResultCode = 1618) then  //ERROR_INSTALL_ALREADY_RUNNING
          begin
            MsgBox(ExpandConstant('{cm:UpdateErrorAnotherInstaller}'), mbInformation, MB_OK);
            Result := false
          end
        else if (ResultCode = 1625) then  //ERROR_INSTALL_PACKAGE_REJECTED
          begin
            MsgBox(ExpandConstant('{cm:UpdateErrorInvalidRights}'), mbInformation, MB_OK);
            Result := false
          end
        else if (ResultCode = 1632) then  //ERROR_INSTALL_TEMP_UNWRITABLE
          begin
            MsgBox(ExpandConstant('{cm:UpdateErrorTempUnwritable}'), mbInformation, MB_OK);
            Result := false
          end
        else if (ResultCode <> 0) then  //Unknown error
          begin
            MsgBox(ExpandConstant('{cm:VCUpdateError}') + #13 + #10 + Format('%s%d', [ExpandConstant('{cm:ErrorCode}'), ResultCode]), mbInformation, MB_OK);
            Result := false
          end
      end
    end;

  Result := true
end;
