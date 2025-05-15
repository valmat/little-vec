#!/bin/bash
# littlevec

CUR_DIR="$(dirname "$0")"
cd $CUR_DIR
CUR_DIR="$(pwd)"

# Make sources
cd ../src
make clean
make -j
cd $CUR_DIR

##############################################
SRCDIR="../src"
rm -rf pkg

mkdir -p pkg/usr/lib/rocksserver/plugins
mkdir -p pkg/etc/rocksserver
mkdir -p pkg/DEBIAN

cp "../config.ini"            "pkg/etc/rocksserver/littlevec.ini"
cp "${SRCDIR}/little_vec.so"  "pkg/usr/lib/rocksserver/plugins"

##############################################
last_tag=$(git -C "." describe --tags --candidates=1)
version=${last_tag:1}
echo 
echo "version: [$version]"

SIZE=$(du -sb pkg/ | cut -f1)
SIZE=$(echo "$SIZE/1024" | bc)

cat control \
    | sed "s/%InstalledSize%/$SIZE/g" \
    | sed "s/%version%/$version/g" \
    > pkg/DEBIAN/control

##############################################

fakeroot dpkg-deb --build pkg
mv pkg.deb "littlevec_${version}_amd64.deb"
