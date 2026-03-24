Import("env")
from SCons.Script import DefaultEnvironment

print("[build_all_fs] script loaded")

def build_all_with_fs(target, source, env):
    print("=== Building firmware + filesystem for all environments ===")
    import subprocess

    # Liste des environnements définis dans platformio.ini
    envs = env.GetProjectConfig().sections()
    envs = [e for e in envs if e.startswith("env:")]

    for e in envs:
        name = e.replace("env:", "")
        print(f"\n--- Building {name} ---")
        subprocess.run(["pio", "run", "-e", name])
        subprocess.run(["pio", "run", "-e", name, "-t", "buildfs"])

    print("\n=== All builds completed ===")

env.AddCustomTarget(
    name="buildallfs",
    dependencies=None,
    actions=[build_all_with_fs],
    title="Build All + Filesystem",
    description="Build firmware + spiffs + fullflash for all environments"
)

