"C:\Program Files\MRE_SDK\tools\DllPackage.exe" "D:\MyGitHub\mre_split_txt\mre_split_txt.vcproj"
if %errorlevel% == 0 (
 echo postbuild OK.
  copy mre_split_txt.vpp ..\..\..\MoDIS_VC9\WIN32FS\DRIVE_E\mre_split_txt.vpp /y
exit 0
)else (
echo postbuild error
  echo error code: %errorlevel%
  exit 1
)

