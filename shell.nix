# shell.nix
{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
    buildInputs = with pkgs; [
        nasm
        xorriso
        gcc
    ];

    shellHook = ''
        zsh
    '';
}

