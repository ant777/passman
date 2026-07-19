CWD="$(pwd)"
 
# sed -i "s/<\/title>/<\/title><link rel=\"manifest\" href=\"\/passman\/res\/site\.webmanifest\" \/><link rel=\"icon\" type=\"image\/png\" href=\"\/passman\/res\/favicon-96x96.png\" sizes=\"96x96\" \/><link rel=\"icon\" type=\"image\/svg+xml\" href=\"\/passman\/res\/favicon.svg\" \/><link rel=\"shortcut icon\" href=\"\/passman\/res\/favicon\.ico\" \/><link rel=\"apple-touch-icon\" sizes=\"180x180\" href=\"\/passman\/res\/apple-touch-icon\.png\" \/>/" "./docs/index.html"
if [ "$1" = "inc" ]; then
    # File containing: VERSION="1.2.3"
    WEB_VERSION=$(sed -n 's/.*version = "\(.*\)".*/\1/p' "$WEB_VERSION_FILE")

    WEB_VERSION_FILE="./web/src/app/config.tsx"

    # 1. Extract current version
    CURRENT_VERSION=$(sed -n 's/.*version = "\(.*\)".*/\1/p' "$WEB_VERSION_FILE")

    # 2. Split into Major, Minor, Patch
    IFS='.' read -r major minor patch <<< "$CURRENT_VERSION"

    # 3. Increment (e.g., the patch version)
    patch=$((patch + 1))
    NEW_VERSION="$major.$minor.$patch"


    sed -i "s/version = \".*\"/version = \"$NEW_VERSION\"/" "$WEB_VERSION_FILE"
    sed -i "s/\",]/\",\\n\"$NEW_VERSION\",]/" "$WEB_VERSION_FILE"
    cd ./web/
    npm run build
    cd ../
    cp ./docs/$NEW_VERSION/index.html ./docs/index.html
    cp ./docs/$NEW_VERSION/registerSW.js ./docs/registerSW.js
    cp ./docs/$NEW_VERSION/sw.js ./docs/sw.js
    sed -i "s/passman\/\(.*\)\/registerSW\.js/passman\/registerSW\.js/" "./docs/index.html"

    sed -i "s/\/passman\/\(.*\)\/sw\.js/\/passman\/sw\.js/" "./docs/registerSW.js"
    sed -i "s/scope: '\/passman\/\(.*\)\//scope: '\/passman\//" "./docs/registerSW.js"
    sed -i "s/\.\/workbox/\.\/$NEW_VERSION\/workbox/g" "./docs/sw.js"
    sed -i "s/\.\.\/res/\.\/res/g" "./docs/sw.js"
    sed -i "s/assets\//$NEW_VERSION\/assets\//g" "./docs/sw.js"
    

    echo $NEW_VERSION 
fi
cd $CWD
echo "$CWD"
if [ "$3" = "pack" ] || [ "$1" = "pack" ] ; then

    VERSION_STRING=$(sed -nE 's/.*FIRMWARE_VERSION = [^0-9]*([0-9][0-9.]*).*/\1/p' ./firmware/general_config.h)
    echo $VERSION_STRING 
    cd ../arduino
    mkdir -p ../passman/docs/res/firmware/$VERSION_STRING
    python3 /home/ant777/snap/arduino/85/.arduino15/packages/esp32/tools/esptool_py/4.5.1/esptool.py --chip esp32s3 merge_bin --flash_mode dio --flash_size keep --flash_freq 80m 0x0 firmware.ino.bootloader.bin 0x00008000 firmware.ino.partitions.bin 0x00010000 firmware.ino.bin -o ../passman/docs/res/firmware/$VERSION_STRING/passman.bin

    cd $CWD
    sed -i "s/\',]/\',\\n\'$NEW_VERSION\',]/" "$WEB_VERSION_FILE"
    cp ./docs/res/manifest.json ./docs/res/firmware/$VERSION_STRING/manifest.json

    sed -i "s/1\.0\.0/$VERSION_STRING/" "./docs/res/firmware/$VERSION_STRING/manifest.json"
fi
if [ "$2" = "push" ]; then
COMMIT_MSG=upd
if [ "$3" != "" ] ; then
COMMIT_MSG=$3
fi
git add -A
git commit -n -m "$COMMIT_MSG"
git push
fi