print("[build_all_fs] script loaded")

import os
import shutil
import subprocess
Import("env")

# Nettoyage du dossier release
release_dir = os.path.join(os.getcwd(), "release")
if os.path.exists(release_dir):
    print("Cleaning release directory...")
    shutil.rmtree(release_dir)
os.makedirs(release_dir, exist_ok=True)

# 🔥 Supprimer tous les firmware.bin pour forcer la reconstruction
build_root = os.path.join(os.getcwd(), ".pio", "build")
if os.path.exists(build_root):
    print("Cleaning all firmware.bin files...")
    for root, dirs, files in os.walk(build_root):
        for f in files:
            if f == "firmware.bin":
                path = os.path.join(root, f)
                os.remove(path)
                print(f"Removed: {path}")

def build_all_with_fs(target, source, env):
    print("=== Building firmware + filesystem for all environments ===")

    envs = [e[4:] for e in env.GetProjectConfig().sections() if e.startswith("env:")]

    for name in envs:
        if name == "tools":
            print("Skipping tools environment")
            continue

        print(f"\n--- Building {name} ---")
        subprocess.run(["pio", "run", "-e", name], check=True)
        subprocess.run(["pio", "run", "-e", name, "-t", "buildfs"], check=True)

    print("\n=== All builds completed ===")

env.AddCustomTarget(
    name="buildallfs",
    dependencies=None,
    actions=[build_all_with_fs],
    title="Build All + Filesystem",
    description="Build firmware + spiffs + fullflash for all environments"
)

