param(
	[Parameter(Mandatory=$true)]
	[ValidateSet("windows", "linux", "all")]
	[string]$Target,

	[Parameter(Mandatory=$true)]
	[string]$ProjectName
)

if (Test-Path "Deploy"){
	Remove-Item "Deploy" -Recurse -Force
}

mkdir "Deploy"

function Build-Windows {
	Write-Host "=== Compilation Windows ==="

	Remove-Item "build" -Recurse -Force

	cmake -B build -S . -G "Visual Studio 17 2022" -T v143 -DCMAKE_PREFIX_PATH=C:/Qt/6.10.2/Static/msvc

	cmake --build build --config Release

	cp build/Release/*.exe Deploy/

	Write-Host "=== Exécutable généré ==="
}

function Build-Linux {
	Write-Host "=== Compilation Linux ==="

		$clean = @"
rm -rf /home/echo/Projet/$ProjectName/build-linux
rm -rf /home/echo/Projet/$ProjectName/$ProjectName.tar.gz
"@
	wsl --distribution archlinux --user echo -- bash -lc "$clean"

	$cmd = @"
mkdir -p /home/echo/Projet/$ProjectName
cmake -B /home/echo/Projet/$ProjectName/build-linux -S . -DCMAKE_PREFIX_PATH=/home/echo/Qt/6.10.2/Static/gcc
cmake --build /home/echo/Projet/$ProjectName/build-linux

cd /home/echo/Projet/$ProjectName/build-linux
tar -czf ../$ProjectName.tar.gz *
"@

	wsl --distribution archlinux --user echo -- bash -lc "$cmd"

	wsl --distribution archlinux --user echo -- bash -lc "cat /home/echo/Projet/$ProjectName/$ProjectName.tar.gz > Deploy/$ProjectName.tar.gz"
	
	Write-Host "=== Binaire généré ==="
}

switch ($Target) {
	"windows" { Build-Windows }
	"linux" { Build-Linux }
	"all" { Build-Windows; Build-Linux }
}