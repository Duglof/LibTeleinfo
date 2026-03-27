import os
import json
from pathlib import Path
from SCons.Script import Import

Import("env")

DEFAULT_LITTLEFS_4MB = """
# Name,     Type, SubType, Offset,   Size,     Flags
nvs,        data, nvs,     0x9000,   0x5000,
otadata,    data, ota,     0xe000,   0x2000,
app0,       app,  ota_0,   0x10000,  0x140000,
app1,       app,  ota_1,   0x150000, 0x140000,
littlefs,   data, littlefs,0x290000, 0x170000,
"""

def get_partition_address(csv_file, name):
    with open(csv_file, "r") as f:
        for line in f:
            if line.strip().startswith("#"):
                continue
            parts = [p.strip() for p in line.split(",")]
            if len(parts) >= 4 and parts[0].lower() == name.lower():
                return int(parts[3], 16)
    return None

def get_partition_address_from_string(csv_text, name):
    for line in csv_text.splitlines():
        if line.strip().startswith(name + ","):
            parts = [p.strip() for p in line.split(",")]
            return int(parts[3], 16)
    return None

def create_fullflash(source, target, env):
    build_dir = Path(env.subst("$BUILD_DIR"))
    env_name = env["PIOENV"]
    project_dir = Path(env.subst("$PROJECT_DIR"))
    mcu = env.BoardConfig().get("build.mcu")

    # 🔥 TRACE MCU
    print(f"[fullflash] MCU detected: {mcu}")

    release_dir = project_dir / "release"
    release_dir.mkdir(exist_ok=True)

    flash_size = 4 * 1024 * 1024
    fullflash = bytearray([0xFF] * flash_size)

    # ============================================================
    # ESP32 / C2 / C3 / S3 (sans flash_args.json)
    # ============================================================
    if "esp32" in mcu:
        print(f"[fullflash] {env_name}: ESP32-family mode (manual)")

        bootloader = build_dir / "bootloader.bin"
        partitions = build_dir / "partitions.bin"
        firmware = build_dir / "firmware.bin"
        littlefs = build_dir / "littlefs.bin"

        if not (bootloader.exists() and partitions.exists() and firmware.exists() and littlefs.exists()):
            print("[fullflash] Missing files → no fullflash generated")
            return

        # Trouver le fichier CSV interne
        part_name = env.GetProjectOption("board_build.partitions")
        framework_dir = Path(env.PioPlatform().get_package_dir("framework-arduinoespressif32"))
        partition_csv = framework_dir / "tools" / "partitions" / part_name

        print("[DEBUG] Using partition CSV:", partition_csv)

        littlefs_addr = get_partition_address(partition_csv, "littlefs")

        if littlefs_addr is None:
            print("[fullflash] WARNING: No littlefs partition found in CSV → using built‑in 4MB layout")
            littlefs_addr = get_partition_address_from_string(DEFAULT_LITTLEFS_4MB, "littlefs")

        # Adresse bootloader selon MCU
        if mcu in ("esp32c2", "esp32c3", "esp32s3"):
            boot_addr = 0x0000
        else:
            boot_addr = 0x1000

        sections = [
            (boot_addr, bootloader),
            (0x8000, partitions),
            (0x10000, firmware),
            (littlefs_addr, littlefs)
        ]

        for addr, file in sections:
            data = file.read_bytes()
            fullflash[addr:addr+len(data)] = data
            print(f"[fullflash] Added {file.name} at 0x{addr:06X}")

        out_file = release_dir / f"{env_name}-4MB-fullflash.bin"
        out_file.write_bytes(fullflash)
        print(f"[fullflash] Generated ESP32-family fullflash: {out_file}")
        return

    # ============================================================
    # ESP8266
    # ============================================================
    print(f"[fullflash] {env_name}: ESP8266 mode")

    fw = build_dir / "firmware.bin"
    littlefs = build_dir / "littlefs.bin"

    if not fw.exists() or not littlefs.exists():
        print("[fullflash] Missing firmware or littlefs → no fullflash generated")
        return

    # Firmware à 0x00000
    data = fw.read_bytes()
    fullflash[0x00000:0x00000+len(data)] = data
    print("[fullflash] Added firmware at 0x00000")

    # Taille flash 4M et FS 1M → littlefs à 0x300000
    flash_size = 4 * 1024 * 1024
    fs_size = 1 * 1024 * 1024   # si tu es en 1M littlefs dans platformio.ini
    littlefs_addr = flash_size - fs_size    # littlefs à 0x300000 (4M flash, 1M FS)

    data = littlefs.read_bytes()
    fullflash[littlefs_addr:littlefs_addr+len(data)] = data
    print(f"[fullflash] Added littlefs at 0x{littlefs_addr:06X}")

    # Zones système
    blank = bytes([0xFF] * 0x2000)
    fullflash[0x3FC000:0x3FE000] = blank
    fullflash[0x3FE000:0x400000] = blank

    out_file = release_dir / f"{env_name}-4MB-fullflash.bin"
    out_file.write_bytes(fullflash)
    print(f"[fullflash] Generated ESP8266 fullflash: {out_file}")

# Hook sur buildprog et buildfs
env.AddPostAction("buildprog", create_fullflash)
env.AddPostAction("buildfs", create_fullflash)

