{
  lib,
  stdenv,
}: let
  fs = lib.fileset;
in
  stdenv.mkDerivation {
    name = "donothing";
    src = fs.toSource {
      root = ./.;
      fileset = fs.unions [
        ./donothing.c
        ./Makefile
      ];
    };

    installPhase = ''
      mv donothing.so $out
    '';
  }
