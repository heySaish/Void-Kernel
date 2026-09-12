# 🐉 Void Kernel for Xiaomi Miatoll (SM6250) 🪷

[![Build Void Kernel](https://github.com/heySaish/Void-Kernel/actions/workflows/build.yml/badge.svg)](https://github.com/heySaish/Void-Kernel/actions/workflows/build.yml)
![Kernel Version](https://img.shields.io/badge/Kernel-4.14.357-blue.svg)
![Toolchain](https://img.shields.io/badge/Toolchain-Neutron--Clang-purple.svg)
![Root](https://img.shields.io/badge/Root-Official--KernelSU-success.svg)
![Architecture](https://img.shields.io/badge/Arch-ARM64--v8a-orange.svg)

A high-performance, clean, and battery-optimized custom Linux Kernel designed for the **Xiaomi Miatoll** unified family.

---

## 📱 Supported Devices (Miatoll Unified)

- **Redmi Note 9 Pro** (`curtana`)
- **Redmi Note 9 Pro Max** (`excalibur`)
- **Redmi Note 9S** (`gram`)
- **Poco M2 Pro** (`joyeuse`)

---

## 🔥 Features & Highlights

- ⚡ **Official KernelSU Integrated:** Built-in official kernel root engine with manual hooks.
- 🚀 **Neutron Clang Compiler:** Compiled using the latest Neutron LLVM toolchain for maximum performance & code optimizations.
- 🔋 **Battery & Thermal Balance:** Optimized power consumption with smooth daily responsiveness.
- 🎮 **Gaming Stability:** Reduced frame drops, lower latency, and enhanced memory throughput.
- 📦 **AnyKernel3 Installer:** Universal zip installer compatible with TWRP, OrangeFox, and Kernel Flasher apps.

---

## 📥 Installation

1. Download the latest `Void-Kernel-Clean-Build.zip` from **Releases** or **GitHub Actions Artifacts**.
2. Reboot your device into Custom Recovery (TWRP / OrangeFox) or open **Kernel Flasher**.
3. Flash `Void-Kernel-Clean-Build.zip`.
4. Reboot system.
5. Install the official **KernelSU-Next Manager APK** (`v3.3.0` / `v1.0.5+`) to manage root privileges.

---

## 🛠️ Building From Source

### Via GitHub Actions (Recommended)
Simply go to the **Actions** tab on GitHub and run the **Build Void Kernel** workflow via `workflow_dispatch`.

### Local Compilation
```bash
# 1. Load defconfig
make O=out ARCH=arm64 vendor/xiaomi/miatoll_defconfig

# 2. Compile Kernel with LLVM/Clang
make -j$(nproc --all) O=out \
    ARCH=arm64 \
    CC=clang \
    CLANG_TRIPLE=aarch64-linux-gnu- \
    CROSS_COMPILE=aarch64-linux-gnu- \
    CROSS_COMPILE_ARM32=arm-linux-gnueabi- \
    LLVM=1 \
    LLVM_IAS=1 \
    KBUILD_BUILD_USER=Saish \
    KBUILD_BUILD_HOST=VoidOS
```

---

## 🤝 Credits & Acknowledgements

- **Developer / Maintainer:** [heySaish](https://github.com/heySaish)
- **Linux Kernel Organization**
- **Qualcomm & Xiaomi** for kernel source base
- **KernelSU-Next Team** for next-gen kernel root support
- **Neutron Toolchains** for antman & clang
- **osm0sis** for AnyKernel3

---

<p align="center">Made with ❤️ for Xiaomi Miatoll Community by <b>Saish</b></p>
