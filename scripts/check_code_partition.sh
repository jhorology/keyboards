#!/usr/bin/env bash
# Compare zmk.bin size against devicetree code_partition (and storage start).
set -euo pipefail

build_dir=${1:?"usage: $0 BUILD_DIR (e.g. build/q60)"}

dts="$build_dir/zephyr/zephyr.dts"
bin="$build_dir/zephyr/zmk.bin"
config="$build_dir/zephyr/.config"

if [[ ! -f $dts ]]; then
  echo "check_code_partition: skip (no $dts)"
  exit 0
fi

if [[ ! -f $bin ]]; then
  echo "check_code_partition: error: missing $bin" >&2
  exit 1
fi

read -r part_off part_size <<EOF
$(awk '
  /code_partition:/ { in_part = 1; next }
  in_part && /reg = </ {
    gsub(/[<>,;]/, " ")
    off = size = ""
    for (i = 1; i <= NF; i++) {
      if ($i ~ /^0x[0-9a-fA-F]+$/) {
        if (off == "") off = $i
        else size = $i
      }
    }
    if (off != "" && size != "") print off, size
    exit
  }
' "$dts")
EOF

if [[ -z ${part_off:-} || -z ${part_size:-} ]]; then
  echo "check_code_partition: skip (no code_partition in devicetree)"
  exit 0
fi

part_off=$((part_off))
part_size=$((part_size))

load_off=0
if [[ -f $config ]] && grep -q '^CONFIG_FLASH_LOAD_OFFSET=' "$config"; then
  load_off=$(grep '^CONFIG_FLASH_LOAD_OFFSET=' "$config" | cut -d= -f2)
  load_off=$((load_off))
fi

fw_size=$(wc -c <"$bin" | tr -d ' ')
fw_end=$((load_off + fw_size))
part_end=$((part_off + part_size))

storage_off=$(awk '
  /storage_partition:/ { in_part = 1; next }
  in_part && /reg = </ {
    gsub(/[<>,;]/, " ")
    for (i = 1; i <= NF; i++) {
      if ($i ~ /^0x[0-9a-fA-F]+$/) {
        print $i
        exit
      }
    }
  }
' "$dts")

fmt_k() {
  awk -v b="$1" 'BEGIN {
    if (b >= 1048576) printf "%.2f MiB (%d B)", b/1048576, b
    else if (b >= 1024) printf "%.2f KiB (%d B)", b/1024, b
    else printf "%d B", b
  }'
}

echo "check_code_partition: build=$build_dir"
echo "  code_partition: offset $(fmt_k "$part_off"), size $(fmt_k "$part_size")"
echo "  firmware:       $(fmt_k "$fw_size") (load offset $(fmt_k "$load_off"))"
echo "  firmware end:   $(fmt_k "$fw_end")  /  partition end: $(fmt_k "$part_end")"

if (( fw_end > part_end )); then
  over=$((fw_end - part_end))
  echo "  FAIL: firmware exceeds code_partition by $(fmt_k "$over")" >&2
  if [[ -n $storage_off ]]; then
    storage_off=$((storage_off))
    echo "  storage_partition starts at $(fmt_k "$storage_off") — NVS will be corrupted at boot" >&2
  fi
  exit 1
fi

headroom=$((part_end - fw_end))
echo "  OK: headroom $(fmt_k "$headroom")"

if [[ -n $storage_off ]]; then
  storage_off=$((storage_off))
  if (( fw_end > storage_off )); then
    over=$((fw_end - storage_off))
    echo "  FAIL: firmware overlaps storage_partition by $(fmt_k "$over")" >&2
    exit 1
  fi
  nvs_gap=$((storage_off - fw_end))
  echo "  OK: $(fmt_k "$nvs_gap") before storage_partition"
fi
