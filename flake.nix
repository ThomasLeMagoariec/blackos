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
        devShells.default = pkgs.mkShellNoCC {
            packages = with pkgs; [
                gnumake
                nasm
                qemu
                grub2
                mtools
                xorriso
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
                grub2
                mtools
                xorriso
            ];

            buildPhase = ''
                make
            '';

            installPhase = ''
                mkdir -p $out

                cp build/kernel.iso $out/
                cp build/kernel.bin $out/
            '';
        };

        apps.default = {
            type = "app";

            program = toString ( pkgs.writeShellScript "run-os" ''
                set -e 
                ISO="${self.packages.${system}.default}/kernel.iso"

                echo "using: $ISO"

                exec qemu-system-i386 -cdrom "$ISO" -m 6G -debugcon stdio
            '' );
        };
    }
    );
}
