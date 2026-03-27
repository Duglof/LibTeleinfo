import os
import shutil
Import("env")

def after_spiffs(source, target, env):
    build_dir = env.subst("$BUILD_DIR")
    env_name = env["PIOENV"]

    release_dir = os.path.join(env.subst("$PROJECT_DIR"), "release")
    os.makedirs(release_dir, exist_ok=True)

    src_fs = os.path.join(build_dir, "littlefs.bin")
    dst_fs = os.path.join(release_dir, f"{env_name}-littlefs.bin")

    if os.path.exists(src_fs):
        shutil.copy(src_fs, dst_fs)
        print(f"[LITTLEFS] Generated: {dst_fs}")

env.AddPostAction("buildfs", after_spiffs)

