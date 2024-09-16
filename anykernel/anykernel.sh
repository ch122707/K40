# AnyKernel3 Ramdisk Mod Script

properties() { '
kernel.string=Redmi K40 by HK122707
do.devicecheck=1
do.modules=0
do.systemless=1
do.cleanup=1
do.cleanuponabort=0
device.name1=alioth
device.name2=aliothin
device.name3=
device.name4=
device.name5=
supported.versions=
supported.patchlevels=
'; }

# shell variables
block=boot;
is_slot_device=1;
ramdisk_compression=auto;

# import patching
. tools/ak3-core.sh;

# set permissions/ownership
set_perm_recursive 0 0 750 750 $ramdisk/*;

# AnyKernel boot install
dump_boot;

# migrate from
if [ -d $ramdisk/overlay ]; then
  rm -rf $ramdisk/overlay;
fi;

# end boot install
write_boot;

# vendor_boot shell variables
block=/dev/block/bootdevice/by-name/vendor_boot;
is_slot_device=auto;
ramdisk_compression=auto;
patch_vbmeta_flag=auto;

# reset for vendor_boot patching
reset_ak;

# vendor_boot install
dump_boot;

# end vendor_boot install
write_boot
