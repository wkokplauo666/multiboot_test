So this is an operating system test that i will be developing for weeks on end. Currently this project can only work with BIOS (it can run in EFI but there wont be any console since i cant use text mode in efi.)
# Overview
I started this project atleast 2 days ago with respect to the initial commit and i am looking to hopefully finish this operating system and make it fulfill its purpose, drawing images on the screen.
Actually, no, i will not make the BIOS version of this operating system draw "silly" images on the screen, i will be making a kernel that acts like a shell.
for now i will try developing it in BIOS first and then try to port it into EFI for global support, althought im not sure this will be accepted globally because of its crappy quality. Nevertheless this is such an interesting project that i'll be working on.
# Usage
To copy, simply run:
```bash
git clone https://github.com/wkokplauo666/multiboot_test
cd multiboot_test
```
After changing directory to the project, simply build the project by using `make` or run the project (assuming `qemu-system-x86_64` is installed) with `make runbios`. Or, if i had fixed the EFI issue, simply run `make run` (assuming `ovmf` is installed).

To change the project or the behaviour of the project you can change the files in `src/`, it contains the source files for this project. 
And to be honest, my project is kind of messy, the header files are cluttering the directories.

And moreover, you absolutely need a linux computer to build the project, but you can run the project in Windows or Linux by downloading the `out.iso` file and using it as a bootable CD-ROM. 
