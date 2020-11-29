[Setup]
AppName=Strategr
AppVersion=@VERSION_SHORT@
AppVerName=Strategr v@VERSION_SHORT@
AppPublisher=Dmitry Khrykin
AppPublisherURL=https://khrykin.github.io/strategr
DefaultGroupName=Strategr
ChangesAssociations=yes

DefaultDirName={pf@WINDOWS_64_OR_EMPTY@}\Strategr
OutputBaseFilename=Strategr-@VERSION@-windows-@WINDOWS_PLATFORM_NAME@

[Files]
Source: "$<SHELL_PATH:$<TARGET_FILE:Strategr>>"; DestDir: {app};
Source: "$<SHELL_PATH:$<TARGET_FILE_DIR:Strategr>>\WinSparkle.dll"; DestDir: {app};
Source: "$<SHELL_PATH:$<TARGET_FILE_DIR:Strategr>>\vc_redist.@WINDOWS_PLATFORM_NAME@.exe"; DestDir: {app};

[Run]
Filename: "{app}\vc_redist.@WINDOWS_PLATFORM_NAME@.exe"; StatusMsg: "Installing Visual C++ Runtime..."; Parameters: "/passive /norestart"; Check: VC2019RedistNeedsInstall; Flags: waituntilterminated
Filename: "{app}\Strategr.exe"; Description: "Launch Strategr"; Flags: postinstall nowait

[Icons]
Name: "{group}\Strategr"; Filename: "{app}\Strategr.exe"; WorkingDir: "{app}"
Name: "{group}\Uninstall Strategr"; Filename: "{uninstallexe}"
Name: "{commondesktop}\Strategr"; Filename: "{app}\Strategr.exe"; WorkingDir: "{app}"; Tasks: desktopicon    

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";

[Registry]
Root: HKLM; Subkey: "Software\Classes\.stg"; ValueType: string; ValueName: ""; ValueData: "Strategy"; Flags: uninsdeletevalue
Root: HKLM; Subkey: "Software\Classes\Strategy"; ValueType: string; ValueName: ""; ValueData: "Strategy"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\Classes\Strategy\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\Strategr.exe,1"
Root: HKLM; Subkey: "Software\Classes\Strategy\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\Strategr.exe"" ""%1"""

[Code]
function VC2019RedistNeedsInstall: Boolean;
var
  Version: String;
begin
  if (RegQueryStringValue(HKEY_LOCAL_MACHINE, '\SOFTWARE\Wow6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\@WINDOWS_PLATFORM_NAME@', 'Version', Version)) then
  begin
    // Is the installed version at least 14.24?
    Log('VC Redist Version check : found ' + Version);
    Result := (CompareStr(Version, 'v14.24.28127.04')<0);
  end
  else
  begin
    // Not even an old version installed
    Result := True;
  end;
  if (Result) then
  begin
    ExtractTemporaryFile('vc_redist.@WINDOWS_PLATFORM_NAME@.exe');
  end;
end;
