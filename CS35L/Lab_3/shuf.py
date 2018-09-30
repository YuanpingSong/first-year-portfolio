#!/usr/bin/python

"""
Output lines selected randomly from a file

Copyright 2005, 2007 Paul Eggert.
Copyright 2010 Darrell Benjamin Carbajal.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Please see <http://www.gnu.org/licenses/> for a copy of the license.

$Id: randline.py,v 1.4 2010/04/05 20:04:43 eggert Exp $
"""

import random, sys
from optparse import OptionParser
import argparse

# randline class provides some of the functionality of numpy.random not seen in the standard library
class randline:
    def __init__(self, lines, numLines, repeat): # expect [string], int, bool
        self.lines = lines
        self.repeat = repeat
        self.numLines = numLines
        
    def chooseline(self): # builds the text output of program as an array
        arr = []
        for idx in range(self.numLines):
            arr.append(random.choice(self.lines))
            if not self.repeat:
                self.lines.remove(arr[-1])
        return arr

def main():
    version_msg = "%prog 2.0"
    usage_msg = """%prog [OPTION]... FILE

Output shuffled lines from FILE."""

    parser = OptionParser(version=version_msg,
                          usage=usage_msg)

    eHelpMsg = "Treat each command-line operand as an input line."
    nHelpMsg = "Output at most count lines. By default, all input lines are output."
    rHelpMsg = "Repeat output values, that is, select with replacement. With this option the output is not a permutation of the input; instead, each output line is randomly chosen from all the inputs. This option is typically combined with --head-count; if --head-count is not given, shuf repeats indefinitely."

    parser.add_option("-e", "--echo", action="store_true", dest="shouldEcho", default=False, help=eHelpMsg)
    parser.add_option("-n", "--head-count", action='store', type='int', dest='numlines', default=sys.maxsize, help=nHelpMsg)
    parser.add_option("-r", "--repeat", action="store_true", dest="shouldRepeat", default=False, help=rHelpMsg)
    # the echo flag causes the program to accept multiple arguements and interprept them as literals instead of file names
    # the -n flag sets the maximum limit on the number of outputs produced by the program. The default is no limit
    # the -r flag allows repeated selection
    
    options, args = parser.parse_args(sys.argv[1:])
    # options - shouldEcho : Bool = False , numlines : Int = MAX_INT, shouldRepeat : Bool = False
    # args - a list of names supplied by user. Interpreptation depends on flags. 

    # check that numlines is valid
    try:
        numlines = int(options.numlines)
    except:
        parser.error("invalid NUMLINES: {0}".
                     format(options.numlines))
    if numlines < 0:
        parser.error("negative count: {0}".
                     format(numlines))
    
    # construct an array of lines for randline obj
    lines = None
    # Case: no argument, read from stdin
    if len(args) == 0 or (args[0] == '-' and not options.shouldEcho)
        lines = sys.stdin.readlines()
  
    # Case: one argument as filename
    elif len(args) == 1 and not options.shouldEcho:
        with open(args[0]) as f:
            lines = f.readlines()
    # Case: one or more arguments as literals
    elif len(args) >= 1 and options.shouldEcho:
        lines = args

    # Undefined argument
    else:
        parser.error("wrong number of operands")

    try:
        # 
        if not options.shouldRepeat:
            numlines = min(numlines, len(lines))
        generator = randline(lines, numlines, options.shouldRepeat)
        for line in generator.chooseline():
            sys.stdout.write(line.rstrip() + '\n')
    except IOError as err:
        errno, strerror = err.args
        parser.error("I/O error({0}): {1}".
                     format(errno, strerror))
    except ValueError as err:
        errno, strerror = err.args
        parser.error("Value error({0}): {1}".
                     format(errno, strerror))

if __name__ == "__main__":
    main()