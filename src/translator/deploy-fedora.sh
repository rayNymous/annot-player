#!/bin/bash -v
# 4/4/2012
# See: http://fedoraproject.org/wiki/How_to_create_an_RPM_package

## environment
VERSION=0.1.2.0

PACKAGE_NAME=annot-dict
PACKAGE_VERSION=$VERSION

SRC_PREFIX=/Volumes/local/dev/annot
SRC_SPEC="$SRC_PREFIX/src/translator/rpm.spec"
BUILD_PREFIX=$HOME/rpmbuild
BUILD_SRC="$BUILD_PREFIX/SOURCES/$PACKAGE_NAME-$PACKAGE_VERSION"
BUILD_SPEC="$BUILD_PREFIX/SPECS/$PACKAGE_NAME-$PACKAGE_VERSION.spec"

## copy

#rm -Rf "$BUILD_PREFIX"
#rpmdev-setuptree
test -e "$BUILD_PREFIX" || exit 1

#cp -R "$SRC_PREFIX" "$BUILD_SRC" || exit 1
cp -R "$SRC_PREFIX" "$BUILD_SRC"
rm -f "$BUILD_SRC"/TODO
rm -f "$BUILD_SRC"/*.user
repair-permissions

cd "$BUILD_PREFIX/SOURCES" || exit 1
archive * || exit 1
rm -Rf "$BUILD_SRC"

## build
cp -v "$SRC_SPEC" "$BUILD_SPEC" || exit 1
cd "$BUILD_PREFIX/SPECS" || exit 1
rpmbuild -ba "$BUILD_SPEC"

# EOF
