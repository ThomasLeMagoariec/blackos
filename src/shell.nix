# shell.nix
{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
    buildInputs = with pkgs; [
        nasm
        xorriso
    ];

    shellHook = ''
        zsh
    '';
}

