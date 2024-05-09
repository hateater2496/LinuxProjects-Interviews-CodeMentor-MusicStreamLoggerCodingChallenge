#!/bin/bash

export INSTALL_DIR="${INSTALL_DIR:=install}"

echo "${INSTALL_DIR}/bin/MusicStreamLogger -a http://localhost:8080 -r 15 -t 1000 -n 300"
${INSTALL_DIR}/bin/MusicStreamLogger -a http://localhost:8080 -r 15 -t 1000 -n 300

