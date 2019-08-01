#!python3

"""
This is a short script which keeps track of how many local builds were made
# and the overall line count of the project
"""

import sys
import os
import os.path
import shutil
import datetime

from standard_logging import *

SCRIPT_PATH = os.path.dirname(os.path.realpath(__file__))
ROOT_DIR = SCRIPT_PATH + "/../"
BUILD_DIR = ROOT_DIR + "/build/"
OUTPUT_DIR = BUILD_DIR + "/piranha/"
BINARY_PATH = ROOT_DIR + "/project/"
INCLUDE_DIR = ROOT_DIR + "/include/"
AUTOGEN_DIR = ROOT_DIR + "/autogen/"
FLEX_DIR = ROOT_DIR + "/dependencies/libraries/flex/include/"

BINARY_NAME = 'piranha.lib'
PDB_NAME = 'piranha.pdb'


def generate_build_lib_path(architecture, mode):
    return OUTPUT_DIR + "/lib/{0}/{1}/".format(architecture, mode)


def generate_build_header_path():
    return OUTPUT_DIR + "/include/include/"


def generate_main_path():
    return OUTPUT_DIR + "/include/"


def generate_build_autogen_path():
    return OUTPUT_DIR + "/include/autogen/"


def generate_library_header_path():
    return OUTPUT_DIR + "/include/lib/"


def genrate_binary_source_path(architecture, mode):
    return BINARY_PATH + "/{0}/{1}/".format(architecture, mode)


def clean_build():
    log("INFO", "Deleting last build...")

    try:
        shutil.rmtree(OUTPUT_DIR)
    except FileNotFoundError:
        log("INFO", "No previous build found, skipping clean")
    except OSError:
        log("ERROR", "Could not clean build, files are likely in use")
        sys.exit(1) # Return with an error


def make_directory(name):
    try:
        os.makedirs(name)
    except FileExistsError:
        log("WARNING", "Build folder was not cleaned")
        pass


def generate_dir():
    log("INFO", "Generating new build path")
    make_directory(OUTPUT_DIR)


def copy_binary(architecture, mode):
    binary_path = genrate_binary_source_path(architecture, mode)
    output_path = generate_build_lib_path(architecture, mode)
    
    log("INFO", "Generating new output path: {}".format(output_path))
    make_directory(output_path)

    log("INFO", "Copying .lib binary")
    shutil.copy(binary_path + BINARY_NAME, output_path)

    log("INFO", "Copying PDB (debug) information")
    try:
        shutil.copy(binary_path + PDB_NAME, output_path)
    except FileNotFoundError:
        log("INFO", "No PDB information found for this build")
        pass


def copy_include_files(architecture, mode):
    log("INFO", "Copying include headers")

    output_path = generate_build_header_path()
    make_directory(output_path)

    all_headers = []
    for root, sub_dirs, files in os.walk(INCLUDE_DIR):
        for file_entry in files:
            shutil.copy(os.path.join(root, file_entry.strip()), output_path)
            all_headers.append(file_entry)

    output_path = generate_build_autogen_path()
    make_directory(output_path)
    for root, sub_dirs, files in os.walk(AUTOGEN_DIR):
        for file_entry in files:
            if file_entry.endswith('.h'):
                shutil.copy(os.path.join(root, file_entry.strip()), output_path)

    output_path = generate_library_header_path()
    make_directory(output_path)
    for root, sub_dirs, files in os.walk(FLEX_DIR):
        for file_entry in files:
            if file_entry.endswith('.h'):
                shutil.copy(os.path.join(root, file_entry.strip()), output_path) 
    
    output_path = generate_main_path()
    log("INFO", "Generating main include header: piranha.h")
    with open(output_path + 'piranha.h', 'w') as f:
        f.write('// Auto-generated on: {}\n'.format(datetime.datetime.now()))
        f.write('\n')

        f.write("#ifndef PIRANHA_PIRANHA_H\n")
        f.write('#define PIRANHA_PIRANHA_H\n')

        f.write('\n')
        f.write("#define PIRANHA_DIST\n")
        f.write('\n')

        for filename in all_headers:
            f.write('#include \"include/{}\"\n'.format(filename))

        f.write('\n')

        f.write('#endif /* PIRANHA_PIRANHA_H */\n')


def write_info_file(architecture, mode):
    log("INFO", "Writing info file")

    with open(BUILD_DIR + 'build_info.txt', 'w') as f:
        f.write("{} = {}\n".format("timestamp", str(datetime.datetime.now())))
        f.write("{} = {}\n".format("architecture", architecture))
        f.write("{} = {}\n".format("configuration", mode))


if __name__ == "__main__":
    architecture = sys.argv[1]
    mode = sys.argv[2]

    print_full_header("Packaging Piranha Build")
    clean_build()
    generate_dir()
    copy_binary(architecture, mode)
    copy_include_files(architecture, mode)
    write_info_file(architecture, mode)
    print_footer()
