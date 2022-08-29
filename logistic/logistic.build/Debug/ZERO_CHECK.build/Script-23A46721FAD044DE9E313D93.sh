#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/logistic"
  make -f /Users/giy/Documents/Max\ 8/Packages/max-sdk/source/my/logistic/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/logistic"
  make -f /Users/giy/Documents/Max\ 8/Packages/max-sdk/source/my/logistic/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/logistic"
  make -f /Users/giy/Documents/Max\ 8/Packages/max-sdk/source/my/logistic/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/logistic"
  make -f /Users/giy/Documents/Max\ 8/Packages/max-sdk/source/my/logistic/CMakeScripts/ReRunCMake.make
fi

