{
    description = "BlackOS";

    inputs = {
        nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
        flake-utils.url = "github:numtide/flake-utils";
    };

    outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
    let
        pkgs = import nixpkgs { inherit system; };
        cross = pkgs.pkgsCross.x86_64-embedded;
    in {
        devShells.default = pkgs.mkShell {
            packages = with pkgs; [
                gnumake
                nasm
                qemu
                xorriso
                grub2
            ];

            shellHook = ''
                echo "loaded!"
            '';
        };

        packages.default = pkgs.stdenv.mkDerivation {
            pname = "blackos";
            version = "0.0.1";

            src = ./.;

            nativeBuildInputs = with pkgs; [
                gnumake
                nasm
                gcc
            ];

            buildPhase = ''
                make
            '';

            installPhase = ''
                mkdir -p $out
                cp build/kernel.bin $out/
            '';
        };

        apps.default = {
            type = "app"

            program = toString ( pkgs.writeShellScript "run-os" ''
                qemu-system-i386 -cdrom $(ISO) -m 6G -debugcon stdio
            '' );
        };
    }
    );
}
