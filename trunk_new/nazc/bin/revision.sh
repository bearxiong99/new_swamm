#!/bin/bash
echo -e \
"#ifndef __REVISION_H__\n"\
"#define __REVISION_H__\n"\
"#define SW_REVISION_FIX \"`svnversion -c ../.. |  sed -e 's/\([0-9]*:\)\([0-9]\+[MS]*\).*/\2/'`\"\n"\
"#endif"
