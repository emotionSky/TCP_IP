#!/bin/bash

git submodule update --init
git submodule update --remote
git submodule foreach 'git checkout main'