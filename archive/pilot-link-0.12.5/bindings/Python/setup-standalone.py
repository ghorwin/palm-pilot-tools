#!/usr/bin/python
#
# Copyright (c) 2005, Florent Pillet.
# 
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at
# your option) any later version.
# 
# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library
# General Public License for more details.
# 
# You should have received a copy of the GNU Library General Public License
# along with this library; if not, write to the Free Software Foundation,
# Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
# 
# $Id: setup-standalone.py.in,v 1.4 2006/05/31 17:59:55 desrod Exp $

from distutils.core import setup, Extension
import sys

if sys.platform.startswith('darwin'):
    # additional link options for Mac OS X
    link_options = ['-framework','Carbon','-framework','System','-framework','IOKit']
else:
    # TODO: test on other platforms and add options as appropriate
    link_options = None

setup(name = "python-libpisock",
      version = "0.12.5",
      description = "Python bindings for the pisock library linked in a standalone module (not requiring the libpisock shared lib).",
      author = "Florent Pillet",
      author_email = "pilot-link@florentpillet.com",
      url = "http://www.pilot-link.org/",

      ext_modules = [Extension("_pisock", ["src/pisock_wrap.c"],
                               include_dirs = ['../../include'],
                               extra_objects = ['../../libpisock/.libs/libpisock.a'],
                               extra_link_args = link_options
                               )
                     ],
      package_dir = {'': 'src'},
      py_modules = ["pisock","pisockextras"])
