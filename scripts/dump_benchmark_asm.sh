#!/usr/bin/env bash
set -euo pipefail
OBJ="$1"
shift
for sym in "$@"; do
  echo "---- $sym ----"
  objdump -d --demangle "$OBJ" |
  awk "/<${sym}.*>:$/ {flag=1;print;next} /^$/ {if(flag){print;exit}} flag"
done
