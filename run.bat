@echo off
title BarqOS Windows Installer & Runner ⚡
echo [INFO] Starting BarqOS Build, Run, and Dependency Check Sequence... 💻

:: 1. Check for QEMU and download if missing via winget
where qemu-system-x86_64 >nul 2>nul
if %errorlevel% neq 0 (
    echo [WARNING] QEMU not found in system PATH!
    echo [INFO] Trying to install QEMU via winget...
    winget install Schlosski.QEMU --silent
    if %errorlevel% neq 0 (
        echo [ERROR] Automatic QEMU installation failed. Please install QEMU manually and add it to your PATH. ❌
        pause & exit /b 1
    )
    echo [SUCCESS] QEMU installed! Please restart this script to apply environment changes. 🎉
    pause & exit /b 0
)

:: 2. Check and Download OVMF.fd (UEFI BIOS) locally if not exists
if not exist .\ovmf\OVMF.fd (
    echo [INFO] OVMF.fd is missing. Downloading UEFI support files...
    mkdir ovmf >nul 2>nul
    powershell -Command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri 'https://github.com/osdev-packages/ovmf-binaries/raw/main/OVMF.fd' -OutFile '.\ovmf\OVMF.fd'"
    if not exist .\ovmf\OVMF.fd (
        echo [WARNING] Failed to download OVMF.fd automatically. 
        echo [INFO] Place your OVMF.fd inside .\ovmf\ folder manually.
    ) else (
        echo [SUCCESS] UEFI Bootloader binaries saved to .\ovmf\ 📂
    )
)

:: 3. Check for xorriso (Crucial for ISO creation)
if not exist .\xorriso.exe (
    echo [INFO] xorriso.exe missing. Pulling standalone windows binary...
    powershell -Command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/pbatard/rufus-web/gh-pages/files/xorriso/xorriso.exe' -OutFile '.\xorriso.exe'"
)

:: 4. Clean and Build (Requires Make & GCC in PATH, e.g., MinGW or running inside Git Bash)
echo [BUILD] Cleaning and compiling BarqOS kernel...
where make >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERROR] 'make' command not found! Please run this script from Git Bash, MSYS2, or install MinGW. ❌
    pause & exit /b 1
)

make clean
make
if %errorlevel% neq 0 (
    echo [ERROR] Kernel Build Failed! Check your GCC/compiler output. ❌
    pause & exit /b %errorlevel%
)

:: 5. Create ISO Root Directory Tree
echo [DIR] Creating ISO deployment structure...
if not exist iso_root\boot\limine mkdir iso_root\boot\limine
if not exist iso_root\EFI\BOOT mkdir iso_root\EFI\BOOT

:: 6. Copy Kernel and Limine Artifacts
echo [COPY] Deploying BarqOS Kernel & Limine binaries...
copy /Y bin\BarqOS iso_root\boot\BarqOS

copy /Y limine\limine.conf iso_root\boot\limine\
copy /Y limine\limine-bios.sys iso_root\boot\limine\
copy /Y limine\limine-bios-cd.bin iso_root\boot\limine\
copy /Y limine\limine-uefi-cd.bin iso_root\boot\limine\

copy /Y limine\BOOTX64.EFI iso_root\EFI\BOOT\
copy /Y limine\BOOTIA32.EFI iso_root\EFI\BOOT\

:: 7. Build the ISO Image using the downloaded or local xorriso
echo [ISO] Generating bootable UEFI image.iso...
set XORRISO_CMD=xorriso
if exist .\xorriso.exe set XORRISO_CMD=.\xorriso.exe

%XORRISO_CMD% -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin ^
        -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus ^
        -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin ^
        -efi-boot-part --efi-boot-image --protective-msdos-label ^
        iso_root -o image.iso

if %errorlevel% neq 0 (
    echo [ERROR] ISO Creation Failed! ❌
    pause & exit /b %errorlevel%
)

:: 8. Install Limine for Legacy BIOS Boot
echo [LIMINE] Deploying MBR/BIOS boot stages...
if exist .\limine\limine.exe (
    .\limine\limine.exe bios-install image.iso
) else (
    echo [WARNING] limine.exe not found in .\limine\. Skipping BIOS stage 1/2 install.
)

:: 9. Launching Emulator via QEMU
echo [QEMU] Booting BarqOS in UEFI Mode... 🚀
set BIOS_PATH=.\ovmf\OVMF.fd
if not exist %BIOS_PATH% set BIOS_PATH="C:\Program Files\qemu\OVMF.fd"

qemu-system-x86_64 ^
 -no-reboot ^
 -d int,cpu_reset ^
 -D qemu.log ^
 -m 512 ^
 -bios %BIOS_PATH% ^
 -drive format=raw,file=image.iso

echo [SUCCESS] QEMU Session Closed. BarqOS execution finished. 😎
pause
