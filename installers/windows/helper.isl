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

