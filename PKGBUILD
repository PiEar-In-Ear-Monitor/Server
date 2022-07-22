# Maintainer: Alex O'Connor <Alex.nderOConnor@gmail.com>
pkgname=piear
pkgver=0.5.0
pkgrel=1
pkgdesc="A Software based in-ear solution"
arch=('x86_64')
url="https://github.com/PiEar-In-Ear-Monitor/Server"
license=('custom:MIT')
groups=()
depends=(portaudio nodejs)
makedepends=(cmake gcc boost nlohmann-json npm)
checkdepends=(gtest)
optdepends=()
provides=(piear)
conflicts=()
replaces=()
backup=()
options=()
install=
changelog=
source=("$pkgname-$pkgver.tar.gz::https://github.com/PiEar-In-Ear-Monitor/Server/archive/refs/tags/v0.5.0.tar.gz")
noextract=()
sha256sums=("ce0173b5d9c0d112dcd89bbf8cdf53024da4039d4db1daceb22431908c8b97b8")
validpgpkeys=()

prepare() {
    tar -xvf "$pkgname-$pkgver.tar.gz"
    mv "Server-$pkgver/" "$pkgname"
}

build() {
    cd "$pkgname"
    [[ -d build ]] && rm -r build
    cmake -DCMAKE_INSTALL_PREFIX=/usr -DPACKAGE_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -B build
    cmake --build build
}

package() {
    cd "$pkgname"
    npm --prefix webserver install
    cmake --install build --prefix $pkgdir/usr
    mkdir -p $pkgdir/usr/share/licenses/piear
    cp LICENSE $pkgdir/usr/share/licenses/piear/LICENSE-MIT
    mkdir -p $pkgdir/lib/systemd/service
    cp piear.service $pkgdir/lib/systemd/service
}
