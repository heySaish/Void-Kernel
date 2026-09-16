### AnyKernel3 Ramdisk Mod Script
## osm0sis @ xda-developers

### AnyKernel setup
# global properties
properties() { '
kernel.string=Void Kernel by heySaish
do.devicecheck=1
do.modules=0
do.systemless=1
do.cleanup=1
do.cleanuponabort=0
device.name1=curtana
device.name2=joyeuse
device.name3=excalibur
device.name4=gram
device.name5=miatoll
supported.versions=
supported.patchlevels=
supported.vendorpatchlevels=
'; } # end properties

# boot shell variables
BLOCK=/dev/block/bootdevice/by-name/boot;
IS_SLOT_DEVICE=0;
RAMDISK_COMPRESSION=auto;
PATCH_VBMETA_FLAG=auto;

# import functions/variables and setup patching - see for reference (DO NOT REMOVE)
. tools/ak3-core.sh;

# Banner
ui_print " ";
ui_print " __     __    _     _   _  __                    _   ";
ui_print " \\ \\   / /__ (_) __| | | |/ /___ _ __ _ __   ___| |  ";
ui_print "  \\ \\ / / _ \\| |/ _\` | | ' // _ \\ '__| '_ \\ / _ \\ |  ";
ui_print "   \\ V / (_) | | (_| | | . \\  __/ |  | | | |  __/ |  ";
ui_print "    \\_/ \\___/|_|\\__,_| |_|\\_\\___|_|  |_| |_|\\___|_|  ";
ui_print " ";
ui_print "       --- Void Kernel by heySaish ---";
ui_print " ";

# boot install
dump_boot;

write_boot;
## end boot install
