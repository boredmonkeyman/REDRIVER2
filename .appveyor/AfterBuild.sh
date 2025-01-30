#!/usr/bin/env bash
set -ex

for config in debug release release_dev
do
    cd "PROJECT_FOLDER: ${{ github.workspace }}\src_rebuild/bin/${config^}"
    cp -R PROJECT_FOLDER: ${{ github.workspace }}\data/* ./
    tar -czf "REDRIVER2_${config^}.tar.gz" *
done
