#!/usr/bin/awk -f
BEGIN {
        FS = "(: )|(:)";
}

$3 == "warning" && $4 == "ISO C does not allow extra ‘;’ outside of a function" {
        system("sed -i '" $2 "s/;$//' " $1)
}
