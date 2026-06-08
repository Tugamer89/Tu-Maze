#!/bin/bash

git config core.hooksPath .githooks

chmod +x .githooks/pre-commit

echo "Git hooks successfully configured! They now point to .githooks/"