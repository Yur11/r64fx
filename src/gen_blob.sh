#!/bin/bash
script_name="$(basename $0)"
output_file=''
text_input=''
binary_input=''

while [[ "$#" -gt 0 ]]; do
    case "$1" in
        -o)
            shift
            output_file="$1"
        ;;

        -t)
            shift
            text_input="$1"
        ;;

        -b)
            shift
            binary_input="$1"
        ;;

        -h)
            echo "Usage"
            echo "    $script_name -o <output_file> ( -t <text_input_file> | -b <binary_input_file> )"
            echo ""
            echo "Terminating null byte is added at the end if text input file is used."
            exit 0
        ;;

        *)
            echo "Bad input '$1' !"
            exit 1
        ;;
    esac
    shift
done

[[ -n "$output_file" ]] || { echo "No output file!"; exit 1; }
[[ -z "$text_input" && -z "$binary_input" ]] && { echo "No input file!"; exit 1; }
[[ -n "$text_input" && -n "$binary_input" ]] && { echo "One input file at a time!"; exit 1; }

blob="$(hexdump "$text_input$binary_input" -ve '/1 "%d" ","')"

if [[ -n "$text_input" ]]; then
    echo "{${blob}0}" > "$output_file"
else
    echo "{${blob::-1}}" > "$output_file"
fi

exit 0
