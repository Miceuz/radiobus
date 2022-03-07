Import("env", "projenv")

# access to global build environment
print( env)

# access to project build environment (is used source files in "src" folder)
print( projenv)

print( "Current build targets", map(str, BUILD_TARGETS))

# Dump build environment (for debug purpose)
# print( env.Dump())

# Convert the built executable in .bin format into UF2 format for flashing via bootloader.
# e.g. python uf2conv.py --convert --base 0x00002000 --output firmware.uf2 firmware.bin
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.bin",
    env.VerboseAction(" ".join([
        "python", "scripts/uf2conv.py", 
            "--convert", 
            "--base", "0x00002000", 
            "--output", "$PROJECT_DIR/${PROGNAME}.uf2", 
            "$BUILD_DIR/${PROGNAME}.bin"
    ]), "Building $PROJECT_DIR/${PROGNAME}.uf2")
)

# For verification, extract the executable in .bin format from the UF2 file.
# e.g. python2 uf2conv.py --convert --output firmware.bin --base 0x00002000 firmware.uf2
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.bin",
    env.VerboseAction(" ".join([
        "python", "scripts/uf2conv.py", 
            "--convert", 
            "--base", "0x00002000", 
            "--output", "$PROJECT_DIR/${PROGNAME}.bin",
            "$PROJECT_DIR/${PROGNAME}.uf2"
    ]), "Building $PROJECT_DIR/${PROGNAME}.bin")
)

# Change build flags in runtime
#
#env.ProcessUnFlags("-DVECT_TAB_ADDR")
#env.Append(CPPDEFINES=("VECT_TAB_ADDR", 0x123456789))

#
# Upload actions
#

def before_upload(source, target, env):
    print( "before_upload")
    # do some actions

    # call Node.JS or other script
    env.Execute("node --version")


def after_upload(source, target, env):
    print( "after_upload")
    # do some actions

#env.AddPreAction("upload", before_upload)
#env.AddPostAction("upload", after_upload)

#
# Custom actions when building program/firmware
#

#env.AddPreAction("buildprog", callback...)
#env.AddPostAction("buildprog", callback...)

#
# Custom actions for specific files/objects
#

#env.AddPreAction("$BUILD_DIR/${PROGNAME}.elf", [callback1, callback2,...])
#env.AddPostAction("$BUILD_DIR/${PROGNAME}.hex", callback...)

# custom action before building SPIFFS image. For example, compress HTML, etc.
#env.AddPreAction("$BUILD_DIR/spiffs.bin", callback...)

# custom action for project's main.cpp
#env.AddPostAction("$BUILD_DIR/src/main.cpp.o", callback...)