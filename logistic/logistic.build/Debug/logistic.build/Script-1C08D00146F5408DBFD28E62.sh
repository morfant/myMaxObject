#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/logistic"
  cp /Users/giy/Documents/Max\ 8/Packages/max-sdk/source/max-sdk-base/script/PkgInfo /Users/giy/Documents/Max\ 8/Packages/max-sdk/source/my/logistic/../../../externals/logistic.mxo/Contents/PkgInfo
fi
if test "$CONFIGURATION" = "Release"; then :
  cd "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/logistic"
  cp /Users/giy/Documents/Max\ 8/Packages/max-sdk/source/max-sdk-base/script/PkgInfo /Users/giy/Documents/Max\ 8/Packages/max-sdk/source/my/logistic/../../../externals/logistic.mxo/Contents/PkgInfo
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/logistic"
  cp /Users/giy/Documents/Max\ 8/Packages/max-sdk/source/max-sdk-base/script/PkgInfo /Users/giy/Documents/Max\ 8/Packages/max-sdk/source/my/logistic/../../../externals/logistic.mxo/Contents/PkgInfo
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd "/Users/giy/Documents/Max 8/Packages/max-sdk/source/my/logistic"
  cp /Users/giy/Documents/Max\ 8/Packages/max-sdk/source/max-sdk-base/script/PkgInfo /Users/giy/Documents/Max\ 8/Packages/max-sdk/source/my/logistic/../../../externals/logistic.mxo/Contents/PkgInfo
fi

