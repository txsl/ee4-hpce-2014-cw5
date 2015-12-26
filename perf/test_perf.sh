#!/bin/bash

cat /dev/zero | head -c $1 | eval "$2 $3 $4 $5 $6"