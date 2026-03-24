import os
import json
from pathlib import Path
from SCons.Script import Import

Import("env")

def create_fullflash(source, target, env):
    build_dir = Path(env.subst("$BUILD_DIR"))
    env_name = env["PIOENV"]
    project_dir = Path(env.subst("$PROJECT_DIR"))

    release_dir = project_dir / "release"
    release_dir.mkdir(exist_ok=True)

    flash_args_file = build_dir / "flash_args.json"

    # ============================================================
    # CAS 1 : ESP32 / S2 / C3 / S3 → flash_args.json présent
    # ============================================================
    if flash_args_file.exists():
        print(f"[fullflash] {env_name}: ESP32 mode")

        with open(flash_args_file, "r") as f:
            flash_data = json.load(f)

        # Vérifier que TOUS les fichiers existent
        missing = []
        for _, filename in flash_data["flash_files"]:
            if not (build_dir / filename).exists():
                missing.append(filename)

        if missing:
            print(f"[fullflash] Missing files → no fullflash generated: {missing}")
            return

        # Taille flash par défaut : 4MB
        flash_size = 4 * 1024 * 1024
        fullflash = bytearray([0xFF] * flash_size)

        for addr_str, filename in flash_data["flash_files"]:
            addr = int(addr_str, 16)
            data = (build_dir / filename).read_bytes()
            fullflash[addr:addr+len(data)] = data
            print(f"[fullflash] Added {filename} at {addr_str}")

        out_file = release_dir / f"{env_name}-fullflash.bin"
        out_file.write_bytes(fullflash)
        print(f"[fullflash] Generated ESP32 fullflash: {out_file}")
        return

    # ============================================================
    # CAS 2 : ESP8266
    # ============================================================
    print(f"[fullflash] {env_name}: ESP8266 mode")

    fw = build_dir / "firmware.bin"
    spiffs = build_dir / "spiffs.bin"

    # Vérification stricte
    if not fw.exists() or not spiffs.exists():
        print("[fullflash] Missing firmware or spiffs → no fullflash generated")
        return

    flash_size = 4 * 1024 * 1024
    fullflash = bytearray([0xFF] * flash_size)

    # Firmware
    data = fw.read_bytes()
    fullflash[0x00000:0x00000+len(data)] = data
    print("[fullflash] Added firmware at 0x00000")

    # SPIFFS
    data = spiffs.read_bytes()
    fullflash[0x10000:0x10000+len(data)] = data
    print("[fullflash] Added SPIFFS at 0x10000")

    # Zones système (optionnelles)
    blank = bytes([0xFF] * 0x2000)
    fullflash[0x3FC000:0x3FE000] = blank
    fullflash[0x3FE000:0x400000] = blank

    out_file = release_dir / f"{env_name}-fullflash.bin"
    out_file.write_bytes(fullflash)
    print(f"[fullflash] Generated ESP8266 fullflash: {out_file}")


# Hook sur buildprog (firmware) et buildfs (filesystem)
env.AddPostAction("buildprog", create_fullflash)
env.AddPostAction("buildfs", create_fullflash)

