# lv_egl_drm

# Setting up Buildroot image

## Building the image

```bash
cd boards/buildroot
make defconfig O=../output BR2_DEFCONFIG=../resources/configs/raspberrypi3b-plus_drm_egl_defconfig
cd ../output
make
```
The image is then available inside `images/sdcard.img`.

## Flashing the image

WARNING: Choosing the wrong disk can compromise your PC and require a new installation. 

```bash
# Inside boards/output
sudo dd if=images/sdcard.img of=/dev/<sdX> bs=4M status=progress
```

To find out the disk you want to select, you can find it with:

```bash
lsblk
```

Now you can boot your board with the new SD Card.

The image contains the `dropbear` package that allows you to connect to the board with `ssh`. 

Username: `root` 
Password: `root`

## Building the SDK

Inside the `boards/output` folder, you can simply build the sdk

```bash
# Inside boards/output
make sdk
```

### Exporting the SDK

We'll export the SDK outside this project's repository
```bash
# Inside boards/output
tar -xzvf images/aarch64-buildroot-linux-gnu_sdk-buildroot.tar.gz -C ../../..
```

You can then create a shell script to source the necessary environment variables:

```bash
#!/bin/bash
SDK_PATH="<path/to/sdk>" # < replace with the path to your SDK

export SYSROOT="${SDK_PATH}/aarch64-buildroot-linux-gnu/sysroot"
export CROSS_COMPILE="aarch64-buildroot-linux-gnu-"
export CC="${CROSS_COMPILE}gcc"
export CXX="${CROSS_COMPILE}g++"
export LD="${CROSS_COMPILE}ld"
export AR="${CROSS_COMPILE}ar"
export AS="${CROSS_COMPILE}as"
export CFLAGS="--sysroot=${SYSROOT}"
export LDFLAGS="--sysroot=${SYSROOT}"
```

Finally, source it using `source path/to/bash/script.sh` before building the application.

# Build the application

```bash
# Inside lv_egl_drm with the SDK env variables sourced
cmake -B build
cmake --build -j$(nproc)
```

# Copy the application

```bash
scp -O ./build/lv_egl_drm root@<ip_address>:~
```
