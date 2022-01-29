#!/usr/bin/bash

clean_dirs=(build build_debug tests/build)
for dir in ${clean_dirs[@]}; do
  if [ -d $dir ]; then
    printf "[ninja] %-11s: $(ninja -C $dir -t clean)\n" $dir
  fi
done
