#! /bin/sh

command="./main output.txt < input.txt; exec zsh"

gnome-terminal -- /bin/sh -c "$command" & gnome-terminal -- /bin/sh -c "$command"
