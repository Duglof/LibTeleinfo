import os
import shutil
Import("env")

def after_firmware(source, target, env):

    print(f"[rename_firmware] after_firmware called for env: {env['PIOENV']}")

    build_dir = env.subst("$BUILD_DIR")
    env_name = env["PIOENV"]

    release_dir = os.path.join(env.subst("$PROJECT_DIR"), "release")
    os.makedirs(release_dir, exist_ok=True)

    src_fw = os.path.join(build_dir, "firmware.bin")
    dst_fw = os.path.join(release_dir, f"{env_name}-firmware.bin")

    if os.path.exists(src_fw):
        shutil.copy(src_fw, dst_fw)
        print(f"[Firmware] Generated: {dst_fw}")

env.AddPostAction("buildprog", after_firmware)
