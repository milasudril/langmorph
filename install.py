#!/usr/bin/python3

import sys
import json
import os
import shutil

def install_include_file(src_filename, dest_filename):
	print('Installing %s to %s'%(src_filename, dest_filename))
	with open(src_filename) as src_file:
		with open(dest_filename, 'w') as dest_file:
			for line in src_file:
				if not line.startswith('//@'):
					dest_file.write(line)
	return 0

def install_library(target_dir, specfile_name, destdir):
	with open(specfile_name) as f:
		libinfo = json.load(f)

	in_dir = os.path.dirname(specfile_name)
	incdir = destdir + '/include/langmorph'
	os.makedirs(incdir)

	for item in libinfo['dependencies']:
		src_file_name = item['ref']
		if src_file_name.startswith('./'):
			src_file_name = in_dir + '/' + src_file_name
		if src_file_name.endswith('.hpp'):
			install_include_file(src_file_name, incdir + '/' + os.path.relpath(src_file_name, './src'))

	libdir = destdir + '/lib'
	src_file_name = target_dir + '/' + in_dir + '/'  + libinfo['target']['name']
	dest_file_name = libdir + '/' + libinfo['target']['name']
	print('Installing %s to %s'%(src_file_name, dest_file_name))
	try:
		os.makedirs(libdir)
	except FileExistsError:
		pass
	shutil.copyfile(src_file_name, dest_file_name)

	pkgconfig_dir = libdir + '/pkgconfig'
	try:
		os.makedirs(pkgconfig_dir)
	except FileExistsError:
		pass
	with open(pkgconfig_dir + '/langmorph.pc', 'w') as f:
		f.write(
			'''Name: Langmorph
Description: A word generator library
Version: 0.1.0
Requires: wad64
Libs: -L%s -llangmorph
Cflags: -I%s
'''%(libdir, incdir)
		)

	return 0

def install_application(target_dir, specfile_name, destdir):
	with open(specfile_name) as f:
		libinfo = json.load(f)

	in_dir = os.path.dirname(specfile_name)
	appdir = destdir + '/bin'
	src_file_name = target_dir + '/' + in_dir + '/'  + libinfo['target']['name']
	dest_file_name = appdir + '/' + libinfo['target']['name']
	print('Installing %s to %s'%(src_file_name, dest_file_name))
	try:
		os.makedirs(appdir)
	except FileExistsError:
		pass
	shutil.copyfile(src_file_name, dest_file_name)
	return 0

def install_data(target_dir, specfile_name, destdir):
	share_dir = destdir + '/share/langmorph'
	shutil.copytree(specfile_name, share_dir)

def install(target_dir, specfile_name, install_to):
	if specfile_name.endswith('.lib.maikerule'):
		return install_library(target_dir, specfile_name, install_to)

	if specfile_name.endswith('.app.maikerule'):
		return install_application(target_dir, specfile_name, install_to)

	if specfile_name == 'data/':
		return install_data(target_dir, specfile_name, install_to)

if __name__ == '__main__':
	if len(sys.argv) < 2:
		print('Missing arguments')
		sys.exit(-1)

	args = json.loads(sys.argv[1])
	install_to = args['install_to']
	target_dir = args['target_dir']
	targets = args['targets']

	for target in targets:
		install(target_dir, target, install_to)

