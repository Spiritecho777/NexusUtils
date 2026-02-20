param(
	[Parameter(Mandatory=$true)]
	[ValidateSet("all", "all_secure", "all_web", "all_web_secure")]
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

function Build-Windows-OpenSSL {
	Write-Host "=== Compilation Windows avec WebEngine ==="

	Remove-Item "build" -Recurse -Force

	cmake -B build -S . -G "Visual Studio 17 2022" -T v143 -DCMAKE_PREFIX_PATH=C:/Qt/6.10.2/Static/msvc -DOPENSSL_ROOT_DIR=C:/dev/vcpkg/installed/x64-windows-static -DOPENSSL_USE_STATIC_LIBS=ON

	cmake --build build --config Release

	cp build/Release/*.exe Deploy/

	Write-Host "=== Exécutable généré ==="
}

function Build-Windows-Web {
	Write-Host "=== Compilation Windows avec WebEngine ==="

	Remove-Item "build" -Recurse -Force

	cmake -B build -S . -G "Visual Studio 17 2022" -T v143 -DCMAKE_PREFIX_PATH=C:/Qt/6.10.2/Static/msvc

	cmake --build build --config Release

	cp build/Release/*.exe Deploy/

	Write-Host "=== Exécutable généré ==="
}

function Build-Windows-Web-OpenSSL {
	Write-Host "=== Compilation Windows avec WebEngine ==="

	Remove-Item "build" -Recurse -Force

	cmake -B build -S . -G "Visual Studio 17 2022" -T v143 -DCMAKE_PREFIX_PATH=C:/Qt/6.10.2/Static/msvc -DOPENSSL_ROOT_DIR=C:/dev/vcpkg/installed/x64-windows-static -DOPENSSL_USE_STATIC_LIBS=ON

	cmake --build build --config Release

	cp build/Release/*.exe Deploy/

	Write-Host "=== Exécutable généré ==="
}

function Build-Linux {
	Write-Host "=== Compilation Linux ==="

		$clean = @"
rm -rf /home/echo/Projet/$ProjectName/build-linux
rm -rf /home/echo/Projet/$ProjectName/package
rm -rf /home/echo/Projet/$ProjectName/$ProjectName.tar.gz
"@
	wsl --distribution archlinux --user echo -- bash -lc "$clean"

	$cmd = @"
mkdir -p /home/echo/Projet/$ProjectName
cmake -B /home/echo/Projet/$ProjectName/build-linux -S . -DCMAKE_PREFIX_PATH=/home/echo/Qt/6.10.2/Static/gcc
cmake --build /home/echo/Projet/$ProjectName/build-linux

mkdir -p /home/echo/Projet/$ProjectName/package
cp /home/echo/Projet/$ProjectName/build-linux/$ProjectName /home/echo/Projet/$ProjectName/package/
cp -r /mnt/c/Users/Asumi/source/repos/$ProjectName/Asset /home/echo/Projet/$ProjectName/package/

cd /home/echo/Projet/$ProjectName/package
tar -czf ../$ProjectName.tar.gz *
"@

	wsl --distribution archlinux --user echo -- bash -lc "$cmd"

	wsl --distribution archlinux --user echo -- bash -lc "cat /home/echo/Projet/$ProjectName/$ProjectName.tar.gz > Deploy/$ProjectName.tar.gz"
	
	Write-Host "=== Binaire généré ==="
}

function Build-Linux-OpenSSL {
	Write-Host "=== Compilation Linux ==="

		$clean = @"
rm -rf /home/echo/Projet/$ProjectName/build-linux
rm -rf /home/echo/Projet/$ProjectName/package
rm -rf /home/echo/Projet/$ProjectName/$ProjectName.tar.gz
"@
	wsl --distribution archlinux --user echo -- bash -lc "$clean"

	$cmd = @"
mkdir -p /home/echo/Projet/$ProjectName
cmake -B /home/echo/Projet/$ProjectName/build-linux -S . -DCMAKE_PREFIX_PATH=/home/echo/Qt/6.10.2/Static/gcc -DOPENSSL_ROOT_DIR=/usr
cmake --build /home/echo/Projet/$ProjectName/build-linux

mkdir -p /home/echo/Projet/$ProjectName/package
cp /home/echo/Projet/$ProjectName/build-linux/$ProjectName /home/echo/Projet/$ProjectName/package/
cp -r /mnt/c/Users/Asumi/source/repos/$ProjectName/Asset /home/echo/Projet/$ProjectName/package/

cd /home/echo/Projet/$ProjectName/package
tar -czf ../$ProjectName.tar.gz *
"@

	wsl --distribution archlinux --user echo -- bash -lc "$cmd"

	wsl --distribution archlinux --user echo -- bash -lc "cat /home/echo/Projet/$ProjectName/$ProjectName.tar.gz > Deploy/$ProjectName.tar.gz"
	
	Write-Host "=== Binaire généré ==="
}

function Build-Linux-Web {
	Write-Host "=== Compilation Linux ==="

		$clean = @"
rm -rf /home/echo/Projet/$ProjectName/build-linux
rm -rf /home/echo/Projet/$ProjectName/package
rm -rf /home/echo/Projet/$ProjectName/$ProjectName.tar.gz
"@
	wsl --distribution archlinux --user echo -- bash -lc "$clean"

	$cmd = @"
mkdir -p /home/echo/Projet/$ProjectName
cmake -B /home/echo/Projet/$ProjectName/build-linux -S . -DCMAKE_PREFIX_PATH=/home/echo/Qt/6.10.2/Static/gcc
cmake --build /home/echo/Projet/$ProjectName/build-linux

mkdir -p /home/echo/Projet/$ProjectName/package
cp /home/echo/Projet/$ProjectName/build-linux/$ProjectName /home/echo/Projet/$ProjectName/package/
cp -r /mnt/c/Users/Asumi/source/repos/$ProjectName/Asset /home/echo/Projet/$ProjectName/package/

cd /home/echo/Projet/$ProjectName/package
tar -czf ../$ProjectName.tar.gz *
"@

	wsl --distribution archlinux --user echo -- bash -lc "$cmd"

	wsl --distribution archlinux --user echo -- bash -lc "cat /home/echo/Projet/$ProjectName/$ProjectName.tar.gz > Deploy/$ProjectName.tar.gz"
	
	Write-Host "=== Binaire généré ==="
}

function Build-Linux-Web-OpenSSL {
	Write-Host "=== Compilation Linux ==="

		$clean = @"
rm -rf /home/echo/Projet/$ProjectName/build-linux
rm -rf /home/echo/Projet/$ProjectName/package
rm -rf /home/echo/Projet/$ProjectName/$ProjectName.tar.gz
"@
	wsl --distribution archlinux --user echo -- bash -lc "$clean"

	$cmd = @"
mkdir -p /home/echo/Projet/$ProjectName
cmake -B /home/echo/Projet/$ProjectName/build-linux -S . -DCMAKE_PREFIX_PATH=/home/echo/Qt/6.10.2/Static/gcc -DOPENSSL_ROOT_DIR=/usr
cmake --build /home/echo/Projet/$ProjectName/build-linux

mkdir -p /home/echo/Projet/$ProjectName/package
cp /home/echo/Projet/$ProjectName/build-linux/$ProjectName /home/echo/Projet/$ProjectName/package/
cp -r /mnt/c/Users/Asumi/source/repos/$ProjectName/Asset /home/echo/Projet/$ProjectName/package/

cd /home/echo/Projet/$ProjectName/package
tar -czf ../$ProjectName.tar.gz *
"@

	wsl --distribution archlinux --user echo -- bash -lc "$cmd"

	wsl --distribution archlinux --user echo -- bash -lc "cat /home/echo/Projet/$ProjectName/$ProjectName.tar.gz > Deploy/$ProjectName.tar.gz"
	
	Write-Host "=== Binaire généré ==="
}

switch ($Target) {
	"all" { Build-Windows; Build-Linux }
	"all_secure" { Build-Windows-OpenSSL; Build-Linux-OpenSSL }
	"all_web" { Build-Windows-Web; Build-Linux-Web }
	"all_web_secure" { Build-Windows-Web-OpenSSL; Build-Linux-Web-OpenSSL }
}