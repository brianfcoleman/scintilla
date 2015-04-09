#!/usr/bin/env python

from __future__ import print_function
from argparse import ArgumentParser
from subprocess import Popen, PIPE
import json
import multiprocessing
import os
import shutil
import subprocess
import sys

OPEN = 'open'
CLOSE = 'close'
DISTCLEAN = 'distclean'
CONFIGURE = 'configure'
CLEAN = 'clean'
BUILD = 'build'
REBUILD = 'rebuild'
COMMANDS = [
    OPEN,
    CLOSE,
    DISTCLEAN,
    CONFIGURE,
    CLEAN,
    BUILD,
    REBUILD
]

DEBUG = 'debug'
RELEASE = 'release'
CONFIGURATIONS = [
    DEBUG,
    RELEASE
]

x86 = 'x86'
x64 = 'x64'
ARCHITECTURES = [
    x86,
    x64
]

QT_INSTALL_DIR = 'qt_install_dir'
GENERATOR = 'generator'
CONFIG_OPTIONS = [
    QT_INSTALL_DIR,
    GENERATOR
]

BUILD_OUTPUT_LOG = 'build-output.log'
BUILD_ERROR_LOG = 'build-error.log'
OUTPUT = 'output'
ERROR = 'error'

WINDOWS = 'win32'
VISUAL_STUDIO = 'Visual Studio'

def log_message(log_file, message, newline=True):
    if newline:
        message = '{}\n'.format(message)
    else:
        message = '{}'.format(message)
    log_file.write(message)

def log_output(log_files, message, newline=True):
    log_message(log_files[OUTPUT], message, newline=newline)

def log_error(log_files, message, newline=True):
    log_message(log_files[ERROR], message, newline=newline)

def log_header(log_file, header):
    separator = '*' * (len(header) + 4)
    message = '\n{}\n* {} *\n{}\n\n'.format(separator, header, separator)
    log_message(log_file, message, newline=False)

def repo_dir_path():
    return os.path.abspath(os.path.dirname(sys.argv[0]))

def config_file_path():
    return os.path.join(repo_dir_path(), '.build.config.json')

def output_dir_path(architecture=None):
    path = os.path.join(repo_dir_path(), 'out')
    if architecture is not None:
        return os.path.join(path, architecture)
    return path

def solution_file_path(architecture):
    return os.path.join(output_dir_path(architecture), 'Cursor.sln')

def generator(name, architecture):
    if sys.platform == WINDOWS:
        if name.startswith(VISUAL_STUDIO):
            platforms = {
                x86: '',
                x64: 'Win64'
            }
            return '{} {}'.format(name, platforms[architecture])
    return name

def read_config_file(config_file_path):
    if not os.path.exists(config_file_path):
        raise Exception('Missing config file {}'.format(config_file_path))
    with open(config_file_path, 'r') as config_file:
        return json.load(config_file)

def validate_config_options(config_options):
    for config_option in CONFIG_OPTIONS:
        if not config_option in config_options:
            raise Exception('{} config option must be specified in {}'.format(
                config_option, config_file_path()))
    if sys.platform == WINDOWS:
        if not config_options[GENERATOR].startswith(VISUAL_STUDIO):
            raise Exception(
                'Generators other than Visual Studio not tested on Windows')

def parse_arguments():
    parser = ArgumentParser()
    parser.add_argument('command_names', nargs='+', choices=COMMANDS,
        metavar='COMMAND')
    parser.add_argument('-c', '--configuration', nargs='+',
        choices=CONFIGURATIONS, dest='configurations', default=[DEBUG])
    parser.add_argument('-a', '--architecture', type=int,
        choices=ARCHITECTURES, dest='architectures', default=[x64])
    return parser.parse_args()

def format_command(command):
    return ' '.join(command)

def call(command, log_files, cwd=os.getcwd()):
    message = '{} (cwd: {})'.format(format_command(command), cwd)
    print(message)
    log_header(log_files[OUTPUT], message)
    log_header(log_files[ERROR], message)
    process = Popen(command, cwd=cwd, stdout=PIPE, stderr=PIPE)
    output, error = process.communicate()
    log_output(log_files, output, newline=False)
    log_error(log_files, error, newline=False)
    return_code = process.returncode
    if return_code != 0:
        raise Exception('Error {} calling {}'.format(
            return_code, format_command(command)))

def cmake(architecture, config_options, log_files):
    CMAKE = 'cmake'
    command = [
        CMAKE,
            '-DQT_INSTALL_DIR={}'.format(config_options[QT_INSTALL_DIR]),
            '-G', generator(config_options[GENERATOR], architecture),
            repo_dir_path()
    ]
    call(command, log_files, cwd=output_dir_path(architecture))

def msbuild(target, architecture, configuration, log_files):
    MSBUILD = 'msbuild'
    command = [
        MSBUILD,
            solution_file_path(architecture),
            '/target:{}'.format(target),
            '/property:Platform={}'.format(architecture),
            '/property:Configuration={}'.format(configuration),
            '/maxcpucount:{}'.format(multiprocessing.cpu_count()),
            '/verbosity:detailed',
            '/consoleloggerparameters:{}'.format(';'.join([
                'Summary', 'ShowCommandLine', 'ShowTimestamp', 'ForceNoAlign',
                'DisableConsoleColor'])),
            '/nologo'
    ]
    call(command, log_files)

def cscript(script_path, arguments, log_files):
    CSCRIPT = 'cscript'
    command = [
        CSCRIPT,
            script_path,
            '//NoLogo'
    ]
    call(command + arguments, log_files)

def open_solution(architecture, log_files):
    OPEN_SOLUTION = os.path.join(repo_dir_path(), 'build',
        'open-vs-solution.js')
    solution_path = solution_file_path(architecture)
    if os.path.exists(solution_path):
        cscript(OPEN_SOLUTION, [solution_path], log_files)

def open_solutions(options, config_options, log_files):
    for architecture in options.architectures:
        open_solution(architecture, log_files)

def close_solution(architecture, log_files):
    CLOSE_SOLUTION = os.path.join(repo_dir_path(), 'build',
        'close-vs-solution.js')
    solution_path = solution_file_path(architecture)
    if os.path.exists(solution_path):
        cscript(CLOSE_SOLUTION, [solution_path], log_files)

def close_solutions(options, config_options, log_files):
    for architecture in ARCHITECTURES:
        close_solution(architecture, log_files)

def rmdir(path):
    def on_error(func, path, exc_info):
        raise Exception('Operation {} on path {} failed'.format(
            func.__name__, path))
    if os.path.exists(path):
        shutil.rmtree(path, onerror=on_error)

def distclean(options, config_options, log_files):
    close_solutions(options, config_options, log_files)
    rmdir(output_dir_path())

def mkdir(path):
    if not os.path.exists(path):
        os.makedirs(path)

def configure(options, config_options, log_files):
    close_solutions(options, config_options, log_files)
    for architecture in options.architectures:
        mkdir(output_dir_path(architecture))
        cmake(architecture, config_options, log_files)

def build_target(build_tool, target, architectures, configurations, log_files):
    for architecture in architectures:
        for configuration in configurations:
            build_tool(target, architecture, configuration, log_files)

def target(name, options, config_options, log_files):
    architectures = options.architectures
    configurations = options.configurations
    os_targets = {
        WINDOWS: lambda: \
            build_target(msbuild, name, architectures, configurations,
                log_files)
    }
    os_target = os_targets[sys.platform]
    os_target()

def clean(options, config_options, log_files):
    target(CLEAN, options, config_options, log_files)

def build(options, config_options, log_files):
    target(BUILD, options, config_options, log_files)

def rebuild(options, config_options, log_files):
    target(REBUILD, options, config_options, log_files)

def main():
    options = parse_arguments()
    with \
        open(BUILD_OUTPUT_LOG, 'w') as build_output_log, \
        open(BUILD_ERROR_LOG, 'w') as build_error_log:
        try:
            log_files = {
                OUTPUT: build_output_log,
                ERROR: build_error_log
            }
            if sys.platform != WINDOWS:
                raise Exception('Not tested on platforms other than Windows')
            config_options = read_config_file(config_file_path())
            validate_config_options(config_options)
            commands = {
                OPEN: open_solutions,
                CLOSE: close_solutions,
                DISTCLEAN: distclean,
                CONFIGURE: configure,
                CLEAN: clean,
                BUILD: build,
                REBUILD: rebuild
            }
            for command_name in options.command_names:
                command = commands[command_name]
                command(options, config_options, log_files)
        except Exception as e:
            log_error(log_files, e)
            sys.exit(1)

if __name__ == '__main__':
    main()
