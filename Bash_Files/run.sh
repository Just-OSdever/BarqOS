cd ~/Documents/BarqOS

qemu-system-x86_64 \
 -no-reboot \
 -d int,cpu_reset \
 -D qemu.log \
 -m 1024\
 -bios /usr/share/OVMF/OVMF_CODE.fd \
 -drive format=raw,file=image.iso