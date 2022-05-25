set OSNAME=fzzOS
set BUILDDIR=\\wsl$\Debian\home\jon\fzzOS5\kernel\bin
set OVMFDIR=C:\Users\Jon\Desktop\OSDEV\OMVF

"C:\Program Files\qemu\qemu-system-x86_64" -machine q35 -d cpu_reset -drive file=%BUILDDIR%\%OSNAME%.img -drive file=%BUILDDIR%\blank.img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file=%OVMFDIR%\OVMF_CODE-pure-efi.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=%OVMFDIR%\OVMF_VARS-pure-efi.fd -net none -no-reboot -no-shutdown
pause
