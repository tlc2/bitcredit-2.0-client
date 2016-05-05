Release Process
====================

Before every release candidate:

* Update translations (ping wumpus on IRC) see [translation_process.md](https://github.com/bitcredit/bitcredit/blob/master/doc/translation_process.md#syncing-with-transifex)

Before every minor and major release:

* Update [bips.md](bips.md) to account for changes since the last release.
* Update version in sources (see below)
* Write release notes (see below)

Before every major release:

* Update hardcoded [seeds](/contrib/seeds/README.md), see [this pull request](https://github.com/bitcredit/bitcredit/pull/7415) for an example. 

### First time / New builders

Check out the source code in the following directory hierarchy.

    cd /path/to/your/toplevel/build
    git clone https://github.com/bitcredit-core/gitian.sigs.git
    git clone https://github.com/bitcredit-core/bitcredit-detached-sigs.git
    git clone https://github.com/devrandom/gitian-builder.git
    git clone https://github.com/bitcredit/bitcredit.git

### Bitcredit maintainers/release engineers, update version in sources

Update the following:

- `configure.ac`:
    - `_CLIENT_VERSION_MAJOR`
    - `_CLIENT_VERSION_MINOR`
    - `_CLIENT_VERSION_REVISION`
    - Don't forget to set `_CLIENT_VERSION_IS_RELEASE` to `true`
- `src/clientversion.h`: (this mirrors `configure.ac` - see issue #3539)
    - `CLIENT_VERSION_MAJOR`
    - `CLIENT_VERSION_MINOR`
    - `CLIENT_VERSION_REVISION`
    - Don't forget to set `CLIENT_VERSION_IS_RELEASE` to `true`
- `doc/README.md` and `doc/README_windows.txt`
- `doc/Doxyfile`: `PROJECT_NUMBER` contains the full version
- `contrib/gitian-descriptors/*.yml`: usually one'd want to do this on master after branching off the release - but be sure to at least do it before a new major release

Write release notes. git shortlog helps a lot, for example:

    git shortlog --no-merges v(current version, e.g. 0.7.2)..v(new version, e.g. 0.8.0)

(or ping @wumpus on IRC, he has specific tooling to generate the list of merged pulls
and sort them into categories based on labels)

Generate list of authors:

    git log --format='%aN' "$*" | sort -ui | sed -e 's/^/- /'

Tag version (or release candidate) in git

    git tag -s v(new version, e.g. 0.8.0)

### Setup and perform Gitian builds

Setup Gitian descriptors:

    pushd ./bitcredit
    export SIGNER=(your Gitian key, ie bluematt, sipa, etc)
    export VERSION=(new version, e.g. 0.8.0)
    git fetch
    git checkout v${VERSION}
    popd

Ensure your gitian.sigs are up-to-date if you wish to gverify your builds against other Gitian signatures.

    pushd ./gitian.sigs
    git pull
    popd

Ensure gitian-builder is up-to-date to take advantage of new caching features (`e9741525c` or later is recommended).

    pushd ./gitian-builder
    git pull
    popd

### Fetch and create inputs: (first time, or when dependency versions change)

    pushd ./gitian-builder
    mkdir -p inputs
    wget -P inputs https://bitcreditcore.org/cfields/osslsigncode-Backports-to-1.7.1.patch
    wget -P inputs http://downloads.sourceforge.net/project/osslsigncode/osslsigncode/osslsigncode-1.7.1.tar.gz
    popd

Register and download the Apple SDK: see [OS X readme](README_osx.txt) for details.

https://developer.apple.com/devcenter/download.action?path=/Developer_Tools/xcode_6.1.1/xcode_6.1.1.dmg

Using a Mac, create a tarball for the 10.9 SDK and copy it to the inputs directory:

    tar -C /Volumes/Xcode/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/ -czf MacOSX10.9.sdk.tar.gz MacOSX10.9.sdk

### Optional: Seed the Gitian sources cache and offline git repositories

By default, Gitian will fetch source files as needed. To cache them ahead of time:

    pushd ./gitian-builder
    make -C ../bitcredit/depends download SOURCES_PATH=`pwd`/cache/common
    popd

Only missing files will be fetched, so this is safe to re-run for each build.

NOTE: Offline builds must use the --url flag to ensure Gitian fetches only from local URLs. For example:

    pushd ./gitian-builder
    ./bin/gbuild --url bitcredit=/path/to/bitcredit,signature=/path/to/sigs {rest of arguments}
    popd

The gbuild invocations below <b>DO NOT DO THIS</b> by default.

### Build and sign Bitcredit Core for Linux, Windows, and OS X:

    pushd ./gitian-builder
    ./bin/gbuild --commit bitcredit=v${VERSION} ../bitcredit/contrib/gitian-descriptors/gitian-linux.yml
    ./bin/gsign --signer $SIGNER --release ${VERSION}-linux --destination ../gitian.sigs/ ../bitcredit/contrib/gitian-descriptors/gitian-linux.yml
    mv build/out/bitcredit-*.tar.gz build/out/src/bitcredit-*.tar.gz ../

    ./bin/gbuild --commit bitcredit=v${VERSION} ../bitcredit/contrib/gitian-descriptors/gitian-win.yml
    ./bin/gsign --signer $SIGNER --release ${VERSION}-win-unsigned --destination ../gitian.sigs/ ../bitcredit/contrib/gitian-descriptors/gitian-win.yml
    mv build/out/bitcredit-*-win-unsigned.tar.gz inputs/bitcredit-win-unsigned.tar.gz
    mv build/out/bitcredit-*.zip build/out/bitcredit-*.exe ../

    ./bin/gbuild --commit bitcredit=v${VERSION} ../bitcredit/contrib/gitian-descriptors/gitian-osx.yml
    ./bin/gsign --signer $SIGNER --release ${VERSION}-osx-unsigned --destination ../gitian.sigs/ ../bitcredit/contrib/gitian-descriptors/gitian-osx.yml
    mv build/out/bitcredit-*-osx-unsigned.tar.gz inputs/bitcredit-osx-unsigned.tar.gz
    mv build/out/bitcredit-*.tar.gz build/out/bitcredit-*.dmg ../
    popd

Build output expected:

  1. source tarball (`bitcredit-${VERSION}.tar.gz`)
  2. linux 32-bit and 64-bit dist tarballs (`bitcredit-${VERSION}-linux[32|64].tar.gz`)
  3. windows 32-bit and 64-bit unsigned installers and dist zips (`bitcredit-${VERSION}-win[32|64]-setup-unsigned.exe`, `bitcredit-${VERSION}-win[32|64].zip`)
  4. OS X unsigned installer and dist tarball (`bitcredit-${VERSION}-osx-unsigned.dmg`, `bitcredit-${VERSION}-osx64.tar.gz`)
  5. Gitian signatures (in `gitian.sigs/${VERSION}-<linux|{win,osx}-unsigned>/(your Gitian key)/`)

### Verify other gitian builders signatures to your own. (Optional)

Add other gitian builders keys to your gpg keyring

    gpg --import bitcredit/contrib/gitian-keys/*.pgp

Verify the signatures

    pushd ./gitian-builder
    ./bin/gverify -v -d ../gitian.sigs/ -r ${VERSION}-linux ../bitcredit/contrib/gitian-descriptors/gitian-linux.yml
    ./bin/gverify -v -d ../gitian.sigs/ -r ${VERSION}-win-unsigned ../bitcredit/contrib/gitian-descriptors/gitian-win.yml
    ./bin/gverify -v -d ../gitian.sigs/ -r ${VERSION}-osx-unsigned ../bitcredit/contrib/gitian-descriptors/gitian-osx.yml
    popd

### Next steps:

Commit your signature to gitian.sigs:

    pushd gitian.sigs
    git add ${VERSION}-linux/${SIGNER}
    git add ${VERSION}-win-unsigned/${SIGNER}
    git add ${VERSION}-osx-unsigned/${SIGNER}
    git commit -a
    git push  # Assuming you can push to the gitian.sigs tree
    popd

Wait for Windows/OS X detached signatures:

- Once the Windows/OS X builds each have 3 matching signatures, they will be signed with their respective release keys.
- Detached signatures will then be committed to the [bitcredit-detached-sigs](https://github.com/bitcredit-core/bitcredit-detached-sigs) repository, which can be combined with the unsigned apps to create signed binaries.

Create (and optionally verify) the signed OS X binary:

    pushd ./gitian-builder
    ./bin/gbuild -i --commit signature=v${VERSION} ../bitcredit/contrib/gitian-descriptors/gitian-osx-signer.yml
    ./bin/gsign --signer $SIGNER --release ${VERSION}-osx-signed --destination ../gitian.sigs/ ../bitcredit/contrib/gitian-descriptors/gitian-osx-signer.yml
    ./bin/gverify -v -d ../gitian.sigs/ -r ${VERSION}-osx-signed ../bitcredit/contrib/gitian-descriptors/gitian-osx-signer.yml
    mv build/out/bitcredit-osx-signed.dmg ../bitcredit-${VERSION}-osx.dmg
    popd

Create (and optionally verify) the signed Windows binaries:

    pushd ./gitian-builder
    ./bin/gbuild -i --commit signature=v${VERSION} ../bitcredit/contrib/gitian-descriptors/gitian-win-signer.yml
    ./bin/gsign --signer $SIGNER --release ${VERSION}-win-signed --destination ../gitian.sigs/ ../bitcredit/contrib/gitian-descriptors/gitian-win-signer.yml
    ./bin/gverify -v -d ../gitian.sigs/ -r ${VERSION}-win-signed ../bitcredit/contrib/gitian-descriptors/gitian-win-signer.yml
    mv build/out/bitcredit-*win64-setup.exe ../bitcredit-${VERSION}-win64-setup.exe
    mv build/out/bitcredit-*win32-setup.exe ../bitcredit-${VERSION}-win32-setup.exe
    popd

Commit your signature for the signed OS X/Windows binaries:

    pushd gitian.sigs
    git add ${VERSION}-osx-signed/${SIGNER}
    git add ${VERSION}-win-signed/${SIGNER}
    git commit -a
    git push  # Assuming you can push to the gitian.sigs tree
    popd

### After 3 or more people have gitian-built and their results match:

- Create `SHA256SUMS.asc` for the builds, and GPG-sign it:

```bash
sha256sum * > SHA256SUMS
```

The list of files should be:
```
bitcredit-${VERSION}-linux32.tar.gz
bitcredit-${VERSION}-linux64.tar.gz
bitcredit-${VERSION}-osx64.tar.gz
bitcredit-${VERSION}-osx.dmg
bitcredit-${VERSION}.tar.gz
bitcredit-${VERSION}-win32-setup.exe
bitcredit-${VERSION}-win32.zip
bitcredit-${VERSION}-win64-setup.exe
bitcredit-${VERSION}-win64.zip
```

- GPG-sign it, delete the unsigned file:
```
gpg --digest-algo sha256 --clearsign SHA256SUMS # outputs SHA256SUMS.asc
rm SHA256SUMS
```
(the digest algorithm is forced to sha256 to avoid confusion of the `Hash:` header that GPG adds with the SHA256 used for the files)
Note: check that SHA256SUMS itself doesn't end up in SHA256SUMS, which is a spurious/nonsensical entry.

- Upload zips and installers, as well as `SHA256SUMS.asc` from last step, to the bitcredit.org server
  into `/var/www/bin/bitcredit-core-${VERSION}`

- A `.torrent` will appear in the directory after a few minutes. Optionally help seed this torrent. To get the `magnet:` URI use:
```bash
transmission-show -m <torrent file>
```
Insert the magnet URI into the announcement sent to mailing lists. This permits
people without access to `bitcredit.org` to download the binary distribution.
Also put it into the `optional_magnetlink:` slot in the YAML file for
bitcredit.org (see below for bitcredit.org update instructions). 

- Update bitcredit.org version

  - First, check to see if the Bitcredit.org maintainers have prepared a
    release: https://github.com/bitcredit-dot-org/bitcredit.org/labels/Releases

      - If they have, it will have previously failed their Travis CI
        checks because the final release files weren't uploaded.
        Trigger a Travis CI rebuild---if it passes, merge.

  - If they have not prepared a release, follow the Bitcredit.org release
    instructions: https://github.com/bitcredit-dot-org/bitcredit.org#release-notes

  - After the pull request is merged, the website will automatically show the newest version within 15 minutes, as well
    as update the OS download links. Ping @saivann/@harding (saivann/harding on Freenode) in case anything goes wrong

- Announce the release:

  - bitcredit-dev and bitcredit-core-dev mailing list

  - Bitcredit Core announcements list https://bitcreditcore.org/en/list/announcements/join/

  - bitcreditcore.org blog post

  - Update title of #bitcredit on Freenode IRC

  - Optionally twitter, reddit /r/Bitcredit, ... but this will usually sort out itself

  - Notify BlueMatt so that he can start building [the PPAs](https://launchpad.net/~bitcredit/+archive/ubuntu/bitcredit)

  - Add release notes for the new version to the directory `doc/release-notes` in git master

  - Celebrate
