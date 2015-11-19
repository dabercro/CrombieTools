#! /bin/bash

find . -name '*.so'  | xargs rm
find . -name '*.d'   | xargs rm
find . -name '*.pcm' | xargs rm
find . -name '*.pyc' | xargs rm
